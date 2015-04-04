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
    QString verinfo = QString::fromLocal8Bit("¼������ ") + QString::fromLocal8Bit(ScreenCapVersion);;
    ui->statusBar->showMessage(verinfo);
}

/*
�տ�ʼ��δ����˰�ť��
��һ�ε��ʱ��Ĭ�Ͻ���ֹͣ��ť������Ϊ������...
�ڶ��ε��ʱ�����뿪ʼ��ť������Ϊ�������
*/
void ScreenCap::on_pushButtonStart_clicked()
{
    //    qDebug() << " pushbutton start clicked!!";
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
        BtnStopPix();
        BtnDisable();
        showTextStart();


        //��ʼ����
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

//��ʼ����
void ScreenCap::StartCapScreen()
{


}

//ֹͣ����
void ScreenCap::StopCapScreen()
{


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
