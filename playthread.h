#ifndef PLAYTHREAD_H
#define PLAYTHREAD_H
#include <QThread>

class PlayThread : public QThread
{
    Q_OBJECT

public:
    static PlayThread* getInstance()
    {
        static PlayThread pt;
        return &pt;
    }

    ~PlayThread();

    void run();


    void myStart();
    void myStop();

protected:
    PlayThread();

private:
    bool isStart;
};


#endif // PLAYTHREAD_H
