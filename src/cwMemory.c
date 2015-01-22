//
//  cwMemory.c
//  

#include "string.h"
#include "cwMemory.h"
#include "Audio.h"
#include "cwSoundFile.h"
#include "tm_stm32f4_disco.h"

#define f_tell(fp)		((fp)->fptr)
#define SINWAVE_STEREO

const int16_t cwMemoryAudioSample [] =
{
#include "Sinwave.h"
};


void cwMemoryPlayFile(char* filename) {
  if (1) {
    InitializeAudio(Audio44100HzSettings);
    SetAudioVolume(0xAF);
    PlayAudioWithCallback(cwMemoryAudioCallback, 0);
    for(;;) {
      if (TM_DISCO_ButtonPressed()) {
        StopAudio();
        
        // Re-initialize and set volume to avoid noise
        InitializeAudio(Audio44100HzSettings);
        SetAudioVolume(0);
        
        // Wait for user button release
        while(TM_DISCO_ButtonPressed()){};
        
        // Return to previous function
        return;
      }
    }
  }
}

/*
 * Called by the audio driver when it is time to provide data to
 * one of the audio buffers (while the other buffer is sent to the
 * CODEC using DMA). One mp3 frame is decoded at a time and
 * provided to the audio driver.
 */
void cwMemoryAudioCallback(void *context, int buffer) {
  static int16_t audio_buffer0[4096];
  static int16_t audio_buffer1[4096];

  int16_t *samples;
  if (buffer) {
    samples = audio_buffer0;
    TM_DISCO_LedOn(LED_RED);
    TM_DISCO_LedOff(LED_GREEN);
  } else {
    samples = audio_buffer1;
    TM_DISCO_LedOff(LED_RED);
    TM_DISCO_LedOn(LED_GREEN);
  }
  
  for (int i=0; i<4; i++) {
    for (int j=0; j<512; j++) {
      samples[i*512+j] = cwMemoryAudioSample[j];
    }
  }
  
  ProvideAudioBuffer(samples, 512*4);
}
