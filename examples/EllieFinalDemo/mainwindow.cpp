#include "mainwindow.h"
#include "ui_mainwindow.h"






MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    //find out whether Recieving or Streaming
    ui->setupUi(this);

    //if streaming
    //if recieving
    /**
    receive = new QCvReceiver(this);
    cameraFeed = new QGraphicsScene();
    ui->videoStream->setScene(cameraFeed);
    udp = new QUdpSocket(this);
    if(udp->bind(QHostAddress("192.168.1.15"), 85000))//QHostAddress::AnyIPv4
    {
        qDebug()<<"Port successfully opened!";
    }
    else qDebug()<<"Port assignment failed!";
    QApplication::connect(udp,SIGNAL(readyRead()),this,SLOT(updateImage()));
    **/

    disableReceivingOptions();
    pose= new Omega7Udp(0, this);
    force = new Omega7Udp(1, this);
    myOmega7 = new Omega7(0);
    myOmega7->getDeviceName();
    Eigen::Quaterniond temp (1,0,0,0);
    master_q_slave  = temp;
    bool left = myOmega7->isLeftHanded();


    menu = ui->menuBar->addMenu(tr("&Image"));
    changeImage = new QAction(tr("&Change Image"),this);
    menu->addAction(changeImage);
    connect(changeImage ,SIGNAL(triggered()),this,SLOT(setDiagnosticImage()));

    udpUpdateTimer = new QTimer();
    QApplication::connect(udpUpdateTimer, SIGNAL(timeout()), this, SLOT(updateUdp()));
    udpUpdateTimer->start(50);
    if(left) force->setForces(0, 0, 0, -.8);
    else force->setForces(0, 0, 0, .8);
    force->setPortNumber(19900);
    force->setAddress("192.168.1.15");
    pose->setAddress("192.168.1.167");
    pose->setPortNumber(27000);

    QApplication::connect(ui->StreamingCamera, SIGNAL(clicked()), this, SLOT(startCamera()));
    QApplication::connect(ui->ReceivingCamera, SIGNAL(clicked()), this, SLOT(enableReceivingOptions()));
    QApplication::connect(ui->bOkayReceiver, SIGNAL(clicked()), this, SLOT(startRecieving()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::disableCameraOptions()
{
    ui->lstreamRecieve->setVisible(false);
    ui->StreamingCamera->setEnabled(false);
    ui->StreamingCamera->setVisible(false);
    ui->ReceivingCamera->setEnabled(false);
    ui->ReceivingCamera->setVisible(false);
}

void MainWindow::disableReceivingOptions()
{
    ui->lAddress->setVisible(false);
    ui->lPort->setVisible(false);
    ui->tbAddress->setVisible(false);
    ui->tbAddress->setEnabled(false);
    ui->tbPort->setVisible(false);
    ui->tbPort->setEnabled(false);
    ui->bOkayReceiver->setVisible(false);
    ui->bOkayReceiver->setEnabled(false);
}

void MainWindow::enableReceivingOptions()
{
    disableCameraOptions();
    ui->lAddress->setVisible(true);
    ui->lPort->setVisible(true);
    ui->tbAddress->setVisible(true);
    ui->tbAddress->setEnabled(true);
    ui->tbPort->setVisible(true);
    ui->tbPort->setEnabled(true);
    ui->bOkayReceiver->setVisible(true);
    ui->bOkayReceiver->setEnabled(true);
}

void MainWindow::startCamera()
{
    disableCameraOptions();
    cam = new QCvCamera(this);
    sendImages = new QCvImageUdp(this);
    cameraFeed = new QGraphicsScene();
    ui->videoStream->setScene(cameraFeed);
    cam->setOutputView(ui->videoStream);
    QApplication::connect(cam, SIGNAL(imageReady(QImage)), this, SLOT(updateDisplay(QImage)));
}

void MainWindow::startRecieving()
{
    disableReceivingOptions();
    cameraFeed = new QGraphicsScene();
    ui->videoStream->setScene(cameraFeed);
    udp = new QUdpSocket(this);
    if(udp->bind(QHostAddress(ui->tbAddress->text()), ui->tbPort->text().toInt()))//QHostAddress::AnyIPv4
    {
        qDebug()<<"Port successfully opened!";
    }
    else qDebug()<<"Port assignment failed!";
    QApplication::connect(udp,SIGNAL(readyRead()),this,SLOT(updateImage()));
    ui->label->setText("Send images to port 85000");
}

void MainWindow::updateImage()
{
    QByteArray buff;
    buff.resize(udp->pendingDatagramSize());
    QHostAddress sender;
    quint16 senderPort;
    udp->readDatagram(buff.data(),buff.size(),&sender,&senderPort);
    std::vector<uchar> videoBuffer;
    int result = buff.size();
    videoBuffer.resize(result);
    //qDebug()<<"Received "+QString::number(result)+" bytes";
    std::memcpy((char*)(&videoBuffer[0]), buff.data(), result);
    cv::Mat jpegimage = imdecode(cv::Mat(videoBuffer),CV_LOAD_IMAGE_COLOR);
    QImage img = QtCv::matToQImage(jpegimage,QImage::Format_RGB888);
    img = img.scaledToHeight(ui->videoStream->height()-10);
    cameraFeed->clear();
    cameraFeed->addPixmap(QPixmap::fromImage(img));
}

void MainWindow::updateDisplay(QImage img)
{
    cameraFeed->clear();
    cameraFeed->addPixmap(QPixmap::fromImage((img)));
    if(sendImages->stream) sendImages->updateImage(cam->acquireImage());
}

void MainWindow::setDiagnosticImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open Image File",QDir::currentPath());
    QImage image(fileName);
    //QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(picture));

    imageView = new QGraphicsScene();
    ui->diagnosticImage->setScene(imageView);
    imageScaled = QImage(image.scaled(ui->diagnosticImage->width()-3, ui->diagnosticImage->height()-3));
    imageView->clear();
    imageView->addPixmap(QPixmap::fromImage(imageScaled));
}

void MainWindow::viewPoseForce()
{
    double g, x, y, z;
    pose->getPose(g, x, y, z);
    QString gripper = "gripper= " + QString::number(g);
    QString xd = "x= " + QString::number(x);
    QString yd = "y= " + QString::number(y);
    QString zd = "z= " + QString::number(z);
    ui->g->setText(gripper);
    ui->x->setText(xd);
    ui->y->setText(yd);
    ui->label->setText(zd);

    force->getForces(x, y, z, g);
    gripper = "gripper= " + QString::number(g);
    xd = "x= " + QString::number(x);
    yd = "y= " + QString::number(y);
    zd = "z= " + QString::number(z);
    ui->fg->setText(gripper);
    ui->fx->setText(xd);
    ui->fy->setText(yd);
    ui->fz->setText(zd);
}

void MainWindow::updateUdp()
{
    getPose();
    if (pose->sendReceiveDataEnabled) pose->sendPose();
    updateForces();

    viewPoseForce();
}


/** Interface with Omega7 **/
void MainWindow::updateForces()
{
    double fx, fy, fz, fg;
    force->getForces(fx, fy, fz, fg);
    QString forceLabel = QString::number(fx) + ", " + QString::number(fy) + ", " + QString::number(fz) + ", " + QString::number(fg);
    Eigen::Quaterniond qf(0,fx,fy,fz);
    Eigen::Quaterniond qff;

    qff =master_q_slave.conjugate()*qf*master_q_slave;
    myOmega7->setConstantForce(qff.x(),qff.y(),qff.z(),fg);
}

void MainWindow::getPose()
{
    double  px, py, pz, Rg;
    double f[4];
    myOmega7->getPositionAndOrientation(&px,&py,&pz,&Rg,q);
    myOmega7->getAppliedForce(f);;
    Eigen::Quaterniond qc(q[0],q[1],q[2],q[3]);
    Eigen::Quaterniond qv(0,px,py,pz);
    Eigen::Quaterniond qs = master_q_slave*qc*master_q_slave.conjugate();
    Eigen::Quaterniond qt = master_q_slave*qv*master_q_slave.conjugate();
    pose->setPose(Rg ,qt.x(), qt.y(), qt.z(), qs.w(), qs.x(), qs.y(), qs.z());

}


