//
//  capture_blocking.c
//  
//
//  Created by Kevin Elfenbein on 1/27/13.
//
//

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "portaudio.h"
#include <math.h>
#include "capture_blocking.h"
#include "wav2feat.hpp"

int main(void)
{
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream = NULL;
    PaError err;
    SAMPLE *sampleBlock;
    float *windowBuffer;
    float buf[50000];
    int i, stop = 0, start = 0, forget = 4;
    float adjust = 0.0001, alpha = 0.95;
    float avgEnergy = 0, level, bg, lowThresh, highThresh, currEnergy[2];
    int numBytes, buflen, elapsedSpeech = 0, elapsedSilence = 0, elapsedSilenceThresh = 1000;
    int m, n, index, speech = 0, timeElapsed = 0;
    FILE *fid;
    float max_value = 32768.0;
    // for feature computation
    Features comp("filters/");
	Mat wav(50000, 1, CV_16U, (void*) buf, sizeof(float));;
	Mat feat;
	
    numBytes = FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE ;
    sampleBlock = (SAMPLE *) malloc( numBytes );
    buflen = 2 * FRAMES_PER_BUFFER * NUM_CHANNELS;
    windowBuffer = (float *) malloc( buflen * sizeof(float) );
    if( sampleBlock == NULL )
    {
        printf("Could not allocate record array.\n");
        exit(1);
    }
    CLEAR( sampleBlock );
    
    err = Pa_Initialize();
    if( err != paNoError ) goto error;
    
    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    printf( "Input device # %d.\n", inputParameters.device );
    printf( "Input LL: %g s\n", Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency );
    printf( "Input HL: %g s\n", Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency );

    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency ;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    
    fid = fopen("recorded.raw", "wb");
    if(fid == NULL)
        printf("Could not open file.");
    
    /* -- setup -- */
    
    err = Pa_OpenStream(
                        &stream,
                        &inputParameters,
                        NULL,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paClipOff,      /* we won't output out of range samples so don't bother clipping them */
                        NULL, /* no callback, use blocking API */
                        NULL ); /* no callback, so no callback userData */
    if( err != paNoError ) goto error;
    
    i = 0;
    
    if(getchar() == 'r'){
        err = Pa_StartStream( stream );
        if( err != paNoError ) goto error;
        printf("Wire on. Will record %d samples until speech stops\n", FRAMES_PER_BUFFER); fflush(stdout);
    }
    
    while( stop == 0 )
    {
        
        err = Pa_ReadStream( stream, sampleBlock, FRAMES_PER_BUFFER );
        if( err && CHECK_OVERFLOW ) goto xrun;
                
        if(i < 2){
            for(n = 0; n < FRAMES_PER_BUFFER; n++){
                index = n + (i*FRAMES_PER_BUFFER);
                windowBuffer[index] = (float)(sampleBlock[n] / max_value);
            }
        }
        else if((i >= 2) && (i < 8)){
            
            currEnergy[0] = 0; currEnergy[1] = 0;
            for(n = 0; n < FRAMES_PER_BUFFER; n++){
                windowBuffer[n] = (float)(windowBuffer[n + FRAMES_PER_BUFFER]);
                windowBuffer[n + FRAMES_PER_BUFFER] = (float)(sampleBlock[n] / max_value);
                
                currEnergy[0] += pow(windowBuffer[n], (float) 2.0);
                currEnergy[1] += pow(windowBuffer[n + (FRAMES_PER_BUFFER / 2)], (float) 2.0);
            }
            
            if(i == 3)
                level = currEnergy[0];
            else{
                level = ((level * forget) + currEnergy[0]) / (forget + 1);
                level = ((level * forget) + currEnergy[1]) / (forget + 1);
            }
            
            avgEnergy += (currEnergy[0] + currEnergy[1]);
            avgEnergy = avgEnergy / 3;
            bg = avgEnergy;
            lowThresh = 4 * bg;
            highThresh = 40 * lowThresh;
            
            //if(i == 7) printf("Background energy = %f\n", bg);
        }
        else{
            currEnergy[0] = 0; currEnergy[1] = 0;
            for(n = 0; n < FRAMES_PER_BUFFER; n++){
                windowBuffer[n] = (float)(windowBuffer[n + FRAMES_PER_BUFFER]);
                windowBuffer[n + FRAMES_PER_BUFFER] = (float)(sampleBlock[n] / max_value);
                
                currEnergy[0] += pow(windowBuffer[n], (float)2);
                currEnergy[1] += pow(windowBuffer[n + (FRAMES_PER_BUFFER / 2)], (float)2);
            }
            
            //printf("buffer[0] = %f\n", windowBuffer[0]);
            //printf("currEnergy[0] = %f\n", currEnergy[0]);
            
            for(m = 0; m < 2; m++){
                
                level = ((level * forget) + currEnergy[m]) / (forget + 1);
                
                //printf("level = %f, bg = %f, lowThresh = %f, highThresh = %f\n", level,  bg, lowThresh, highThresh);
                
                if(speech == 1){
                    if((level - bg) < lowThresh){
                        if(elapsedSilence < elapsedSilenceThresh){
                            speech = 1;
                        }
                        else{
                            speech = 0;
                            elapsedSpeech = 0;
                        }
                        
                        elapsedSilence += HOP_SIZE;
                        //printf("Elapsed silence = %d\n", elapsedSilence);
                    }
                    else{
                        speech = 1;
                        elapsedSpeech += HOP_SIZE;
                    }
                }
                else{
                    if((level - bg) > highThresh){
                        speech = 1;
                        
                        if(start == 0){
                            printf("Speech started\n");
                            start = 1;
                        }
                        
                        elapsedSilence = 0;
                    }
                    else{
                        elapsedSpeech = 0;
                        elapsedSilence += HOP_SIZE;
                        //printf("Elapsed silence = %d\n", elapsedSilence);
                    }
                }
                
                //printf("speech = %d\n", speech);
                
                if(speech == 1){
                    bg += (level - bg) * adjust;
                }
                else{
                    bg = alpha * bg + (1 - alpha) * (level);
                }
                
                lowThresh = 1.2 * bg;
                highThresh = 2 * lowThresh;
                
            }
            
        }
        
        //printf("speech = %d\n", speech);
        
        if(fid != NULL){
            fwrite(sampleBlock, NUM_CHANNELS * sizeof(SAMPLE), FRAMES_PER_BUFFER, fid);
        }
        
        timeElapsed += WINDOW_SIZE;
        //printf("Time elapsed = %d\n", timeElapsed);
        
        i++;
                
        if((start == 1) && (speech == 0)){
            stop = 1;
            printf("Speech stopped\n");
        }
        
        /*if(i == 500){
            stop = 1;
            printf("Stopped by default\n");
        }*/
    }
    
    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;
    
    CLEAR( sampleBlock );
    
    if(fid != NULL)
        fclose(fid);
    
    free( sampleBlock );
    
    fid = fopen("recorded.raw","r");
    n = fread(buf,sizeof(float),50000,fid);
    comp.wav2feat(wav,feat);
    write2file(feat);
    while(n != 0) {
		n = fread(buf,sizeof(float),50000,fid);
    	comp.wav2feat(wav,feat);
    	write2file(feat);
    }
	comp.wav2feat(wav,feat);
	
    Pa_Terminate();
    return 0;
    
xrun:
    if( stream ) {
        Pa_AbortStream( stream );
        Pa_CloseStream( stream );
    }
    free( sampleBlock );
    Pa_Terminate();
    if( err & paInputOverflow )
        fprintf( stderr, "Input Overflow.\n" );
    return -2;
    
error:
    if( stream ) {
        Pa_AbortStream( stream );
        Pa_CloseStream( stream );
    }
    free( sampleBlock );
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;
}

void write2file(cv::Mat mat) {

}
