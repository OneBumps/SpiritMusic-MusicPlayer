#ifndef MEDIADATABASE_H
#define MEDIADATABASE_H

#include <QObject>
//数据库
#include <QSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QWidget>
#include <QListWidget>//传入ListWidget进行控制
#include <QListWidgetItem>
#include <QMediaMetaData>
//结构体用于发送元数据包
struct MetaPackage{
    int file_size;
    QByteArray cover;
    QString music;
    QString singer;
    QString album;
    bool isLiked = false;//是否存储在“我喜欢”列表中
};

class MediaDataBase : public QObject
{
    Q_OBJECT
public:
    //排序
    enum ListSort{
        FileName,//默认按照文件名
        DateAndTime
    };
    Q_ENUM(ListSort)
public:
    explicit MediaDataBase(QObject *parent = nullptr);
    //连接数据库
    void connectDB();
    //导入数据库表,item项仅保留样式
    bool importTable(QListWidget*& listWidget,QString name = "", qint32 sort = FileName);
    //创建表
    bool createTable(QString table);
    //删除表
    void deleteTable(QString table);
    //重命名表
    bool renameTable(QString table, QString newname);
    //从数据库重新导入数据（排序默认为按表名）
    bool flash(QString name = "",qint32 sort = FileName);
    //导入表中数据
    QWidget* importData(QString name);
    //添加表中数据
    bool insertData(QString path,QMediaMetaData metadata);
    //导入歌曲信息
    bool importMusicMetaData(QString tablename,QString path);
    //操作“我喜欢”这张表
    void controlLikeTable(QString talbename, QString path, bool isLiked);
    //关闭数据库
    void closeDB();
private:

signals:
    //发送结构体
    void musicMetaData(MetaPackage data);
private:
    /*保存样式*/
    QListWidget *list;//控制列表
};

#endif // MEDIADATABASE_H
