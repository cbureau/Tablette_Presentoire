/*
 *     connect(ScanTime, SIGNAL(timeout()), discoveryAgent, SLOT(stop()));
 *     connect(discoveryAgent, SIGNAL(canceled()), this, SLOT(startScan()));
 *
 *
 *
 *
 *
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>
#include <QTimer>
#include "androidrfcomm.h"
#include <QFile>
#include <QFileInfo>
#include <QTextFormat>
#include <QTime>
#include <QtNetwork>
#include <QMenu>
#include <QDebug>


MainWindow::MainWindow(QString rfcommmac, AndroidRfComm *bluecom,QWidget *parent) :
    DeviceRfcomm(rfcommmac),rfcomm(bluecom),QMainWindow(parent),
ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    player = new QMediaPlayer;
    ScanTime = new QTimer(this);
    ScanTime2 = new QTimer(this);
    ScanTime3 = new QTimer(this);
    QTimer::singleShot(700, this, SLOT(TimeToInit()));
    connect(&client, SIGNAL(newMessage(QString,QString)),
            this, SLOT(appendMessage(QString,QString)));
    connect(ScanTime, SIGNAL(timeout()), this, SLOT(sendping()));
        connect(ScanTime2, SIGNAL(timeout()), this, SLOT(PingTest()));
            connect(ScanTime3, SIGNAL(timeout()), this, SLOT(ping()));
    Timer_reception_PC();
    myNickName = client.nickName();
    tableFormat.setBorder(0);
    nbrping = 0;
}
void MainWindow::ping()
{
    nbrping++;
    if (nbrping == 30)
    {
        nbrping = 0;
        RfcommReload();
    }
    if(PingState)
    {
        PingState = 0;
    }
    else if (PingState == 0)
    {
        QProcess::execute("svc wifi enable");
    }

    QTimer::singleShot(3500, this, SLOT(sendping()));
}
void MainWindow::sendping()
{
    client.sendMessage(IDMACHINE + "ping");
    ScanTime2->start(5000);
}
void MainWindow::PingTest()
{
    qDebug() << "PING TEST\n PING TEST\nPING TEST\nPING TEST\n PING TEST\nPING TEST\n";
    if (PingState == 0)
    {
        QProcess::execute("svc wifi disable");
    }
    ping();
}


void MainWindow::ChangeRfcommTel()
{
    if (WIFISTATE)
    {
        WIFISTATE = 0;
        QProcess::execute("svc wifi enable");
    }
    else
    {
        WIFISTATE = 1;
        QProcess::execute("svc wifi disable");
    }
}
//=============================================================================||
void MainWindow::appendMessage(const QString &from, const QString &message)
{
    if (from.isEmpty() || message.isEmpty())
        return;
    if (message.mid(0,2).contains(IDMACHINE))
    {
        if (message.contains("!H"))
        {
            toldmewhatIshoulddo("H");
            rfcomm -> sendLine ("$H" + IDMACHINE + IDPUPITRE + IDBPTABLETTE);
        }
        else if (message.contains("!L15"))
           qApp->quit();
        else if (message.contains("!L"))
            rfcomm -> sendLine("$L" + IDMACHINE + IDPUPITRE + message.mid(4,2) + "00" );
        else if (message.contains("!V"))
            toldmewhatIshoulddo(message.mid(3,5));
        else if (message.contains("!OIFG"))
        {
            if (ui->pushButton->isHidden())
                OIFG = 0;
            else if (!(ui->pushButton->isHidden()))
                OIFG = 1;

            if (OIFG == 0)
            {
                OIFG = 1;
                toldmewhatIshoulddo("F");
                rfcomm -> sendLine("$F" + IDMACHINE + IDPUPITRE + IDBPTABLETTE);
            }
            else
            {
                OIFG = 0;
                toldmewhatIshoulddo("G");
                rfcomm -> sendLine("$G" + IDMACHINE + IDPUPITRE + IDBPTABLETTE);
             }
        }
        else if (message.contains("pong"))
        {
            toldmewhatIshoulddo("pong");
        }

    }
}

void MainWindow::toldmewhatIshoulddo(QString Com)
{
    QFile fichier9(GENERALPATH + "img/hs.png");
    QString volume(Com.mid(0,1));

    if(Com.contains("F"))
    {
        bfr = true;
        ui->pushButton->show();
    }
    else if(Com.contains("G"))
    {
        bfr = false;
        ui->pushButton->hide();
    }
    else if(Com.contains("H")) //affichage hors service
    {
        ui->pushButton->hide();
        ui->pushButton_2->hide();
        ui->pushButton_3->hide();
        ui->pushButton_4->hide();
        ui->pushButton_5->hide();
        ui->pushButton_6->hide();
        ui->pushButton_7->hide();
        ui->STOP->hide();
        if(fichier9.exists())
        {
            ui->centralWidget->setStyleSheet("background-image: url( /storage/emulated/0/Download/Pupitre/img/hs.png)");
            ui->centralWidget->show();
        }
        player->stop();
        MyTimer();    // lancement timer
    }
    else if (Com.contains("#C"))
        changemyconfig(Com);
    else if (Com.contains("pong"))
    {
        PingState = 1;
    }
    if (volume.contains("V"))
    {
        SonString = Com.mid(1); // enleve le "V"
        qDebug() << SonString;
        SonValue = SonString.toInt(&ok);
        player->setVolume(SonValue);
    }

}


void MainWindow::TimeToInit() // INITIALISATION
{   /*********************lecture config***************************/
    QString texte;
    GENERALPATH = "/storage/emulated/0/Download/Pupitre/";
    QFile fichier(GENERALPATH + "config/config.text");
    if(fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       QTextStream flux(&fichier);
       IDMACHINE = flux.readLine();
       IDPUPITRE = flux.readLine();
       IDBPTABLETTE = flux.readLine();
//       VOLUME = flux.readLine();
       TEMPHS = flux.readLine().toInt(&ok,10);
    }
    fichier.close();
    OIFG = 1;
    VOLUME = 100;
    BASEVOLUME = VOLUME;
    height = this->geometry().height();
    width  = this->geometry().width();
    heightP = height/100;
    widthP = width/100;
    TestNumber = 0;
    CycleNumber = 0;
    Recapok = 0;
    PingState = 0;
    ui->gridLayoutWidget->setGeometry(0,0,width,height);
    ui->horizontalSpacer->changeSize(widthP*12,heightP * 1,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->horizontalSpacer_4->changeSize(widthP*2,heightP * 1,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->horizontalSpacer_3->changeSize(widthP*2,heightP * 1,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->horizontalSpacer_9->changeSize(widthP*2,heightP * 1,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->horizontalSpacer_11->changeSize(widthP*2,heightP * 1,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->verticalSpacer->changeSize(widthP*1,heightP * 15,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->verticalSpacer_2->changeSize(widthP*1,heightP * 10,QSizePolicy::Maximum,QSizePolicy::Maximum);
    ui->verticalSpacer_3->changeSize(widthP*1,heightP * 15,QSizePolicy::Maximum,QSizePolicy::Maximum);
    WIFISTATE = 0;
    LastDevice = "0";
    QProcess::execute("svc wifi disable");
    InitMyButton();
 //ui->list->hide();
    ping();

}
void MainWindow::RfcommReload()
{
    qDebug() << " try rfcomm";

        rfcomm->connect(DeviceRfcomm);

}

/**********************reception rfcomm regulierement *****************/
void MainWindow::itstime()
{
   QString Com(rfcomm->receiveLine(100,500));
    if (!Com.isEmpty())
    {   
        qDebug() << "rfcomm \n rfcomm \n";
        if (Com.contains("#F"))
            toldmewhatIshoulddo("F");
        else if (Com.contains("#G"))
            toldmewhatIshoulddo("G");
        else if (Com.contains("#H"))
            toldmewhatIshoulddo("H");
        else if (Com.contains("#V"))
            toldmewhatIshoulddo(Com.mid(1,3));
        else if (Com.contains("#C"))
            toldmewhatIshoulddo(Com);
        rfcomm->sendLine("$" + IDMACHINE + IDPUPITRE + IDBPTABLETTE +"OK");
    }
}
/***********************Reception provenance pc par rfcomm toute les secondes************************/

void MainWindow::Timer_reception_PC()
{
    QTimer *timer2 = new QTimer(this);
    connect(timer2, SIGNAL(timeout()), this, SLOT(itstime()));
    timer2->start(1000);
}
/***********************tempo reaffichage icones apres hors service*************************/
void MainWindow::MyTimer()
{
    QTimer::singleShot(TEMPHS*1000, this, SLOT(MyTimerSlot()));
}

void MainWindow::MyTimerSlot()
{
    if (bfr==true)
    ui->pushButton->show();
    ui->pushButton_2->show();
    ui->pushButton_3->show();
    ui->pushButton_4->show();
    ui->pushButton_5->show();
    ui->pushButton_6->show();
    ui->pushButton_7->show();
    ui->STOP->show();
    ui->centralWidget->setStyleSheet("background-image: url(/storage/emulated/0/Download/Pupitre/img/f.png)");
}


void MainWindow::InitMyButton()
{
    int                     Num(1);
    QAndroidJniObject       mediaDir = QAndroidJniObject::callStaticObjectMethod("android/os/Environment", "getExternalStorageDirectory", "()Ljava/io/File;");
    QAndroidJniObject       mediaPath = mediaDir.callObjectMethod( "getAbsolutePath", "()Ljava/lang/String;" );
    QString                 dataAbsPath = mediaPath.toString()+"/Download/Pupitre/" + Num + ".png";
    QAndroidJniEnvironment  env;
    // fond d'ecran
    QFile fichier8(GENERALPATH + "img/f.png");
    if(fichier8.exists())
        ui->centralWidget->setStyleSheet("background-image: url(" + GENERALPATH + "img/f.png)");
    //Bouton 1
    QFile fichier(GENERALPATH + "img/1.png");
    if(fichier.exists())
        ui->pushButton->setStyleSheet("background-image: url(" + GENERALPATH + "img/1.png)");
    //Bouton2
    QFile fichier1(GENERALPATH + "img/2.png");
    if(fichier1.exists())
        ui->pushButton_2->setStyleSheet("background-image: url( " + GENERALPATH + "img/2.png)");

    //Bouton3
    QFile fichier2(GENERALPATH + "img/3.png");
    if(fichier2.exists())
        ui->pushButton_3->setStyleSheet("background-image: url( " + GENERALPATH + "img/3.png)");

    //Bouton4
    QFile fichier3(GENERALPATH + "img/4.png");
    if(fichier3.exists())
        ui->pushButton_4->setStyleSheet("background-image: url( " + GENERALPATH + "img/4.png)");

    //Bouton5
    QFile fichier4(GENERALPATH + "img/5.png");
    if(fichier4.exists())
        ui->pushButton_5->setStyleSheet("background-image: url( " + GENERALPATH + "img/5.png)");

    //Bouton6
    QFile fichier5(GENERALPATH + "img/6.png");
    if(fichier5.exists())
        ui->pushButton_6->setStyleSheet("background-image: url( " + GENERALPATH + "img/6.png)");

    //Bouton7
    QFile fichier6(GENERALPATH + "img/7.png");
    if(fichier6.exists())
        ui->pushButton_7->setStyleSheet("background-image: url( " + GENERALPATH + "img/7.png)");

    //Bouton stop
    QFile fichier7(GENERALPATH + "img/8.png");
    if(fichier7.exists())
        ui->STOP->setStyleSheet("background-image: url( " + GENERALPATH + "img/8.png)");
}
void MainWindow::changemyconfig(QString Com)
{
    QString idmachine       = Com.mid(2,2);
    QString idpupitre       = Com.mid(4,2);
    QString idbptablette    = Com.mid(6,2);
    QString volume          = Com.mid(8,2);
    QString tempo           = Com.mid(10,3);
    QString texte           = idmachine + "\n" + idpupitre + "\n" + idbptablette + "\n" + volume + "\n" + tempo;
    QFile FichierConf(GENERALPATH + "config/config.text");
    QTextStream flux(&FichierConf);
    if(FichierConf.exists())
    {
        FichierConf.open(QIODevice::WriteOnly |  QIODevice::Truncate | QIODevice::Text);
        flux << texte;
    }
    FichierConf.close();
}

void MainWindow::on_pushButton_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "01");
    player->stop();
    player->setVolume(VOLUME);
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/1.mp3"));
    player->play();
    VOLUME = BASEVOLUME;

}
void MainWindow::on_pushButton_2_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "02");
    player->stop();
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/2.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}

void MainWindow::on_pushButton_3_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "03");
    player->stop();
    player->setVolume(VOLUME);
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/3.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}
void MainWindow::on_pushButton_4_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "04");
    player->stop();
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/4.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}
void MainWindow::on_pushButton_5_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "05");
    player->stop();
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/5.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}
void MainWindow::on_pushButton_6_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "06");
    player->stop();
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/6.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}
void MainWindow::on_pushButton_7_clicked()
{
    rfcomm->sendLine("$L" + IDMACHINE + IDPUPITRE + IDBPTABLETTE + "07");
    player->stop();
    player->setMedia(QUrl::fromLocalFile(GENERALPATH + "son/7.mp3"));
    player->play();
    VOLUME = BASEVOLUME;
}
void MainWindow::on_STOP_clicked()
{
    VOLUME = BASEVOLUME;
    player->stop();
}

MainWindow::~MainWindow()
{
    delete ui;
}
