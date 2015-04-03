#ifndef CAPTHREAD_H
#define CAPTHREAD_H

#define __STDC_CONSTANT_MACROS

#define MAINWORKING
#ifdef MAINWORKING
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "SDL/SDL.h"
//#include "SDL/SDL_main.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <SDL/SDL.h>
#ifdef __cplusplus
};
#endif
#endif


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

public:
    void show_dshow_device();
    void show_avfoundation_device();
private:

    QTimer* timer;
    int resize_width, resize_height;


    AVFormatContext	*pFormatCtx; /*  */
    AVCodecContext	*pEc,*pDc; /* ±àÂë¡¢½âÂë */
    AVCodec			*pEcodec,*pDcodec;
    AVPacket *pkt;/* °ü */
    AVFrame	 *pEframe,*pDframe,*pDFrameYUV;/* ±àÂëÖ¡¡¢½âÂëÖ¡ */
    FILE* f;/* ÎÄ¼þ¾ä±ú */
    int	i,ret,got_output,got_picture, videoindex;/*  */


public:
    explicit CapThread(int width, int height, QObject *parent = 0);
    
private:
    void run();
signals:
    
public slots:
    void capFrame();
};

#endif // CAPTHREAD_H
