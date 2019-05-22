#ifndef FAUDIO_H
#define FAUDIO_H

#include <QtMultimedia/QAudioOutput>
#include <QMutex>

class FAudio{
public:
    static FAudio *getInstance(){
        static FAudio instance;
        return &instance;
    }

    virtual  ~FAudio(){}


    void stop();
    void start();
    void volume(qreal value);
    void play(bool isPlay);
    bool write(const char*data,int dataSize);

    int getFree();

    int sampleRate=48000;
    int sampleSize=16;
    int channel =2;

public:
    QAudioOutput *output=nullptr;
    QIODevice* io=nullptr;
    QMutex mutex;

private:
    FAudio(){}
    FAudio(const FAudio&){}
    FAudio &operator=(const FAudio&){}

};
#endif // FAUDIO_H
