#include "ffmpeg.h"
#include "faudio.h"
#include <iostream>
#include <QDebug>

using namespace std;
static double r2d(AVRational r)
{
    return r.num == 0 || r.den == 0 ? 0. : (double)r.num / (double)r.den;
}

FFMpeg::FFMpeg()
{
    av_register_all();//ffmpeg 编程第一句 注册
    mIsPlay=false;
}

FFMpeg::~FFMpeg()
{

}

int FFMpeg::openVideo(const char *path)
{
    qDebug()<<"openVideo:"<<path;
    closeForeVideo();
    mtx.lock();
    int nRet = avformat_open_input(&mAfc,path,0,0);
    if(nRet!=0){
        mtx.unlock();
        char buf[1024]={0};
        av_strerror(nRet,buf,sizeof(buf));
        qDebug()<<"av_strerror:"<<buf<<nRet;
        return 0;
    }

    //视频时间，毫秒
    mTotalMs= (mAfc->duration/AV_TIME_BASE)*1000;
    qDebug()<<"mTotalMs:"<<mTotalMs;

    //打开解码器
    for(int i=0;i<mAfc->nb_streams;i++){//nb_streams打开的视频文件中流的数量，一般nb_streams = 2，音频流和视频流
        AVCodecContext *acc = mAfc->streams[i]->codec;//分别获取音频流和视频流的解码器
        if(acc->codec_type==AVMEDIA_TYPE_VIDEO)//如果是视频
        {
            mVideoStream=i;
            AVCodec *codec=avcodec_find_decoder(acc->codec_id);//解码器
            //找不到 解码器
            if(!codec){
                mtx.unlock();
                qDebug()<<"找不到视频编码器";
                return 0;
            }
            //解码器打开失败
            int err = avcodec_open2(acc,codec,NULL);
            if(err!=0){
                mtx.unlock();
                char buf[1024]={0};
                av_strerror(err,buf,sizeof(buf));
                qDebug()<<"视频编码器打开失败";
                return 0;
            }
        }else if(acc->codec_type==AVMEDIA_TYPE_AUDIO)//如果是 音频
        {
            mAudioStream =i;
            AVCodec *codec = avcodec_find_decoder(acc->codec_id);
            int aRet = avcodec_open2(acc,codec,NULL);
            if(aRet<0){
                mtx.unlock();
                qDebug()<<"音频编码器打开失败";
                return 0;
            }
            //设置音频参数
            this->mSampleRate = acc->sample_rate;//采样率
            this->mChannel = acc->channels;//通道数
            switch (acc->sample_fmt) {
            case AV_SAMPLE_FMT_S16:
                this->mSampleSize=16;
                break;
            case AV_SAMPLE_FMT_S32:
                this->mSampleSize=32;
                break;
            default:
                break;
            }
            qDebug()<<"采样率，通道数："<<acc->sample_rate<<acc->channels<<acc->sample_fmt;
        }
    }
    mtx.unlock();
    return mTotalMs;
}

void FFMpeg::closeForeVideo()
{
    mtx.lock();
    if(mAfc)
        avformat_close_input(&mAfc);

    if(mYUV)
        av_frame_free(&mYUV);

    if(mcCtx){
        sws_freeContext(mcCtx);
        mcCtx=nullptr;
    }

    if(maCtx)
        swr_free(&maCtx);

    mtx.unlock();
}

bool FFMpeg::seek(float pos)
{
    mtx.lock();

    if (!mAfc)
    {
        mtx.unlock();
        return false;
    }

    int64_t stamp = 0;
    stamp = pos * mAfc->streams[mVideoStream]->duration;
    int re = av_seek_frame(mAfc, mVideoStream, stamp, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME); //向后|关键帧

    //清除之前的解码缓冲
    avcodec_flush_buffers(mAfc->streams[mVideoStream]->codec);
    mtx.unlock();
    if (re >= 0)
    {
        return true;
    }
    //关键帧，P帧， B帧
    return false;
}

int FFMpeg::getPts(const AVPacket *pkt)
{
    mtx.lock();
    if (!mAfc)
    {
        mtx.unlock();
        return -1;
    }
    int pts = (pkt->pts * r2d(mAfc->streams[pkt->stream_index]->time_base)) * 1000; //毫秒数
    mtx.unlock();
    return pts;
}

