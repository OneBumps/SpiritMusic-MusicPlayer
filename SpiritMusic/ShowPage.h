#ifndef SHOWPAGE_H
#define SHOWPAGE_H

#include "MusicControl.h"
#include <QWidget>
#include <QSqlTableModel>
#include <QFileDialog>
namespace Ui {
class ShowPage;
}

class ShowPage : public QWidget
{
    Q_OBJECT

public:
    explicit ShowPage(QWidget *parent = nullptr);
    //隐藏为做好的功能按钮-管理音乐列表
    void hideButton();
    //初始化按键行为
    void initActions();
    //双击播放歌曲
    void static playMusic(const QModelIndex &index);
    //调用数据库
    QWidget* importDB(QString name);
    //调整样式
    void setStyle();
    ~ShowPage();
signals:
public:
    static QSqlTableModel *model;//设定的模型
    static QString tablename;//当前表名
    static QModelIndex musicIndex;//存储播放索引
    static QSqlTableModel *currentTable;//存储播放的表单
    static QString currentMusicPath;//当前播放歌曲的文件路径
private:
    Ui::ShowPage *ui;
    QStringList chooseFiles;//选择的音乐文件
    static MusicControl *music; //控制播放
private slots:
    //添加音乐到列表(数据库)
    void addMusicFile();
    //按钮控件位置移动
    void btnmove();
    //按钮控件位置还原
    void btnmoveback();
};

#endif // SHOWPAGE_H
