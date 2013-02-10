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
#define NLOGFILTS 40
#define NPOW2 512
using namespace cv;
using namespace std;

class Features {
	private: 
	cv::Mat hammingWindow;
	cv::Mat logFilterBank;
	cv::Mat frequencies;
	cv::Range logFilterBankRange;
	int nLogFilterBanks;
	int type;
	void preemphasis(cv::Mat wav, cv::Mat pre);
	void spectrogram(cv::Mat wav,cv::Mat spec);
	void generateFreqs(cv::Mat freqs);
	void generateFilterBank(cv::Mat filterbank);
	void setNLogFilterBanks(int nLogFilterBanks);
	void generateHamming(cv::Mat window);
	
	public:
	void wav2feat (cv::Mat wav, cv::Mat feat);
	// takes path to directory with hammingWindow and Filterbank coeffs
	Features();
	Features(int nLogFilterBanks, cv::Range frange, int type);
};
