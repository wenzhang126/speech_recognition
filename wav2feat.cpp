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

Features::Features(int nLogFilterBanks, cv::Range frange, int type) {
	nLogFilterBanks = nLogFilterBanks;
	logFilterBankRange = frange;
	type = type;
	//frequencies((int)NW,1, ) // need float type;
	frequencies = malloc(sizeof(float)*NW);
	generateFreqs(frequencies);
	hammingWindow((int)NW,1,type);
	generateHamming(hammingWindow);
	logFilterBank = generateFilterBank(nLogFilterBanks);
}

void generateFreqs(cv::Mat freqs) {
	cv::Mat freqs(NW,1,);
	for (int i = 0; i < NW; i++) {
		freqs.row(i) = (FS*i)/(2*CV_PI*N);	
	}
}

cv::Mat generateFilterBank(int N, cv::Mat freqVect, cv::Mat melfvect){
    
    Size s = freqVect.size();
    int len = s.height;
    int i;
    float maxf, minf, melbinwidth;
    
    for(i = 0; i < len; i++){
        melfvect.row(i) = 2595 * log10((1 + freqVect.row(i)) / 700);
    }
    
    maxf = melfvect.row(s.height - 1);
    minf = melfvect.row(0);
    melbinwidth = (maxf - minf) / (N + 1);
    
    
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
	cv::Mat mfcc = cv::Mat(NTimeBuckets,40,wav.type());
	
	// pre-emphasize the input
	preemphasis(wav,tmp);
	
	// take the log magnitude of the spectrogram
	spectrogram(tmp,spec);
	abs(spec);
	log10(spec,spec);
	for(int col = 0; col < spec.cols; ++col) {
		mfcc.col(col) = (logFilterBank*spec.colRange(col,col));
		cv::dct(mfcc(col),mfcc(col),NULL);
	}
	std::cout << mfcc;

}



void Features::preemphasis(cv::Mat wav, cv::Mat emph) 
{
	cv::Size s = wav.size();
	cv::Mat tmp(s,wav.type());
	cv::Mat tmp2(s,wav.type());
	cv::Mat z = cv::Mat::zeros(1,1,wav.type());
	vconcat(wav.rowRange(0,s.height-1),z,tmp);
	emph = Mat(tmp-ALPHA*tmp2).clone();
}

void generateHamming(cv::Mat window)
{
    for(i = 0; i < NW; i++){
        window.row(i) = 0.54 - 0.46 * cos(2 * CV_PI * i / N);
    }
}

void Features::spectrogram(cv::Mat wav,cv::Mat spec, cv::Mat freqs)
{
	Size s = wav.size();
	int N = floor(s.height/(NW-NO));
	for (int i = 0; i < N; i++) {
		if (i*(NW-NO)+NW-1 < s.height)	
			cv::dft(wav.rowRange(i*(NW-NO),i*(NW-NO)+NW-1) * hammingWindow,spec.col(i),NPOW2);
	}
}
