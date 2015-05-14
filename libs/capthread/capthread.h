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
#include "capthread_global.h"

class CAPTHREADSHARED_EXPORT Capthread : public QThread
{
    Q_OBJECT
public:
    Capthread();

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
signals:

public slots:
    void capFrame();
};

#endif // CAPTHREAD_H
