#include "faudio.h"
#include <QDebug>
void FAudio::stop()
{
    mutex.lock();
    if(output){
        output->stop();
        delete output;
        output=nullptr;
        io=nullptr;
    }
    mutex.unlock();
}

void FAudio::start()
{
    stop();
    mutex.lock();
    QAudioFormat fmt;//Qt音频格式
    fmt.setSampleRate(this->sampleRate);//1秒 采集48000个声音
    fmt.setSampleSize(this->sampleSize);//16位
    fmt.setChannelCount(this->channel);//双声道
    fmt.setCodec("audio/pcm");//编码格式
    fmt.setByteOrder(QAudioFormat::LittleEndian);//次序
    fmt.setSampleType(QAudioFormat::UnSignedInt);//样本的类别

    output = new QAudioOutput(fmt);
    io=output->start();
    mutex.unlock();
    qDebug()<<"audio start"<<output->error();
}

void FAudio::volume(qreal value)
{
    if(output){
        qDebug()<<"volume:"<<value;
        output->setVolume(value);
    }
}

void FAudio::play(bool isPlay)
{
    mutex.lock();
    if(!output)
    {
        mutex.unlock();
        return;
    }
    if(isPlay)
        output->resume();
    else
        output->suspend();

    mutex.unlock();
}

bool FAudio::write(const char *data, int dataSize)
{
    if(!data||dataSize<=0)
        return false;

    mutex.lock();
    if(io)
    {
        mutex.unlock();
        //qDebug()<<"write:"<<dataSize;
        io->write(data,dataSize);// //输入输出设备播放声音,将数据从零终止的8位字符字符串写入设备。
        return true;
    }
    mutex.unlock();
}

int FAudio::getFree()
{
    mutex.lock();
    if(!output)
    {
        mutex.unlock();
        return 0;
    }
    int free = output->bytesFree();

    mutex.unlock();
    //qDebug()<<"getFree:"<<free;
    return free;
}


