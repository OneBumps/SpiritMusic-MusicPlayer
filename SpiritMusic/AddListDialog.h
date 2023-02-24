#ifndef ADDLISTDIALOG_H
#define ADDLISTDIALOG_H

#include <QDialog>

namespace Ui {
class AddListDialog;
}

class AddListDialog : public QDialog
{
    Q_OBJECT
public:
    enum DialogType{
        AddList,
        RenameList
    };
    Q_ENUM(DialogType)
public:
    explicit AddListDialog(qint32 type = AddList, QWidget *parent = nullptr);
    //设置标题
    void setTitle(QString title);
    //设置错误为数据库表重复
    void setErrLabel_repeat();
    //设置错误为输入为空
    void setsetErrLabel_empty();
    ~AddListDialog();
private:
    //确保样式表生效
    void paintEvent(QPaintEvent *) override;
    //设置确认时的行为
    void setAction();
private:
    Ui::AddListDialog *ui;
    qint32 dialogType;
signals:
    //提供一个列表名
    QString addList(QString listname);
    QString renameList(QString listname);
};

#endif // ADDLISTDIALOG_H
