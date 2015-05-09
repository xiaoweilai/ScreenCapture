/************************************************/
/*声 明:  @db.com                                */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*功 能:用于捕屏的初始化，捕屏进程创建，网络传输头文件    */
/*author :wxj                                    */
/*email  :wxjlmr@126.com                         */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
/*更新记录:                                        */
/*                                                */
/*************************************************/

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
    int CreateCapturethread();
    int CreateTcpSocket();

private slots:
    void on_pushButtonStart_clicked();
    void LineTextTips(QString str);
    void StopActionSets();
private:
    void showTextTransfering();
    void showTextTransferOver();
    void showStateBarInfo(const char *pstr);
    void showAppVerion(void);
    void showTextClickToStart(void);
    void showTextClickOverToReStart(void);
    void showTextConnecting(void);
    void BtnStartPix(void);
    void BtnStopPix(void);
    void BtnSetPix(QString str);
    void BtnEnable(void);
    void BtnDisable(void);
    int CaptureScreenOn();
    void StopCapScreen();

    /*************[网络传输]**********************/
    qint64 writeNetData(const QByteArray &iData);
private:
    Ui::ScreenCap *ui;
    static int isStarted;
    static unsigned int mNo;

    /*************[网络传输]**********************/
    QTcpSocket *p_tcpClient;
    QByteArray outBlock;       //缓存一次发送的数据
    QByteArray outBlkData;//缓存一次发送的数据
    qint64 TotalBytes;
    qint64 byteWritten;
    qint64 bytesToWrite;
    quint64 picNametime;
    QTimer *pNetSendTimer;
    qint64  loadSize;          //被初始化为一个4Kb的常量
    QBuffer buffer;//传输网络数据的一个过程
    QByteArray tmpbyte;//保存网络数据n个的内容
    quint8 sendDoneFlag;//数据是否发生完毕


signals:
    void emitCtrlPthreadStart();
    void emitCtrlPthreadStop();
    void emitCtrlPthreadQuit();


public slots:
    int  CheckIPAddrValid(QString ipaddr);
    int  WithNetworkInit(QString ipaddr);
    void displayNetErr(QAbstractSocket::SocketError socketError);
    QString getSockState(QAbstractSocket::SocketState state);
    void updateClientProgress(qint64 numBytes);
    void NetSendData();
    void TimerSets();
    void MergeMessage();
};

#endif // SCREENCAP_H
