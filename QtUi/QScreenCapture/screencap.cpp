#include "screencap.h"
#include "ui_screencap.h"
#include "version.h"
#include <QtDebug>
#include "capthread.h"
#include <QtGui>
#include <QDesktopWidget>
#include "capthread.h"

int ScreenCap::isStarted = STAT_STOPPED;





int ScreenCap::WithNetworkInit(QString ipaddr)
{
    if(RET_SUCESS != CheckIPAddr(ipaddr))
    {
        return RET_FAIL;
    }

    p_tcpClient = NULL;//tcp socket
    TotalBytes = 0;

    //创建tcpsocket
    p_tcpClient = new QTcpSocket;
    if(!p_tcpClient)
        return RET_FAIL;

    connect(p_tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayErr(QAbstractSocket::SocketError)));
    connect(p_tcpClient,SIGNAL(bytesWritten(qint64)),this,
            SLOT(updateClientProgress(qint64)));
    p_tcpClient->connectToHost(ipaddr,
                               QString(DEFAULT_PORT).toInt());
    p_tcpClient->setSocketOption(QAbstractSocket::LowDelayOption, 1);//优化为最低延迟，后面的1代码启用该优化。


    //waitForConnected()等待连接知道超过最大等待时间。如果连接建立函数返回true；否则返回false。
    //当返回false时可以调用error来确定无法连接的原因
    if(!p_tcpClient->waitForConnected(3000))
    {
        qDebug() <<"here:" << p_tcpClient;
        if(NULL != p_tcpClient)
        {
            qDebug() <<"Error: "<<p_tcpClient->errorString();
            p_tcpClient->deleteLater();
            p_tcpClient = NULL;

            QMessageBox::information(NULL, str_china("网络"), str_china("产生如下错误：连接失败"),NULL,NULL);
            return RET_FAIL;
        }
    }

    return RET_SUCESS;
}


void ScreenCap::displayErr(QAbstractSocket::SocketError socketError)
{
    qDebug() << "display err";
    if(NULL == p_tcpClient)
    {
        QMessageBox::information(NULL,str_china("网络"),
                                 str_china("产生如下错误：连接失败"),NULL,NULL);
    }else{
        QMessageBox::information(NULL,str_china("网络"),
                                 str_china("产生如下错误： %1")
                                 .arg(p_tcpClient->errorString()),NULL,NULL);
    }

    if(NULL != p_tcpClient)
    {
        p_tcpClient->abort();
        p_tcpClient->waitForDisconnected();
        p_tcpClient->disconnectFromHost();
        p_tcpClient->close();
        p_tcpClient->deleteLater();
        p_tcpClient = NULL;
    }
}


void ScreenCap::MergeMessage()
{

    /************************************************************
*                数据流传送格式
*
*  宽度 ---|   8    |   8       |   8   |   n   |.....
*  含义 ---|  总长度 |  头长度(8) | 头内容 | 数据内容|.....
*  总长度 = |<----------------------------->|.....
*  总长度 = 8 + 8 + 0 + n
*  头内容 ： 0xFFFEFFFE;//值
*  头长度 ： 8
************************************************************/
    TotalBytes = pCapThread->arrayNetSize.at(0); //数据大小
    qDebug() << "net size:" << TotalBytes;

    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_3);

    qint64 headerFlag = 0xFFFEFFFE;//值
    sendOut << qint64(0) << qint64(0)  << headerFlag;
    qDebug() << "outBlock size:" << outBlock.size();

    TotalBytes +=  outBlock.size();//总大小 = 数据 + 8字节(存在总大小字节数) + 8"头表达式"
//    qDebug() << "TotalBytes size:" << TotalBytes;
    sendOut.device()->seek(0);
//    qDebug() << "header size:" << qint64((outBlock.size() - sizeof(qint64) * 2));
    //填写实际的总长度和头长度
    sendOut << TotalBytes << qint64((outBlock.size() - sizeof(qint64) * 2));
    //将头发送出去，并计算剩余的数据长度，即数据内容长度(n)
    bytesToWrite = TotalBytes - p_tcpClient->write(outBlock);
    outBlock.resize(0);


    qDebug() << "-->TotalBytes size:" << TotalBytes;
    qDebug() << "-->bytesToWrite size:" << bytesToWrite;

    sendOut << pCapThread->arrayNetData.at(0);
//    QDataStream in(p_tcpClient);
//    in << pCapThread->arrayNetData.at(0);

    p_tcpClient->write(outBlock);
    outBlock.resize(0);
//    in<< quint16(0xFFFF); //此时QIODevice加载了此数据，而且直接发送出去

//    quint16 length = 0;
//    QDataStream out(p_tcpClient);//如果此时tcpSocket直接有数据发送过来
//    out >> length;//length获得第一个整型值，并在tcpSocket中清空该数据




    tmpbyte.clear();


    outBlkData = pCapThread->arrayNetData.at(0);
//    qDebug() << "-->tmpbyte count:" << tmpbyte.count();
//    qDebug() << "-->tmpbyte size :" << tmpbyte.size();

