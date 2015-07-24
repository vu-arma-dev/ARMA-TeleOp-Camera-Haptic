# ARMA-TeleOp-Camera-Haptic
##### Authors: [Ellie Fitzpatrick](https://github.com/efitzpatrick), Jason Pile, and [Long Wang] (https://github.com/wanglong06)
This repository provides utilities for user interface Qt integrations in teleoperation including cameras accessed with opencv and haptic devices. It is currently working with Force Dimension haptic devices, but is avaliable to work with other haptic devices. 

## Table of Contents
* [QCvCamera] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic/blob/master/README.md#qcvcamera)

****
QCvCamera
========

QCvCamera makes it easy to stream video from a camera through USB. 

To use include QCvCamera and QCvFilter:

    MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    {

       ui->setupUi(this);
       cam = new QCvCamera(this);
       scene = new QGraphicsScene();
       ui->graphicsView->setScene(scene);
       cam->setOutputView(ui->graphicsView);
       QApplication::connect(cam, SIGNAL(imageReady(QImage)), this, SLOT(updateDisplay(QImage)));
    }

Features
--------

- Takes snapshots
- Change cameras 
- Implement multiple cameras
- Clear the buffer
 

Dependencies
------------
OpenCv: core, highgui, imgprocessing, video, feature, legacy, flann, nonfree, calibration, contribution .   
  
The libraries below *should* be in your project file of qt if you are using qt.

      INCLUDEPATH += “The path to the location of your opencv .lib files”  
      LIBS +=        opencv_core249.lib \  
                     opencv_highgui249.lib \  
                     opencv_imgproc249.lib \  
                     opencv_video249.lib \  
                     opencv_features2d249.lib \  
                     opencv_legacy249.lib \  
                     opencv_flann249.lib \  
                     opencv_nonfree249.lib \  
                     opencv_calib3d249.lib \  
                     opencv_contrib249.lib  


Installation
------------

Pull the repository and link the files in the project file. Use the repository for your camera need!

Getting Started 
--------------------

- Create a new QCvCamera
- Create a new QGraphics Scene 
- Go to the ui, add a Graphics View and take notice of it’s name
- ui->graphicsView-> setScene( your QGraphicsScene)
- set the output view of your camera to your graphics view
- Create a slot, updateImage(QImage img), which:
    - clears the graphics scene
    - adds the a pixmap to the qgraphics scene (which is the paramater QImage)
- connect QCvCamera’s signal image ready to your slot update image

Example Code
----------------------

    MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
       ui(new Ui::MainWindow)
    {

       ui->setupUi(this);
       cam = new QCvCamera(this);
       scene = new QGraphicsScene();
       ui->graphicsView->setScene(scene);
       cam->setOutputView(ui->graphicsView);
       QApplication::connect(cam, SIGNAL(imageReady(QImage)), this, SLOT(updateDisplay(QImage)));
    }

    MainWindow::~MainWindow()
    {
       delete ui;
    }

    void MainWindow::updateDisplay(QImage img)
    {
       scene->clear();
       scene->addPixmap(QPixmap::fromImage((img)));
    }

Contribute
----------

- Issue Tracker: github.com/$project/$project/issues

Support
-------

If you are having issues, please let us know. Email [Ellie Fitzpatrick] (mailto:efitzpatrick@harpethhall.org)

License
-------

The project is licensed under the BSD license.

***
FAQ
=====


