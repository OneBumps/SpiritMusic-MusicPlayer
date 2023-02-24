#include "NormalPlayer.h"
#include <QPropertyAnimation>
#include "InitPlayer.h"
#include "ui_NormalPlayer.h"
#include "ShowPage.h"
NormalPlayer::NormalPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NormalPlayer)
{
    ui->setupUi(this);
    //初始化connect
    initConnect();
    //初始化控件
    initWidget();
    //准备完成
    show();
}
//发送ui
Ui::NormalPlayer* NormalPlayer::getUi()
{
    return ui;
}

//设置ShowPage
void NormalPlayer::setShowPage(QWidget *page, qint32 ShowPageType)
{
    if(ShowPageType == ShowPageType::TypeMusicList)
    {
        if(ui->stackedWidget->currentIndex() == TypeMusicList)
        {
            //移除原来的ShowPage
            ui->stackedWidget->removeWidget(ui->stackedWidget->widget(TypeMusicList));
        }
        ui->stackedWidget->insertWidget(TypeMusicList,page);
        ui->stackedWidget->setCurrentIndex(TypeMusicList);
    }
    else //ShowPageType == ShowPageType::TypeMusicPage
    {
        if(ui->stackedWidget->currentIndex() == TypeMusicPage)
        {
            ui->stackedWidget->setCurrentIndex(TypeMusicList);
        }
        else
        {
            ui->stackedWidget->setCurrentIndex(TypeMusicPage);
        }
    }
}

NormalPlayer::~NormalPlayer()
{
    delete ui;
}

//初始化connect
void NormalPlayer::initConnect()
{
    /*按钮位置移动*/
    connect(ui->last,&QPushButton::pressed,this,&NormalPlayer::btnmove);
    connect(ui->next,&QPushButton::pressed,this,&NormalPlayer::btnmove);
    connect(ui->volume,&QPushButton::pressed,this,&NormalPlayer::btnmove);
    connect(ui->order,&QPushButton::pressed,this,&NormalPlayer::btnmove);
    connect(ui->like,&QPushButton::pressed,this,&NormalPlayer::btnmove);
    connect(ui->playControl,&QPushButton::pressed,this,&NormalPlayer::btnmove);
    connect(ui->mute,&QPushButton::pressed,this,&NormalPlayer::btnmove);
    connect(ui->last,&QPushButton::released,this,&NormalPlayer::btnmoveback);
    connect(ui->next,&QPushButton::released,this,&NormalPlayer::btnmoveback);
    connect(ui->volume,&QPushButton::released,this,&NormalPlayer::btnmoveback);
    connect(ui->order,&QPushButton::released,this,&NormalPlayer::btnmoveback);
    connect(ui->like,&QPushButton::released,this,&NormalPlayer::btnmoveback);
    connect(ui->playControl,&QPushButton::released,this,&NormalPlayer::btnmoveback);
    connect(ui->mute,&QPushButton::released,this,&NormalPlayer::btnmoveback);
    //音量控制控件组显示
    connect(ui->volume,&QPushButton::toggled,this,[this](bool isChecked){
        if(isChecked)
        {
            ui->NormalPlayer_frame_volume->setHidden(false);
        }
        else
        {
            ui->NormalPlayer_frame_volume->setHidden(true);
        }
    });
    //控制音量条与静音按钮同步
    connect(ui->slider_volume,&QSlider::valueChanged,this,[this](qint32 volume){
        if(volume == 0)
        {
            ui->mute->setChecked(true);
        }
        else
        {
            ui->mute->setChecked(false);
        }
    });
    //设置点击静音按钮后改变滑动条
    connect(ui->mute,&QPushButton::clicked,this,[this]{
        if(ui->mute->isChecked())
        {
            recodeVolume = ui->slider_volume->value();
            ui->slider_volume->setValue(0);
        }
        else
        {
            ui->slider_volume->setValue(recodeVolume);
        }
    });
    /*控制上一曲和下一曲*/
    connect(ui->last,&QPushButton::clicked,this,[&]{
        //防止越界
        if(ShowPage::musicIndex.row() == 0)
        {
            ShowPage::playMusic(ShowPage::musicIndex.siblingAtRow(ShowPage::currentTable->rowCount() - 1));
        }
        else
        {
            ShowPage::playMusic(ShowPage::musicIndex.siblingAtRow((ShowPage::musicIndex.row() - 1) % ShowPage::currentTable->rowCount()));
        }
    });
    connect(ui->next,&QPushButton::clicked,this,[&]{
        ShowPage::playMusic(ShowPage::musicIndex.siblingAtRow((ShowPage::musicIndex.row() + 1) % ShowPage::currentTable->rowCount()));
    });
    //监控播放列表改变
    connect(ui->order,&QPushButton::clicked,this,&NormalPlayer::changeListControl);
    //点击musicPicture时改变QStackWidget容器为MusicPage
    connect(ui->musicPicture,&QPushButton::clicked,this,[&]{
            setShowPage(musicpage,ShowPageType::TypeMusicPage);
    });
    //添加删除红心对数据库同步操作
    connect(ui->like,&QPushButton::clicked,this,[&]{
        if(ui->like->isChecked())
        {//放入我喜欢表中
            InitPlayer::mediadb->controlLikeTable(ShowPage::currentTable->tableName(),ShowPage::currentMusicPath,false);
        }
        else
        {//从表中删除
            InitPlayer::mediadb->controlLikeTable(ShowPage::currentTable->tableName(),ShowPage::currentMusicPath,true);
        }
    });
}

