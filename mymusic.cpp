#include "mymusic.h"
#include "ui_mymusic.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QBitmap>
#include <QPainter>
#include "windows.h"
#include "windowsx.h"

MyMusic::MyMusic(QWidget *parent)
    : FramelessMainWindow(parent)
    , ui(new Ui::MyMusic)
    , network_manager(new QNetworkAccessManager())
    , network_request(new QNetworkRequest())
    , player(new QMediaPlayer)
    , timer(new QTimer)
{
    ui->setupUi(this);
    setMinimumSize(900,700);
    setTitleBar(ui->title_widget);
    connect(ui->find_btn,&QToolButton::clicked,this,&MyMusic::on_FindBtn);
    connect(ui->myMusicList,&QListWidget::currentItemChanged,this,&MyMusic::on_myMusicList);
    connect(ui->close_btn,&QToolButton::clicked,this,&MyMusic::close);
    connect(ui->min_btn,&QToolButton::clicked,this,&MyMusic::showMinimized);
    connect(this,&MyMusic::titleDblClick,this,[&](){
        qDebug() << "放大";
        if (this->isMaximized()) {
            this->showNormal();
            ui->max_btn->setText("最大");
        } else {
            this->showMaximized();
            ui->max_btn->setText("还原");
        }
    });
    connect(ui->tableWidget,&QTableWidget::doubleClicked,this,&MyMusic::on_myMusicList_click);
    connect(ui->play,&QToolButton::clicked,this,&MyMusic::on_Play_click);
    connect(ui->state,&QSlider::sliderMoved,this,[&]()
    {

        qDebug() << "+++++++++++++   " << ui->state->value()*dest_num/100 << "   " << ui->state->value();
        qDebug() << "移动";
        timer->stop();
        player->stop();
        cur_num = ui->state->value()*dest_num*1000/100;
        player->setPosition(cur_num);
        int cur_time = cur_num/1000;
        QString time_info = QString("%1:%2").arg(cur_time/60).arg(cur_time%60);
        ui->current_time->setText(time_info);
    });
    connect(ui->state,&QSlider::sliderReleased,this,[&]()
    {
        player->play();
        qDebug() << "释放";
        timer->start();
    });
    QStringList headers = {"歌曲","歌手","时长","专辑"};
    ui->tableWidget->setHeaderLabels(headers);

    ui->play->setAccessibleName("OFF");
}

MyMusic::~MyMusic()
{
    delete ui;
}

void MyMusic::on_FindBtn()
{
    if( ui->find_lineEdit->text().isEmpty() )
    {
        qDebug() << "无内容";
        return;
    }
    QString KGAPISTR1 = QString("http://mobilecdn.kugou.com/api/v3/search/song?format=json"
                               "&keyword=%1&page=1&pagesize=18").arg(ui->find_lineEdit->text());
    qDebug() << KGAPISTR1;
    network_request->setUrl(QUrl(KGAPISTR1));
    QNetworkReply *reply = network_manager->get(*network_request);
    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    QJsonDocument doc;
    //无错误返回
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);  //转化为字符串
        doc = QJsonDocument::fromJson(result.toUtf8());
        ui->tableWidget->setRowCount(0);
    }
    else
    {
        //处理错误
        qDebug()<<"搜索失败";
        return;
    }
    QString songname_original; //歌曲名
    QString singername;        //歌手
    QString hashStr;           //hash
    QString album_name;        //专辑
    int duration;          	   //时间

    QJsonDocument parse_doucment = doc;

    QJsonObject data = parse_doucment.object().value("data").toObject();
    QJsonArray music_list = data.value("info").toArray();
    int size = musicList.size();
    for(int i=0; i < size; i++)
    {
         musicList.removeFirst();
    }
    for (int idx = 0; idx < music_list.size(); idx++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        QJsonObject tmp = music_list.at(idx).toObject();
        songname_original = tmp.value("songname_original").toString();
        album_name = tmp.value("album_name").toString();
        duration = tmp.value("duration").toInt();
        singername = tmp.value("singername").toString();
        QString time = QString("%1:%2").arg(duration/60).arg(duration%60);
        ui->tableWidget->setItem(idx,0,new QTableWidgetItem(songname_original));
        ui->tableWidget->setItem(idx,1,new QTableWidgetItem(singername));
        ui->tableWidget->setItem(idx,2,new QTableWidgetItem(time));
        ui->tableWidget->setItem(idx,3,new QTableWidgetItem(album_name));
        musicList.append(tmp);

//        qDebug() << "filename" << songname_original;
    }
    ui->tableWidget->tableDataCenter();
}

