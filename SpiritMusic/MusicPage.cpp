#include "MusicPage.h"
#include "InitPlayer.h"
#include "ui_MusicPage.h"
#include <QTime>
#include <QTextCodec>
MusicPage::MusicPage(QWidget *parent) :
    QWidget(parent)
{
    /*绑定显示Ui*/
    ui = new Ui::MusicPage;
    ui->setupUi(this);
    //获取光标位置
    cursor = ui->MusicPage_lyric->textCursor();
    //初始化控件大小位置
    initWidget();
}
//切换封面
void MusicPage::changeCover(QImage cover)
{
    ui->MusicPage_cover->setPixmap(QPixmap::fromImage(cover));
    //自动缩放
    ui->MusicPage_cover->setScaledContents(true);
}
//切换歌词
bool MusicPage::changeLyric(QString path)
{
    //清空原来的歌词
    ui->MusicPage_lyric->clear();
    //设置歌词界面样式
    lyricStyle();
    /*打开歌词文件*/
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //标记为无歌词
        hadLyric = false;
        //添加标识
        ui->MusicPage_lyric->append("\n\n\n\n\n\n\n\n纯音乐/没有歌词文件");
        return false;
    }
    else
    {
        //标记为有歌词
        hadLyric = true;
        /*匹配歌词*/
        //得到正确文本格式,加入QMap容器
        matchLine(getCorrectUnicode(file));
        /*导入歌词到TextEdit控件*/
        importLrics();
        //切换光标到第一排
        cursor.movePosition(QTextCursor::Start);
        ui->MusicPage_lyric->setTextCursor(cursor);
        //重置currentKey
        currentKey = 10000000;
        return true;
    }
}

void MusicPage::importLrics()
{
    //插入页首空白(虽然有4个\n，但是包含8个Block)
    for(int i = 0 ; i < 4 ; i++)
    {
        ui->MusicPage_lyric->append("\n");
    }
    //添加标识
    QList<QString> lyricList = lyrics.values();
    for(int index = 0; index < lyricList.size(); index++)
    {
        //按行加入歌词
        ui->MusicPage_lyric->append(lyricList.at(index));
    }
    //插入页尾空白
    for(int i = 0 ; i < 4 ; i++)
    {
        ui->MusicPage_lyric->append("\n");
    }
}

MusicPage::~MusicPage()
{
    delete ui;
}
//切换歌手
void MusicPage::changeSinger(QString singer)
{
    QString init = "歌手：";
    if(!singer.isEmpty())
    {
        ui->MusicPage_singer->setText(init + singer);
    }
    else
    {
        ui->MusicPage_singer->setText(init + "无");
    }
}
//切换专辑名
void MusicPage::changeAlbum(QString album)
{
    QString init = "专辑名：";
    if(!album.isEmpty())
    {
        ui->MusicPage_album->setText(init + album);
    }
    else
    {
        ui->MusicPage_album->setText(init + "无");
    }
}
//切换歌词位置
void MusicPage::changeLyricPosition(qint64 position)
{
    /*记录迭代器指向目标歌词位置*/
    QMap<qint64,QString>::iterator it;
    //保证迭代器不会越界指向
    if(lyrics.lowerBound(position) == lyrics.begin())
    {
        it = lyrics.begin();
    }
    else
    {
        it = --lyrics.lowerBound(position);
    }
    if(it.key() != currentKey)
    {
        currentKey = it.key();
        //清空原来的歌词
        ui->MusicPage_lyric->clear();
        //重新设置歌词样式
        lyricStyle();
        //重新刷新歌词
        importLrics();
        /*设置选中样式*/
        cursor.movePosition(QTextCursor::Start);
        //从歌词第一段开始索引
        int index = 8;
        for (auto findit = lyrics.begin(); findit.key() != it.key(); ++findit) {
            index++;
        }
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, index);
        cursor.select(QTextCursor::BlockUnderCursor);
        //设置选中后的样式
        QTextCharFormat format;
        format.setFontPointSize(25);
        QColor color(101, 126, 191);
        format.setForeground(color);
        format.setFontWeight(QFont::Bold);
        cursor.mergeCharFormat(format);
        //传回光标
        ui->MusicPage_lyric->setTextCursor(cursor);
        //移动到合适位置
        cursor.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor,8);
        cursor.movePosition(QTextCursor::StartOfLine,QTextCursor::MoveAnchor);
        //传回光标
        ui->MusicPage_lyric->setTextCursor(cursor);
    }
}

//发送ui
Ui::MusicPage* MusicPage::getUi()
{
    return ui;
}

//初始化控件大小位置
void MusicPage::initWidget()
{
    ui->MusicPage_cover->setFixedSize(350 / InitPlayer::pixelRatio,350 / InitPlayer::pixelRatio);
    ui->MusicPage_lyric->setFixedHeight(500 / InitPlayer::pixelRatio);
}
//设置歌词界面样式
void MusicPage::lyricStyle()
{
    //设置歌词水平居中
    ui->MusicPage_lyric->setAlignment(Qt::AlignCenter);
    //设置歌词大小
    QTextCharFormat format;
    format.setFontPointSize(14);
    cursor.mergeCharFormat(format);
    ui->MusicPage_lyric->setTextCursor(cursor);
}

//匹配歌词行
void MusicPage::matchLine(QString allLyrics)
{
    //先清空容器
    lyrics.clear();
    //分割每一段
    QStringList lines = allLyrics.split("\n");
    for (const QString &line : lines) {
        QRegularExpressionMatch match = reg.match(line);
        if (match.hasMatch()) {
            //匹配时间戳
            qint64 time = QTime::fromString(match.captured(1), "mm:ss").msecsSinceStartOfDay();
            //匹配行内歌词
            QString lyric = match.captured(2);
            //非空歌词放入容器内
            if(!lyric.isEmpty())
            {
                lyrics.insert(time, lyric);
            }
        }
    }
}
//得到正确的文本格式（仅支持识别utf-8和gbk）
QString MusicPage::getCorrectUnicode(QFile &file)
{
    //获取头特征码(可能会误判)
    QByteArray test = file.read(3);
    if(test.startsWith("\xEF\xBB\xBF"))//Unicode编码格式
    {
        qDebug() << "Unicode编码";
        QTextStream stream(&file);
        return stream.readAll();
    }
    else//GBK编码格式
    {
        qDebug() << "GBK编码";
        //将GBK格式的文本转换为Unicode编码
        QTextDecoder *decoder = QTextCodec::codecForName("GBK")->makeDecoder();
        return decoder->toUnicode(file.readAll());
    }
}

Ui::MusicPage* MusicPage::ui = nullptr;
QTextCursor MusicPage::cursor;
QMap<qint64, QString> MusicPage::lyrics;
QRegularExpression MusicPage::reg("\\[(\\d{2}:\\d{2})\\.\\d{2}\\](.+)");
bool MusicPage::hadLyric = false;
qint64 MusicPage::currentKey = -1;