int FFMpeg::decodeFrame(const AVPacket *pkt)
{
    mtx.lock();
    if(!mAfc){
        mtx.unlock();
        return 0;
    }

    if(mYUV==nullptr)
        mYUV=av_frame_alloc();

    if(mPCM==nullptr)
        mPCM=av_frame_alloc();

    AVFrame *frame = mYUV;//解码后 YUV 会改变
    if(pkt->stream_index==mAudioStream)
        frame=mPCM;

    //根据索引 stream_index 来判断是音频还是视频
    int re = avcodec_send_packet(mAfc->streams[pkt->stream_index]->codec,pkt);
    if(re!=0){
        mtx.unlock();
        return 0;
    }

    re = avcodec_receive_frame(mAfc->streams[pkt->stream_index]->codec,frame);
    if(re!=0)
    {
        //失败
        mtx.unlock();
        return 0;
    }
    mtx.unlock();

    int p=(frame->pts*r2d(mAfc->streams[pkt->stream_index]->time_base))*1000;
    if(pkt->stream_index==mAudioStream)
        this->mPts=p;

    return mPts;
}

AVPacket FFMpeg::readFrame()
{
    AVPacket pkt;
    memset(&pkt, 0, sizeof(AVPacket));

    mtx.lock();
    if (!mAfc)
    {
        mtx.unlock();
        return pkt;
    }

    int err = av_read_frame(mAfc, &pkt);
    if (err != 0)
    {
        //失败
    }
    mtx.unlock();

   // qDebug()<<"readFrame:"<<pkt.pts;
    return  pkt;
}

bool FFMpeg::yuvToRGB(char *out, int outWidth, int outHeight)
{
    mtx.lock();
    if (!mAfc || !mYUV) //像素转换的前提是视频已经打开
    {
        mtx.unlock();
        return false;
    }

    AVCodecContext *videoCtx = mAfc->streams[this->mVideoStream]->codec;
    mcCtx = sws_getCachedContext(mcCtx, videoCtx->width, videoCtx->height,
                                 videoCtx->pix_fmt,  //像素点的格式
                                 outWidth, outHeight,  //目标宽度与高度
                                 AV_PIX_FMT_BGRA,  //输出的格式
                                 SWS_BICUBIC,  //算法标记
                                 NULL, NULL, NULL
                                 );

    if (mcCtx)
    {
        //sws_getCachedContext 成功"
    }
    else
    {
        mtx.unlock();
        //"sws_getCachedContext 失败"
        return false;
    }

    uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };

    data[0] = (uint8_t *)out;  //指针传值，形参的值会被改变，out的值一直在变，所以QImage每次的画面都不一样，画面就这样显示出来了，这应该是整个开发过程最难的点
    int linesize[AV_NUM_DATA_POINTERS] = { 0 };
    linesize[0] = outWidth * 4;  //每一行转码的宽度

    //返回转码后的高度
    int h = sws_scale(mcCtx, mYUV->data, mYUV->linesize, 0, videoCtx->height,
                      data,
                      linesize
                      );

    //return true;
    //已返回，何来解锁，永远死锁。
    //曾经画面一直无法播放，无法找到原因。一句代码，影响整个系统，越简单的错误，越容易忽视。
    mtx.unlock();
}

int FFMpeg::toPCM(char *out)
{
    mtx.lock();
    if (!mAfc || !mPCM || !out)
    {
        mtx.unlock();
        return 0;
    }

    AVCodecContext *ctx = mAfc->streams[mAudioStream]->codec;
    if (maCtx == NULL)
    {
        maCtx = swr_alloc();
        swr_alloc_set_opts(maCtx, ctx->channel_layout,
                           AV_SAMPLE_FMT_S16,
                           ctx->sample_rate,
                           ctx->channels,
                           ctx->sample_fmt,
                           ctx->sample_rate,
                           0, 0);

        swr_init(maCtx);
    }

    uint8_t *data[1];
    data[0] = (uint8_t *)out;
    int len = swr_convert(maCtx, data, 10000, (const uint8_t **)mPCM->data, mPCM->nb_samples);
    if (len <= 0)
    {
        mtx.unlock();
        return 0;
    }

    int outsize = av_samples_get_buffer_size(NULL, ctx->channels, mPCM->nb_samples, AV_SAMPLE_FMT_S16, 0);
    mtx.unlock();

    return outsize;
}



