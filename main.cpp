#include "mymusic.h"

#include <QApplication>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QRect>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file(":/qss/style.qss"); // 读取qss文件
    file.open(QFile::ReadOnly); // 以只读方式打开文件

    QTextStream fileStream(&file);
    QString styleSheet = fileStream.readAll(); // 读取文件内容

    qApp->setStyleSheet(styleSheet); // 设置全局qss样式
    MyMusic w;
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    int mainWindowWidth = w.width();
    int mainWindowHeight = w.height();
    int x = (screenWidth - mainWindowWidth) / 2;
    int y = (screenHeight - mainWindowHeight) / 2;
    w.move(x, y);
    w.show();
    return a.exec();
}
