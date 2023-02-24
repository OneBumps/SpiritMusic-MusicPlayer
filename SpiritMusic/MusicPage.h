#ifndef MUSICPAGE_H
#define MUSICPAGE_H

#include <QWidget>
#include <QTextCursor>
#include <QFile>

namespace Ui {
class MusicPage;
}

class MusicPage : public QWidget
{
    Q_OBJECT

public:
    explicit MusicPage(QWidget *parent = nullptr);
    //切换封面
    static void changeCover(QImage cover);
    //切换歌词
    static bool changeLyric(QString path);
    //导入歌词
    static void importLrics();
    //切换歌手
    static void changeSinger(QString singer);
    //切换专辑名
    static void changeAlbum(QString album);
    //切换歌词位置
    static void changeLyricPosition(qint64 position);
    //发送ui
    Ui::MusicPage* getUi();
    ~MusicPage();
private:
    static Ui::MusicPage *ui;
    //初始化控件大小位置
    void initWidget();
    //设置歌词界面样式
    static void lyricStyle();
    //存入匹配的时间标签和歌词
    static void matchLine(QString lyrics);
    //得到正确的文本格式
    static QString getCorrectUnicode(QFile &file);
signals:
public slots:
public:
    static bool hadLyric;//有无歌词
    static qint64 currentKey;//记录下一个key值
private:
    static QTextCursor cursor;//保存歌词光标
    static QMap<qint64, QString> lyrics;//保存时间标签和歌词
    static QRegularExpression reg;//匹配规则
};

#endif // MUSICPAGE_H
