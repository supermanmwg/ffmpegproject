//
//  FFmpegInvoke.m
//  FFmpegSample
//
//  Created by 维光孟 on 2020/9/5.
//  Copyright © 2020 维光孟. All rights reserved.
//

#import "FFmpegInvoke.h"
#include "ffmpeg_api.h"

@implementation FFmpegInvoke

+ (void)decodeVideoWithInputFilePath:(NSString *)inputFilePath outputFilePath:(NSString *)outputFilePath {
    char *charInputFilePath = [inputFilePath cStringUsingEncoding:NSASCIIStringEncoding];
    char *charOutputFilePath = [outputFilePath cStringUsingEncoding:NSASCIIStringEncoding];
    decode_video(charInputFilePath, charOutputFilePath);
}

+ (void)decodeAudioWithInputFilePath:(NSString *)inputFilePath outputFilePath:(NSString *)outputFilePath {
    char *charInputFilePath = [inputFilePath cStringUsingEncoding:NSASCIIStringEncoding];
    const char *charOutputFilePath = [outputFilePath cStringUsingEncoding:NSASCIIStringEncoding];
    decode_audio(charInputFilePath, charOutputFilePath);
}
@end
