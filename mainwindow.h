#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "androidrfcomm.h"
#include <QMainWindow>
#include <QMediaPlayer>
#include "client.h"
#include <QTextTableFormat>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(AndroidRfComm *bluecomm,QWidget *parent = 0);
    ~MainWindow();
    void SaveMyImage(QString Com);
    void SaveMySong(QString Com);
    void InitMyButton();
    void changemyconfig(QString Com);
public slots:
    void on_STOP_clicked();    
    void itstime();
    void TimeToInit();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void appendMessage(const QString &from, const QString &message);
    void toldmewhatIshoulddo(QString Com);
    void MyTimer();
    void MyTimerSlot();
    void Timer_reception_PC();
private slots:


private:
    QMediaPlayer *blablaquiveutriendire;
    AndroidRfComm *rfcomm;
    Ui::MainWindow *ui;
    QString IDMACHINE, IDPUPITRE, IDBPTABLETTE, IDBPTELEC, GENERALPATH;
    int VOLUME;
    int TEMPHS;
    int OIFG;
    QTimer *RFTime;
    QMediaPlayer *player;
    float height;
    float heightP;
    float width;
    float widthP;
    int my_init;
    bool bfr;
    bool ok;
    Client client;
    QString myNickName;
    QTextTableFormat tableFormat;
    QString SonString;
    int SonValue;


};


#endif // MAINWINDOW_H
