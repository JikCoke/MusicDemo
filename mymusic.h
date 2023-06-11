#ifndef MYMUSIC_H
#define MYMUSIC_H

#include <QJsonArray>
#include <QJsonObject>
#include <QListWidget>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QMediaPlayer>
#include <QTimer>
#include "framelessmainwindow.h"

#pragma execution_character_set("utf-8")

QT_BEGIN_NAMESPACE
namespace Ui { class MyMusic; }
QT_END_NAMESPACE

class MyMusic : public FramelessMainWindow
{
    Q_OBJECT

public:
    explicit MyMusic(QWidget *parent = nullptr);
    ~MyMusic();

public slots:
    void on_FindBtn();
    void on_myMusicList(QListWidgetItem *current);
    void on_myMusicList_click(const QModelIndex index);
    void on_Play_click();
private:
    Ui::MyMusic *ui;
    QNetworkAccessManager * network_manager;
    QNetworkRequest * network_request;
    QJsonArray musicList;
    QJsonObject musicInfo;
    QMediaPlayer *player;
    QTimer *timer;
    int cur_num;
    int dest_num;
};
#endif // MYMUSIC_H
