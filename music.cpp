#include "music.h"
#include<qmessagebox.h>
music::music(QWidget *parent)
    : QMainWindow(parent),ui(new Ui::musicClass())
{
    ui->setupUi(this);
    networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(reply_finished(QNetworkReply*)));
    Player = new QMediaPlayer();
    connect(ui->playButton, &QPushButton::clicked, this, &music::play_clicked);
    connect(ui->local_Music_Button, &QPushButton::clicked, this, &music::local_music);
    connect(ui->online_Music_Button, &QPushButton::clicked, this, &music::online_music);
    connect(ui->horizontalSlider, &QSlider::sliderMoved, this, &music::setmusic_time);
    this->setStyleSheet("QMainWindow{background-image:url(icon/f83d097ec2b95c80fe22ab3078c96114.jpeg)}");
    timer = new QTimer();
    timer->setInterval(1000);
    timer->start();
    init();
}
QString music::settime(int time)
{
    int h, m, s;
    time /= 1000;  
    h = time / 3600;
    m = (time - h * 3600) / 60;
    s = time - h * 3600 - m * 60;
    return QString("%1:%2:%3").arg(h).arg(m).arg(s);
}

music::~music()
{
    delete ui;
}
void music::setmusic_time(int position)
{
    Player->setPosition(Player->duration() * position/100);
}
void music::online_music()
{
    isLocal = 0;
    ui->musicList->clear();
}
void music::init()
{  
    ui->playButton->setText("播放");
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &music::search);
    this->musicpath = "music file";
    QDir dir(this->musicpath);
    QStringList nameFileters;
    nameFileters << "*mp3";
    playlist = new QMediaPlaylist();
    QStringList files = dir.entryList(nameFileters, QDir::Files | QDir::Readable, QDir::Name);
    ui->musicList->addItems(files);
    ui->musicList->setCurrentRow(0);
    Player->setMedia(QUrl::fromLocalFile(musicpath + '/' + ui->musicList->item(0)->text()));
    connect(ui->online_Music_Button, &QPushButton::clicked, this, &music::online_music);
    connect(ui->musicList, &QListWidget::currentRowChanged, this, &music::local_change);
    connect(ui->nextButton, &QPushButton::clicked, this, &music::next_music);
    connect(ui->beforeButton, &QPushButton::clicked, this, &music::last_music);
    connect(this->timer, SIGNAL(timeout()), this,SLOT(now_sliderPosition()));
    connect(ui->verticalSlider, &QSlider::valueChanged, this, &music::setmusic_volun);
    local_music();
  
}

void music::setmusic_volun(int position)
{
    this->Player->setVolume(position);
}
void music::now_sliderPosition()
{
    ui->horizontalSlider->setValue(Player->position() * 100 / Player->duration());
    ui->nowTime->setText(settime(Player->position()));
}
void music::next_music()
{
    if (ui->musicList->count() > 0&&isLocal)
    {
        int i = ui->musicList->currentRow();
        i += 1;
        if (i + 1 > 0 && i + 1 <= ui->musicList->count())
        {
            ui->musicList->setCurrentRow(i);
            Player->setMedia(QUrl::fromLocalFile(musicpath + '/' + ui->musicList->item(i)->text()));
        }
   
        
    }
}
void music::last_music()
{
    if (ui->musicList->count() > 0 && isLocal)
    {
        int i = ui->musicList->currentRow();
        i -= 1;
        qDebug() << "hh" << i;
        if (i +1 > 0 && i + 1 <= ui->musicList->count())
        {
            ui->musicList->setCurrentRow(i);
            Player->setMedia(QUrl::fromLocalFile(musicpath + '/' + ui->musicList->item(i)->text()));
        }


    }
}
void music::search()
{  
    if (!isLocal)
    {
        
        QNetworkRequest request;
        request.setUrl(QString("http://iwxyi.com:3000/search?keywords=%0").arg(ui->lineEdit->text()));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded; charset=UTF-8");
        request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0(Windows NT 10.0;Win64;x64)AppleWebKit/537.36(KHTML,like Gecko)Chrome/86.0.4240.111 Safari/537.36");
        networkAccessManager->get(request);
     
    }
    else {
        ui->musicList->clear();
        QString searchtarget = ui->lineEdit->text();
        ui->musicList->addItems(local_list);
        QList<QListWidgetItem*>match = ui->musicList->findItems(searchtarget, Qt::MatchStartsWith);
        QStringList search_w;
        if (!match.isEmpty())
        {
            for (int i = 0; i < match.size(); i++)
            {
                search_w << match.at(i)->text();
            }
            ui->musicList->clear();
            for (int i = 0; i < search_w.length(); i++)
            {
                ui->musicList->addItem(search_w.at(i));
                qDebug() << match.at(i)<<endl;
            }
        }
        else {
            ui->musicList->clear();
        }
        
    }
}
void music::local_change()
{
    if (isLocal)
    {
        if (!ui->musicList->currentItem())
            ui->musicList->setCurrentItem(0);
        else
        {
            Player->setMedia(QUrl::fromLocalFile(musicpath + '/' + ui->musicList->currentItem()->text()));
            isplay = 0;
        }
    }
    else {

        if (!ui->musicList->currentItem())
            ui->musicList->setCurrentItem(0);
        else
        {
          playlist->setCurrentIndex(ui->musicList->currentRow());
          isplay = 0;
        }
    }
}


