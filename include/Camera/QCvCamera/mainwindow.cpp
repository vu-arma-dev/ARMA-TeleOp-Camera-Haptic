#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    cam = new QCvCamera(this);
    cam->setCurrentCamera(0);
    ui->setupUi(this);
    view = new QGraphicsView();
    scene = new QGraphicsScene();
    ui->leftEye->setScene(scene);
    cam->setOutputView(ui->leftEye);
    cam->start();
    QApplication::connect(cam,SIGNAL(imageReady(QImage)), this, SLOT(updateDisplay(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDisplay(QImage img)
{
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(img));
}

