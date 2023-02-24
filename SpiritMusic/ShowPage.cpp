#include "ShowPage.h"
#include "MusicControl.h"
#include "WindowFrame.h"
#include "ui_ShowPage.h"
#include <QStandardPaths>
#include "InitPlayer.h"
ShowPage::ShowPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowPage)
{
    ui->setupUi(this);
    //隐藏为做好的功能按钮-管理音乐列表
    hideButton();
    //初始化按键行为
    initActions();
}

void ShowPage::hideButton()
{
    ui->ShowPage_btn_manageMusic->hide();//未做好，不显示
    ui->ShowPage_btn_dontdelete->hide();//隐藏取消按钮
}
//初始化按键行为
void ShowPage::initActions()
{
    //初始化按键移动
    connect(ui->ShowPage_btn_manageMusic,&QPushButton::pressed,this,&ShowPage::btnmove);
    connect(ui->ShowPage_btn_addMusic,&QPushButton::pressed,this,&ShowPage::btnmove);
    connect(ui->ShowPage_btn_deleteMusic,&QPushButton::pressed,this,&ShowPage::btnmove);
    connect(ui->ShowPage_btn_dontdelete,&QPushButton::pressed,this,&ShowPage::btnmove);
    connect(ui->ShowPage_btn_dontdelete,&QPushButton::released,this,&ShowPage::btnmoveback);
    connect(ui->ShowPage_btn_deleteMusic,&QPushButton::released,this,&ShowPage::btnmoveback);
    connect(ui->ShowPage_btn_manageMusic,&QPushButton::released,this,&ShowPage::btnmoveback);
    connect(ui->ShowPage_btn_addMusic,&QPushButton::released,this,&ShowPage::btnmoveback);
    //添加文件进入列表(数据库)
    connect(ui->ShowPage_btn_addMusic,&QPushButton::clicked,this,&ShowPage::addMusicFile);
    //删除音乐列表
    connect(ui->ShowPage_btn_deleteMusic,&QPushButton::clicked,this,[&](){
        //判断是否进行删除模式
        if(ui->ShowPage_btn_deleteMusic->isChecked() == true)
        {
            //临时设置多选模式
            ui->ShowPage_tableView->setSelectionMode(QAbstractItemView::MultiSelection);
            //显示删除按钮
            ui->ShowPage_btn_dontdelete->show();
        }
        //进行确认删除操作
        else
        {
            //获取选中行
            QModelIndexList selected = ui->ShowPage_tableView->selectionModel()->selectedRows();
            //批量删除行
            for (int i = 0; i < selected.count(); i++) {
                int row = selected.at(i).row();
                model->removeRow(row);
            }
            //设置单选模式
            ui->ShowPage_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
            //隐藏取消按钮
            ui->ShowPage_btn_dontdelete->hide();
        }
    });
    //添加取消删除按钮
    connect(ui->ShowPage_btn_dontdelete,&QPushButton::clicked,this,[&](){
        //设置单选模式
        ui->ShowPage_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        //隐藏自身
        ui->ShowPage_btn_dontdelete->hide();
        //恢复正常模式
        ui->ShowPage_btn_deleteMusic->setChecked(false);
    });
    //双击播放歌曲
    connect(ui->ShowPage_tableView,&QTableView::doubleClicked,this,[&](const QModelIndex &index){
        //切换到对应表单
        currentTable = model;
        playMusic(index);
    });
}
//双击播放歌曲
void ShowPage::playMusic(const QModelIndex &index)
{
    //设置next、last和like可用
    WindowFrame::ui_normal->last->setEnabled(true);
    WindowFrame::ui_normal->next->setEnabled(true);
    WindowFrame::ui_normal->like->setEnabled(true);
    //获取音乐文件路径的列在第1列
    QModelIndex modelIndex = index.sibling(index.row(),1);
    //获取音乐文件路径,如果路径相同,则不重新加载
    if(currentMusicPath != currentTable->data(modelIndex,Qt::DisplayRole).toString())
    {
        currentMusicPath = currentTable->data(modelIndex,Qt::DisplayRole).toString();
        //控制播放
        if(music == nullptr)
        {
            //进入后第一次使用
            music = new MusicControl(currentMusicPath,WindowFrame::ui_normal);
        }
        else
        {
            //随后的使用
            music->initMusic(currentMusicPath);
        }
        //记录音乐列表信息索引
        musicIndex = index;
    }
}
//调用数据库
QWidget* ShowPage::importDB(QString name)
{
    //设置模型
    model = new QSqlTableModel;
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->setTable(name);
    model->select();
    //时长一列右对齐
    ui->ShowPage_tableView->setModel(model);
    setStyle();
    //存入表名
    tablename = model->tableName();
    //使得QStackWidgt中加入ShowPage
    return qobject_cast<QWidget*>(this);
}
//调整样式
void ShowPage::setStyle()
{
    //设置标题
    ui->showPage_Title->setText(model->tableName());
    //设置描述

    //设置表格显示列
    ui->ShowPage_tableView->setColumnHidden(0,true);
    ui->ShowPage_tableView->setColumnHidden(1,true);
    ui->ShowPage_tableView->setColumnHidden(5,true);
    ui->ShowPage_tableView->setColumnHidden(7,true);
    //设置高度
    for (int i = 0; i < ui->ShowPage_tableView->model()->rowCount(); ++i)
    {
        ui->ShowPage_tableView->setRowHeight(i, 40 / InitPlayer::pixelRatio);
    }
    //设置列宽
    ui->ShowPage_tableView->setColumnWidth(2,350 / InitPlayer::pixelRatio);
    ui->ShowPage_tableView->setColumnWidth(3,350 / InitPlayer::pixelRatio);
    ui->ShowPage_tableView->setColumnWidth(4,350 / InitPlayer::pixelRatio);
    //设置不显示全选按钮
    ui->ShowPage_tableView->horizontalHeader()->setSectionsClickable(false);
    //设置文本左对齐
    ui->ShowPage_tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
}

