#include "WindowFrame.h"
#include "ui_NormalPlayer.h"
#include "ui_MiniPlayer.h"
#include "ui_WindowFrame.h"
#include "InitPlayer.h"//仅在CPP文件中使用，防止递归循环

WindowFrame::WindowFrame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WindowFrame)
{
    ui->setupUi(this);
    //初始化调度指针
    initPointer();
    //设置标题
    setWindowTitle("Spirit  Music");
    //初始化connect
    initConnect();
    //初始化窗体
    initWindow();
    //初始化控件
    initWidget();
    //完成
    QTimer::singleShot(500,this,[this]{
        show();
    });
}

WindowFrame::~WindowFrame()
{
    delete ui;
}
//初始化调度指针
void WindowFrame::initPointer()
{
    //初始化子Ui指针
    normalplayer = getNormalPlayerClass();
    miniplayer = getMiniPlayerClass();
    //获取Ui
    ui_normal = normalplayer->getUi();
    ui_mini = miniplayer->getUi();
}

//初始化connect
void WindowFrame::initConnect()
{
    //切换框架响应
    connect(this,&WindowFrame::playerFrameChanged,this,&WindowFrame::initTargetFrame);
    //功能一按钮响应
    connect(ui->btn_windowControl_1,&QPushButton::pressed,this,&WindowFrame::btnmove);
    connect(ui->btn_windowControl_1,&QPushButton::released,this,&WindowFrame::btnmoveback);
    connect(ui->btn_windowControl_1,&QPushButton::toggled,this,[this](bool isChecked){
        showNormal();
        if(isChecked)
        {
            //最大化时额外判断
            if(playerframe == PlayerFrame::MaxFrame)
            {
                setGeometry((geometry().width() - ui->widget_miniplayer->width()) / 2,geometry().height() * 0.1,ui->widget_miniplayer->width(),height() * 0.8);
            }
            emit playerFrameChanged(MiniFrame);
        }
        else
        {
            isMiniToNormal = true;
            emit playerFrameChanged(NormalFrame);
        }
    });
    //最小化按钮响应
    connect(ui->btn_minimize,&QPushButton::pressed,this,&WindowFrame::btnmove);
    connect(ui->btn_minimize,&QPushButton::released,this,&WindowFrame::btnmoveback);
    connect(ui->btn_minimize,&QPushButton::clicked,this,&QWidget::showMinimized);
    //功能二按钮响应
    connect(ui->btn_windowControl_2,&QPushButton::pressed,this,&WindowFrame::btnmove);
    connect(ui->btn_windowControl_2,&QPushButton::released,this,&WindowFrame::btnmoveback);
    connect(ui->btn_windowControl_2,&QPushButton::clicked,this,[this]{
        if(playerframe == PlayerFrame::NormalFrame)
        {
            emit playerFrameChanged(MaxFrame);
        }
        else
        {
            emit playerFrameChanged(NormalFrame);
        }
    });
    //关闭按钮响应
    connect(ui->btn_close,&QPushButton::pressed,this,&WindowFrame::btnmove);
    connect(ui->btn_close,&QPushButton::released,this,&WindowFrame::btnmoveback);
    connect(ui->btn_close,&QPushButton::clicked,this,&QWidget::close);
    //设置ShowPage界面响应
    connect(miniplayer,&MiniPlayer::changeShowPage,normalplayer,[this](QWidget* page){
        normalplayer->setShowPage(page);
    });
}

