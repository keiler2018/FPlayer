#include "fvideo.h"
#include "playthread.h"
#include "ffmpeg.h"
#include "faudio.h"
#include <iostream>
#include <list>
#include <QPainter>
using namespace std;

FVideo::FVideo():timeId(0)
{
}

FVideo::~FVideo()
{
    stop();
}

void FVideo::start(QString fileName)
{
    fileName.replace("file:///","");
    FFMpeg::getInstance()->openVideo(fileName.toStdString().c_str());

    //播放总时间
    int sec =  FFMpeg::getInstance()->mTotalMs/1000;//秒
    int min = sec/60;//分
    int remainSec = sec-min*60;
    totTime= QString::number(min)+":"+QString::number(remainSec);
    qDebug()<<"start"<<min<<remainSec<<totTime;
    emit totalTimeChanged(totTime);
    //

    FAudio::getInstance()->sampleRate = FFMpeg::getInstance()->mSampleRate;
    FAudio::getInstance()->channel = FFMpeg::getInstance()->mChannel;
    FAudio::getInstance()->sampleSize = 16;
    FAudio::getInstance()->start();  //播放音频
    FFMpeg::getInstance()->mIsPlay=true;

    startTimer(20);//刷新频率
    PlayThread::getInstance()->myStart();
}

void FVideo::stop()
{
    if(timeId!=0)
        killTimer(timeId);
    PlayThread::getInstance()->myStop();
    FAudio::getInstance()->stop();

}

void FVideo::pause()
{
    FFMpeg::getInstance()->mIsPlay=false;
}

void FVideo::volume(qreal value)
{
    FAudio::getInstance()->volume(value);
}

void FVideo::paint(QPainter *pPainter)
{
    static QImage *image = NULL;

    static int w = 0; //记录之前的窗口大小
    static int h = 0;
    if (w != width() || h != height())
    {
        //如果窗口大小发生变化，删除内存
        if (image)
        {
            delete image->bits();  //删除QImage的内部空间
            delete image;
            image = NULL;
        }
    }

    if (image == NULL)
    {
        uchar *buf = new uchar[width() * height() * 4];
        image = new QImage(buf, width(), height(), QImage::Format_ARGB32);
    }

    FFMpeg::getInstance()->yuvToRGB((char *)(image->bits()), width(), height());
    pPainter->drawImage(QPoint(0, 0), *image);
}

void FVideo::timerEvent(QTimerEvent *e)
{
    timeId = e->timerId();
    this->update();

    int currMin = (FFMpeg::getInstance()->mPts/1000)/60;
    int currSec = (FFMpeg::getInstance()->mPts/1000)%60;
    curTime=QString::number(currMin)+":"+QString::number(currSec);
    emit currentTimeChanged(curTime);
}


QString FVideo::currentTime() const
{
   return curTime;
}

QString FVideo::totalTime() const
{
    return totTime;
}
