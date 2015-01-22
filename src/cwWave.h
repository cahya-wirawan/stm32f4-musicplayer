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

typedef struct
{
  uint32_t   ChunkID;       /* 0 */
  uint32_t   FileSize;      /* 4 */
  uint32_t   FileFormat;    /* 8 */
  uint32_t   SubChunk1ID;   /* 12 */
  uint32_t   SubChunk1Size; /* 16*/
  uint16_t   AudioFormat;   /* 20 */
  uint16_t   NbrChannels;   /* 22 */
  uint32_t   SampleRate;    /* 24 */
  
  uint32_t   ByteRate;      /* 28 */
  uint16_t   BlockAlign;    /* 32 */
  uint16_t   BitPerSample;  /* 34 */
  uint32_t   SubChunk2ID;   /* 36 */
  uint32_t   SubChunk2Size; /* 40 */
  
}WAVE_FormatTypeDef;

void cwWavePlayFile(char* filename);
void cwWaveAudioCallback(void *context,int buffer);

#endif /* defined(__CW_WAVE__) */