void MyMusic::on_myMusicList(QListWidgetItem *current)
{
    QString cur_text = current->text();
    int cur_row = ui->myMusicList->row(current);
    qDebug() << cur_row << "::" << cur_text;
    ui->stackedWidget->setCurrentIndex(cur_row);
}

void MyMusic::on_myMusicList_click(const QModelIndex index)
{
    qDebug() << index.row();
    QJsonObject current_music = musicList.at(index.row()).toObject();
    int time = current_music.value("duration").toInt();
    dest_num = time;
    QString time_info = QString("%1:%2").arg(time/60).arg(time%60);
    ui->end_time->setText(time_info);

    QString music_hash = current_music.value("hash").toString();
    QString album_id = current_music.value("album_id").toString();

    QString KGAPISTR1 =QString("http://m.kugou.com/app/i/getSongInfo.php?cmd=playInfo"
//    QString KGAPISTR1 =QString("http://www.kugou.com/yy/index.php?r=play/getdata"
        "&hash=%1").arg(music_hash);
    network_request->setUrl(QUrl(KGAPISTR1));
    qDebug() << KGAPISTR1;
//    //这句话很重要，我们手动复制url放到浏览器可以获取json，但是通过代码不行，必须加上下面这句才可以
//    network_request->setRawHeader("Cookie","kg_mid=2333");
//    network_request->setHeader(QNetworkRequest::CookieHeader, 2333);
    QNetworkReply *reply = network_manager->get(*network_request);
    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    if(reply->error() == QNetworkReply::NoError)
    {
        qDebug() << "++++++++++++++++++";
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);  //转化为字符串
        QJsonDocument doc = QJsonDocument::fromJson(bytes);
        musicInfo = doc.object();
        //播放进度的信号提示
        connect(player, &QMediaPlayer::positionChanged, this, [&](qint64 num)
        {
            qDebug() << "cur_time" << num << " end time "<< dest_num;
            cur_num = num;
        });
        if( musicInfo.value("url").toString().isEmpty() )
        {
            qDebug() << "当前歌曲为付费歌曲";
        }
        player->setMedia(QUrl::fromLocalFile(musicInfo.value("url").toString()));
        player->setVolume(50); //0~100音量范围,默认是100
        player->play();
        timer->start(1000);
        connect(timer,&QTimer::timeout,this,[&]()
        {
            int cur_time = cur_num/1000;
            QString time_info = QString("%1:%2").arg(cur_time/60).arg(cur_time%60);
            ui->current_time->setText(time_info);
            double state_num = ((double)cur_num/(dest_num*1000))*100;
            qDebug() << "进度 " << state_num;
            ui->state->setValue(state_num);
//            if( player->state() == QMediaPlayer::StoppedState )
//            {
//                qDebug() << "播放停止";
//                timer->stop();
//                on_Play_click();
//            }
        });
        on_Play_click();
//        QByteArray byte_array;
//        QFile file("D:/data/music_info.json");
//        file.open(QIODevice::WriteOnly);
//        file.write(doc.toJson());
//            qDebug() << doc.toJson();
    }
    else
    {
        //处理错误
        qDebug()<<"歌曲播放失败";
    }
}

void MyMusic::on_Play_click()
{
    qDebug() <<   ui->play->accessibleName();
    if( ui->play->accessibleName() == "OFF")
    {
        qDebug() << "播放" <<  ui->tableWidget->currentIndex().row();
        if(  ui->tableWidget->currentIndex().row() < 0 )
        {
            return;
        }
        ui->play->setAccessibleName("ON");
        ui->play->setIcon(QIcon(":/icon/suspend.png"));
        if( player->state() == QMediaPlayer::PausedState )
        {
            qDebug() << "播放继续";
            timer->start();
            player->play();
            return;
        }
        else if(player->state() == QMediaPlayer::PausedState)
        {
            on_myMusicList_click(ui->tableWidget->currentIndex());
        }

    }
    else {
        if( player->state() == QMediaPlayer::PlayingState )
        {
            qDebug() << "播放停止";
            timer->stop();
            player->pause();
        }
        ui->play->setAccessibleName("OFF");
        ui->play->setIcon(QIcon(":/icon/play.png"));
    }
}

