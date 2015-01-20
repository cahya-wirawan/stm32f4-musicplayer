//
//  cwWave.h
// 
//
//

#ifndef __CW_WAVE__
#define __CW_WAVE__

#include <stdio.h>
#include <stdint.h>
#include "Audio.h"
#include "ff.h"

void AudioCallback(void *context,int buffer);
void cwWavePlayFile(char* filename);

#endif /* defined(__CW_WAVE__) */
