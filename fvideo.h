#ifndef FVIDEO_H
#define FVIDEO_H

#include <QQuickPaintedItem>
#include <QImage>

class FVideo: public QQuickPaintedItem{

    Q_OBJECT
public:
    explicit FVideo();
    ~FVideo();

    Q_PROPERTY(QString currentTime READ currentTime NOTIFY currentTimeChanged)
    Q_PROPERTY(QString totalTime READ totalTime NOTIFY totalTimeChanged)

    Q_INVOKABLE void start(QString );
    Q_INVOKABLE void stop();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void volume(qreal value);

protected:
    virtual void paint(QPainter *pPainter);
    virtual void timerEvent(QTimerEvent*e);

private:
    int timeId;
    QString curTime;
    QString totTime;

    QString currentTime() const;
    QString totalTime() const;

signals:
    void currentTimeChanged(QString);
    void totalTimeChanged(QString);
};
#endif // FVIDEO_H
