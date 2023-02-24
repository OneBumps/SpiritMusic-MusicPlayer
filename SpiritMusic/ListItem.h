#ifndef LISTITEM_H
#define LISTITEM_H

#include <QWidget>

namespace Ui {
class ListItem;
}

class ListItem : public QWidget
{
    Q_OBJECT

public:
    explicit ListItem(QWidget *parent = nullptr);
    explicit ListItem(QString text,QWidget *parent = nullptr);
    //设置图标
    void setIcon(QString iconPath);
    //设置文字
    void setText(QString text);
    ~ListItem();

private:
    Ui::ListItem *ui;
};

#endif // LISTITEM_H
