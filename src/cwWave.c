//
//  cwMP3.c
//  

#include "string.h"
#include "cwMP3.h"
#include "mp3dec.h"
#include "Audio.h"
#include "cwSoundFile.h"
#include "tm_stm32f4_disco.h"

#define f_tell(fp)		((fp)->fptr)
//#define BUTTON			(GPIOA->IDR & GPIO_Pin_0)

// MP3 Variables
MP3FrameInfo			mp3FrameInfo;
HMP3Decoder				hMP3Decoder;

extern FIL file;
extern char file_read_buffer[FILE_READ_BUFFER_SIZE];
extern volatile int bytes_left;
extern char *read_ptr;

void cwMP3PlayFile(char* filename) {
  unsigned int br, btr;
  FRESULT res;
  
  bytes_left = FILE_READ_BUFFER_SIZE;
  read_ptr = file_read_buffer;
  
  if (FR_OK == f_open(&file, filename, FA_OPEN_EXISTING | FA_READ)) {

    // Read ID3v2 Tag
    char szArtist[120];
    char szTitle[120];
    Mp3ReadId3V2Tag(&file, szArtist, sizeof(szArtist), szTitle, sizeof(szTitle));
    
    // Fill buffer
    res = f_read(&file, file_read_buffer, FILE_READ_BUFFER_SIZE, &br);
    
    // Play mp3
    hMP3Decoder = MP3InitDecoder();
    InitializeAudio(Audio44100HzSettings);
    SetAudioVolume(0xAF);
    PlayAudioWithCallback(AudioCallback, 0);
    
    for(;;) {
      /*
       * If past half of buffer, refill...
       *
       * When bytes_left changes, the audio callback has just been executed. This
       * means that there should be enough time to copy the end of the buffer
       * to the beginning and update the pointer before the next audio callback.
       * Getting audio callbacks while the next part of the file is read from the
       * file system should not cause problems.
       */
      if (bytes_left < (FILE_READ_BUFFER_SIZE / 2)) {
        // Copy rest of data to beginning of read buffer
        memcpy(file_read_buffer, read_ptr, bytes_left);
        
        // Update read pointer for audio sampling
        read_ptr = file_read_buffer;
        
        // Read next part of file
        btr = FILE_READ_BUFFER_SIZE - bytes_left;
        res = f_read(&file, file_read_buffer + bytes_left, btr, &br);
        
        // Update the bytes left variable
        bytes_left = FILE_READ_BUFFER_SIZE;
        
        // Out of data or error or user button... Stop playback!
        if (br < btr || res != FR_OK || TM_DISCO_ButtonPressed()) {
          StopAudio();
          
          // Re-initialize and set volume to avoid noise
          InitializeAudio(Audio44100HzSettings);
          SetAudioVolume(0);
          
          // Close currently open file
          f_close(&file);
          
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
  
  offset = MP3FindSyncWord((unsigned char*)read_ptr, bytes_left);
  bytes_left -= offset;
  read_ptr += offset;
  
  err = MP3Decode(hMP3Decoder, (unsigned char**)&read_ptr, (int*)&bytes_left, samples, 0);
  
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

/*
 * Taken from
 * http://www.mikrocontroller.net/topic/252319
 */
uint32_t Mp3ReadId3V2Text(FIL* pInFile, uint32_t unDataLen, char* pszBuffer, uint32_t unBufferSize)
{
  UINT unRead = 0;
  BYTE byEncoding = 0;
  if((f_read(pInFile, &byEncoding, 1, &unRead) == FR_OK) && (unRead == 1))
  {
    unDataLen--;
    if(unDataLen <= (unBufferSize - 1))
    {
      if((f_read(pInFile, pszBuffer, unDataLen, &unRead) == FR_OK) ||
         (unRead == unDataLen))
      {
        if(byEncoding == 0)
        {
          // ISO-8859-1 multibyte
          // just add a terminating zero
          pszBuffer[unDataLen] = 0;
        }
        else if(byEncoding == 1)
        {
          // UTF16LE unicode
          uint32_t r = 0;
          uint32_t w = 0;
          if((unDataLen > 2) && (pszBuffer[0] == 0xFF) && (pszBuffer[1] == 0xFE))
          {
            // ignore BOM, assume LE
            r = 2;
          }
          for(; r < unDataLen; r += 2, w += 1)
          {
            // should be acceptable for 7 bit ascii
            pszBuffer[w] = pszBuffer[r];
          }
          pszBuffer[w] = 0;
        }
      }
      else
      {
        return 1;
      }
    }
    else
    {
      // we won't read a partial text
      if(f_lseek(pInFile, f_tell(pInFile) + unDataLen) != FR_OK)
      {
        return 1;
      }
    }
  }
  else
  {
    return 1;
  }
  return 0;
}

/*
 * Taken from
 * http://www.mikrocontroller.net/topic/252319
 */
uint32_t Mp3ReadId3V2Tag(FIL* pInFile, char* pszArtist, uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize)
{
  pszArtist[0] = 0;
  pszTitle[0] = 0;
  
  BYTE id3hd[10];
  UINT unRead = 0;
  if((f_read(pInFile, id3hd, 10, &unRead) != FR_OK) || (unRead != 10))
  {
    return 1;
  }
  else
  {
    uint32_t unSkip = 0;
    if((unRead == 10) &&
       (id3hd[0] == 'I') &&
       (id3hd[1] == 'D') &&
       (id3hd[2] == '3'))
    {
      unSkip += 10;
      unSkip = ((id3hd[6] & 0x7f) << 21) | ((id3hd[7] & 0x7f) << 14) | ((id3hd[8] & 0x7f) << 7) | (id3hd[9] & 0x7f);
      
      // try to get some information from the tag
      // skip the extended header, if present
      uint8_t unVersion = id3hd[3];
      if(id3hd[5] & 0x40)
      {
        BYTE exhd[4];
        f_read(pInFile, exhd, 4, &unRead);
        size_t unExHdrSkip = ((exhd[0] & 0x7f) << 21) | ((exhd[1] & 0x7f) << 14) | ((exhd[2] & 0x7f) << 7) | (exhd[3] & 0x7f);
        unExHdrSkip -= 4;
        if(f_lseek(pInFile, f_tell(pInFile) + unExHdrSkip) != FR_OK)
        {
          return 1;
        }
      }
      uint32_t nFramesToRead = 2;
      while(nFramesToRead > 0)
      {
        char frhd[10];
        if((f_read(pInFile, frhd, 10, &unRead) != FR_OK) || (unRead != 10))
        {
          return 1;
        }
        if((frhd[0] == 0) || (strncmp(frhd, "3DI", 3) == 0))
        {
          break;
        }
        char szFrameId[5] = {0, 0, 0, 0, 0};
        memcpy(szFrameId, frhd, 4);
        uint32_t unFrameSize = 0;
        uint32_t i = 0;
        for(; i < 4; i++)
        {
          if(unVersion == 3)
          {
            // ID3v2.3
            unFrameSize <<= 8;
            unFrameSize += frhd[i + 4];
          }
          if(unVersion == 4)
          {
            // ID3v2.4
            unFrameSize <<= 7;
            unFrameSize += frhd[i + 4] & 0x7F;
          }
        }
        
        if(strcmp(szFrameId, "TPE1") == 0)
        {
          // artist
          if(Mp3ReadId3V2Text(pInFile, unFrameSize, pszArtist, unArtistSize) != 0)
          {
            break;
          }
          nFramesToRead--;
        }
        else if(strcmp(szFrameId, "TIT2") == 0)
        {
          // title
          if(Mp3ReadId3V2Text(pInFile, unFrameSize, pszTitle, unTitleSize) != 0)
          {
            break;
          }
          nFramesToRead--;
        }
        else
        {
          if(f_lseek(pInFile, f_tell(pInFile) + unFrameSize) != FR_OK)
          {
            return 1;
          }
        }
      }
    }
    if(f_lseek(pInFile, unSkip) != FR_OK)
    {
      return 1;
    }
  }
  
  return 0;
}

