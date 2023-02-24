#include "InitPlayer.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    /*应用QSS样式表*/
    QFile res_file(":/Frame/res/SpiritMusic.qss");
    if(res_file.open(QIODevice::ReadOnly))
    {
        a.setStyleSheet(res_file.readAll());
    }
    res_file.setFileName(":/Frame/res/Cover.qss");
    if(res_file.open(QIODevice::ReadOnly))
    {
        a.setStyleSheet(a.styleSheet() + res_file.readAll());
    }
    res_file.setFileName(":/Icon/res/Icon.qss");
    if(res_file.open(QIODevice::ReadOnly))
    {
        a.setStyleSheet(a.styleSheet() + res_file.readAll());
    }
    res_file.close();
    /*进入初始化步骤*/
    InitPlayer initplayer;
    initplayer.initPlayer();
    return a.exec();
}
