//
//  ffmpeg_api.h
//  FFmpegSample
//
//  Created by 维光孟 on 2020/9/5.
//  Copyright © 2020 维光孟. All rights reserved.
//

#ifndef ffmpeg_api_h
#define ffmpeg_api_h

#include <stdio.h>

int testffmpeg(char * filepath);

int decode_video(char *inputFilePath, char *outputFilePath);

int decode_audio(char *inputFilePath, char *outputFilePath);

#endif /* ffmpeg_api_h */
