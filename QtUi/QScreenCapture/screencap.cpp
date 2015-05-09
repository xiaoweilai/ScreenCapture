/************************************************/
/*声 明:  @db.com                                */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*XXXXX:                                         */
/*功 能:用于捕屏的初始化，捕屏进程创建，网络传输         */
/*author :wxj                                    */
/*email  :wxjlmr@126.com                         */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
/*更新记录:                                        */
/*                                                */
/*************************************************/
#include "screencap.h"
#include "ui_screencap.h"
#include "version.h"
#include <QtDebug>
#include "capthread.h"
#include <QtGui>
#include <QDesktopWidget>
#include <windows.h>
#include "capthread.h"

#define NO_FIRST 1

int ScreenCap::isStarted = STAT_STOPPED;
unsigned int ScreenCap::mNo = NO_FIRST;

/************************************************/
/*函 数:ScreenCap                                */
/*入 参:parent-父类                               */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:界面、变量、定时器初始化、版本显示              */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
ScreenCap::ScreenCap(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenCap)
{
    ui->setupUi(this);
    ui->lineEditIp->setStyleSheet("QLineEdit{font: bold italic large \"Times New Roman\";font-size:25px;color:rgb(55,100,255);height:50px;border:4px solid rgb(155,200,33);background-color: rgba(0,0,0,30);border-radius:15px;selection-color:pink}");
    showAppVerion();
    BtnStartPix();
    connect(ui->lineEditIp,SIGNAL(textChanged(QString)),this,SLOT(LineTextTips(QString)));

    pCapThread = NULL;
    TotalBytes = 0;
    byteWritten = 0;
    bytesToWrite = 0;
    picNametime = 1;
    loadSize = 4*1024;     // 4Kb

    sendDoneFlag = SEND_DONE;

    //定时网络发送
    TimerSets();
}

/************************************************/
/*函 数:WithNetworkInit                          */
/*入 参:ipaddr-IP地址                             */
/*出 参:无                                        */
/*返 回:RET_SUCESS-成功，RET_FAIL-失败              */
/*功 能:网络传输初始化，检测IP地址，连接远程IP地址       */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
int ScreenCap::WithNetworkInit(QString ipaddr)
{
    if(RET_SUCESS != CheckIPAddrValid(ipaddr))
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
            SLOT(displayNetErr(QAbstractSocket::SocketError)));
//    connect(p_tcpClient,SIGNAL(bytesWritten(qint64)),this,
//            SLOT(updateClientProgress(qint64)));
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

/************************************************/
/*函 数:displayNetErr                            */
/*入 参:socketError-socket各种错误                 */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:网络各种错误情况处理                         */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::displayNetErr(QAbstractSocket::SocketError socketError)
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
        emit emitCtrlPthreadQuit();
        p_tcpClient->abort();
        p_tcpClient->waitForDisconnected(3000);
        p_tcpClient->disconnectFromHost();
        p_tcpClient->close();

        pNetSendTimer->stop();
        pCapThread->arrayNetData.clear();
        pCapThread->arrayNetSize.clear();
        isStarted == STAT_STOPPED;
        StopActionSets();
        mNo = NO_FIRST;
        p_tcpClient->deleteLater();

//        p_tcpClient = NULL;
    }
}

/************************************************/
/*函 数:writeNetData                             */
/*入 参:iData-待传输的字节                         */
/*出 参:无                                        */
/*返 回:传递成功的数据字节长度                       */
/*功 能:传递网络数据                               */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
qint64 ScreenCap::writeNetData(const QByteArray &iData)
{
    qint64 len = p_tcpClient->write(iData);
//    bool res = p_tcpClient->waitForBytesWritten();
//    qDebug("res:%d\n",res);
    qDebug("State:%d\n",p_tcpClient->state());  // State: 3（ConnectedState）正确

    qDebug() << "len:" << len;
    return(len);
}

/************************************************/
/*函 数:MergeMessage                             */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:组合报文，包括数据总长度，头长度，头内容和数据内容 */
/*     同时将头内容发送出去                          */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
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
    qint64 len = writeNetData(outBlock);
    bytesToWrite = TotalBytes - len;
    outBlock.resize(0);
    qDebug() << "-->TotalBytes size:" << TotalBytes;
    qDebug() << "-->bytesToWrite size:" << bytesToWrite;

    outBlkData = pCapThread->arrayNetData.at(0);

    writeNetData(outBlkData);

    qDebug() << "-->outBlkData size :" << outBlkData.size();
}

