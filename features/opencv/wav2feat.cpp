

#include "wav2feat.h"
#include <opencv.h>

// accepts 16k PCM 16 bit.
void wav2feat (InputArray* wav, OuptutArray* feat) {
		Size wav_s = wav.size();
		NTimeBuckets = ;
		Mat f = zeros(NPOW2,1,wav.type);
		Mat tmp = Mat(wav_s,wav.type);
		Mat spec = ;
		Mat mfcc = Mat(,40)
;	preemphasis(wav,tmp);
		spectrogram(tmp,spec);
	
		abs(spec,tmp);
		log(tmp,spec);
		// load log_filter, logfrequencies and frequencies
		for(int col = 0; col < spec.cols; ++col) {
				mfcc.col(col).assignTo(filter*spec.colRange(col,col));		//points to each pixel B,G,R value in turn assuming a CV_8UC3 color image 
    	}
}

void preemphasis(InputArray* wav, OutputArray* emph) {
		Size s = wav.size();	
emph.setTo(wav);
		emph.assignTo(vconcat(wav.rowRange(0,s.height-1,z,tmp)-
ALPHA*vconcat(z,wav.rowrange(1,s.height),tmp2);
}

void spectrogram(InputArray* wav,OutputArray* spec){
		Mat window = ;	//load hamming coefficients
		Size s = wav.size();
		for (int i = 0; i < s.height; i++) {
				if (i((NW-NO)+NW-1 < s.height))
						cvdft(wav.rowRange(i((NW-NO),i((NW-NO)+NW-1) * window,spec.colRange(i),NPOW2);
			 }
	}
