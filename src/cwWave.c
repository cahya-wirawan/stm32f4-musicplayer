//
//  cwWave.c
//  

#include "string.h"
#include "Audio.h"
#include "cwSoundFile.h"
#include "tm_stm32f4_disco.h"

#define f_tell(fp)		((fp)->fptr)

extern FIL cwSFFile;
extern char cwSFFileReadBuffer[CW_FS_FILE_READ_BUFFER_SIZE];
extern volatile int cwSFBytesLeft;
extern char *cwSFReadPtr;

void cwWavePlayFile(char* filename) {
  unsigned int br, btr;
  FRESULT res;
  
  cwSFBytesLeft = CW_FS_FILE_READ_BUFFER_SIZE;
  cwSFReadPtr = cwSFFileReadBuffer;
  
  if (FR_OK == f_open(&cwSFFile, filename, FA_OPEN_EXISTING | FA_READ)) {
    
    // Read ID3v2 Tag
    char szArtist[120];
    char szTitle[120];
    Mp3ReadId3V2Tag(&cwSFFile, szArtist, sizeof(szArtist), szTitle, sizeof(szTitle));
    
    // Fill buffer
    res = f_read(&cwSFFile, cwSFFileReadBuffer, CW_FS_FILE_READ_BUFFER_SIZE, &br);
    
    // Play mp3
    hMP3Decoder = MP3InitDecoder();
    InitializeAudio(Audio44100HzSettings);
    SetAudioVolume(0xAF);
    PlayAudioWithCallback(AudioCallback, 0);
    
    for(;;) {
      /*
       * If past half of buffer, refill...
       *
       * When cwSFBytesLeft changes, the audio callback has just been executed. This
       * means that there should be enough time to copy the end of the buffer
       * to the beginning and update the pointer before the next audio callback.
       * Getting audio callbacks while the next part of the file is read from the
       * file system should not cause problems.
       */
      if (cwSFBytesLeft < (CW_FS_FILE_READ_BUFFER_SIZE / 2)) {
        // Copy rest of data to beginning of read buffer
        memcpy(cwSFFileReadBuffer, cwSFReadPtr, cwSFBytesLeft);
        
        // Update read pointer for audio sampling
        cwSFReadPtr = cwSFFileReadBuffer;
        
        // Read next part of file
        btr = CW_FS_FILE_READ_BUFFER_SIZE - cwSFBytesLeft;
        res = f_read(&cwSFFile, cwSFFileReadBuffer + cwSFBytesLeft, btr, &br);
        
        // Update the bytes left variable
        cwSFBytesLeft = CW_FS_FILE_READ_BUFFER_SIZE;
        
        // Out of data or error or user button... Stop playback!
        if (br < btr || res != FR_OK || TM_DISCO_ButtonPressed()) {
          StopAudio();
          
          // Re-initialize and set volume to avoid noise
          InitializeAudio(Audio44100HzSettings);
          SetAudioVolume(0);
          
          // Close currently open file
          f_close(&cwSFFile);
          
          // Wait for user button release
          while(TM_DISCO_ButtonPressed()){};
          
          // Return to previous function
          return;
        }
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
void AudioCallback(void *context, int buffer) {
  static int16_t audio_buffer0[4096];
  static int16_t audio_buffer1[4096];
  
  int offset, err;
  int outOfData = 0;
  
  int16_t *samples;
  if (buffer) {
    samples = audio_buffer0;
    GPIO_SetBits(GPIOD, GPIO_Pin_13);
    GPIO_ResetBits(GPIOD, GPIO_Pin_14);
  } else {
    samples = audio_buffer1;
    GPIO_SetBits(GPIOD, GPIO_Pin_14);
    GPIO_ResetBits(GPIOD, GPIO_Pin_13);
  }
  
  offset = MP3FindSyncWord((unsigned char*)cwSFReadPtr, cwSFBytesLeft);
  cwSFBytesLeft -= offset;
  cwSFReadPtr += offset;
  
  err = MP3Decode(hMP3Decoder, (unsigned char**)&cwSFReadPtr, (int*)&cwSFBytesLeft, samples, 0);
  
  if (err) {
    /* error occurred */
    switch (err) {
      case ERR_MP3_INDATA_UNDERFLOW:
        outOfData = 1;
        break;
      case ERR_MP3_MAINDATA_UNDERFLOW:
        /* do nothing - next call to decode will provide more mainData */
        break;
      case ERR_MP3_FREE_BITRATE_SYNC:
      default:
        outOfData = 1;
        break;
    }
  } else {
    // no error
    MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
    
    // Duplicate data in case of mono to maintain playback speed
    if (mp3FrameInfo.nChans == 1) {
      for(int i = mp3FrameInfo.outputSamps;i >= 0;i--) 	{
        samples[2 * i]=samples[i];
        samples[2 * i + 1]=samples[i];
      }
      mp3FrameInfo.outputSamps *= 2;
    }
  }
  
  if (!outOfData) {
    ProvideAudioBuffer(samples, mp3FrameInfo.outputSamps);
  }
}
