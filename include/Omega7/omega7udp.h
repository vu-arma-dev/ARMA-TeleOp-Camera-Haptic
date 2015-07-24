#ifndef OMEGA7UDP_H
#define OMEGA7UDP_H

#include <QObject>
#include <QUdpSocket>
#include <stdio.h>
//#include <dhdc.h>
//#include <C:/Development/EllieSummer/code/Omega7/Omega7/Omega7.h>
#include <QTimer>
#include <Eigen>
#include <QSettings>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QMainWindow>
#include <QMenuBar>
#include <QDialog>
#include <QFormLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDialogButtonBox>

struct sOmega7UdpSettings{
    QString address;
    int port;
    bool force, pose;

    sOmega7UdpSettings():
        address("127.0.0.1"),
        port(25000),
        force(true),
        pose(false){}

};

class Omega7UdpSettingsDialog :public QDialog
{
    Q_OBJECT
public:
    Omega7UdpSettingsDialog(sOmega7UdpSettings *s);
    sOmega7UdpSettings getProperties();
    void setProperties (sOmega7UdpSettings s);

signals:
    void updateSettings(sOmega7UdpSettings);

private:
    QLabel *poseForceLabel;
    QLabel *blankLabel;
    QLabel *blankLabel2;
    QLabel *addressLabel;
    QLabel *portLabel;

    QLineEdit *addressEdit;
    QLineEdit *portEdit;

    QDialogButtonBox *buttonBox;


};

//class Omega7Udp : public QObject
class Omega7Udp :public QMainWindow
{
    Q_OBJECT
public:
    //explicit Omega7Udp(bool force, QObject *parent = 0);
    explicit Omega7Udp(bool force, QMainWindow *parent = 0);
    ~Omega7Udp();

    bool sendReceiveDataEnabled;
    int getPortNumber();
    QString getAddress();
    void setPortNumber(int number);
    void setAddress(QString string);


    void setForces(double sFX, double sFY, double sFZ, double sFG);
    void getForces (double &gFX, double &gFY, double &gFZ, double &gFG);
    void getForcesArray(double *returnArray);

    void setPose(double posG, double posX, double posY, double posZ, double radW, double radX, double radY, double radZ);
    void getPose(double &posG, double &posX, double &posY, double &posZ);
    void getOrientArray(double (&returnArray)[4]);

protected:
    sOmega7UdpSettings s;
    Omega7UdpSettingsDialog *settingsDlg;

    QMainWindow *ui;

    QMenu *menu;
    QAction *aSettings;
    QAction *aConnect;
    //eventually QAction *aDisconnect;
private:
    int portNumber;
    QString address;
    QUdpSocket *socket;
    bool bForce;
    bool bPose;
    bool bwindows;
    bool blinux;

    QString configFileName;
    int iDeviceCount;
    double fPeakForce;
    double fScale;
    double pg,px,py,pz,Rx,Ry,Rz,Rg;  // position variables
    double fx, fy, fz, fg;
    double q[4]; // orientation quaternion


signals:
public slots:
    bool connect();
    bool recieve();
    bool sendPose();
    bool recievePose(QByteArray datagram);
    bool recievePose();
    bool sendForces();
    bool recieveForce(QByteArray datagram);
    bool recieveForce();
    void getSettingsDialog();

private:


};

#endif // OMEGA7UDP_H