ShowPage::~ShowPage()
{
    delete ui;
}
//添加音乐到列表(数据库)
void ShowPage::addMusicFile()
{
    //创建文件对话框类对象，进行初始化
    QFileDialog filesDialog(this);
    //设置背景白色(已经无效)
    //filesDialog.setStyleSheet("QWidget{background: white;}");
    filesDialog.setWindowTitle("添加音乐");
    //选择多文件
    filesDialog.setFileMode(QFileDialog::ExistingFiles);
    filesDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath()));
    filesDialog.setNameFilters({"音乐 (*.mp3 *.flac *.ogg *.ogg *.wav *.m4a)","所有文件 (*)"});
    //弹出文件选择对话框（模态）
    filesDialog.exec();
    //得到选择的文件列表
    chooseFiles = filesDialog.selectedFiles();
    //存入数据库中
    for(int index = 0; index < chooseFiles.size(); ++index)
    {
        new MusicControl(chooseFiles.at(index));
    }
}

//按钮控件位置移动
void ShowPage::btnmove()
{
    QPushButton *btn = qobject_cast<QPushButton*>(QObject::sender());
    btn->move(btn->geometry().x() + 1, btn->geometry().y() + 1);
}
//按钮控件位置还原
void ShowPage::btnmoveback()
{
    QPushButton *btn = qobject_cast<QPushButton*>(QObject::sender());
    btn->move(btn->geometry().x() - 1, btn->geometry().y() - 1);
}

QSqlTableModel* ShowPage::model = nullptr;
QString ShowPage::tablename = "";
MusicControl* ShowPage::music = nullptr;
QModelIndex ShowPage::musicIndex;
QSqlTableModel* ShowPage::currentTable = nullptr;
QString ShowPage::currentMusicPath = "";