/************************************************/
/*函 数:updateClientProgress                     */
/*入 参:numBytes-传递的字节数                      */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:更新客户侧传递的进度条                        */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::updateClientProgress(qint64 numBytes)
{
//#ifdef DEBUG
//    qDebug() << "numBytes:--------->>"<<numBytes;
//#endif
//    byteWritten += (int)numBytes;
//#ifdef DEBUG
//    qDebug() << "byteWritten:" << byteWritten;
//    qDebug() << "bytesToWrite:" << bytesToWrite;
//#endif
////    if (bytesToWrite > 0) {
////        //        outBlock = buffer.read(qMin(bytesToWrite, loadSize));
//////        outBlock = buffer.read(qMin(bytesToWrite, loadSize));
//////        outBlkData = buffer.data();

////        qDebug() << "outBlkData size:--------->>"<<outBlkData.size();
////        qDebug() << "bytesToWrite size:--------->>"<<bytesToWrite;
////        if(bytesToWrite == qMin(bytesToWrite, loadSize))
////        {
////            qDebug() << "!!write size:--------->>"<<bytesToWrite;
////            bytesToWrite -= (int)p_tcpClient->write(outBlkData,bytesToWrite);
////        }else{
////            qDebug() << "write size:--------->>"<<loadSize;
////            bytesToWrite -= (int)p_tcpClient->write(outBlkData,loadSize);
////        }

////    }
////    else{
////#ifdef DEBUG
////        qDebug() << "-->: send image done!!";
////#endif
////        sendDoneFlag = SEND_DONE;
////        picNametime++;
////        TotalBytes = 0;
////        byteWritten = 0;
////        bytesToWrite = 0;
////        if(pCapThread->arrayNetData.count() > 0)
////        {
////            pCapThread->arrayNetData.removeAt(0);
////            pCapThread->arrayNetSize.removeAt(0);
////        }
////        buffer.close();
////        outBlkData.resize(0);
////    }


//#if 0

//#ifdef DEBUG
//    qDebug() << "numBytes:--------->>"<<numBytes;
//#endif
//    byteWritten += (int)numBytes;
//    if(bytesToWrite > 0)
//    {
//#ifdef DEBUG
//        //        qDebug() <<"-->:outBlockFile size:" << outBlockFile.size();
//#endif

//        bytesToWrite -= (int)p_tcpClient->write(outBlockFile);
//#ifdef DEBUG
//        qDebug() <<"-->:bytesToWrite size:" << bytesToWrite;
//#endif
//    }
//    else
//    {
//#ifdef DEBUG
//        qDebug() << "-->: send image done!!";
//#endif
//        picNametime++;
//        TotalBytes = 0;
//        byteWritten = 0;
//        //        sendDoneFlag = SEND_DONE;
//    }
//#endif
}

/************************************************/
/*函 数:CreateCapturethread                      */
/*入 参:numBytes-传递的字节数                      */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:创建捕屏线程，默认捕捉全屏，发送信号控制是否捕屏  */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
int ScreenCap::CreateCapturethread()
{
    int ret = RET_SUCESS;
    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();
    qDebug("screen rect,w:%d h:%d\n", w, h);

    QString textIp = ui->lineEditIp->text();

    if(RET_SUCESS != WithNetworkInit(textIp))
    {
        qDebug("CapThread create RET_FAIL!!\n");
        return RET_FAIL;
    }

    pCapThread = new CapThread(w, h, NULL);
    qDebug() << "pCapThread start addr:" << pCapThread;
    //ctrl pthread  of capthread
    connect(this,SIGNAL(emitCtrlPthreadStart()),pCapThread,
            SLOT(SetStartThread()));
    connect(this,SIGNAL(emitCtrlPthreadStop()),pCapThread,
            SLOT(SetStopThread()));
    connect(this,SIGNAL(emitCtrlPthreadQuit()),pCapThread,
            SLOT(SetQuitThread()));

    return RET_SUCESS;
}

/************************************************/
/*函 数:TimerSets                                */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:定时器集，包括（发送网络数据）                 */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::TimerSets()
{
    pNetSendTimer = new QTimer();
    connect(pNetSendTimer,SIGNAL(timeout()),this,SLOT(NetSendData()));
    pNetSendTimer->start(40);
}

/************************************************/
/*函 数:NetSendData                              */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:网络数据发送槽函数                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::NetSendData()
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

        pCapThread->arrayNetData.removeAt(0);
        pCapThread->arrayNetSize.removeAt(0);

        sendDoneFlag = SEND_DONE;
    }
//    qDebug() << "~~~arrayNetData count:" <<pCapThread->arrayNetData.count();
    qDebug() << "~~~arrayNetSize count:" <<pCapThread->arrayNetSize.count();

}

/************************************************/
/*函 数:StopActionSets                           */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:停止传输处理函数集合                          */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::StopActionSets()
{
    qDebug() << " stop!!";
    BtnStartPix();
    BtnDisable();
    showTextTransferOver();

    //停止传输
    StopCapScreen();

    BtnEnable();
}

/************************************************/
/*函 数:on_pushButtonStart_clicked               */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:开始/停止按钮动作，                          */
/*     第一次点击时，默认进入停止按钮，设置为传输中...   */
/*     第二次点击时，进入开始按钮，设置为传输结束        */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::on_pushButtonStart_clicked()
{
    if(0 == ((mNo++)%2)) /* 传输结束 */
    {
        StopActionSets();
    }
    else /* 开始传输 */
    {
        qDebug() << " starting!!";
        BtnStopPix();//显示停止图片

        //开始传输
        if(STAT_STOPPED == CaptureScreenOn())//建立连接
        {
            BtnStartPix();//显示开始图片
            showTextClickToStart();//显示点击传输
            mNo = NO_FIRST;//
        }else
        {
            showTextTransfering();//连接成功
        }

    }
}

