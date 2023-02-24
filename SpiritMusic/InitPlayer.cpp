#include "InitPlayer.h"
#include "ui_StartMask.h"

InitPlayer::InitPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InitPlayer)
{
    //载入MaskUI
    ui->setupUi(this);
    //设置标题
    setWindowTitle("Spirit  Music");
}

InitPlayer::~InitPlayer()
{
    delete ui;
}

//加入遮罩
void InitPlayer::addStartMask(QRect &maskRect,QString text)
{
    /*设置遮罩窗体样式*/
    setWindowFlag(Qt::FramelessWindowHint,true);//设置窗体无边框
    setWindowFlag(Qt::WindowStaysOnTopHint,true);//窗体总是顶层显示
    //设置遮罩窗体大小和位置
    setGeometry(maskRect);
    //设置字体显示
    if(text.isEmpty())
    {
        ui->title->setVisible(false);
    }
    else
    {
        ui->title->setText(text);
    }
    //展示遮罩
    show();
}
//移除遮罩
void InitPlayer::deleteStartMask()
{
    QTimer::singleShot(1000,this,[&]{
        qDebug() << "初始化播放器完成";
        close();
    });
}
//第一次进入进行初始化操作
void InitPlayer::initPlayer()
{
    //初始化窗体
    initWindow();
    //初始化数据库
    initMusicDB();
    //初始化播放器主框体
    initPlayerWindow();
    //删除遮罩
    deleteStartMask();
}

//初始化窗体
void InitPlayer::initWindow()
{
    /*获取显示分辨率*/
    //获取像素密度
    pixelRatio = screen()->devicePixelRatio();
    //获取屏幕像素大小
    QScreen *screen = QGuiApplication::primaryScreen();
    this->screenGeometry = screen->geometry();
    /*添加遮罩*/
    //设置遮罩矩形值
    QRect maskRect((screenGeometry.width() - 1600 / pixelRatio) / 2 / pixelRatio,(screenGeometry.height() - 845  / pixelRatio) / 2 / pixelRatio,1600 / pixelRatio,845 / pixelRatio);
    addStartMask(maskRect,"Spirit  Music");
}

//初始化播放器主框体
void InitPlayer::initPlayerWindow()
{
    windowframe = new WindowFrame;
}

void InitPlayer::paintEvent(QPaintEvent *)
{
    //确保样式表生效
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
//初始化数据库
void InitPlayer::initMusicDB()
{
    mediadb = new MediaDataBase;
    //连接数据库
    mediadb->connectDB();
}
//设置圆角
void InitPlayer::resizeEvent(QResizeEvent *e)
{
    QBitmap bmp(this->size());
    bmp.fill();
    QPainter painter(&bmp);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
    //此处可修改圆角矩形大小，也可以绘制为其他想要的形状，比如圆形
    painter.drawRoundedRect(bmp.rect(), 35, 35);
    //该函数便使得QBitmap上像素为1的区域才显示出来
    setMask(bmp);
}

//初始化static变量
qreal InitPlayer::pixelRatio = 1.0;
QRect InitPlayer::screenGeometry = QRect(100, 100, 100, 100);
MediaDataBase* InitPlayer::mediadb = nullptr;
