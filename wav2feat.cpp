#include <stdio.h>
#include <stdlib.h>
#include <cv.hpp>
#include <highgui.h>
#include <string.h>
#include "wav2feat.hpp"

// accepts 16k PCM 16 bit.
using namespace cv;
using namespace std;
Features::Features(std::string path) {
	//load hamming window
	
	//load log filter bank
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
	
	log(spec,spec);

		
	for(int col = 0; col < spec.cols; ++col) {
		mfcc.col(col) = (logFilterBank*spec.colRange(col,col));
	}
	

}



void Features::preemphasis(cv::Mat wav, cv::Mat emph) 
{
	
	cv::Size s = wav.size();
	cv::Mat tmp(s,wav.type());
	cv::Mat tmp2(s,wav.type());
	cv::Mat z = cv::Mat::zeros(1,1,wav.type());
	vconcat(wav.rowRange(0,s.height-1),z,tmp);
	emph.setTo(tmp-ALPHA*tmp2);
}



void Features::spectrogram(cv::Mat wav,cv::Mat spec)
{
	Size s = wav.size();
	int N floor(s.height/(NW-NO));
	for (int i = 0; i < N; i++) {
		if (i*(NW-NO)+NW-1 < s.height)	
			cv::dft(wav.rowRange(i*(NW-NO),i*(NW-NO)+NW-1) * hammingWindow,spec.colRange(i,i),NPOW2);
	 }
}
