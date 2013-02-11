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

int main(void)
{
    PaStreamParameters inputParameters, outputParameters;
    PaStream *stream = NULL;
    PaError err;
    SAMPLE *sampleBlock;
    float *windowBuffer;
    float buf[50000];
    int i, stop = 0, start = 0, forget = 4;
    float adjust = 0.0001, alpha = 0.95, tmp;
    float avgEnergy = 0, level, bg, lowThresh, highThresh, currEnergy[2];
    int numBytes, buflen, elapsedSpeech = 0, elapsedSilence = 0, elapsedSilenceThresh = 1000;
    int m, n, index, speech = 0, timeElapsed = 0;
    FILE *fid;
    float max_value = 32768.0;
    // for feature computation
    Features comp(40, CV_32F);
    cv::Mat wav;
    //Features comp;
	//Mat wav(50000, 1, CV_16U, (void*) buf, sizeof(float));;
	Mat feat;
    
    char *filename = new char[20];
    strcpy(filename, "recorded.dat");
	
    numBytes = FRAMES_PER_BUFFER * NUM_CHANNELS * SAMPLE_SIZE ;
    sampleBlock = (SAMPLE *) malloc( numBytes );
    buflen = 2 * FRAMES_PER_BUFFER * NUM_CHANNELS;
    windowBuffer = (float *) malloc( buflen * sizeof(float) );
    
    int framesRecorded, totalSamples;
    
    if( sampleBlock == NULL )
    {
        printf("Could not allocate record array.\n");
        exit(1);
    }
    CLEAR( sampleBlock );
    
    err = Pa_Initialize();
    if( err != paNoError ) goto error;
    
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = NUM_CHANNELS;
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency ;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    
    //fid = fopen("recorded.raw", "wb");
    /*fid = fopen(filename, "wb");
    if(fid == NULL)
        printf("Could not open file.");
    
    err = Pa_OpenStream(
                        &stream,
                        &inputParameters,
                        NULL,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paClipOff,      
                        NULL, 
                        NULL );
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
            lowThresh = bg + 0.4;
            highThresh = 20 * lowThresh;
        }
        else{
            currEnergy[0] = 0; currEnergy[1] = 0;
            for(n = 0; n < FRAMES_PER_BUFFER; n++){
                windowBuffer[n] = (float)(windowBuffer[n + FRAMES_PER_BUFFER]);
                windowBuffer[n + FRAMES_PER_BUFFER] = (float)(sampleBlock[n] / max_value);
                
                currEnergy[0] += pow(windowBuffer[n], (float)2);
                currEnergy[1] += pow(windowBuffer[n + (FRAMES_PER_BUFFER / 2)], (float)2);
            }
            
            for(m = 0; m < 2; m++){
                
                level = ((level * forget) + currEnergy[m]) / (forget + 1);
                
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
                    }
                }
                                
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
                
        if(fid != NULL){
            //fwrite(sampleBlock, NUM_CHANNELS * sizeof(SAMPLE), FRAMES_PER_BUFFER, fid);
            for(n = 0; n < FRAMES_PER_BUFFER; n++){
                
                tmp = (float)sampleBlock[n] / max_value;
                //if(n == 0) printf("%d, %f\n", sampleBlock[n], tmp); fflush(stdout);
                fprintf(fid, "%f\n", tmp);
            }
        }
        
        timeElapsed += WINDOW_SIZE;
        
        i++;
                
        if((start == 1) && (speech == 0)){
            stop = 1;
            printf("Speech stopped\n");
        }
        
    }
    
    framesRecorded = i;
    totalSamples = framesRecorded * FRAMES_PER_BUFFER;
    
    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;
    
    CLEAR( sampleBlock );
    
    if(fid != NULL)
        fclose(fid); */
    
    free( sampleBlock );
    
    /*fid = fopen("recorded.raw","r");
    
    if(fid != NULL) {
        n = fread(buf,sizeof(float),50000,fid);
    }
    
    comp.wav2feat(wav,feat);
    write2file(feat, "features.raw");
    while(n != 0) {
		n = fread(buf,sizeof(float),50000,fid);
    	comp.wav2feat(wav,feat);
    	write2file(feat, "features.raw");
    }
	comp.wav2feat(wav,feat);*/
    
    wav = comp.readFile(filename);
    comp.wav2feat(wav, feat);
    
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
