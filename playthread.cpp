#include "PlayThread.h"
#include "ffmpeg.h"
#include <list>
#include "FAudio.h"

using namespace std;

static list<AVPacket> g_videos; //存放视频帧

PlayThread::PlayThread():isStart(true)
{
}

PlayThread::~PlayThread()
{
}

void PlayThread::run()
{
    char out[10000] = { 0 };
    //在子线程里做什么，当然是读视频帧，解码视频了
    //何时读，何时解码呢，在视频打开之后, 读帧解码线程要一直运行
    //视频没打开之前线程要阻塞, run,while(1)这是基本套路
    while (isStart)
    {
        if (!FFMpeg::getInstance()->mIsPlay)
        {
            msleep(10); //调试方便，5微秒后窗口又关闭了，线程继续阻塞，此时可以点击【打开视频按钮】选择视频
            continue;
        }

        while (g_videos.size() > 0)
        {
            AVPacket pack = g_videos.front();

            int pts = FFMpeg::getInstance()->getPts(&pack);

            FFMpeg::getInstance()->decodeFrame(&pack);
            av_packet_unref(&pack);
            g_videos.pop_front(); //解码完成的帧从前面弹出list
        }

        int free = FAudio::getInstance()->getFree();
        if (free < 10000)
        {
            msleep(1);
            continue;
        }

        AVPacket pkt =FFMpeg::getInstance()->readFrame();

        if (pkt.size <= 0)
        {
            msleep(10);
        }

        if (pkt.stream_index == FFMpeg::getInstance()->mAudioStream)
        {
            FFMpeg::getInstance()->decodeFrame(&pkt);
            av_packet_unref(&pkt);
            int len =  FFMpeg::getInstance()->toPCM(out); //转换音频
            FAudio::getInstance()->write(out, len);       //播放音频
            continue;
        }

        if (pkt.stream_index != FFMpeg::getInstance()->mVideoStream)
        {
            av_packet_unref(&pkt);
            continue;
        }

        g_videos.push_back(pkt);
    }
}

void PlayThread::myStart()
{
    isStart=true;
    start();
}

void PlayThread::myStop()
{
    isStart=false;
    exit();
}