//初始化窗体
void WindowFrame::initWindow()
{
    //设置窗体无边框
    setWindowFlag(Qt::FramelessWindowHint,true);
    /*初始化窗体*/
    resize(1600 / InitPlayer::pixelRatio,845 / InitPlayer::pixelRatio);
    move((InitPlayer::screenGeometry.width() - width()) / 2 / InitPlayer::pixelRatio,(InitPlayer::screenGeometry.height() - height()) / 2 / InitPlayer::pixelRatio);
    //设置窗体最小空间
    setMinimumSize(1400 / InitPlayer::pixelRatio,0);
    //初始化框架状态
    playerframe = PlayerFrame::NormalFrame;
    //初始化拖动区域
    dragArea = QRect(0,0,width(),80 / InitPlayer::pixelRatio);
    //其它初始化
    isMiniToNormal = false;
}
//初始化控件
void WindowFrame::initWidget()
{
   //隐藏为做好的设置按钮
    ui->WindowFrame_btn_setting->hide();
    /*初始化标题栏控件*/
    //初始化Logo大小
    ui->logo->setFixedSize(50 / InitPlayer::pixelRatio, 45 / InitPlayer::pixelRatio);
    //初始化标题控件字体大小
    QFont font_title = ui->title->font();
    font_title.setPixelSize(30 / InitPlayer::pixelRatio);
    ui->title->setFont(font_title);
    /*初始化窗体控制控件大小*/
    ui->btn_windowControl_1->setMinimumSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->btn_windowControl_2->setMinimumSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->btn_close->setMinimumSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->btn_minimize->setFixedSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->btn_windowControl_1->setMaximumSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->btn_windowControl_2->setMaximumSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->btn_close->setMaximumSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    ui->WindowFrame_btn_setting->setFixedSize(30 / InitPlayer::pixelRatio, 30 / InitPlayer::pixelRatio);
    //设置迷你窗口大小
    ui->widget_miniplayer->setFixedWidth(440 / InitPlayer::pixelRatio);
}
/*得到MiniPlayer、NormalPlayer两个类*/
MiniPlayer* WindowFrame::getMiniPlayerClass()
{
    return findChild<MiniPlayer*>("widget_miniplayer");
}

NormalPlayer *WindowFrame::getNormalPlayerClass()
{
    return findChild<NormalPlayer*>("widget_normalplayer");
}


void WindowFrame::mousePressEvent(QMouseEvent *e)
{
    //判断鼠标在窗体操作区域时
    if (dragArea.contains(e->position().toPoint()))
    {
        mousePos = e->globalPosition() - QPointF(frameGeometry().left(), frameGeometry().top());//记录鼠标按下时的位置
        movable = true;
    }
}
void WindowFrame::mouseMoveEvent(QMouseEvent *e)
{
    //判断鼠标是否按下并且可以拖拽
    if (e->buttons() == Qt::LeftButton && movable == true)
    {
        //判断是否最大化，如果已经最大化那就先缩小再拖动
        if(playerframe == PlayerFrame::MaxFrame)
        {
            emit playerFrameChanged(NormalFrame);
            move(e->position().x(),0);
        }
        move((e->globalPosition() - mousePos).toPoint());//更新窗口位置
    }
}
void WindowFrame::mouseDoubleClickEvent(QMouseEvent *e)
{

    //双击区域在窗体操作区域时
    if(dragArea.contains(e->position().toPoint()))
    {
        //判断播放器框架类型，实现缩放
        if(playerframe == PlayerFrame::NormalFrame)
        {
            showMaximized();
            emit playerFrameChanged(MaxFrame);
        }
        else if(playerframe == PlayerFrame::MaxFrame)
        {
            showNormal();
            emit playerFrameChanged(NormalFrame);
        }
    }


}
void WindowFrame::mouseReleaseEvent(QMouseEvent *e)
{
    //恢复不可拖拽状态
    movable = false;
    //判断鼠标是否达到顶层，使得窗体可以最大化
    if(playerframe == PlayerFrame::NormalFrame && e->globalPosition().y() <= 5)
    {
        showMaximized();
        emit playerFrameChanged(MaxFrame);
    }
}

void WindowFrame::resizeEvent(QResizeEvent *e)
{
    //如果迷你窗体拉伸，则自动转为普通窗体
    if(playerframe == PlayerFrame::MiniFrame && width() >= minimumWidth() * 1.2)
    {
        isMiniToNormal = true;
        ui->btn_windowControl_1->setChecked(false);//重置按钮按下属性，后续将重构按钮！！
        emit playerFrameChanged(PlayerFrame::NormalFrame);
    }
    /*设置窗体圆角*/
    if(playerframe != PlayerFrame::MaxFrame)
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
    else
    {
        QBitmap bmp(this->size());
        bmp.fill();
        QPainter painter(&bmp);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.setRenderHint(QPainter::Antialiasing);
        //此处可修改圆角矩形大小，也可以绘制为其他想要的形状，比如圆形
        //该函数便使得QBitmap上像素为1的区域才显示出来
        setMask(bmp);
    }
    e->accept();
}

