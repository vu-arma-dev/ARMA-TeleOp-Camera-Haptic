#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //set text on tbForceAddress
    myOmega7 = new Omega7(0);
    force = new Omega7Udp(1);
    pose = new Omega7Udp(0);
    udpUpdateTimer = new QTimer(this);
    toggleUpdateTimer = new QTimer(this);

    toggleUpdateTimer->start(500);

    QApplication::connect(ui->bStart, SIGNAL(clicked()), this, SLOT(update()));
    QApplication::connect(udpUpdateTimer, SIGNAL(timeout()), this, SLOT(send()));
    QApplication::connect(toggleUpdateTimer, SIGNAL(timeout()), this, SLOT(updateToggle()));


    /**QApplication::connect(ui->bStart, SIGNAL(clicked()), [=] ()
    { force->setAddress(QString(ui->tbForceAddress->toPlainText()));
        force->setPortNumber(ui->tbForcePort->toPlainText().toInt());
        pose->setAddress(QString(ui->tbPoseAddress->toPlainText()));
        pose->setPortNumber(ui->tbPosePort->toPlainText().toInt());
    })


    QApplication::connect(udpUpdateTimer, SIGNAL(timeout()), [=] ()
    { pose->sendPose();
      force->recieve();
      ui->nToggle->display(1);
    });

    QApplication::connect(toggleUpdateTimer, SIGNAL(timeout()), [=] () {ui->nToggle->display(0);});**/
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getPose()

{
    double f[4];
    double q[4];
    double px, py, pz, Rg;
    myOmega7->getPositionAndOrientation(&px,&py,&pz,&Rg,q);
    myOmega7->getAppliedForce(f);
    Eigen::Quaterniond qc(q[0],q[1],q[2],q[3]);
    Eigen::Quaterniond qv(0,px,py,pz);
    Eigen::Quaterniond qs = qc.conjugate();
    Eigen::Quaterniond qt = qv.conjugate();

    pose->setPose(Rg, qt.x(), qt.y(), qt.z(), qs.w(), qs.x(), qs.y(), qs.z());
    QString poseText;
    poseText = QString::number(qt.x()) + QString::number(qt.y()) + QString::number(qt.z());
    ui->lPoseView->setText(poseText);

}

void MainWindow::update()
{   //logic to make sure all fields are filled in
    myOmega7->getDeviceName();
    force->setAddress(QString(ui->tbForceAddress->text()));
    force->setPortNumber(ui->tbForcePort->text().toInt());
    pose->setAddress(QString(ui->tbPoseAddress->text()));
    pose->setPortNumber(ui->tbPosePort->text().toInt());
    bool connected = pose->connect();
    force->connect();
    udpUpdateTimer->start(1000);
}

void MainWindow::send()
{

    getPose();

    pose->sendPose();
    double fx, fy, fz, fg;
    force->getForces(fx, fy, fz, fg);
    ui->lForceView->setText(QString::number(fx));
    ui->nToggle->display(1);
}

void MainWindow:: updateToggle(){
    ui->nToggle->display(0);
}


