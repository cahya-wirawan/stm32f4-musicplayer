//
//  cwSoundFile.c
//  
//
//  Created by Cahya Wirawan on 20/01/15.
//
//

#include "string.h"
#include "cwSoundFile.h"
#include "cwMP3.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_usb_msc_host.h"

FIL file;
char file_read_buffer[FILE_READ_BUFFER_SIZE];
volatile int bytes_left;
char *read_ptr;

const char *cwSFGetFilenameExt(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}

FRESULT cwSFPlayDirectory (const char* path, unsigned char seek) {
  FATFS USB_Fs;
  FRESULT res;
  FILINFO fno;
  DIR dir;
  char *fn; /* This function is assuming non-Unicode cfg. */
  char buffer[200];
#if _USE_LFN
  static char lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif
  
  if (TM_USB_MSCHOST_Device() != TM_USB_MSCHOST_Result_Connected)
    return 0;
  if (f_mount(&USB_Fs, "1:", 1) != FR_OK)
    return 0;
  
  res = f_opendir(&dir, path); /* Open the directory */
  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &fno); /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break; /* Break on error or end of dir */
      if (fno.fname[0] == '.') continue; /* Ignore dot entry */
#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif
      if (fno.fattrib & AM_DIR) { /* It is a directory */
        
      } else { /* It is a file. */
        sprintf(buffer, "%s/%s", path, fn);
        
        // Check if it is an mp3 file
        if ((strcmp("mp3", cwSFGetFilenameExt(buffer)) == 0) || (strcmp("MP3", cwSFGetFilenameExt(buffer)) == 0)) {
          // Skip "seek" number of mp3 files...
          if (seek) {
            seek--;
            continue;
          }
          
          cwMP3PlayFile(buffer);
        }
      }
    }
  }
  
  return res;
}
