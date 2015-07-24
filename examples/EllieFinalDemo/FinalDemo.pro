#-------------------------------------------------
#
# Project created by QtCreator 2015-07-16T14:48:04
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FinalDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../include/Camera/QCvCamera/qcvimageudp.cpp \
    ../../include/Camera/QCvCamera/qcvfilter.cpp \
    ../../include/Camera/QCvCamera/qcvcamera.cpp \
    ../../include/Omega7/Omega7.cpp \
    ../../include/Omega7/omega7udp.cpp

HEADERS  += mainwindow.h \
    ../../include/Camera/QCvCamera/qcvimageudp.h \
    ../../include/Camera/QCvCamera/qcvfilter.h \
    ../../include/Camera/QCvCamera/qcvcamera.h \
    ../../include/Omega7/Omega7.h \
    ../../include/Omega7/omega7udp.h

FORMS    += mainwindow.ui

INCLUDEPATH += "C:\Users\long\Desktop\GIT_Repositories_W530\armaGenLib.git\include\Omega7"
INCLUDEPATH += "C:\Users\long\Desktop\GIT_Repositories_W530\armaGenLib.git\opencv\include"
INCLUDEPATH += "C:\Users\long\Desktop\GIT_Repositories_W530\armaGenLib.git\opencv\include\opencv2"
INCLUDEPATH += "C:\Users\long\Desktop\GIT_Repositories_W530\armaGenLib.git\include\Camera\QCvCamera"
INCLUDEPATH += "C:\Dev\Eigen_3_2_1\Eigen"
INCLUDEPATH += "C:\Program Files\Force Dimension\sdk-3.4.2\include"
INCLUDEPATH += "C:\Users\long\Desktop\GIT_Repositories_W530\armaGenLib.git\include\Utilities\Conversions"

Release: QMAKE_LIBDIR += C:\dev\opencv\build\x86\vc10\lib
Debug: QMAKE_LIBDIR +=  C:\dev\opencv\build\x86\vc10\lib

Debug {
LIBS +=         opencv_core249d.lib \
                opencv_highgui249d.lib \
                opencv_imgproc249d.lib \
                opencv_video249d.lib \
                opencv_features2d249d.lib \
                opencv_legacy249d.lib \
                opencv_flann249d.lib
}

Release {
LIBS +=         opencv_core249.lib \
                opencv_highgui249.lib \
                opencv_imgproc249.lib \
                opencv_video249.lib \
                opencv_features2d249.lib \
                opencv_legacy249.lib \
                opencv_flann249.lib \
                opencv_nonfree249.lib \
                opencv_calib3d249.lib \
                opencv_contrib249.lib
}

LIBS += "C:\Program Files\Force Dimension\sdk-3.4.2\lib\dhdms.lib"\
        "C:\Program Files\Force Dimension\sdk-3.4.2\lib\drdms.lib"
