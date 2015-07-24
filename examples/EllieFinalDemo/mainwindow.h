#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "omega7udp.h"
#include "qcvcamera.h"
#include "qcvimageudp.h"
#include <QFileDialog>
#include <QTimer>
#include "Omega7.h"
#include <Eigen>
#include <QtTest/QTest>
#include <Windows.h>
#include <QDialog>
#include "QtOpenCvConversions.hpp"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void disableCameraOptions();
    void disableReceivingOptions();

    void viewPoseForce();
public slots:
    void updateDisplay(QImage img);
    void setDiagnosticImage();

    void updateUdp();
    void getPose();
    void updateForces();
    void startCamera();
    void startRecieving();
    void updateImage();
    void enableReceivingOptions();

private:
    Ui::MainWindow *ui;

    QTimer * udpUpdateTimer;
    Omega7Udp *pose;
    Omega7Udp *force;

    Omega7 *myOmega7;
    Eigen::Quaterniond master_q_slave;

    QCvCamera *cam;
    QGraphicsScene *cameraFeed;

    QGraphicsScene *imageView;
    QImage imageScaled;


    QCvImageUdp *sendImages;

    QUdpSocket *udp;

    //Eigen::Quaterniond master_q_slave;

    double q[4];
    bool cameraTRecieverF;


protected:
    QMenu *menu;
    QAction *changeImage;

};

#endif // MAINWINDOW_H
