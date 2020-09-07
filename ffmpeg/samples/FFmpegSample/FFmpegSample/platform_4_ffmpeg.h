//
//  platform_4_ffmpeg.h
//  FFmpegSample
//
//  Created by 维光孟 on 2020/9/5.
//  Copyright © 2020 维光孟. All rights reserved.
//

#ifndef platform_4_ffmpeg_h
#define platform_4_ffmpeg_h

#ifdef __ANDROID__
    extern "C" {
        #include "3rdparty/ffmpeg/include/libavformat/avformat.h"
        #include "3rdparty/ffmpeg/include/libswscale/swscale.h"
        #include "3rdparty/ffmpeg/include/libswresample/swresample.h"
        #include "3rdparty/ffmpeg/include/libavutil/pixdesc.h"
        #include "3rdparty/ffmpeg/include/libavutil/imgutils.h"
    }
#elif defined(__APPLE__) // iOS或OS X
    #ifdef __cplusplus
        extern "C" {
            #include "libavformat/avformat.h"
            #include "libswscale/swscale.h"
            #include "libswresample/swresample.h"
            #include "libavutil/pixdesc.h"
            #include "libavutil/imgutils.h"
        }
    #else
        #include "libavformat/avformat.h"
        #include "libswscale/swscale.h"
        #include "libswresample/swresample.h"
        #include "libavutil/pixdesc.h"
        #include "libavutil/imgutils.h"
    #endif
#endif
  

#endif /* platform_4_ffmpeg_h */
