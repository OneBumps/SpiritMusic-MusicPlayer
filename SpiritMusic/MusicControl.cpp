#include "MusicControl.h"
#include "InitPlayer.h"
#include "ShowPage.h"
#include "WindowFrame.h"
#include <QBuffer>
#include <QImageReader>
#include <QTemporaryFile>
#include <QFileInfo>
//专门用于播放
MusicControl::MusicControl(QString path,Ui::NormalPlayer *ui,QObject *parent)  :
    QObject{parent}
{
    this->ui_normal = ui;
    //创建媒体播放器
    player = new QMediaPlayer;
    //设置输出音频为系统选择
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    //获取音量
    audioOutput->setVolume(ui_normal->slider_volume->value());
    //初始化connect
    initConnect();
    //初始化音乐文件
    initMusic(path);
}

//专门用于获取元数据存表
MusicControl::MusicControl(QString path, QObject *parent)  :
    QObject{parent}
{
    this->musicPath = path;
    //创建媒体播放器
    player = new QMediaPlayer;
    //获取音乐文件
    player->setSource(QUrl::fromLocalFile(path));
    //及时获取文件元数据
    connect(player, &QMediaPlayer::metaDataChanged,this,&MusicControl::getMeta);
}

MusicControl::~MusicControl()
{

}
//初始化connect
void MusicControl::initConnect()
{
    //接收数据库的音乐信息
    connect(InitPlayer::mediadb,&MediaDataBase::musicMetaData,this,&MusicControl::connectMetaAndControls);
    //音乐文件发生变化时重置播放按钮和滑块的可用状态
    connect(player,&QMediaPlayer::mediaStatusChanged,this,&MusicControl::changeBtnAndSliderStyle);
    //音乐状态改变对按钮的控制
    connect(player,&QMediaPlayer::playbackStateChanged,this,[&](qint32 playbackState){
        qDebug() << "播放状态改变成了" << playbackState;
        if(playbackState == 1)
        {
            ui_normal->playControl->setChecked(true);
        }
        else
        {
            ui_normal->playControl->setChecked(false);
        }
    });
    //播放按钮对音频的控制
    connect(ui_normal->playControl,&QPushButton::toggled,this,[&](bool isChecked){
        if(isChecked)
        {
            player->play();
        }
        else
        {
            player->pause();
        }
    });
    //通过UI设置音量大小
    connect(ui_normal->slider_volume,&QSlider::valueChanged,this,[this](qint32 volume){
        audioOutput->setVolume((qreal)volume / 100.0);
    });
    /*绑定滑动条和音频播放进度*/
    //获取新的播放时长,改变进度条范围
    connect(player,&QMediaPlayer::durationChanged,ui_normal->slider_control,[&](qint64 pos){
        ui_normal->slider_control->setRange(0,pos);
    });
    //获取播放进度，改变进度条进度
    connect(player,&QMediaPlayer::positionChanged,ui_normal->slider_control,[&](qint64 position){
        //如果播放条没有被拖动，则改变进度且更新歌词
        if(!ui_normal->slider_control->isSliderDown())
        {
            ui_normal->slider_control->setValue(position);
            MusicPage::changeLyricPosition(position);
        }
        //改变NormalPlayer_QLabel_time的时间
        ui_normal->NormalPlayer_QLabel_time->setText(QTime::fromMSecsSinceStartOfDay(position).toString());
    });
    //使用进度条控制播放进度
    connect(ui_normal->slider_control, &QSlider::sliderReleased, this,[this]{
        //控制播放进度
        player->setPosition(ui_normal->slider_control->value());
    });
    //改变播放行为
    connect(WindowFrame::normalplayer,&NormalPlayer::MusicListControlChanged,this,[&](qint32 playStatic){
        if(playStatic == NormalPlayer::SingleCirculation)
        {
            player->setLoops(QMediaPlayer::Infinite);
        }
        else
        {
            player->setLoops(QMediaPlayer::Once);
        }
    });
}
//初始化音乐文件
void MusicControl::initMusic(QString path)
{
    //读入文件路径
    musicpath = path;
    //获取音乐文件,准备就绪
    player->setSource(QUrl::fromLocalFile(musicpath));
    //直接播放
    player->play();
    ui_normal->playControl->setChecked(true);
    //传入表名和音乐路径进行查找相应信息
    InitPlayer::mediadb->importMusicMetaData(ShowPage::currentTable->tableName(),musicpath);
}

//对接元数据与控件
void MusicControl::connectMetaAndControls(struct MetaPackage dataPackage)
{
    mediametadata = player->metaData();
    //设置标题
    ui_normal->title->setText(dataPackage.music);
    /*设置封面*/
    if(!dataPackage.cover.isEmpty())
    {
        //转为图片
        QBuffer buffer(&dataPackage.cover);
        buffer.open(QIODevice::ReadOnly);
        QImageReader reader(&buffer,"PNG");
        QImage image = reader.read();
        /*设置封面*/
        /*将数据存为临时图片文件*/
        QTemporaryFile *file = new QTemporaryFile;
        file->open();
        file->write(dataPackage.cover);
        file->close();
        //设置样式
        ui_normal->musicPicture->setStyleSheet(QString("QPushButton#musicPicture {border-image: url('%1');border-radius: 50%;}").arg(file->fileName()));
        MusicPage::changeCover(image);
    }
    else
    {
        ui_normal->musicPicture->setStyleSheet("QPushButton#musicPicture {border-radius: 50%;border-image: url(':/Icon/res/logo.svg');}");
        MusicPage::changeCover(QImage());
    }
    /*设置歌手，专辑名*/
    MusicPage::changeSinger(dataPackage.singer);
    MusicPage::changeAlbum(dataPackage.album);
    //设置我喜欢/红心控件
    if(dataPackage.isLiked)
    {
        ui_normal->like->setChecked(true);
    }
    else
    {
        ui_normal->like->setChecked(false);
    }
    /*设置歌词文件*/
    QFileInfo fileInfo(musicpath);
    MusicPage::changeLyric(fileInfo.dir().path() + "/" + fileInfo.baseName() + ".lrc");
}

//获取元数据
void MusicControl::getMeta()
{
    //获取元数据
    InitPlayer::mediadb->insertData(musicPath,player->metaData());
}

void MusicControl::changeBtnAndSliderStyle(qint32 mediaStatus)
{
    qDebug() << "媒体文件状态改变成" << mediaStatus;
    //无法播放媒体文件
    if(mediaStatus == QMediaPlayer::NoMedia || mediaStatus == QMediaPlayer::InvalidMedia)
    {
        ui_normal->playControl->setChecked(false);
        ui_normal->playControl->setEnabled(false);
        //文件无法播放，不显示播放条
        ui_normal->slider_control->setVisible(false);
    }
    //媒体播放到末尾，判断播放状态是否是列表循环
    else if(mediaStatus == QMediaPlayer::EndOfMedia && NormalPlayer::musicliststate == NormalPlayer::MusicListControl::ListLoop)
    {
        ShowPage::playMusic(ShowPage::musicIndex.siblingAtRow((ShowPage::musicIndex.row() + 1) % ShowPage::currentTable->rowCount()));
    }
    //正常播放
    else
    {
        ui_normal->playControl->setEnabled(true);
        ui_normal->slider_control->setVisible(true);
    }
}
//控制播放
void MusicControl::control()
{

}

Ui::NormalPlayer* MusicControl::ui_normal = nullptr;
