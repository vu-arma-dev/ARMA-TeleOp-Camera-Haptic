#-------------------------------------------------
#
# Project created by QtCreator 2015-06-23T16:35:34
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Omega7Test
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += "C:/Program Files/Force Dimension/sdk-3.4.2/include"\
                "C:/Development/Eigen_3_2_1/Eigen"\
                "C:/Development/EllieSummer/code/include/Omega7"


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../include/Omega7/Omega7.cpp \
    ../../include/Omega7/omega7udp.cpp

HEADERS  += mainwindow.h \
    ../../include/Omega7/Omega7.h \
    ../../include/Omega7/omega7udp.h

FORMS    += mainwindow.ui


LIBS += "C:\Program Files\Force Dimension\sdk-3.4.2\lib\dhdms.lib"\
        "C:\Program Files\Force Dimension\sdk-3.4.2\lib\drdms.lib"

