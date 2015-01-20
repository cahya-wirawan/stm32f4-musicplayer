//
//  cwSoundFile.h
//  
//
//  Created by Cahya Wirawan on 20/01/15.
//
//

#ifndef __CW_SOUND_FILE__
#define __CW_SOUND_FILE__

#include <stdio.h>
#include "Audio.h"
#include "ff.h"

#define   CW_FS_FILE_READ_BUFFER_SIZE 8192

const char *cwSFGetFilenameExt(const char *filename);
FRESULT cwSFPlayDirectory (const char* path, unsigned char seek);

#endif /* defined(__CW_SOUND_FILE__) */
