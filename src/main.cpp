#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationDisplayName("智能车图像上位机");
    MainWindow w;
    w.show();
    return a.exec();
}