//移动播放组件
void WindowFrame::moveControls()
{
    if(isMiniToNormal)
    {
        //重新设定迷你框架播放控件区域
        ui_mini->frame_musicControl->setFixedHeight(0);
        //设置封面大小
        ui_normal->musicPicture->setFixedSize(100 / InitPlayer::pixelRatio,100 / InitPlayer::pixelRatio);
        //获取播放器控件的layout
        QHBoxLayout *layout1 = static_cast<QHBoxLayout*>(ui_normal->widget_control->layout());
        QHBoxLayout *layout2 = static_cast<QHBoxLayout*>(ui_normal->musicControl->layout());
        //设置控件转移
        layout1->insertWidget(1,ui_normal->last);
        layout1->insertWidget(2,ui_normal->playControl);
        layout1->insertWidget(3,ui_normal->next);
        layout2->insertWidget(0,ui_normal->musicPicture);
        layout2->insertWidget(4,ui_normal->volume);
    }
    else
    {
        //重新设定迷你框架播放控件区域
        ui_mini->frame_musicControl->setFixedHeight(100 / InitPlayer::pixelRatio);
        //获取播放器控件的layout
        QHBoxLayout *layout = static_cast<QHBoxLayout*>(ui_mini->frame_musicControl->layout());
        //设置封面大小
        ui_normal->musicPicture->setFixedSize(60 / InitPlayer::pixelRatio,60 / InitPlayer::pixelRatio);
        //设置控件转移
        layout->addWidget(ui_normal->musicPicture);
        layout->addWidget(ui_normal->last);
        layout->addWidget(ui_normal->playControl);
        layout->addWidget(ui_normal->next);
        //layout->addWidget(ui_normal->volume);
        layout->setSpacing(10);
    }
}

