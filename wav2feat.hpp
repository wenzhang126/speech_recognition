/*
 *  wav2feat.h
 *  
 *
 *  Created by Christopher Palmer on 2/3/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <cv.hpp>
#include <highgui.h>
#include <string.h>

#define FS 16000
#define ALPHA (.95)		// emphasis coefficient
#define TW 25
#define TO 15 
#define NW (((float)TW/1000)*FS)
#define NO (((float)TO/1000)*FS)
#define NPOW2 512
#define NPOW2LOG 64
#define NMFCCS 13
using namespace cv;
using namespace std;

class Features {
	private: 
	cv::Mat hammingWindow;
	cv::Mat logFilterBank;
	cv::Mat frequencies;
	cv::Mat logFrequencies;
	int nLogFilterBanks;
	int type;
    //cv::Mat readFile(char *filename);
    //void write2file(cv::Mat mat, const char* file);
	void preemphasis(cv::Mat wav, cv::Mat pre);
	void spectrogram(cv::Mat wav,cv::Mat spec);
	void generateFreqs(cv::Mat freqs);
	cv::Mat generateFilterBank(int N, cv::Mat freqVect, cv::Mat melfvect);
	void setNLogFilterBanks(int nLogFilterBanks);
	void generateHamming(cv::Mat window);
	void calcDeriv(cv::Mat in, cv::Mat out) ;
	void mfccFilter(cv::Mat spec,cv::Mat mfcc);

	public:
	void wav2feat (cv::Mat wav, cv::Mat feat);
    cv::Mat readFile(char *filename);
    void write2file(cv::Mat mat, const char* file);
	// takes path to directory with hammingWindow and Filterbank coeffs
	Features();
	Features(int nLogFilterBanks, int type);
};
