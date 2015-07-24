#-------------------------------------------------
#
# Project created by QtCreator 2015-06-09T12:58:43
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QCvCamera
TEMPLATE = app

#***
#Makes the qmake search through the directories: opencv2, 3rdParty\cmu1394\include, Utilities, camera, and open cv to find the libraries when called from #include <''>
#When used on a diffrent computer, make sure that these are changed to navigate to your files above
#***

INCLUDEPATH = C:\Development\EllieSummer\code\opencv\include\opencv2 C:\Development\EllieSummer\code\3rdParty\cmu1394\include C:\Development\EllieSummer\code\Include\Utilities \
                C:\Development\EllieSummer\code\Include\Camera\QCvCamera C:\Development\EllieSummer\code\opencv\include\opencv
SOURCES += main.cpp\
        mainwindow.cpp \
    cameracalibration.cpp \
    camerapreview.cpp \
    cvcamera.cpp \
    qcvcamera.cpp \
    qcvcameracalibration.cpp \
    qcvfilter.cpp \
    qcvfirewirecamera.cpp \
    qcvimagestorage.cpp \
    qcvimageudp.cpp \
    qcvimpimagestorage.cpp

HEADERS  += mainwindow.h \
    cameracalibration.h \
    camerapreview.h \
    cvcamera.h \
    qcvcamera.h \
    qcvcameracalibration.h \
    qcvfilter.h \
    qcvfirewirecamera.h \
    qcvimagestorage.h \
    qcvimageudp.h \
    qcvimpimagestorage.h

FORMS    += mainwindow.ui \
    camerapreview.ui

#Libraries

#CMU Properties

INCLUDEPATH += C:\Development\EllieSummer\code\3rdParty\cmu1394\lib

Release: QMAKE_LIBDIR += C:\Development\EllieSummer\code\3rdParty\cmu1394\lib
Debug: QMAKE_LIBDIR += C:\Development\EllieSummer\code\3rdParty\cmu1394\lib
Debug {
LIBS += 1394camerad.lib
}
Release {
LIBS += 1394camera.lib
}

#OpenCv Libraries/Properties
#INCLUDEPATH += C:\Development\opencv\build\x86\vc12\lib
Release: QMAKE_LIBDIR += C:\Development\opencv\build\x86\vc12\lib
Debugs: QMAKE_LIBDIR += C:\Development\opencv\build\x686\vc12\lib

Debug {
LIBS +=  opencv_core249d.lib\
        opencv_highgui249d.lib
       }

Release {
LIBS += opencv_core249.lib\
        opencv_highgui249.lib\
        opencv_imgproc249.lib\
        opencv_video249.lib \
        opencv_features2d249.lib \
        opencv_legacy249.lib \
        opencv_flann249.lib \
        opencv_nonfree249.lib \
        opencv_calib3d249.lib \
        opencv_contrib249.lib

}


