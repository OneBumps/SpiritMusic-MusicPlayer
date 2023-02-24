#include "MiniPlayer.h"
#include <QFileDialog>
#include "ui_MiniPlayer.h"
#include "InitPlayer.h"
#include "AddListDialog.h"
#include <QStandardPaths>

MiniPlayer::MiniPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MiniPlayer)
{
    ui->setupUi(this);
    //初始化connect
    initConnect();
    //初始化控件
    initWidget();
    //SongSheet初始化
    initSongSheet();
}
//发送ui
Ui::MiniPlayer* MiniPlayer::getUi()
{
    return ui;
}

MiniPlayer::~MiniPlayer()
{
    delete ui;
}

//初始化connect
void MiniPlayer::initConnect()
{
    ui->MiniPlayer_btn_changebackground->hide();
    /*按钮位置移动*/
    connect(ui->searchoption,&QPushButton::pressed,this,&MiniPlayer::btnmove);
    connect(ui->addList,&QPushButton::pressed,this,&MiniPlayer::btnmove);
    connect(ui->MiniPlayer_QPushButton_delete,&QPushButton::pressed,this,&MiniPlayer::btnmove);
    connect(ui->MiniPlayer_btn_changebackground,&QPushButton::pressed,this,&MiniPlayer::btnmove);
    connect(ui->MiniPlayer_btn_changebackground,&QPushButton::released,this,&MiniPlayer::btnmoveback);
    connect(ui->MiniPlayer_btn_rename,&QPushButton::pressed,this,&MiniPlayer::btnmove);
    connect(ui->searchoption,&QPushButton::released,this,&MiniPlayer::btnmoveback);
    connect(ui->addList,&QPushButton::released,this,&MiniPlayer::btnmoveback);
    connect(ui->MiniPlayer_QPushButton_delete,&QPushButton::released,this,&MiniPlayer::btnmoveback);
    connect(ui->MiniPlayer_btn_rename,&QPushButton::released,this,&MiniPlayer::btnmoveback);
    //添加按钮的功能实现
    connect(ui->addList,&QPushButton::clicked,this,&MiniPlayer::addList);
    //实时搜索功能的实现
    connect(ui->musicSearch,&QLineEdit::textChanged,this,&MiniPlayer::searchList);
    //点击歌单列表响应数据库和showPage
    connect(ui->MiniPlayer_showList,&QListWidget::itemClicked,this,&MiniPlayer::addShowPage);
    //点击删除按钮开始删除表单
    connect(ui->MiniPlayer_QPushButton_delete,&QPushButton::clicked,this,[&]{
        if(ui->MiniPlayer_showList->currentRow() != -1)
        {
            //SQL操作：删除表
            InitPlayer::mediadb->deleteTable(ui->MiniPlayer_showList->currentItem()->text());
            //清除表
            ui->MiniPlayer_showList->clear();
            //刷新数据
            InitPlayer::mediadb->flash();
            //跟进搜索数据
            searchList(ui->musicSearch->text());
        }
    });
    //重命名表单
    connect(ui->MiniPlayer_btn_rename,&QPushButton::clicked,this,[&]{
        if(ui->MiniPlayer_showList->currentRow() != -1)
        {
            //弹出对话框
            AddListDialog *deleteList = new AddListDialog(AddListDialog::DialogType::RenameList);
            deleteList->setTitle("重命名音乐表单");
            deleteList->setWindowTitle("重命名音乐表单");
            deleteList->show();
            connect(deleteList,&AddListDialog::renameList,this,[deleteList, this](QString newname){
                if(InitPlayer::mediadb->renameTable(ui->MiniPlayer_showList->currentItem()->text(),newname))
                {
                    deleteList->close();
                    //清除表
                    ui->MiniPlayer_showList->clear();
                    //刷新数据
                    InitPlayer::mediadb->flash();
                    //跟进搜索数据
                    searchList(ui->musicSearch->text());
                }
                else//数据库表重复
                {
                    deleteList->setErrLabel_repeat();
                }
            });
        }
    });
    //切换图片
    connect(ui->MiniPlayer_btn_changebackground,&QPushButton::clicked,this,[&](){
        //创建文件对话框类对象，进行初始化
        QFileDialog backgroundFile(this);
        backgroundFile.setWindowTitle("更换背景");
        //单选文件
        backgroundFile.setFileMode(QFileDialog::ExistingFiles);
        backgroundFile.setDirectory(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0, QDir::homePath()));
        //弹出文件选择对话框（模态）
        backgroundFile.exec();
        //得到选择的文件
        qDebug() << QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).value(0, QDir::homePath()) + "/SpiritMusic/bg.jpg";
        qDebug() << QFile::copy("D:/52809c3ea23f484ab5a85344d5831306.jpg","E:/Pictures/SpiritMusic/bg.jpg");
    });
}
//初始化控件
void MiniPlayer::initWidget()
{
    ui->addList->setFixedSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->musicSearch->setFixedHeight(30 / InitPlayer::pixelRatio);
    ui->searchoption->setFixedSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->MiniPlayer_QPushButton_delete->setFixedSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->MiniPlayer_btn_rename->setFixedSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->MiniPlayer_btn_changebackground->setFixedSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    //绑定鼠标焦点在输入框
    ui->musicSearch->setFocus();
}
//SongSheet初始化
void MiniPlayer::initSongSheet()
{
    //得到数据库
    InitPlayer::mediadb->importTable(ui->MiniPlayer_showList);
}

void MiniPlayer::paintEvent(QPaintEvent *)
{
    //确保样式表生效
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
//添加歌单列表
void MiniPlayer::addList()
{
    AddListDialog *addlist = new AddListDialog;
    addlist->setTitle("添加音乐列表");
    addlist->setWindowTitle("添加音乐列表");
    addlist->show();
    connect(addlist,&AddListDialog::addList,this,[addlist, this](QString listname){
        if(InitPlayer::mediadb->createTable(listname))
        {
            addlist->close();
            //清除表
            ui->MiniPlayer_showList->clear();
            //刷新数据
            InitPlayer::mediadb->flash();
            //跟进搜索数据
            searchList(ui->musicSearch->text());
        }
        else//数据库表重复
        {
            addlist->setErrLabel_repeat();
        }
    });
}
//搜索功能
void MiniPlayer::searchList(QString text)
{
    if(!text.isEmpty())
    {
        ui->MiniPlayer_showList->clear();
        InitPlayer::mediadb->flash(text);
    }
    else
    {
        ui->MiniPlayer_showList->clear();
        InitPlayer::mediadb->flash();
    }
}
//点击列表将表中数据加入ShowPage
void MiniPlayer::addShowPage(QListWidgetItem *tablename)
{
    emit changeShowPage(InitPlayer::mediadb->importData(tablename->text()));
}
//按钮控件位置移动
void MiniPlayer::btnmove()
{
    QPushButton *btn = qobject_cast<QPushButton*>(QObject::sender());
    btn->move(btn->geometry().x() + 1, btn->geometry().y() + 1);
}
//按钮控件位置还原
void MiniPlayer::btnmoveback()
{
    QPushButton *btn = qobject_cast<QPushButton*>(QObject::sender());
    btn->move(btn->geometry().x() - 1, btn->geometry().y() - 1);
}
