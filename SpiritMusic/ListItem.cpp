#include "ListItem.h"
#include "ui_ListItem.h"

ListItem::ListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItem)
{
    ui->setupUi(this);
}
ListItem::ListItem(QString text,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItem)
{
    ui->setupUi(this);
    ui->ListItem_text->setText(text);
}
 //设置图标
void ListItem::setIcon(QString iconPath)
{
    ui->ListItem_icon->setStyleSheet("ListItem_icon#QLabel{border-image: url('" + iconPath + "');border-radius: 50%;}");
}
//设置文字
void ListItem::setText(QString text)
{
    QFont font;
    font.setPixelSize(20);
    ui->ListItem_text->setFont(font);
    ui->ListItem_text->setText(text);
}

ListItem::~ListItem()
{
    delete ui;
}
