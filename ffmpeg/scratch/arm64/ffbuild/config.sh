# Automatically generated by configure - do not modify!
shared=no
build_suffix=
prefix=/Users/weiguangmeng/ffmpegworkspace/sourceCode/ffmpeg/thin/arm64
libdir=${prefix}/lib
incdir=${prefix}/include
rpath=
source_path=src
LIBPREF=lib
LIBSUF=.a
extralibs_avutil="-pthread -lm -framework VideoToolbox -framework CoreFoundation -framework CoreMedia -framework CoreVideo"
extralibs_avcodec="-pthread -liconv -lm -framework VideoToolbox -framework CoreFoundation -framework CoreMedia -framework CoreVideo"
extralibs_avformat="-lm -lz"
extralibs_avdevice="-lm"
extralibs_avfilter="-pthread -lm"
extralibs_avresample="-lm"
extralibs_postproc="-lm"
extralibs_swscale="-lm"
extralibs_swresample="-lm"
avdevice_deps="avformat avcodec avutil"
avfilter_deps="avutil"
swscale_deps="avutil"
postproc_deps="avutil"
avformat_deps="avcodec avutil"
avcodec_deps="avutil"
swresample_deps="avutil"
avresample_deps="avutil"
avutil_deps=""
