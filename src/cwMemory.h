//
//  cwMemory.h
// 
//
//

#ifndef __CW_MEMORY__
#define __CW_MEMORY__

#include <stdio.h>
#include <stdint.h>
#include "Audio.h"
#include "ff.h"

void cwMemoryPlayFile(char* filename);
void cwMemoryAudioCallback(void *context,int buffer);

#endif /* defined(__CW_MEMORY__) */
