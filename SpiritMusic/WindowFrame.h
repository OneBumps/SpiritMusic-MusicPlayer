#ifndef WINDOWFRAME_H
#define WINDOWFRAME_H

#include "NormalPlayer.h"
#include "MiniPlayer.h"
#include <QWidget>
#include <QMouseEvent>


namespace Ui {
class WindowFrame;
}

class WindowFrame : public QWidget
{
    Q_OBJECT
public:
    explicit WindowFrame(QWidget *parent = nullptr);
    ~WindowFrame();
    //播放器框架类型枚举
    enum PlayerFrame{
        NormalFrame,
        MiniFrame,
        MaxFrame
    };
    Q_ENUM(PlayerFrame)
private:
    //初始化调度指针
    void initPointer();
    //初始化connect
    void initConnect();
    //初始化窗体
    void initWindow();
    //初始化控件
    void initWidget();
    /*得到MiniPlayer、NormalPlayer两个类*/
    MiniPlayer* getMiniPlayerClass();
    NormalPlayer* getNormalPlayerClass();
    //窗体设置鼠标响应
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    //窗体变化事件响应
    void resizeEvent(QResizeEvent *e) override;
    //移动播放组件
    void moveControls();
signals:
    //调整框架结构
    void playerFrameChanged(PlayerFrame targetFrame);
private slots:
    //初始化目标框架
    void initTargetFrame(PlayerFrame targetFrame);
    //按钮控件位置移动
    void btnmove();
    //按钮控件位置还原
    void btnmoveback();
public:
    static Ui::NormalPlayer *ui_normal;//保存普通播放器Ui，方便播放器框架切换时操作
    static Ui::MiniPlayer *ui_mini;//保存迷你播放器Ui，方便播放器框架切换时操作
    static NormalPlayer *normalplayer;//保存普通播放器，方便播放器框架切换时操作
private:
    Ui::WindowFrame *ui;
    void paintEvent(QPaintEvent *) override;//重绘界面
    QPointF mousePos;//鼠标点击的位置
    QRect dragArea;//拖动区域
    bool movable;//是否允许拖动
    PlayerFrame playerframe;//播放器框架状态
    bool isMiniToNormal;//判断框架是否是从迷你播放器向普通播放器转换
    MiniPlayer *miniplayer;//保存迷你播放器，方便播放器框架切换时操作
};

#endif // WINDOWFRAME_H
