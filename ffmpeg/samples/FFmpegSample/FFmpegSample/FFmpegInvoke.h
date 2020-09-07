//
//  FFmpegInvoke.h
//  FFmpegSample
//
//  Created by 维光孟 on 2020/9/5.
//  Copyright © 2020 维光孟. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface FFmpegInvoke : NSObject

+ (void)decodeVideoWithInputFilePath:(NSString *)inputFilePath outputFilePath:(NSString *)outputFilePath;

+ (void)decodeAudioWithInputFilePath:(NSString *)inputFilePath outputFilePath:(NSString *)outputFilePath;

@end

NS_ASSUME_NONNULL_END