void music::reply_finished(QNetworkReply* reply)
{
    playlist->clear();
   data=reply->readAll();
   QJsonParseError err;
   QJsonDocument json = QJsonDocument::fromJson(data, &err);
   if (err.error != QJsonParseError::NoError)
   {
       return;
   }
   QJsonObject object = json.object();
   QStringList keys = object.keys();
   if (keys.contains("result"))
   {
       QJsonObject result = object["result"].toObject();
       QStringList rkeys = result.keys();
       if (result.contains("songs"))
       {
           QJsonArray arr = result["songs"].toArray();
           ui->musicList->clear();
           for (auto i : arr)
           {
               QJsonObject object_1 = i.toObject();
              
               musicId = object_1["id"].toInt();
               music_duration = object_1["duration"].toInt();
               musicname = object_1["name"].toString();
               mId = object_1["mvId"].toInt();
               a = mId;
               QStringList artistsKeys = object.keys();
               if (artistsKeys.contains("artists"))
               {
                   QJsonArray artistsArray = object["artists"].toArray();
                   for (auto j : artistsArray)
                   {
                       QJsonObject object_2 = j.toObject();
                       singername = object_2["name"].toString();
                   }
               }
               if (artistsKeys.contains("album"))
               {
                   QJsonObject aObject = object["album"].toObject();
                   aName = aObject["name"].toString();
               }
               if (!isLocal)
               {
                   QStringList temp;
                   temp.append(musicname);
                   ui->musicList->addItems(temp);
               }
              qDebug()<<musicId<<' '<<musicname<<endl;
              playlist->addMedia(QUrl(QString("https://music.163.com/song/media/outer/url?id=%0").arg(musicId)));
             // Player->setMedia(QUrl(QString("https://music.163.com/song/media/outer/url?id=%0").arg(musicId)));
           }
          
       }
   }
   
   Player->setMedia(playlist);
}


void music::play_clicked()
{
    ui->allTime->setText(settime(Player->duration()));
    if (isplay)
    {
        this->Player->pause();
        this->isplay = 0;
       
    }
    else {
        this->isplay = 1;
        this->Player->play();
        
    }
}
void music::local_music()
{
    isLocal = 1;
    if (isLocal) {
        ui->musicList->clear();
        this->musicpath = "music file";
        QDir dir(this->musicpath); 
        QStringList nameFileters;
        nameFileters << "*mp3";
        QStringList files = dir.entryList(nameFileters, QDir::Files | QDir::Readable, QDir::Name);
        local_list = files;
        ui->musicList->addItems(local_list);
        Player->setMedia(QUrl::fromLocalFile(musicpath + '/' + ui->musicList->item(0)->text()));


        for (int i = 0; i < files.count(); i++)
            qDebug() << files.at(i);
    }
       
}