#ifndef CAPTHREAD_H
#define CAPTHREAD_H

#define MAINWORKING
#ifdef MAINWORKING

extern "C"{
#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
 # include <stdint.h>
#endif
}

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
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QHostAddress>
#include <stdio.h>

struct AVFrame;
struct AVPacket;
struct AVCodec;
struct AVCodecContext;

enum
{
    STAT_THREAD_RUNNING,
    STAT_THREAD_STOPED
};


enum
{
    RET_SUCESS,
    RET_FAIL,
    RET_UNKNOWN
};

enum
{
    STAT_STARTED,
    STAT_STOPPED,
    STAT_UNKNOWN
};

class CapThread : public QThread
{
    Q_OBJECT

public:
    explicit CapThread(int* retInt,int width, int height,QString textIp, QObject *parent = 0);
    ~CapThread();
    void sendSDLQuit();
    void SetThreadFlag(quint8 flag);
    quint8 GetThreadFlag(void);
    int WithNetworkInit(QString ipaddr);
    int CheckIPAddr(QString ipaddr);
    int SendPkgData(AVPacket *pkt);
public:
    void show_dshow_device();
    void show_avfoundation_device();
private:
    int resize_width, resize_height;
    AVFormatContext	*pFormatCtx; /*  */
    AVCodecContext	*pEc,*pDc; /* ±àÂë¡¢½âÂë */
    AVCodec			*pEcodec,*pDcodec;
    AVPacket *pkt;/* °ü */
    AVFrame	 *pEframe,*pDframe,*pDFrameYUV;/* ±àÂëÖ¡¡¢½âÂëÖ¡ */
    FILE* f;/* ÎÄ¼þ¾ä±ú */
    int	i,ret,got_output,got_picture, videoindex;/*  */

    struct SwsContext *img_convert_ctx;
    SDL_Thread *video_tid;
    int execcount;
    int pktnum;
    SDL_Overlay *bmp;
    SDL_Rect rect;
    quint8 m_threadstate;

    /*************[ÍøÂç´«Êä]**********************/
    QTcpSocket *p_tcpClient;
    QByteArray outBlock;
    qint64 TotalBytes;

private:
    void run();
signals:

public slots:
    void capFrame();
    void SetStartThread();
    void SetStopThread();
    void displayErr(QAbstractSocket::SocketError socketError);
};

#endif // CAPTHREAD_H
