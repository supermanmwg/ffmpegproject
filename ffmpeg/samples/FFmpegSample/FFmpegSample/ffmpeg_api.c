//
//  ffmpeg_api.c
//  FFmpegSample
//
//  Created by 维光孟 on 2020/9/5.
//  Copyright © 2020 维光孟. All rights reserved.
//

#include "ffmpeg_api.h"
#include "platform_4_ffmpeg.h"

static int get_format_from_sample_fmt(const char **fmt,
                                      enum AVSampleFormat sample_fmt) {
    int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
        { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
        { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
        { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
        { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
        { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
        { AV_SAMPLE_FMT_FLTP, "f32be", "f32le"},
    };
    *fmt = NULL;

    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }

    fprintf(stderr,
            "sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
    return -1;
}

int interrupt_cb(void *ctx) {
    //printf("interrupt_cb is called\n");
    
    return 0;
}

int decode_audio(char *inputFilePath, char *outputFilePath) {
    AVFormatContext *pFormatCtx = NULL;
    int audioStreamIndex = -1;
    AVCodecParameters *audioPar = NULL;
    AVCodecContext *audioCodecCtx = NULL;
    AVCodec *audioCodec = NULL;
    
    AVPacket *packet = NULL;
    AVFrame *pFrame = NULL;
    
    FILE *fpPcm = NULL;
    
    int ret = 0;
    
    //注册协议，格式和编辑器
    avformat_network_init();
    
    pFormatCtx = avformat_alloc_context();
    int consecutively_stream_conut = 0;
    AVIOInterruptCB int_cb = {interrupt_cb, &consecutively_stream_conut};
    pFormatCtx->interrupt_callback = int_cb;
    if (avformat_open_input(&pFormatCtx, inputFilePath, NULL, NULL) != 0) {
        fprintf(stderr, "Couldn't open input stream.\n");
        return -1;
    }
    
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        fprintf(stderr, "Couldn't find stream info.\n");
        return -1;
    }
    
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        AVStream *stream = pFormatCtx->streams[i];
        if (AVMEDIA_TYPE_AUDIO == stream->codecpar->codec_type) {
            audioStreamIndex = i;
            audioPar = stream->codecpar;
        }
    }
    
    if (audioStreamIndex == -1) {
        fprintf(stderr, "could not find audio stream \n");
    } else {
        audioCodec = avcodec_find_decoder(audioPar->codec_id);
        audioCodecCtx = avcodec_alloc_context3(NULL);
        if (!audioCodecCtx) {
            fprintf(stderr, "alloc audio avcodec fail.");
            return -1;
        }
        
        if (avcodec_parameters_to_context(audioCodecCtx, audioPar) < 0) {
            fprintf(stderr, "audio avcodec_parameters_to_context fail.");
            return -1;
        }
        
        if (avcodec_open2(audioCodecCtx, audioCodec, NULL) < 0) {
            fprintf(stderr, "Could not open codec\n");
            return -1;
        }
    }
    
    printf("----Audio channel num is %d, sample rate is %d\n", audioCodecCtx->channels, audioCodecCtx->sample_rate);
    const char *fmt;
    if (get_format_from_sample_fmt(&fmt, audioCodecCtx->sample_fmt) == 0) {
        printf("----Audio format is %s\n", fmt);
    } else {
        fprintf(stderr, "Could not get format\n");
    }
    
    packet = av_packet_alloc();
    pFrame = av_frame_alloc();
    
    fpPcm = fopen(outputFilePath, "wb+");
    if (!fpPcm) {
        fprintf(stderr, "Cannot open output file.\n");
        return -1;
    }
    
    int dataSize = 0;
    int i = 0;
    int ch = 0;
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == audioStreamIndex) {
            ret = avcodec_send_packet(audioCodecCtx, packet);
            if (ret < 0) {
                printf("it occurs an error when video sending a packet for decoding.\n");
                break;
            }
            
            while (ret >= 0) {
                ret = avcodec_receive_frame(audioCodecCtx, pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if (ret < 0) {
                    fprintf(stderr, "Error during decoding\n");
                    break;
                }
                
                fflush(stdout);
                
                dataSize = av_get_bytes_per_sample(audioCodecCtx->sample_fmt);
                for (i = 0; i < pFrame->nb_samples; i++) {
                    for (ch = 0; ch < audioCodecCtx->channels; ch ++) {
                        fwrite(pFrame->data[ch] + dataSize * i, 1, dataSize, fpPcm);
                    }
                }
                
                printf("Frame Index: %5d.\n", audioCodecCtx->frame_number);
            }
        }
    }
    
    //flush decodec
    while (1) {
        ret = avcodec_send_packet(audioCodecCtx, packet);
        if (ret < 0) {
            printf("flush empty.\n");
            break;
        }
        
        while (ret >= 0) {
            ret = avcodec_receive_frame(audioCodecCtx, pFrame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            else if (ret < 0) {
                fprintf(stderr, "flush Error during decoding\n");
                break;
            }
            
            fflush(stdout);
            
            dataSize = av_get_bytes_per_sample(audioCodecCtx->sample_fmt);
            for (i = 0; i < pFrame->nb_samples; i++) {
                for (ch = 0; ch < audioCodecCtx->channels; ch ++) {
                    fwrite(pFrame->data[ch] + dataSize * i, 1, dataSize, fpPcm);
                }
            }
            
            printf("Frame Index: %5d.\n", audioCodecCtx->frame_number);
        }
    }
    
    
    fclose(fpPcm);
    av_packet_unref(packet);
    av_frame_free(&pFrame);

    return 0;
}

int decode_video(char *inputFilePath, char *outputFilePath) {
    AVFormatContext *pFormatCtx = NULL;
    int videoStreamIndex = -1;
    AVCodecParameters *videoPar = NULL;
    AVCodecContext *videoCodecCtx = NULL;
    AVCodec *videoCodec = NULL;
    
    AVPacket *packet = NULL;
    AVFrame *pFrame = NULL;
    AVFrame *pFrameYUV = NULL;
    
    FILE *fpYuv = NULL;
    
    struct SwsContext *imgConvertCtx;
    
    int consecutively_stream_conut = 0;
    
    int ySize = 0;

    //注册协议，格式和编解码器
    avformat_network_init();
    
    //打开媒体文件源，并设置超时回调
    pFormatCtx = avformat_alloc_context();
    AVIOInterruptCB int_cb = {interrupt_cb, &consecutively_stream_conut};
    pFormatCtx->interrupt_callback = int_cb;
    if (avformat_open_input(&pFormatCtx, inputFilePath, NULL, NULL) != 0){
        fprintf(stderr, "Couldn't open input stream.\n");
        return -1;
    }
    
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        fprintf(stderr, "Couldn't find stream info.\n");
        return -1;
    }
    
    //寻找各个流，并且打开对应的解码器
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        AVStream *stream = pFormatCtx->streams[i];
        if (AVMEDIA_TYPE_VIDEO == stream->codecpar->codec_type) {
            videoStreamIndex = i;
            videoPar = stream->codecpar;
        }
    }
    
    if (videoStreamIndex == -1) {
     } else {
        videoCodec = avcodec_find_decoder(videoPar->codec_id);
        videoCodecCtx = avcodec_alloc_context3(NULL);
        if (!videoCodecCtx) {
            fprintf(stderr, "alloc video avcodec fail.");
            return -1;
        }
        
        if (avcodec_parameters_to_context(videoCodecCtx, videoPar) < 0) {
            fprintf(stderr, "video avcodec_parameters_to_context fail.");
            return -1;
        }
        
        if (avcodec_open2(videoCodecCtx, videoCodec, NULL) < 0) {
            fprintf(stderr, "Could not open codec\n");
            return -1;
        }
    }

    packet = av_packet_alloc();
    pFrame = av_frame_alloc();
    pFrameYUV =  av_frame_alloc();
    uint8_t * out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  videoCodecCtx->width, videoCodecCtx->height, 1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
                         AV_PIX_FMT_YUV420P,videoCodecCtx->width, videoCodecCtx->height,1);
    imgConvertCtx = sws_getContext(videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt,
                                     videoCodecCtx->width, videoCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    fpYuv = fopen(outputFilePath, "wb+");
    if(!fpYuv){
        fprintf(stderr, "Cannot open output file.\n");
        return -1;
    }

    while(av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == videoStreamIndex) {
            int ret = avcodec_send_packet(videoCodecCtx, packet);
            if (ret < 0) {
                printf("it occurs an error when video sending a packet for decoding.\n");
                break;
            }
            
            while (ret >= 0) {
                ret = avcodec_receive_frame(videoCodecCtx, pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if (ret < 0) {
                    fprintf(stderr, "Error during decoding\n");
                    break;
                }
                
                fflush(stdout);
                
                sws_scale(imgConvertCtx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, videoCodecCtx->height,
                          pFrameYUV->data, pFrameYUV->linesize);
                
                ySize = videoCodecCtx->width * videoCodecCtx->height;
                fwrite(pFrameYUV->data[0], 1, ySize, fpYuv);    //Y
                fwrite(pFrameYUV->data[1], 1, ySize/4,fpYuv);  //U
                fwrite(pFrameYUV->data[2], 1, ySize/4,fpYuv);  //V

                char pictype_str[10]={0};
                switch(pFrame->pict_type){
                    case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
                    case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
                    case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
                    default:sprintf(pictype_str,"Other");break;
                }

                printf("Frame Index: %5d. Type:%s\n",videoCodecCtx->frame_number,pictype_str);
            }
        }
    }
    
    // flush decodec
    while (1) {
        int ret = avcodec_send_packet(videoCodecCtx, packet);
        if (ret < 0) {
            printf("end decoding.\n");
            break;
        }
        
        while (ret >= 0) {
            ret = avcodec_receive_frame(videoCodecCtx, pFrame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                break;
            else if (ret < 0) {
                fprintf(stderr, "Error during decoding\n");
                break;
            }
            
            fflush(stdout);
            
            sws_scale(imgConvertCtx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, videoCodecCtx->height,
                      pFrameYUV->data, pFrameYUV->linesize);
            
            ySize = videoCodecCtx->width * videoCodecCtx->height;
            fwrite(pFrameYUV->data[0], 1, ySize, fpYuv);    //Y
            fwrite(pFrameYUV->data[1], 1, ySize/4,fpYuv);  //U
            fwrite(pFrameYUV->data[2], 1, ySize/4,fpYuv);  //V
            
            char pictype_str[10]={0};
            switch(pFrame->pict_type){
                case AV_PICTURE_TYPE_I:sprintf(pictype_str,"I");break;
                case AV_PICTURE_TYPE_P:sprintf(pictype_str,"P");break;
                case AV_PICTURE_TYPE_B:sprintf(pictype_str,"B");break;
                default:sprintf(pictype_str,"Other");break;
            }
            
            printf("Frame Index: %5d. Type:%s\n",videoCodecCtx->frame_number,pictype_str);
        }
    }

    fclose(fpYuv);
    av_packet_unref(packet);
    avcodec_free_context(&videoCodecCtx);
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    
    return 0;
}
