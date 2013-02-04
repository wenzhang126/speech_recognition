/*
 *  wav2feat.h
 *  
 *
 *  Created by Christopher Palmer on 2/3/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include <audio_capture.h>
#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include "audio_capture.h"

#define FS 16000
#define ALPHA (.95)		// emphasis coefficient
#define TW 25
#define TO 15 
#define NW ((TW/1000)*FS)
#define NO ((TO/1000)*FS)
#define N_LOG_FILTS 40

void wav2feat (Mat* wav, Mat* feat);
void preemphasis(Mat* wav);