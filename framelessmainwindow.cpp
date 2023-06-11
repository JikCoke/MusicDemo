#include "framelessmainwindow.h"
#include "qdatetime.h"
#include "qevent.h"
#include "qdebug.h"

#ifdef Q_OS_WIN
#include "windows.h"
#include "windowsx.h"
#endif

#define TIMEMS qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))

FramelessMainWindow::FramelessMainWindow(QWidget *parent) : QMainWindow(parent)
{
    padding = 8;
    moveEnable = true;
    resizeEnable = true;

    mousePressed = false;
    mousePoint = QPoint(0, 0);
    mouseRect = QRect(0, 0, 0, 0);

    for (int i = 0; i < 8; ++i) {
        pressedArea << false;
        pressedRect << QRect(0, 0, 0, 0);
    }

    isMin = false;
    flags = this->windowFlags();
    titleBar = 0;

    //设置背景透明 官方在5.3以后才彻底修复 WA_TranslucentBackground+FramelessWindowHint 并存不绘制的bug
#if (QT_VERSION >= QT_VERSION_CHECK(5,3,0))
    this->setAttribute(Qt::WA_TranslucentBackground);
#endif
    //设置无边框属性
//    this->setWindowFlags(flags | Qt::FramelessWindowHint);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //安装事件过滤器识别拖动
    this->installEventFilter(this);

    //设置属性产生win窗体效果,移动到边缘半屏或者最大化等
    //设置以后会产生标题栏,需要在下面拦截消息WM_NCCALCSIZE重新去掉
#ifdef Q_OS_WIN
    HWND hwnd = (HWND)this->winId();
    DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
    ::SetWindowLong(hwnd, GWL_STYLE, style | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CAPTION);
#endif
}

void FramelessMainWindow::showEvent(QShowEvent *event)
{
    //解决有时候窗体重新显示的时候假死不刷新的bug
    setAttribute(Qt::WA_Mapped);
    QMainWindow::showEvent(event);
}

//void FramelessMainWindow::doResizeEvent(QEvent *event)
//{
//    //非最大化才能移动和拖动大小
//    if (windowState() == Qt::WindowNoState) {
//        qDebug() << "+++++++++++++++";
//        moveEnable = true;
//        resizeEnable = true;
//    } else {
//        qDebug() << "------------------";
//        moveEnable = false;
//        resizeEnable = false;
//    }

//    //发出最大化最小化等改变事件,以便界面上更改对应的信息比如右上角图标和文字
//    emit windowStateChange(!moveEnable);
//}

bool FramelessMainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG") {
        MSG *msg = static_cast<MSG *>(message);
        //不同的消息类型和参数进行不同的处理
        if (msg->message == WM_NCCALCSIZE) {
            *result = 0;
            return true;
        }else if (msg->message == WM_NCHITTEST) {
            //计算鼠标对应的屏幕坐标
            //这里最开始用的 LOWORD HIWORD 在多屏幕的时候会有问题
            //官方说明在这里 https://docs.microsoft.com/zh-cn/windows/win32/inputdev/wm-nchittest
            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);
            QPoint pos = mapFromGlobal(QPoint(x, y));

            //判断当前鼠标位置在哪个区域
            bool left = pos.x() < padding;
            bool right = pos.x() > width() - padding;
            bool top = pos.y() < padding;
            bool bottom = pos.y() > height() - padding;

            //鼠标移动到四个角,这个消息是当鼠标移动或者有鼠标键按下时候发出的
            *result = 0;
            if (resizeEnable) {
                if (left && top) {
                    *result = HTTOPLEFT;
                } else if (left && bottom) {
                    *result = HTBOTTOMLEFT;
                } else if (right && top) {
                    *result = HTTOPRIGHT;
                } else if (right && bottom) {
                    *result = HTBOTTOMRIGHT;
                } /*else if (left) {
                    *result = HTLEFT;
                } */else if (right) {
                    *result = HTRIGHT;
                } else if (top) {
                    *result = HTTOP;
                } else if (bottom) {
                    *result = HTBOTTOM;
                }
            }

            //先处理掉拉伸
            if (0 != *result) {
                return true;
            }

            //识别标题栏拖动产生半屏全屏效果
            if (titleBar && titleBar->rect().contains(pos)) {
                QWidget *child = titleBar->childAt(pos);
                if (!child) {
                    qDebug() << "++++++++++++++";
                    *result = HTCAPTION;
                    return true;
                }
            }
        } else if (msg->wParam == PBT_APMSUSPEND && msg->message == WM_POWERBROADCAST) {
            //系统休眠的时候自动最小化可以规避程序可能出现的问题
            this->showMinimized();
        } else if (msg->wParam == PBT_APMRESUMEAUTOMATIC) {
            //休眠唤醒后自动打开
            this->showNormal();
        }
    }
    return false;
}

void FramelessMainWindow::setPadding(int padding)
{
    this->padding = padding;
}

void FramelessMainWindow::setMoveEnable(bool moveEnable)
{
    this->moveEnable = moveEnable;
}

void FramelessMainWindow::setResizeEnable(bool resizeEnable)
{
    this->resizeEnable = resizeEnable;
}

void FramelessMainWindow::setTitleBar(QWidget *titleBar)
{
    this->titleBar = titleBar;
    this->titleBar->installEventFilter(this);
}
