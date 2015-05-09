#include <QtGui/QApplication>
#include "screencap.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScreenCap w;
    w.show();
    
    return a.exec();
}