//    QDataStream s(&tmpbyte, QIODevice::ReadOnly);
//    s >> outBlkData;      // 读取原始的bindata

    qDebug() << "-->outBlkData size :" << outBlkData.size();
//    buffer.reset();
//    buffer.open(QBuffer::WriteOnly);
//    buffer.setBuffer(&tmpbyte);
//    buffer.write(tmpbyte);

    sendDoneFlag = SEND_DONE;
}

void ScreenCap::updateClientProgress(qint64 numBytes)
{
#ifdef DEBUG
    qDebug() << "numBytes:--------->>"<<numBytes;
#endif
    byteWritten += (int)numBytes;
#ifdef DEBUG
    qDebug() << "byteWritten:" << byteWritten;
    qDebug() << "bytesToWrite:" << bytesToWrite;
#endif
//    if (bytesToWrite > 0) {
//        //        outBlock = buffer.read(qMin(bytesToWrite, loadSize));
////        outBlock = buffer.read(qMin(bytesToWrite, loadSize));
////        outBlkData = buffer.data();

//        qDebug() << "outBlkData size:--------->>"<<outBlkData.size();
//        qDebug() << "bytesToWrite size:--------->>"<<bytesToWrite;
//        if(bytesToWrite == qMin(bytesToWrite, loadSize))
//        {
//            qDebug() << "!!write size:--------->>"<<bytesToWrite;
//            bytesToWrite -= (int)p_tcpClient->write(outBlkData,bytesToWrite);
//        }else{
//            qDebug() << "write size:--------->>"<<loadSize;
//            bytesToWrite -= (int)p_tcpClient->write(outBlkData,loadSize);
//        }

//    }
//    else{
//#ifdef DEBUG
//        qDebug() << "-->: send image done!!";
//#endif
//        sendDoneFlag = SEND_DONE;
//        picNametime++;
//        TotalBytes = 0;
//        byteWritten = 0;
//        bytesToWrite = 0;
//        if(pCapThread->arrayNetData.count() > 0)
//        {
//            pCapThread->arrayNetData.removeAt(0);
//            pCapThread->arrayNetSize.removeAt(0);
//        }
//        buffer.close();
//        outBlkData.resize(0);
//    }


#if 0

#ifdef DEBUG
    qDebug() << "numBytes:--------->>"<<numBytes;
#endif
    byteWritten += (int)numBytes;
    if(bytesToWrite > 0)
    {
#ifdef DEBUG
        //        qDebug() <<"-->:outBlockFile size:" << outBlockFile.size();
#endif

        bytesToWrite -= (int)p_tcpClient->write(outBlockFile);
#ifdef DEBUG
        qDebug() <<"-->:bytesToWrite size:" << bytesToWrite;
#endif
    }
    else
    {
#ifdef DEBUG
        qDebug() << "-->: send image done!!";
#endif
        picNametime++;
        TotalBytes = 0;
        byteWritten = 0;
        //        sendDoneFlag = SEND_DONE;
    }
#endif
}


int ScreenCap::WithCapthread()
{
    int ret = RET_SUCESS;
    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();
    printf("screen rect,w:%d h:%d\n", w, h);

    QString textIp = ui->lineEditIp->text();

    if(RET_SUCESS != WithNetworkInit(textIp))
    {
        qDebug() << "CapThread create RET_FAIL!!";
        return RET_FAIL;
    }



    pCapThread = new CapThread(&ret,w, h,textIp);
    if(RET_SUCESS != ret)
    {
        qDebug() << "pCapThread create failed";
        return RET_FAIL;
    }

    qDebug() << "pCapThread start addr:" << pCapThread;
    //ctrl pthread  of capthread
    connect(this,SIGNAL(emitCtrlPthreadStart()),pCapThread,
            SLOT(SetStartThread()));
    connect(this,SIGNAL(emitCtrlPthreadStop()),pCapThread,
            SLOT(SetStopThread()));


    return RET_SUCESS;
}

ScreenCap::ScreenCap(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenCap)
{
    ui->setupUi(this);
    ui->lineEditIp->setStyleSheet("QLineEdit{font: bold italic large \"Times New Roman\";font-size:25px;color:rgb(55,100,255);height:50px;border:4px solid rgb(155,200,33);background-color: rgba(0,0,0,30);border-radius:15px;selection-color:pink}");
    showVerion();
    BtnStartPix();
    connect(ui->lineEditIp,SIGNAL(textChanged(QString)),this,SLOT(textCheck(QString)));

    pCapThread = NULL;
    TotalBytes = 0;
    byteWritten = 0;
    bytesToWrite = 0;
    picNametime = 1;
    loadSize = 4*1024;     // 4Kb
//    loadSize = 10*1024;     // 10Kb
    sendDoneFlag = SEND_DONE;

//    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
//    sendOut.setVersion(QDataStream::Qt_4_3);

    //定时网络发送
    TimerSets();
}


void ScreenCap::TimerSets()
{
    ptnettimer = new QTimer();
    connect(ptnettimer,SIGNAL(timeout()),this,SLOT(NetSend()));
    ptnettimer->start(200);
}