void NormalPlayer::initWidget()
{
    /*初始化控件*/
    ui->musicPicture->setFixedSize(100 / InitPlayer::pixelRatio,100 / InitPlayer::pixelRatio);
    ui->like->setFixedSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->order->setFixedSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->last->setMinimumSize(60 / InitPlayer::pixelRatio,60 / InitPlayer::pixelRatio);
    ui->playControl->setMinimumSize(60 / InitPlayer::pixelRatio,60 / InitPlayer::pixelRatio);
    ui->next->setMinimumSize(60 / InitPlayer::pixelRatio,60 / InitPlayer::pixelRatio);
    ui->volume->setMinimumSize(40 / InitPlayer::pixelRatio,40 / InitPlayer::pixelRatio);
    ui->mute->setMinimumSize(40 / InitPlayer::pixelRatio,40 / InitPlayer::pixelRatio);
    ui->slider_volume->setMinimumWidth(150 / InitPlayer::pixelRatio);
    ui->slider_control->setFixedHeight(60 / InitPlayer::pixelRatio);
    /*隐藏音量控制时保留尺寸,顺便先隐藏*/
    QSizePolicy sp_retain = ui->NormalPlayer_frame_volume->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    ui->NormalPlayer_frame_volume->setSizePolicy(sp_retain);
    ui->NormalPlayer_frame_volume->setVisible(false);
    /*设置隐藏播放控制条时保留尺寸,顺便先隐藏*/
    sp_retain = ui->slider_control->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    ui->slider_control->setSizePolicy(sp_retain);
    ui->slider_control->setVisible(false);
    /*设置静音按钮样式与滑动条对齐*/
    if(ui->slider_volume->value() == 0)
    {
        ui->mute->setChecked(true);
    }
    else
    {
        ui->mute->setChecked(false);
    }
    //设置last、next和like按钮不可用
    ui->last->setEnabled(false);
    ui->next->setEnabled(false);
    ui->like->setEnabled(false);
    //初始化歌曲详情界面并加入QStackWidget隐藏
    musicpage = new MusicPage;
    ui->stackedWidget->insertWidget(TypeMusicPage,musicpage);
}

//按钮控件位置移动
void NormalPlayer::btnmove()
{
    QPushButton *btn = qobject_cast<QPushButton*>(QObject::sender());
    btn->move(btn->geometry().x() + 1, btn->geometry().y() + 1);
}
//按钮控件位置还原
void NormalPlayer::btnmoveback()
{
    QPushButton *btn = qobject_cast<QPushButton*>(QObject::sender());
    btn->move(btn->geometry().x() - 1, btn->geometry().y() - 1);
}


//确保样式表生效
void NormalPlayer::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
//改变列表状态
void NormalPlayer::changeListControl()
{
    //获取之后的状态
    musicliststate = (musicliststate + 1) % 3;
    //改变行为
    switch (musicliststate) {
    case MusicListControl::ListLoop:
        //设置图标
        ui->order->setStyleSheet("QPushButton#order{image: url(:/MusicControls/res/playmusic-xunhuanbofang.svg)}");
        break;
    case MusicListControl::PauseAfterPlay:
        //设置图标
        ui->order->setStyleSheet("QPushButton#order{image: url(:/MusicControls/res/playmusic-bowanzanting.svg)}");
        break;
    case MusicListControl::SingleCirculation:
        //设置图标
        ui->order->setStyleSheet("QPushButton#order{image: url(:/MusicControls/res/playmusic-danquxunhuan.svg)}");
        break;
    }
    emit MusicListControlChanged(musicliststate);
}

qint32 NormalPlayer::musicliststate = MusicListControl::ListLoop;
