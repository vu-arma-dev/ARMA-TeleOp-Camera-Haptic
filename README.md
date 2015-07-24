# ARMA-TeleOp-Camera-Haptic
##### Authors: [Ellie Fitzpatrick](https://github.com/efitzpatrick), Jason Pile, and [Long Wang] (https://github.com/wanglong06)
This repository provides utilities for user interface Qt integrations in teleoperation including cameras accessed with opencv and haptic devices. It is currently working with Force Dimension haptic devices, but is avaliable to work with other haptic devices. 

## Table of Contents
* [QCvCamera] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic/blob/master/README.md#qcvcamera)
    * [Features] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#features)
    * [Dependencies] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#dependencies)
    * [Installation] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#installation)
    * [Getting Started] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#getting-started-)
    * [Example Code] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#example-code)
* [Omega7 Udp] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#omega7-udp)
    * [Features] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#features-1)
    * [Dependencies] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#dependencies-1)
    * [Installation] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#installation-1)
    * [Getting Started] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#getting-started)
    * [Example Code] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#example-code-1)
* [Contribute](https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#contribute)
* [Support] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#support)
* [License] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#license)
* [FAQ] (https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic#faq)

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


***
Omega7 Udp
========

This project was built for use with the Omega 7 device created by Force Dimension, but can be used in any circumstance that requires forces, position, and orientation to be sent. This sends information about force, position, and orientation over udp. Has been tested with Linux and Windows.   

Features
--------

- Keeps track of position, orientation, and force
- Makes it easy to use the QUdpSocket! 


Dependencies
-----------
- Qt Framework 
- Eigen version 3.2.1

Installation
------------
Clone the repository and use as a library in project.

Getting Started
---------------------

- Create an Omega7Udp object. The constructor contains a boolean value that is whether it is a force udp or not. 
- Create a QTimer that will be used to fetch the position, orientation, and forces from the Omega7Udp object.
- You will need to create 2 Slots:
    - startUdp()
        - this connects the udp to the values inputted (with omega7object->setAddress and omega7object->setPort) to the udp socket
        - start the timer to update the udp
    - updateUdp()
        - every time the timer times out this slot
        - gets the forces from the force object or gets the position from the postion object depending on which is is recieving
        - sends the forces or position depending on which the computer is sending
- Connect startUdp() to a command that you have set. Make sure you have set the port and address
- Connect updateUdp() to the QTimer’s timeout() signal
- Let it run!


Example Code
--------------------
    //Somewhere in this code you need to set the forces and position. ARMA gets this from the omega7, but you need to set it 
    //for your current haptic device
    MainWindow::MainWindow(QWidget *parent) :
       QMainWindow(parent),
       ui(new Ui::MainWindow)
    {
       ui->setupUi(this);

       force = new Omega7Udp(1);
       pose = new Omega7Udp(0);

       udpUpdateTimer = new QTimer(this);
  
       QApplication::connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startUdp()));
       QApplication::connect(udpUpdateTimer, SIGNAL(timeout()), this, SLOT(updateUdp()));
      }

    MainWindow::~MainWindow()
    {
       delete ui;
    }

    void MainWindow::startUdp()
    {   //logic to make sure all fields are filled in
       force->setAddress(“192.168.1.15”);
       force->setPortNumber(20000);
       pose->setAddress( “192.168.1.102”);
       pose->setPortNumber(25000);

       pose->connect();
       force->connect();

       udpUpdateTimer->start(1000);
       }
    }

    void MainWindow::updateUdp()
    {
       getPose();
       pose->sendPose();
       force->recieveForce();
       updateForces();
    }
    }



***
Contribute
----------

- Issue Tracker: https://github.com/vu-arma-dev/ARMA-TeleOp-Camera-Haptic/issues

Support
-------

If you are having issues, please let us know. Email Ellie Fitzpatrick (efitzpatrick@harpethhall.org) or Long Wang (wanglong06@gmail.com)

License
-------

Copyright [2015] [ARMA Laboratory]


FAQ
=====


