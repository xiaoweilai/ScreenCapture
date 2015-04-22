#include "screencap.h"
#include "ui_screencap.h"
#include "version.h"
#include <QtDebug>
#include "capthread.h"
#include <QtGui>
#include <QDesktopWidget>
#include <windows.h>
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

    //����tcpsocket
    p_tcpClient = new QTcpSocket;
    if(!p_tcpClient)
        return RET_FAIL;

    connect(p_tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayErr(QAbstractSocket::SocketError)));
//    connect(p_tcpClient,SIGNAL(bytesWritten(qint64)),this,
//            SLOT(updateClientProgress(qint64)));
    p_tcpClient->connectToHost(ipaddr,
                               QString(DEFAULT_PORT).toInt());
    p_tcpClient->setSocketOption(QAbstractSocket::LowDelayOption, 1);//�Ż�Ϊ����ӳ٣������1�������ø��Ż���


    //waitForConnected()�ȴ�����֪���������ȴ�ʱ�䡣������ӽ�����������true�����򷵻�false��
    //������falseʱ���Ե���error��ȷ���޷����ӵ�ԭ��
    if(!p_tcpClient->waitForConnected(3000))
    {
        qDebug() <<"here:" << p_tcpClient;
        if(NULL != p_tcpClient)
        {
            qDebug() <<"Error: "<<p_tcpClient->errorString();
            p_tcpClient->deleteLater();
            p_tcpClient = NULL;

            QMessageBox::information(NULL, str_china("����"), str_china("�������´�������ʧ��"),NULL,NULL);
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
        QMessageBox::information(NULL,str_china("����"),
                                 str_china("�������´�������ʧ��"),NULL,NULL);
    }else{
        QMessageBox::information(NULL,str_china("����"),
                                 str_china("�������´��� %1")
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

qint64 ScreenCap::writeNetData(const QByteArray &iData)
{
    qint64 len = p_tcpClient->write(iData);
    bool res = p_tcpClient->waitForBytesWritten();
    qDebug("res:%d\n",res);
    qDebug("State:%d\n",p_tcpClient->state());  // State: 3��ConnectedState����ȷ

//    msleep(200);
//    Sleep(200);
    qDebug() << "len:" << len;
    return(len);
}

void ScreenCap::MergeMessage()
{

    /************************************************************
*                ���������͸�ʽ
*
*  ��� ---|   8    |   8       |   8   |   n   |.....
*  ���� ---|  �ܳ��� |  ͷ����(8) | ͷ���� | ��������|.....
*  �ܳ��� = |<----------------------------->|.....
*  �ܳ��� = 8 + 8 + 0 + n
*  ͷ���� �� 0xFFFEFFFE;//ֵ
*  ͷ���� �� 8
************************************************************/
    TotalBytes = pCapThread->arrayNetSize.at(0); //���ݴ�С
    qDebug() << "net size:" << TotalBytes;

    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_3);

    qint64 headerFlag = 0xFFFEFFFE;//ֵ
    sendOut << qint64(0) << qint64(0)  << headerFlag;
    qDebug() << "outBlock size:" << outBlock.size();

    TotalBytes +=  outBlock.size();//�ܴ�С = ���� + 8�ֽ�(�����ܴ�С�ֽ���) + 8"ͷ���ʽ"
//    qDebug() << "TotalBytes size:" << TotalBytes;
    sendOut.device()->seek(0);
//    qDebug() << "header size:" << qint64((outBlock.size() - sizeof(qint64) * 2));
    //��дʵ�ʵ��ܳ��Ⱥ�ͷ����
    sendOut << TotalBytes << qint64((outBlock.size() - sizeof(qint64) * 2));
    //��ͷ���ͳ�ȥ��������ʣ������ݳ��ȣ����������ݳ���(n)
    qint64 len = writeNetData(outBlock);
    bytesToWrite = TotalBytes - len;
    outBlock.resize(0);
    qDebug() << "-->TotalBytes size:" << TotalBytes;
    qDebug() << "-->bytesToWrite size:" << bytesToWrite;



//    sendOut << pCapThread->arrayNetData.at(0);
//    QDataStream in(p_tcpClient);
//    in << pCapThread->arrayNetData.at(0);

//    p_tcpClient->write(outBlock);


//    outBlock.resize(0);
//    in<< quint16(0xFFFF); //��ʱQIODevice�����˴����ݣ�����ֱ�ӷ��ͳ�ȥ

//    quint16 length = 0;
//    QDataStream out(p_tcpClient);//�����ʱtcpSocketֱ�������ݷ��͹���
//    out >> length;//length��õ�һ������ֵ������tcpSocket����ո�����




//    tmpbyte.clear();


    outBlkData = pCapThread->arrayNetData.at(0);

    writeNetData(outBlkData);


//    qDebug() << "-->tmpbyte count:" << tmpbyte.count();
//    qDebug() << "-->tmpbyte size :" << tmpbyte.size();

//    QDataStream s(&tmpbyte, QIODevice::ReadOnly);
//    s >> outBlkData;      // ��ȡԭʼ��bindata

    qDebug() << "-->outBlkData size :" << outBlkData.size();
//    buffer.reset();
//    buffer.open(QBuffer::WriteOnly);
//    buffer.setBuffer(&tmpbyte);
//    buffer.write(tmpbyte);


}

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

    //��ʱ���緢��
    TimerSets();
}


void ScreenCap::TimerSets()
{
    ptnettimer = new QTimer();
    connect(ptnettimer,SIGNAL(timeout()),this,SLOT(NetSend()));
    ptnettimer->start(40);
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

        pCapThread->arrayNetData.removeAt(0);
        pCapThread->arrayNetSize.removeAt(0);

        sendDoneFlag = SEND_DONE;
    }

}