void ScreenCap::NetSend()
{
    if(NULL == pCapThread)
    {
        return;
    }

    if(0 == pCapThread->arrayNetSize.count())
    {
        return;
    }

    if(0 == pCapThread->arrayNetData.count())
    {
        return;
    }

    if(SEND_DONE == sendDoneFlag)
    {
        sendDoneFlag = SEND_UNDO;
        MergeMessage();
    }

}




void ScreenCap::showVerion(void)
{
    QString verinfo = QString::fromLocal8Bit("录屏传输 ") +
            QString::fromLocal8Bit(ScreenCapVersion) +
            QString::fromLocal8Bit("，点击开始传输 ");
    ui->statusBar->showMessage(verinfo);
}


void ScreenCap::showText_ClickToStart(void)
{
    QString verinfo = QString::fromLocal8Bit("点击开始传输 ");
    ui->statusBar->showMessage(verinfo);
}

void ScreenCap::showText_Connecting(void)
{
    QString verinfo = QString::fromLocal8Bit("连接中...，请等待 ");
    ui->statusBar->showMessage(verinfo);
}


/*
刚开始并未进入此按钮中
第一次点击时，默认进入停止按钮，设置为传输中...
第二次点击时，进入开始按钮，设置为传输结束
*/
void ScreenCap::on_pushButtonStart_clicked()
{
    static unsigned int mNo = 1;
    if(0 == ((mNo++)%2)) /* 传输结束 */
    {
        qDebug() << " stop!!";
        BtnStartPix();
        BtnDisable();
        showTextStop();

        //停止传输
        StopCapScreen();

        BtnEnable();
    }
    else /* 开始传输 */
    {
        qDebug() << " starting!!";
        BtnStopPix();//显示停止图片

        //开始传输
        if(STAT_STOPPED == StartCapScreen())//建立连接
        {
            BtnStartPix();//显示开始图片
            showText_ClickToStart();//显示点击传输
            mNo = 1;//
        }else
        {
            showTextStart();//连接成功
        }

    }
}

void ScreenCap::BtnStartPix(void)
{
    QString str = ":images/start.png";
    BtnSetPix(str);
}

void ScreenCap::BtnStopPix(void)
{
    QString str = ":images/stop.png";
    BtnSetPix(str);
}


void ScreenCap::BtnSetPix(QString str)
{
#define PNGSIZEDIV 1
    QPixmap mypixmap;
    mypixmap.load(str);
    ui->pushButtonStart->setIcon(mypixmap);
    ui->pushButtonStart->setIconSize(QSize(mypixmap.width()/PNGSIZEDIV,mypixmap.height()/PNGSIZEDIV));
}

void ScreenCap::BtnEnable(void)
{
    ui->pushButtonStart->setEnabled(TRUE);
}

void ScreenCap::BtnDisable(void)
{
    ui->pushButtonStart->setEnabled(FALSE);
}

//开始传输
int ScreenCap::StartCapScreen()
{
    showText_Connecting();
    if(STAT_STOPPED == isStarted)
    {
        if(RET_SUCESS == WithCapthread())//建立连接并开始抓捕线程
        {
            if(pCapThread)
                pCapThread->start();
            isStarted = STAT_STARTED;
        }
        else
        {
            return STAT_STOPPED;
        }
    }
    else
    {
        emit emitCtrlPthreadStart();
    }
    qDebug() <<"isStarted:"<<isStarted;

    return STAT_STARTED;
}

//停止传输
void ScreenCap::StopCapScreen()
{
    emit emitCtrlPthreadStop();
}

//检测文本变化
void ScreenCap::textCheck(QString str)
{
    str = str.replace(" ","");
    if(str.isEmpty())
    {
        ui->label->setText(QString::fromLocal8Bit(" 请输入IP地址,并点击开始按钮进行传输"));
    }else{
        ui->label->setText(QString::fromLocal8Bit(""));
    }

}

void ScreenCap::showStateBarInfo(const char *pstr)
{
    ui->statusBar->showMessage(QString::fromLocal8Bit(pstr));
}

void ScreenCap::showTextStart()
{
    showStateBarInfo("传输中...");
}

void ScreenCap::showTextStop()
{
    showStateBarInfo("传输结束");
}


int ScreenCap::CheckIPAddr(QString ipaddr)
{
    QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if(!regExp.exactMatch(ipaddr))
    {
        QMessageBox::warning(NULL, str_china("提示"), str_china("ip地址错误"),NULL,NULL);
        return RET_FAIL;
    }

    return RET_SUCESS;
}


ScreenCap::~ScreenCap()
{
    if((NULL != pCapThread)&&(STAT_STARTED == ScreenCap::isStarted))
    {
        qDebug() << "delete pCapThread!!!";
        pCapThread->sendSDLQuit();
        pCapThread->terminate();
        pCapThread->quit();
        pCapThread->deleteLater();
        delete pCapThread;
        pCapThread = NULL;
    }
    delete ui;
}

