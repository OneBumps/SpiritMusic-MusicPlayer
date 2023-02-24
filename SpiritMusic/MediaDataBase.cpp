#include "MediaDataBase.h"
#include "InitPlayer.h"
#include "ShowPage.h"
#include <QVariant>
#include <QByteArray>
#include <QBuffer>
MediaDataBase::MediaDataBase(QObject *parent)
    : QObject{parent}
{

}
//连接数据库
void MediaDataBase::connectDB()
{
    //连接SpiritMusic数据库
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./SpiritMusic.db");
    db.setHostName("localhost");
    db.setUserName("OneBumps");
    db.setPassword("98428942");
    if (db.open())
    {
        qDebug() << "连接SpiritMusic数据库成功";
    }
    else
    {
        qFatal("连接数据库可能存在问题： %s", qPrintable(db.lastError().text()));
    }
    //单独建立“我喜欢”这张表
    QSqlQuery query;
    if(!query.exec("CREATE TABLE IF NOT EXISTS 我喜欢 (ID INTEGER, filepath TEXT NOT NULL, 歌曲 TEXT, 歌手 TEXT, 专辑名 TEXT, file_size INTEGER NOT NULL, 时长 VARCHAR, cover BLOB, UNIQUE (filepath, file_size))"))
    {
        qDebug() << "建立我喜欢这张表时出错：" << query.lastError();
    }
}
//导入数据库表
bool MediaDataBase::importTable(QListWidget*& listWidget,QString name,qint32 sort)
{
    //建立查询
    QSqlQuery query;
    //排序类型
    QString type;
    if(sort == FileName)
    {
        type = "name;";
    }
    if(sort == DateAndTime)
    {
        type = "date;";
    }
    //是否指定名称查询
    if(name.isEmpty())
    {
        //查询表，按指定排序方式排序，"我喜欢"保存在第一个位置，COLLATE指定中文排在英文前面(暂时没做)
        QString init = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY CASE name WHEN '我喜欢' THEN 0 ELSE 1 END, ";
        if(query.exec(init + type))
        {
            //保存指针
            this->list = listWidget;
        }
        else
        {
            qFatal("查询数据库可能存在问题： %s", qPrintable(query.lastError().text()));
            return false;
        }
    }
    else
    {
        if(!query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name LIKE '%" + name + "%' AND name NOT LIKE 'sqlite_%' ORDER BY CASE name WHEN '我喜欢' THEN 0 ELSE 1 END," + type))
        {
            qFatal("查询数据库可能存在问题： %s", qPrintable(query.lastError().text()));
            return false;
        }
    }
    //加载数据进入列表
    while (query.next()) {
        ListItem *item = new ListItem;
        item->setFixedSize(360 / InitPlayer::pixelRatio,60 / InitPlayer::pixelRatio);
        QListWidgetItem *listitem = new QListWidgetItem(listWidget);
        listitem->setSizeHint(QSize(360 / InitPlayer::pixelRatio,60 / InitPlayer::pixelRatio));
        item->setText(query.value(0).toString());
        //设置QListWidgetItem项，并保证不显示出来
        listitem->setText(query.value(0).toString());
        listitem->setFont(QFont("Arial",1));
        listitem->setForeground(QBrush(QApplication::palette().color(QPalette::Base)));
        //导入数据库表名
        listWidget->setItemWidget(listitem,item);
    }
    qDebug() << "加载/查询音乐列表成功";
    return true;
}
//创建表
bool MediaDataBase::createTable(QString table)
{
    QSqlQuery query;
    //创建音乐数据库，包含字段：id（主键），文件路径（必要），音乐名，作家，专辑名，文件大小（必要），音乐时长，封面（二进制存入）
    if(query.exec("CREATE TABLE \"" + table + "\" (ID INTEGER PRIMARY KEY AUTOINCREMENT, filepath TEXT NOT NULL, 歌曲 TEXT, 歌手 TEXT, 专辑名 TEXT, file_size INTEGER NOT NULL, 时长 VARCHAR, cover BLOB, UNIQUE (filepath, file_size))"))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//删除表
void MediaDataBase::deleteTable(QString table)
{
    QSqlQuery query;
    if(!query.exec("DROP TABLE \"" + table + "\""))
    {
        qDebug() << query.lastError();
    }
}
//重命名表
bool MediaDataBase::renameTable(QString table,QString newname)
{
    QSqlQuery query;
    if(query.exec("ALTER TABLE \"" + table + "\" RENAME TO \"" + newname + "\""))
    {
        return true;
    }
    else
    {
        return false;
    }
}
//从数据库重新导入数据
bool MediaDataBase::flash(QString name,qint32 sort)
{
    return importTable(list,name,sort);
}
//导入表中数据
QWidget* MediaDataBase::importData(QString name)
{
    ShowPage *musiclist = new ShowPage;
    return musiclist->importDB(name);
}
//添加表中数据
bool MediaDataBase::insertData(QString path, QMediaMetaData metadata)
{
    //确保表存在
    if(ShowPage::tablename != "")
    {
        //将艺术家列表转为字符串存储
        QStringList artistList= metadata.value(QMediaMetaData::ContributingArtist).toStringList();
        QString artists = "";
        for(int index = 0; index < artistList.size(); index++)
        {
            //艺术家重复时退出
            if(artists.compare(artistList.at(index)) == 0)
            {
                break;
            }
            if(index != 0)
            {
                artists +="、";
            }
            artists += artistList.at(index);
        }
        QSqlQuery query;
        QString preString = "INSERT INTO \"" + ShowPage::tablename + "\" (filepath, file_size, 歌曲";
        QString valueString = ") VALUES (?, ?, ?";
        //如果有元数据，则保存元数据
        if(!artists.isEmpty())
        {
            preString += ", 歌手";
            valueString += ", ?";
        }
        if(!metadata.stringValue(QMediaMetaData::AlbumTitle).isEmpty())
        {
            preString += ", 专辑名";
            valueString += ", ?";
        }
        if(metadata.value(QMediaMetaData::Duration).toInt())
        {
            preString += ", 时长";
            valueString += ", ?";
        }
        if(!metadata.value(QMediaMetaData::ThumbnailImage).value<QImage>().isNull())
        {
            preString += ", cover";
            valueString += ", ?";
        }
        //预处理数据
        query.prepare(preString + valueString + ")");
        //开始绑定数据
        query.addBindValue(path);
        query.addBindValue(path.size());
        if(!metadata.stringValue(QMediaMetaData::Title).isEmpty())
        {
            query.addBindValue(metadata.stringValue(QMediaMetaData::Title));
        }
        else
        {
            QFileInfo fileInfo(path);
            query.addBindValue(fileInfo.baseName());
        }
        if(!artists.isEmpty())
        {
            query.addBindValue(artists);
        }
        if(!metadata.stringValue(QMediaMetaData::AlbumTitle).isEmpty())
        {
            query.addBindValue(metadata.stringValue(QMediaMetaData::AlbumTitle));
        }
        if(metadata.value(QMediaMetaData::Duration).toInt())
        {
            qint64 duration = metadata.value(QMediaMetaData::Duration).toInt();
            //格式化时长
            QString strDuration = QString("%1").arg(duration / 60000, 2, 10, QChar('0')) + ":" + QString("%1").arg(duration / 1000 % 60, 2, 10, QChar('0'));
            query.addBindValue(strDuration);
        }
        if(!metadata.value(QMediaMetaData::ThumbnailImage).value<QImage>().isNull())
        {
            //二进制存储
            QByteArray data;
            //缓冲数据流
            QBuffer buffer(&data);
            buffer.open(QIODevice::WriteOnly);
            //将图片缓冲成二进制数据
            metadata.value(QMediaMetaData::ThumbnailImage).value<QImage>().save(&buffer, "PNG");
            query.addBindValue(data);
        }
        //SQL执行：添加音乐数据
        if(query.exec())
        {
            return true;
        }
        else
        {
            qDebug() << path + "添加失败";
            qDebug() << "SQL语句：" << query.lastQuery();
            qDebug() << "错误原因：" << query.lastError();
            return false;
        }
    }
    else
    {
        qDebug() << "表不存在";
        return false;
    }
}
//导入歌曲信息
bool MediaDataBase::importMusicMetaData(QString tablename,QString path)
{
    QSqlQuery query;
    /*SQL查询语句预处理*/
    query.prepare("SELECT cover, file_size, 歌曲, 歌手, 专辑名 FROM \"" + tablename + "\" WHERE ? = filepath");
    query.addBindValue(path);
    //执行：SQL查询
    if (query.exec() && query.first()) {
        //使用结构体打包数据
        MetaPackage data;
        //图片以二进制形式存入结构体
        data.cover = query.value("cover").toByteArray();
        //音乐文件大小存入结构体
        data.file_size = query.value("file_size").toInt();
        //歌曲名存入结构体
        data.music = query.value("歌曲").toString();
        //歌手存入结构体
        data.singer = query.value("歌手").toString();
        //专辑名存入结构体
        data.album = query.value("专辑名").toString();
        ///*SQL查询语句预处理*/
        query.prepare("SELECT filepath FROM 我喜欢 WHERE ? = filepath");
        query.addBindValue(path);
        //判断是否在我喜欢列表中
        if(query.exec() && query.first())
        {
            data.isLiked = true;
        }
        //发送结构体
        emit musicMetaData(data);
        return true;
    }
    else
    {
        qDebug() << query.lastQuery();
        qDebug() << "查询失败，原因：" << query.lastError();
        return false;
    }
}
//操作“我喜欢”这张表
void MediaDataBase::controlLikeTable(QString talbename,QString path,bool isLiked)
{
    QSqlQuery query;
    if(isLiked)
    {
        query.prepare("DELETE FROM 我喜欢 WHERE filepath = ?");
        query.addBindValue(path);
        if(!query.exec())
        {
            qDebug() << query.lastQuery();
            qDebug() << "删除失败，原因：" << query.lastError();
        }
    }
    else
    {
        /*添加临时表转移数据*/
        query.prepare("CREATE TEMPORARY TABLE tmp AS SELECT * FROM \"" + talbename + "\" WHERE ? = filepath");
        query.addBindValue(path);
        query.exec();
        //插入歌曲数据进入我喜欢表单中
        query.exec("INSERT INTO 我喜欢 SELECT * FROM tmp;");
        //删除临时表
        query.exec("DROP TABLE tmp");
    }
}
//关闭数据库
void MediaDataBase::closeDB()
{
    QSqlDatabase db = QSqlDatabase::database("./SpiritMusic.db");
    if (db.isOpen()) {
        db.close();
    }
}
