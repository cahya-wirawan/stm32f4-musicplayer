//
//  cwMP3.h
// 
//
//

#ifndef __CW_MP3__
#define __CW_MP3__

#include <stdio.h>
#include <stdint.h>
#include "Audio.h"

#include "usb_hcd_int.h"
#include "usbh_usr.h"
#include "usbh_core.h"
#include "usbh_msc_core.h"

void AudioCallback(void *context,int buffer);
uint32_t Mp3ReadId3V2Tag(FIL* pInFile, char* pszArtist,
                                uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize);

void cwMP3PlayFile(char* filename);

#endif /* defined(__CW_MP3__) */
