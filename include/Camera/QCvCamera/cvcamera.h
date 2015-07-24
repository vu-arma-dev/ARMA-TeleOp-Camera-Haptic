#ifndef CVCAMERA_H
#define CVCAMERA_H

#define _CAM_NORMAL_REFRESH_TIME_MS 50 // 20 Hz camera frequency

#include <QObject>
#include <QApplication>
#include <QGraphicsView>
#include <QTimer>
#include <opencv.hpp>
#include <highgui.h>
#include <QMenu>
#include <QAction>
#include <QMainWindow>
#include <QMenuBar>
#include <QStringList>
#include <QDir>
#include <qcvfilter.h>
#include <qcvcameracalibration.h>



class cvCamera : public QObject
{
    Q_OBJECT

public:
    cvCamera(QMainWindow *parent);
    ~cvCamera();
    void setOutputView(QGraphicsView *obj);
    void snapshot(QString fname);
    bool isRunning();
    void enableScaling(bool b);
    void clearFilters();
    void appendFilter(QCvFilter *filter);


signals:
    void imageReady(QImage);
    void requestCamSelection();
    void statusMessage(QString);

private slots:
    void openCamPreviewDlg();
    void updateCurrentCamera(int id);
    void refreshCamera();
    void toggleCamera();
    void takeSnapshot();


private:
    QMainWindow *ui;
    QGraphicsView *view;


    QTimer *timerCamera;

    int iCurrentCamera;
    cv::VideoCapture vidCap;
    cv::Mat imgCaptured;

    QGraphicsScene* scene;
    QImage qimgDisplay;
    QImage qimgScaledDisplay;
    bool bCAM_RUNNING;
    bool bUSE_SCALING;

    std::vector<QCvFilter*> iFilters;

    // Menu Item
    QMenu *menu;
    QAction *action_Camera_Select;
    QAction *action_Capture;
    QAction *action_Snapshot;

};

#endif // CVCAMERA_H
