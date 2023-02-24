#ifndef NORMALPLAYER_H
#define NORMALPLAYER_H


#include "MiniPlayer.h"
#include "MusicPage.h"
#include <QWidget>
#include <QStyleOption>
#include <QPainter>
namespace Ui {
class NormalPlayer;
}

class NormalPlayer : public QWidget
{
    Q_OBJECT
public:
    enum ShowPageType{
        TypeMusicPage,
        TypeMusicList
    };
    enum MusicListControl{
        SingleCirculation,//单曲循环
        PauseAfterPlay,//播完暂停
        ListLoop,//列表循环(默认)
    };
    Q_ENUM(ShowPageType)
    Q_ENUM(MusicListControl)
public:
    explicit NormalPlayer(QWidget *parent = nullptr);
    //发送ui
    Ui::NormalPlayer* getUi();
    //设置ShowPage
    void setShowPage(QWidget *page,qint32 ShowPageType = TypeMusicList);
    ~NormalPlayer();
private:
    //初始化connect
    void initConnect();
    //初始化控件
    void initWidget();
    //调整音量控件
    void setWidgetVolume(qint32 volume);
    //确保样式表生效
    void paintEvent(QPaintEvent *);
signals:
    void volume_mute();//静音
    void MusicListControlChanged(qint32);//列表状态改变
private slots:
    //改变列表状态
    void changeListControl();
    //按钮控件位置移动
    void btnmove();
    //按钮控件位置还原
    void btnmoveback();
public:
    static qint32 musicliststate;//记录音乐列表播放状态
private:
    Ui::NormalPlayer *ui;
    qint32 recodeVolume = 0;//记录静音前声音大小
    MiniPlayer *miniplayer;//存储MiniPlayer类
    MusicPage *musicpage;//存储MusicPage
};
#endif // NORMALPLAYER_H