//初始化目标框架
void WindowFrame::initTargetFrame(PlayerFrame targetFrame)
{
    qDebug() << "开始初始化框架类型为" <<targetFrame;
    //改变框架标识
    playerframe = targetFrame;
    //目标框架为最大框架
    if(playerframe == PlayerFrame::MaxFrame)
    {
        //再次设置系统窗口
        showMaximized();
        /*设置窗口控件改变*/
        ui->btn_windowControl_1->setStyleSheet("QPushButton#btn_windowControl_1{image: url(:/WindowControls/res/normalToMini.svg);}");
        ui->btn_windowControl_2->setStyleSheet("QPushButton#btn_windowControl_2{image: url(:/WindowControls/res/maxToNormal.svg);}");
        ui->btn_windowControl_2->setVisible(true);
    }
    //目标框架为普通框架
    else if(playerframe == PlayerFrame::NormalFrame)
    {
        //再次设置系统窗口
        showNormal();
        /*设置窗口控件改变*/
        ui->btn_windowControl_1->setStyleSheet("QPushButton#btn_windowControl_1{image: url(:/WindowControls/res/normalToMini.svg);}");
        ui->btn_windowControl_2->setStyleSheet("QPushButton#btn_windowControl_2{image: url(:/WindowControls/res/maximize.svg);}");
        ui->btn_windowControl_2->setVisible(true);
        //判断是否是从迷你框架控转换
        if(isMiniToNormal)
        {
            hide();
            //再次设置系统窗口
            setWindowTitle("Spirit  Music");
            //mini部分显示
            ui_mini->widget->show();
            ui_mini->frame_musicList->show();
            ui_mini->frame_musicList->show();
            ui_mini->widget_line->show();
            //重置窗体空间
            setMinimumSize(1400 / InitPlayer::pixelRatio,0);
            setMaximumSize(16777215,16777215);
            //测算窗口大小
            QRect maskRect;
            //重新设置迷你框架大小
            ui->widget_miniplayer->setFixedWidth(440 / InitPlayer::pixelRatio);
            //设置新窗口大小
            setGeometry(geometry().x(),geometry().y(),height() * 2,height() > 850 ? height() : 850);
            //高度超过预计距离，自动最大化
            if(height() * 2 > InitPlayer::screenGeometry.width())
            {
                setGeometry(0,0,InitPlayer::screenGeometry.width(),InitPlayer::screenGeometry.height());
                emit playerFrameChanged(MaxFrame);
            }
            //防止窗口飘至屏幕外
            else if(geometry().x() + height() * 2 > InitPlayer::screenGeometry.width())
            {
                setGeometry(InitPlayer::screenGeometry.width() - height() * 2 - 20,geometry().y(),width(),height());
            }
            /*加入启动遮罩*/
            InitPlayer *mask = new InitPlayer;
            maskRect = geometry();
            mask->addStartMask(maskRect,"Spirit  Music");
            /*显示NormalWidget*/
            ui->widget_normalplayer->setVisible(true);
            //转移播放控制组件
            moveControls();
            //显示Logo
            ui->logo->setVisible(true);
            isMiniToNormal = false;
            //使得最大化按钮生效
            setWindowFlag(Qt::WindowMaximizeButtonHint,true);
            mask->deleteStartMask();
        }
    }
    //目标框架为迷你框架
    else //playerframe == PlayerFrame::MiniFrame
    {
        hide();
        //再次设置系统窗口
        showNormal();
        setWindowTitle("Mini Player");
        //重置窗体最小为控件所需最小空间,同时为控件拉伸响应做准备
        setMinimumSize(440 / InitPlayer::pixelRatio,0);
        //防止窗口飘至屏幕外
        if(geometry().x() < 0)
        {
            move(0,geometry().y());
        }
        //防止播放器伸长距离过长
        if(geometry().height() > 0.8 * InitPlayer::screenGeometry.height())
        {
            resize(ui->widget_miniplayer->width(),height() * 0.8);
        }
        /*加入启动遮罩*/
        InitPlayer *mask = new InitPlayer;
        //测算窗口大小
        QRect maskRect(geometry().x(),geometry().y(),(ui->widget_miniplayer->width()),200 / InitPlayer::pixelRatio);
        /*隐藏NormalWidget和MiniWidget*/
        //normal部分隐藏
        ui->widget_normalplayer->hide();
        //mini部分隐藏
        ui_mini->widget->hide();
        ui_mini->frame_musicList->hide();
        ui_mini->frame_musicList->hide();
        ui_mini->widget_line->hide();
        /*设置窗口控件改变*/
        ui->btn_windowControl_1->setStyleSheet("QPushButton#btn_windowControl_1{image: url(:/WindowControls/res/miniToNormal.svg);}");
        ui->btn_windowControl_2->setVisible(false);
        //隐藏Logo
        ui->logo->setVisible(false);
        //开启遮罩
        mask->addStartMask(maskRect,"");
        //转移播放控制组件
        moveControls();
        /*设置新窗体*/
        //聚拢控件
        setMaximumSize(maskRect.size().width(),200 / InitPlayer::pixelRatio);
        //重新设置迷你框架大小
        ui->widget_miniplayer->setMaximumSize(16777215,16777215);
        //重置最大值
        setMaximumWidth(16777215);
        //使得最大化按钮失效
        setWindowFlag(Qt::WindowMaximizeButtonHint,false);
        mask->deleteStartMask();
    }
    //重新展示窗体
    show();
}

//按钮控件位置移动
void WindowFrame::btnmove()
{
    QPushButton *btn = qobject_cast<QPushButton*>(QObject::sender());
    btn->move(btn->geometry().x() + 1, btn->geometry().y() + 1);
}
//按钮控件位置还原
void WindowFrame::btnmoveback()
{
    QPushButton *btn = qobject_cast<QPushButton*>(QObject::sender());
    btn->move(btn->geometry().x() - 1, btn->geometry().y() - 1);
}

//重绘界面
void WindowFrame::paintEvent(QPaintEvent *)
{
    //确保样式表生效
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

Ui::NormalPlayer* WindowFrame::ui_normal;//保存普通播放器Ui，方便播放器框架切换时操作
Ui::MiniPlayer* WindowFrame::ui_mini;//保存迷你播放器Ui，方便播放器框架切换时操作
NormalPlayer* WindowFrame::normalplayer;