/************************************************/
/*函 数:BtnStartPix                              */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:开始图片显示及样式                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnStartPix(void)
{
    QString str = ":images/start.png";
    BtnSetPix(str);
}

/************************************************/
/*函 数:BtnStopPix                               */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:停止图片显示及样式                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnStopPix(void)
{
    QString str = ":images/stop.png";
    BtnSetPix(str);
}

/************************************************/
/*函 数:BtnSetPix                                */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:图片显示及样式                              */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnSetPix(QString str)
{
#define PNGSIZEDIV 1
    QPixmap mypixmap;
    mypixmap.load(str);
    ui->pushButtonStart->setIcon(mypixmap);
    ui->pushButtonStart->setIconSize(QSize(mypixmap.width()/PNGSIZEDIV,mypixmap.height()/PNGSIZEDIV));
}

/************************************************/
/*函 数:BtnEnable                                */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:按钮使能                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnEnable(void)
{
    ui->pushButtonStart->setEnabled(TRUE);
}

/************************************************/
/*函 数:BtnDisable                                */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:按钮禁止使能                                */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::BtnDisable(void)
{
    ui->pushButtonStart->setEnabled(FALSE);
}

/************************************************/
/*函 数:CaptureScreenOn                          */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:STAT_STARTED-开始，STAT_STOPPED-停止       */
/*功 能:捕屏开始                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
int ScreenCap::CaptureScreenOn()
{
    showTextConnecting();
    if(STAT_STOPPED == isStarted)
    {
        if(RET_SUCESS == CreateCapturethread())//建立连接并开始抓捕线程
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

/************************************************/
/*函 数:StopCapScreen                            */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:捕屏停止                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::StopCapScreen()
{
    emit emitCtrlPthreadStop();
}

/************************************************/
/*函 数:LineTextTips                             */
/*入 参:str-lineText的文本内容                     */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:文本内容提示，当为空，显示文本显示              */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::LineTextTips(QString str)
{
    str = str.replace(" ","");
    if(str.isEmpty())
    {
        ui->label->setText(QString::fromLocal8Bit(" 请输入IP地址,并点击开始按钮进行传输"));
    }else{
        ui->label->setText(QString::fromLocal8Bit(""));
    }

}

/************************************************/
/*函 数:showStateBarInfo                         */
/*入 参:pstr-字符串                               */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示字符串信息                         */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showStateBarInfo(const char *pstr)
{
    ui->statusBar->showMessage(QString::fromLocal8Bit(pstr));
}

/************************************************/
/*函 数:showTextTransfering                      */
/*入 参:pstr-字符串                               */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示"传输中..."                        */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextTransfering()
{
    showStateBarInfo("传输中...");
}

/************************************************/
/*函 数:showTextTransferOver                     */
/*入 参:pstr-字符串                               */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示"传输结束"                         */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextTransferOver()
{
    showStateBarInfo("传输结束");
}

/************************************************/
/*函 数:CheckIPAddrValid                         */
/*入 参:ipaddr-IP地址                             */
/*出 参:无                                        */
/*返 回:RET_SUCESS-成功，RET_FAIL-失败             */
/*功 能:检测IP地址有效性                            */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
int ScreenCap::CheckIPAddrValid(QString ipaddr)
{
    QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if(!regExp.exactMatch(ipaddr))
    {
        QMessageBox::warning(NULL, str_china("提示"), str_china("ip地址错误"),NULL,NULL);
        return RET_FAIL;
    }

    return RET_SUCESS;
}

/************************************************/
/*函 数:showAppVerion                            */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示程序版本                           */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showAppVerion(void)
{
    QString verinfo = QString::fromLocal8Bit("录屏传输 ") +
            QString::fromLocal8Bit(ScreenCapVersion) +
            QString::fromLocal8Bit("，点击开始传输 ");
    ui->statusBar->showMessage(verinfo);
}

/************************************************/
/*函 数:showTextClickToStart                     */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示  "点击开始传输 "                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextClickToStart(void)
{
    QString verinfo = QString::fromLocal8Bit("点击开始传输 ");
    ui->statusBar->showMessage(verinfo);
}

/************************************************/
/*函 数:showTextConnecting                       */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:状态栏显示"连接中...，请等待 "                */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
void ScreenCap::showTextConnecting(void)
{
    QString verinfo = QString::fromLocal8Bit("连接中...，请等待 ");
    ui->statusBar->showMessage(verinfo);
}

/************************************************/
/*函 数:~ScreenCap                               */
/*入 参:无                                        */
/*出 参:无                                        */
/*返 回:无                                        */
/*功 能:析构函数                                   */
/*author :wxj                                    */
/*version:1.0                                    */
/*时 间:2015.4.25                                 */
/*************************************************/
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

