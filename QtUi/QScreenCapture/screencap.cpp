#include "screencap.h"
#include "ui_screencap.h"
#include "version.h"
#include <QtDebug>
#include "capthread.h"
#include <QtGui>
#include <QDesktopWidget>
#include "capthread.h"


ScreenCap::ScreenCap(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenCap)
{
    ui->setupUi(this);
    ui->lineEditIp->setStyleSheet("QLineEdit{font: bold italic large \"Times New Roman\";font-size:25px;color:rgb(55,100,255);height:50px;border:4px solid rgb(155,200,33);background-color: rgba(0,0,0,30);border-radius:15px;selection-color:pink}");
    showVerion();
    BtnStartPix();
    connect(ui->lineEditIp,SIGNAL(textChanged(QString)),this,SLOT(textCheck(QString)));

    int w = QApplication::desktop()->width();
    int h = QApplication::desktop()->height();
    printf("screen rect,w:%d h:%d\n", w, h);
    pCapThread = new CapThread(w, h);
    qDebug() << "pCapThread start addr:" << pCapThread;

    //ctrl pthread  of capthread
    connect(this,SIGNAL(emitCtrlPthreadStart()),pCapThread,
            SLOT(SetStartThread()));
    connect(this,SIGNAL(emitCtrlPthreadStop()),pCapThread,
            SLOT(SetStopThread()));
}



void ScreenCap::showVerion(void)
{
    QString verinfo = QString::fromLocal8Bit("录屏传输 ") +
             QString::fromLocal8Bit(ScreenCapVersion) +
             QString::fromLocal8Bit("，点击开始传输 ");
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
        BtnStopPix();
        //        BtnDisable();
        showTextStart();


        //开始传输
        StartCapScreen();
        //        BtnEnable();
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
void ScreenCap::StartCapScreen()
{
    static int isStarted = 0;
    if(0 == isStarted)
    {

        pCapThread->start();
        isStarted = 1;
    }
    else
    {
        emit emitCtrlPthreadStart();
    }
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


ScreenCap::~ScreenCap()
{
    if(NULL != pCapThread)
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
