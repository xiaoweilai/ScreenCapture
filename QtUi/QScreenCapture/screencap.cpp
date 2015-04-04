#include "screencap.h"
#include "ui_screencap.h"
#include "version.h"
#include <QtDebug>

ScreenCap::ScreenCap(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenCap)
{
    ui->setupUi(this);
    ui->lineEditIp->setStyleSheet("QLineEdit{font: bold italic large \"Times New Roman\";font-size:25px;color:rgb(55,100,255);height:50px;border:4px solid rgb(155,200,33);background-color: rgba(0,0,0,30);border-radius:15px;selection-color:pink}");
    showVerion();
    BtnStartPix();
    connect(ui->lineEditIp,SIGNAL(textChanged(QString)),this,SLOT(textCheck(QString)));
}

ScreenCap::~ScreenCap()
{
    delete ui;
}

void ScreenCap::showVerion(void)
{
    QString verinfo = QString::fromLocal8Bit("录屏传输 ") + QString::fromLocal8Bit(ScreenCapVersion);;
    ui->statusBar->showMessage(verinfo);
}

/*
刚开始并未进入此按钮中
第一次点击时，默认进入停止按钮，设置为传输中...
第二次点击时，进入开始按钮，设置为传输结束
*/
void ScreenCap::on_pushButtonStart_clicked()
{
    //    qDebug() << " pushbutton start clicked!!";
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
        BtnDisable();
        showTextStart();


        //开始传输
        StartCapScreen();
        BtnEnable();
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


}

//停止传输
void ScreenCap::StopCapScreen()
{


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
