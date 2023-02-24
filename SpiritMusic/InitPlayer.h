#ifndef INITPLAYER_H
#define INITPLAYER_H

#include "WindowFrame.h"

#include <QWidget>
#include <QScreen>//获取屏幕信息
#include <QGuiApplication>
#include <QThread>//多线程机制
#include <QTimer>
#include <QApplication>
#include "MediaDataBase.h"//引入数据库

namespace Ui {
class InitPlayer;
}

class InitPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit InitPlayer(QWidget *parent = nullptr);
    ~InitPlayer();
    //第一次进入进行初始化操作
    void initPlayer();
    //加入遮罩
    void addStartMask(QRect &maskRect,QString text);
    //移除遮罩
    void deleteStartMask();
private:
    Ui::InitPlayer *ui;
    //初始化窗体
    void initWindow();
    //初始化数据库
    void initMusicDB();
    //初始化播放器主框体
    void initPlayerWindow();
    //确保样式表生效
    void paintEvent(QPaintEvent *) override;
public:
    WindowFrame *windowframe;//主框架
    static qreal pixelRatio;//屏幕像素密度
    static QRect screenGeometry;//屏幕像素大小
    static MediaDataBase *mediadb;//数据库指针
    void resizeEvent(QResizeEvent *e) override;//设置圆角
private:
signals:

private slots:

};

#endif // INITPLAYER_H
