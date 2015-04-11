#ifndef SCREENCAP_H
#define SCREENCAP_H

#define __STDC_CONSTANT_MACROS


#include <QMainWindow>


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
private slots:
    void on_pushButtonStart_clicked();
    void textCheck(QString str);//检测文本变化
private:
    void showTextStart();
    void showTextStop();
    void showStateBarInfo(const char *pstr);
    void showVerion(void);
    void BtnStartPix(void);
    void BtnStopPix(void);
    void BtnSetPix(QString str);
    void BtnEnable(void);
    void BtnDisable(void);
    void StartCapScreen();
    void StopCapScreen();
private:
    Ui::ScreenCap *ui;
    static unsigned long capThreadAddr;

};

#endif // SCREENCAP_H
