#include "AddListDialog.h"
#include "qpainter.h"
#include "qstyleoption.h"
#include "ui_AddListDialog.h"

AddListDialog::AddListDialog(qint32 type,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddListDialog)
{
    ui->setupUi(this);
    //设置无框
    setWindowFlag(Qt::FramelessWindowHint,true);
    dialogType = type;
    //初始化connect
    connect(ui->TRUE,&QPushButton::clicked,this,&AddListDialog::setAction);
    connect(ui->FALSE,&QPushButton::clicked,this,[this]{
        close();
    });
    //设置隐藏label
    ui->label_wrong->hide();
}

void AddListDialog::setTitle(QString title)
{
    ui->AddListDialog_title->setText(title);
}

AddListDialog::~AddListDialog()
{
    delete ui;
}

void AddListDialog::paintEvent(QPaintEvent *)
{
    //确保样式表生效
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
//设置确认时的行为
void AddListDialog::setAction()
{
    if(!ui->listName->text().trimmed().isEmpty())
    {
        switch (dialogType)
        {
        case AddList:
            emit addList(ui->listName->text().trimmed());
            break;
        case RenameList:
            emit renameList(ui->listName->text().trimmed());
            break;
        }
    }
    else
    {
        setsetErrLabel_empty();
    }
}
//设置错误的显示标签
void AddListDialog::setErrLabel_repeat()
{
    ui->label_wrong->setText("名字重复了");
    ui->label_wrong->show();
}
//设置错误为输入为空
void AddListDialog::setsetErrLabel_empty()
{
    ui->label_wrong->setText("名字不要空");
    ui->label_wrong->show();
}