void ScreenCap::showVerion(void)
{
    QString verinfo = QString::fromLocal8Bit("¼������ ") +
            QString::fromLocal8Bit(ScreenCapVersion) +
            QString::fromLocal8Bit("�������ʼ���� ");
    ui->statusBar->showMessage(verinfo);
}


void ScreenCap::showText_ClickToStart(void)
{
    QString verinfo = QString::fromLocal8Bit("�����ʼ���� ");
    ui->statusBar->showMessage(verinfo);
}

void ScreenCap::showText_Connecting(void)
{
    QString verinfo = QString::fromLocal8Bit("������...����ȴ� ");
    ui->statusBar->showMessage(verinfo);
}


/*
�տ�ʼ��δ����˰�ť��
��һ�ε��ʱ��Ĭ�Ͻ���ֹͣ��ť������Ϊ������...
�ڶ��ε��ʱ�����뿪ʼ��ť������Ϊ�������
*/
void ScreenCap::on_pushButtonStart_clicked()
{
    static unsigned int mNo = 1;
    if(0 == ((mNo++)%2)) /* ������� */
    {
        qDebug() << " stop!!";
        BtnStartPix();
        BtnDisable();
        showTextStop();

        //ֹͣ����
        StopCapScreen();

        BtnEnable();
    }
    else /* ��ʼ���� */
    {
        qDebug() << " starting!!";
        BtnStopPix();//��ʾֹͣͼƬ

        //��ʼ����
        if(STAT_STOPPED == StartCapScreen())//��������
        {
            BtnStartPix();//��ʾ��ʼͼƬ
            showText_ClickToStart();//��ʾ�������
            mNo = 1;//
        }else
        {
            showTextStart();//���ӳɹ�
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

//��ʼ����
int ScreenCap::StartCapScreen()
{
    showText_Connecting();
    if(STAT_STOPPED == isStarted)
    {
        if(RET_SUCESS == WithCapthread())//�������Ӳ���ʼץ���߳�
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

//ֹͣ����
void ScreenCap::StopCapScreen()
{
    emit emitCtrlPthreadStop();
}

//����ı��仯
void ScreenCap::textCheck(QString str)
{
    str = str.replace(" ","");
    if(str.isEmpty())
    {
        ui->label->setText(QString::fromLocal8Bit(" ������IP��ַ,�������ʼ��ť���д���"));
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
    showStateBarInfo("������...");
}

void ScreenCap::showTextStop()
{
    showStateBarInfo("�������");
}


int ScreenCap::CheckIPAddr(QString ipaddr)
{
    QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if(!regExp.exactMatch(ipaddr))
    {
        QMessageBox::warning(NULL, str_china("��ʾ"), str_china("ip��ַ����"),NULL,NULL);
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

