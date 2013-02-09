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
	
}

void Features::wav2feat (cv::Mat wav, cv::Mat feat) 
{
	// initialize necessary variables
	Size wavS = wav.size();
	// Number of collumns in the spectrogram
	int NTimeBuckets = wavS.height/NW;
	cv::Mat f(NPOW2,1,wav.type());
	cv::Mat tmp(wavS,wav.type());
	//cv::Mat spec = ;
	//cv::Mat mfcc = cv::Mat(,40);
	
	// pre-emphasize the input
	preemphasis(wav,tmp);
	
	
	// take the log magnitude of the spectrogram
	//spectrogram(tmp,spec);
	
	
	//abs(spec,tmp);
	
	//log(tmp,spec);

		
	//for(int col = 0; col < spec.cols; ++col) {
		
	//	mfcc.col(col).assignTo(logFilterBank*spec.colRange(col,col));
		//points to each pixel B,G,R value in turn assuming a CV_8UC3 color image 
    
	//}

}



void Features::preemphasis(cv::Mat wav, cv::Mat emph) 
{
	
	Size s = wav.size();
	emph.setTo(wav);
	//emph.assignTo(vconcat(wav.rowRange(0,s.height-1,z,tmp)-
	//				ALPHA*vconcat(z,wav.rowrange(1,s.height),tmp2);
}



void Features::spectrogram(cv::Mat wav,cv::Mat spec)
{
	Size s = wav.size();
	for (int i = 0; i < s.height; i++) {
		if (i*(NW-NO)+NW-1 < s.height)	
			cv::dft(wav.rowRange(i*(NW-NO),i*(NW-NO)+NW-1) * hammingWindow,spec.colRange(i,i),NPOW2);
	 }
}
