#ifndef CAPTHREAD_H
#define CAPTHREAD_H

#include <QThread>
#include <QImage>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <stdio.h>


#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

struct AVFrame;
struct AVPacket;
struct AVCodec;
struct AVCodecContext;


class CapThread : public QThread
{
    Q_OBJECT

public:
    explicit CapThread(int width, int height, QObject *parent = 0);
private:
    void run();
signals:
    void emitMsgBoxSignal(int);
    
public slots:
    void capFrame();
    //public var
public:
    enum
    {
        THREADRUNNING,
        THREADSTOP
    };
    
private:
    FILE* f;
    QTimer* timer;
    AVFrame *frame;
    AVPacket* pkt;
    AVCodec *codec;
    AVCodecContext *c;
    int i, ret, got_output;
    int resize_width, resize_height;
};

#endif // CAPTHREAD_H
