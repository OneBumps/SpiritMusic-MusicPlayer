#ifndef MUSICCONTROL_H
#define MUSICCONTROL_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QTimer>
#include <QVariant>
//控制UI变动
#include "MediaDataBase.h"
#include "ui_NormalPlayer.h"
#include "ui_MiniPlayer.h"

class MusicControl : public QObject
{
    Q_OBJECT
public:
public:
    //专门用于播放
    explicit MusicControl(QString path, Ui::NormalPlayer *ui, QObject *parent = nullptr);
    //专门用于获取元数据存表
    explicit MusicControl(QString path, QObject *parent = nullptr);
    //控制播放
    void control();
    //初始化音乐文件
    void initMusic(QString path);
    //对接元数据与控件
    void connectMetaAndControls(struct MetaPackage dataPackage);
    ~MusicControl();
private:
    //初始化connect
    void initConnect();
private slots:
    //改变按钮和播放条样式
    void changeBtnAndSliderStyle(qint32 mediaStatus);
    //获取元数据
    void getMeta();
public:
    QString musicpath;//文件路径
    QMediaMetaData mediametadata;//音乐文件元数据
private:
    static Ui::NormalPlayer *ui_normal;//管理普通播放器UI
    Ui::MiniPlayer *ui_mini;//管理迷你播放器UI
    QMediaPlayer *player;//媒体接入处理
    QAudioOutput *audioOutput;//音频输出处理
    QString musicPath;//记录音乐路径
signals:

};

#endif // MUSICCONTROL_H
