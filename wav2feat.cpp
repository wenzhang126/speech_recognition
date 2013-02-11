#include <stdio.h>
#include <stdlib.h>
#include <cv.hpp>
#include <highgui.h>
#include <string.h>
#include <math.h>
#include "wav2feat.hpp"

// accepts 16k PCM 16 bit.
using namespace cv;
using namespace std;

Features::Features() {
	//load hamming window
	nLogFilterBanks = 30;
	generateHamming(hammingWindow);
	//load log filter bank
	
}

Features::Features(int nLogFilterBanks, int type) {
	nLogFilterBanks = nLogFilterBanks;
	type = type;
	//frequencies((int)NW,1, ) // need float type;
	generateFreqs(frequencies);
	generateHamming(hammingWindow);
    logFilterBank = generateFilterBank(nLogFilterBanks, frequencies, logFrequencies);
}

void Features::generateFreqs(cv::Mat freqs) {
	for (int i = 0; i < NW; i++) {
		freqs.row(i) = (FS*i)/(2*CV_PI*NW);	
	}
}

cv::Mat Features::generateFilterBank(int N, cv::Mat freqVect, cv::Mat melfvect){
    
    Size s = freqVect.size();
    int len = s.height;
    int i, j, filtlen, start, direction;
    float maxf, minf, melbinwidth, step, val;
    
    for(i = 0; i < len; i++){
        melfvect.at<float>(i, 0) = 2595 * log10((1 + freqVect.at<float>(i, 0)) / 700);
    }

    maxf = melfvect.at<float>(s.height - 1, 0);
    minf = melfvect.at<float>(0, 0);
    melbinwidth = (maxf - minf) / N;
    
    cv::Mat filterbank = cv::Mat::zeros(N, s.height, freqVect.type());
    for (i = 0; i < N; i++){
        filtlen = 0;
        start = 0;
        direction = 0;
        for(j = 0; j < len; j++){
            if((melfvect.at<float>(j, 0) >= (i*melbinwidth+minf)) &&
               (melfvect.at<float>(j, 0) <= (i+1)*melbinwidth+minf)){
                
                if(start == 0)
                    start = j;
                
                filtlen++;
            }
        }
        
        step = 2. / (float)filtlen;
        val = 0;
        
        for(j = start; j < filtlen; j++){
            
            if(direction == 0){
                if(val > 1){
                    direction = 1;
                }
                else{
                    val += step;
                }
            }
            else{
                if(val < 0){
                    val = 0;
                }
                else{
                    val -= step;
                }
            }
                   
            filterbank.at<float>(i, j) = val;
        }
    }
    
    return filterbank;
}

void Features::wav2feat (cv::Mat wav, cv::Mat feat)
{
	// initialize necessary variables
	Size wavS = wav.size();
	// Number of columns in the spectrogram
	int NTimeBuckets = wavS.height/NO;
	cv::Mat f(NPOW2,1,wav.type());
	cv::Mat tmp(wavS,wav.type());
	cv::Mat spec(NPOW2,NTimeBuckets,wav.type());
	cv::Mat mfcc = cv::Mat(NTimeBuckets,nLogFilterBanks,wav.type());
	cv::Mat v(NMFCCS,NTimeBuckets,wav.type());
	cv::Mat a(NMFCCS,NTimeBuckets,wav.type());
	// pre-emphasize the input
	preemphasis(wav,tmp);
	
	// take the log magnitude of the spectrogram
	spectrogram(tmp,spec);
	cv::abs(spec);
	cv::log(spec,spec);
	for(int col = 0; col < spec.cols; ++col) {
		mfcc.col(col) = (logFilterBank*spec.colRange(col,col));
		cv::dct(mfcc.col(col),mfcc.col(col),NULL);
	}
	//mfccfeat = mfcc.rowRange(0,NMFCCS);
	calcDeriv(feat.rowRange(0,NMFCCS),v);
	calcDeriv(v,a);
	std::cout << mfcc;
}

void Features::calcDeriv(cv::Mat in, cv::Mat out) {
	cv::Size s = in.size();
	cv::Mat tmp(s,in.type());
	cv::Mat tmp2(s,in.type());
	cv::Mat z = cv::Mat::zeros(NMFCCS,1,in.type());
	hconcat(in.rowRange(0,s.height-1),z,tmp);
	hconcat(z,in.rowRange(0,s.height-1),tmp2);
	out =tmp - tmp2;
}


void Features::preemphasis(cv::Mat wav, cv::Mat emph) 
{
	cv::Size s = wav.size();
	cv::Mat tmp(s,wav.type());
	cv::Mat tmp2(s,wav.type());
	cv::Mat z = cv::Mat::zeros(1,1,wav.type());
	vconcat(wav.rowRange(0,s.height-1),z,tmp);
	vconcat(z,wav.rowRange(0,s.height-1),tmp2);
	emph = Mat(tmp-ALPHA*tmp2).clone();
}

void Features::generateHamming(cv::Mat window)
{
    for(int i = 0; i < NW; i++)
        window.row(i) = 0.54 - 0.46 * cos(2 * M_PI * i / NW);
}


void Features::spectrogram(cv::Mat wav,cv::Mat spec)
{
	Size s = wav.size();
	int N = floor(s.height/(NW-NO));
	for (int i = 0; i < N; i++) {
		if (i*(NW-NO)+NW-1 < s.height)	
			cv::dft(wav.rowRange(i*(NW-NO),i*(NW-NO)+NW-1) * hammingWindow,spec.col(i),NPOW2);
	}
}
