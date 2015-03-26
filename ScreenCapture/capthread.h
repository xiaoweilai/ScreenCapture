#ifndef CAPTHREAD_H
#define CAPTHREAD_H

#include <QThread>
#include <QImage>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>

#include <stdio.h>

struct AVFrame;
struct AVPacket;
struct AVCodec;
struct AVCodecContext;

class CapThread : public QThread
{
    Q_OBJECT

private:
    FILE* f;
    QTimer* timer;

    AVFrame *frame;
    AVPacket* pkt;
    AVCodec *codec;
    AVCodecContext *c;
    int i, ret, got_output;

    int resize_width, resize_height;

public:
    explicit CapThread(int width, int height, QObject *parent = 0);
    
private:
    void run();
    void updateMouseShape(const QPoint point, Qt::CursorShape shape);
signals:
    
public slots:
    void capFrame();
//    void setImagePixel(const QPoint &pos, bool opaque);
    
};

#endif // CAPTHREAD_H
