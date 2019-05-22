#ifndef FFMPEG_H
#define FFMPEG_H

#include <QMutex>

extern "C"
{
#include <libavformat/avformat.h>
#include<libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class FFMpeg{
public:
    static FFMpeg* getInstance()
    {
        static FFMpeg ff;
        return &ff;
    }

    virtual ~FFMpeg();
    int openVideo(const char* path);
    int decodeFrame(const AVPacket*pkt);
    AVPacket readFrame();
    bool yuvToRGB(char *out,int outWidth,int outHeight);
    int toPCM(char *out);
    void closeForeVideo();

    bool seek(float pos);
    int getPts(const AVPacket* pkt);

public:
    bool mIsPlay;
    AVFrame *mYUV=nullptr;//视频帧数据
    AVFrame *mPCM=nullptr;//音频数据
    SwsContext *mcCtx=nullptr;//转换器

    int mSampleRate=48000;
    int mSampleSize=16;
    int mChannel=2;

    int mVideoStream=0;
    int mAudioStream=1;

    int mPts;//当前视频播放进度
    int mTotalMs;

protected:
    FFMpeg();

    AVFormatContext *mAfc=nullptr;

    QMutex mtx;
    SwrContext *maCtx=nullptr;//音频
};
#endif // FFMPEG_H
