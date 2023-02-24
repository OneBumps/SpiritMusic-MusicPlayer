#ifndef MINIPLAYER_H
#define MINIPLAYER_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QDialog>
#include <QStandardItemModel>
#include <QPixmap>
#include "ListItem.h"
#include <QListWidgetItem>

namespace Ui {
class MiniPlayer;
}

class MiniPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit MiniPlayer(QWidget *parent = nullptr);
    //发送ui
    Ui::MiniPlayer* getUi();
    ~MiniPlayer();

private:
    Ui::MiniPlayer *ui;
    //初始化connect
    void initConnect();
    //初始化控件
    void initWidget();
    //SongSheet初始化
    void initSongSheet();
    //确保样式表生效
    void paintEvent(QPaintEvent *) override;
signals:
    //改变ShowPage
    void changeShowPage(QWidget*);
public slots:
    //点击列表将表中数据加入ShowPage
    void addShowPage(QListWidgetItem *tablename);
private slots:
    //添加歌单列表
    void addList();
    //搜索功能
    void searchList(QString text);
    //按钮控件位置移动
    void btnmove();
    //按钮控件位置还原
    void btnmoveback();
private:
    ListItem *item;
};

#endif // MINIPLAYER_H
