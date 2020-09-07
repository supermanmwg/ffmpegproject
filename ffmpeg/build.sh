#!/bin/sh

CONFIGURE_FLAGS="--disable-shared \
                --enable-static \
                --disable-stripping \
                --disable-ffmpeg \
                --disable-ffplay \
                --disable-ffprobe \
                --disable-avdevice \
                --disable-devices \
                --disable-indevs \
                --disable-outdevs \
                --disable-debug \
                --disable-asm \
                --disable-x86asm \
                --enable-small \
                --enable-dct \
                --enable-dwt \
                --enable-lsp \
                --enable-mdct \
                --enable-rdft \
                --enable-fft \
                --enable-version3 \
                --enable-nonfree \
                --disable-filters \
                --disable-postproc \
                --disable-bsfs \
                --enable-bsf=aac_adtstoasc \
                --enable-bsf=h264_mp4toannexb \
                --enable-bsf=hevc_mp4toannexb \
                --disable-encoders \
                --enable-encoder=pcm_s16le \
                --enable-encoder=aac \
                --enable-encoder=libvo_aacenc \
                --disable-decoders \
                --enable-decoder=aac \
                --enable-decoder=mp3 \
                --enable-decoder=pcm_s16le \
                --disable-parsers \
                --enable-parser=aac \
                --disable-muxers \
                --enable-muxer=flv \
                --enable-muxer=mov \
                --enable-muxer=wav \
                --enable-muxer=adts \
                --disable-demuxers \
                --enable-demuxer=mov \
                --enable-demuxer=flv \
                --enable-demuxer=wav \
                --enable-demuxer=aac \
                --disable-protocols \
                --enable-protocol=rtmp \
                --enable-protocol=file \
                --enable-libfdk_aac \
                --enable-libx264 \
                --enable-gpl \
                --enable-cross-compile "

ARCHS="armv7 arm64 x86_64"

#代码目录
SOURCE="ffmpeg"
FAT="ffmpeg-iOS"
SCRATCH="scratch"
#必须是绝对路径
THIN=`pwd`/"thin"

COMPILE="y"
LIPO="y"
ONLY_CONFIGURE=

if [ "$*" ] #必须有空格
then
    if [ "$*" = "lipo" ]
    then
        COMPILE=
    elif [ "$*" = "conf" ]
    then
        ONLY_CONFIGURE="y"
    else
        ARCHS="$*"
        if [ $# -eq 1 ]
        then
            LIPO=
        fi
    fi
fi

if [ "$COMPILE" ]
then
    CWD=`pwd`
    for ARCH in $ARCHS
    do
        echo "building $ARCH..."
        mkdir -p "$SCRATCH/$ARCH"
        cd "$SCRATCH/$ARCH"

        CFLAGS="-arch $ARCH"

        if [ "$ARCH" = "x86_64" ]
        then
            PLATFORM="iPhoneSimulator"
            HOST="--target-os=darwin"
            CFLAGS="$CFLAGS -mios-simulator-version-min=9.0"
        elif  [ "$ARCH" = "arm64" ]
        then
            PLATFORM="iPhoneOS"
            HOST="--target-os=darwin"
            CFLAGS="$CFLAGS -miphoneos-version-min=9.0"
        elif [ "$ARCH" = "armv7" ]
        then
            PLATFORM="iPhoneOS"
            HOST="--target-os=darwin"
            CFLAGS="$CFLAGS -miphoneos-version-min=9.0"
        else
            echo "unknow arch....."
            exit
        fi

        XCRUN_SDK=`echo $PLATFORM | tr '[:upper:]' '[:lower:]'`
        CC="xcrun -sdk $XCRUN_SDK clang -Wno-error=unused-command-line-argument-hard-error-in-future"
        EXTERNAL_INCLUDE="-I$CWD/external-libs/fdk-aac/include -I$CWD/external-libs/x264/include"
        EXTERNAL_LIB="-L$CWD/external-libs/fdk-aac/lib -L$CWD/external-libs/x264/lib"
        
        $CWD/$SOURCE/configure \
            $CONFIGURE_FLAGS \
            $HOST \
            --cc="$CC" \
            --extra-cflags="$CFLAGS $EXTERNAL_INCLUDE" \
            --extra-ldflags="$CFLAGS $EXTERNAL_LIB" \
            --prefix="$THIN/$ARCH"
        
        if [ "$ONLY_CONFIGURE" ]
        then
            exit
        fi
        make clean
        make -j8
        make install

        cd $CWD
    done
fi

if [ "$LIPO" ]
then
    echo "building fat binary..."
    mkdir -p $FAT/lib
    set - $ARCHS
    echo $*
    CWD=`pwd`
    cd $THIN/$1/lib
    for LIB in *.a
    do
        cd $CWD
        lipo -create `find $THIN -name $LIB` -output $FAT/lib/$LIB
    done
    
    cd $CWD
    cp -rf $THIN/$1/include $FAT

fi
