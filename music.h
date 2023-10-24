#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_music.h"
#include<qstandarditemmodel.h>
#include<qnetworkrequest.h>
#include<qnetworkaccessmanager.h>
#include<qnetworkreply.h>

#include<qjsonparseerror>
#include<qjsondocument>
#include<qjsonobject.h>
#include<qjsonarray.h>
#include<qmediaplayer.h>
#include<qfiledialog.h>
#include<qmediaplaylist.h>
#include<qtimer.h>
namespace {
    class musicClass;
}
class music : public QMainWindow
{
    Q_OBJECT

public:
    music(QWidget *parent = nullptr);
    ~music();
    void init();
public slots:
    void search();
    void reply_finished(QNetworkReply*reply);
   
    void play_clicked();
    void local_music();
    void local_change();
    void next_music();
    void last_music();
    void online_music();
    void setmusic_time(int position);
    void setmusic_volun(int);
    void now_sliderPosition();
    
private:
    Ui::musicClass *ui;
    QStandardItemModel m_model;
    QNetworkAccessManager* networkAccessManager;
    QMediaPlaylist* playlist;
    int musicId;
    int music_duration;
    QString musicname;
    int mId;
    int a;
    QString singername;
    QByteArray data;
    QString aName;
    QMediaPlayer* Player;
    QString musicpath;
    bool isplay=false;
    bool isLocal = 1;
    QTimer* timer;
    QString settime(int time);
    QStringList local_list;
};
