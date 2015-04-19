
#ifndef SCREENCAP_H
#define SCREENCAP_H
//#define __STDC_CONSTANT_MACROS

extern "C"{
#ifdef __cplusplus
 #define __STDC_CONSTANT_MACROS
 #ifdef _STDINT_H
  #undef _STDINT_H
 #endif
 # include <stdint.h>
#endif
}

#include <QMainWindow>
#include <capthread.h>
#include <QBuffer>
#include <stdio.h>
#include <stdint.h>
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

enum{
    SEND_UNDO,
    SEND_DONE,
    SEND_UNKNOWN
};


namespace Ui {
class ScreenCap;
}

class ScreenCap : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit ScreenCap(QWidget *parent = 0);
    void StartTransferThread(void);
    ~ScreenCap();

    void SetThreadFlag(quint8 flag);
    quint8 GetThreadFlag(void);
    CapThread *pCapThread;
    int WithCapthread();

private slots:
    void on_pushButtonStart_clicked();
    void textCheck(QString str);//检测文本变化
private:
    void showTextStart();
    void showTextStop();
    void showStateBarInfo(const char *pstr);
    void showVerion(void);
    void showText_ClickToStart(void);
    void showText_Connecting(void);
    void BtnStartPix(void);
    void BtnStopPix(void);
    void BtnSetPix(QString str);
    void BtnEnable(void);
    void BtnDisable(void);
    int StartCapScreen();
    void StopCapScreen();
private:
    Ui::ScreenCap *ui;
    static int isStarted;

    /*************[网络传输]**********************/
    QTcpSocket *p_tcpClient;
    QByteArray outBlock;       //缓存一次发送的数据
    QByteArray outBlkData;//缓存一次发送的数据
    qint64 TotalBytes;
    qint64 byteWritten;
    qint64 bytesToWrite;
    quint64 picNametime;
    QTimer *ptnettimer;
    qint64  loadSize;          //被初始化为一个4Kb的常量
    QBuffer buffer;//传输网络数据的一个过程
    QByteArray tmpbyte;//保存网络数据n个的内容
    quint8 sendDoneFlag;//数据是否发生完毕


signals:
    void emitCtrlPthreadStart();
    void emitCtrlPthreadStop();

public slots:
    int  CheckIPAddr(QString ipaddr);
    int  WithNetworkInit(QString ipaddr);
    void displayErr(QAbstractSocket::SocketError socketError);
    void updateClientProgress(qint64 numBytes);
    void NetSend();
    void TimerSets();
    void MergeMessage();
};

#endif // SCREENCAP_H
