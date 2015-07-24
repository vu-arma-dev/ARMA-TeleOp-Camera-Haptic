/*
 * QCvCamera
 * Written by Ellie Fitzpatrick
 * Last Edited: 1/21/15
 *
 * The purpose of this class is to access usb cameras easily.
 *
 * Warning: There is a known issue with connecting multiple cameras.
 * You might need to play with the camera selected to get it to work.
 */

#ifndef QCVCAMERA_H
#define QCVCAMERA_H
#define _CAM_NORMAL_REFRESH_TIME_MS 50 // 20 Hz camera frequency

#include <QObject>
#include <QApplication>
#include <QGraphicsView>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <QMenu>
#include <QAction>
#include <QMainWindow>
#include <QMenuBar>
#include <QStringList>
#include <QDir>
#include <QImage>
#include <QDebug>
#include <QTransform>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <qcvfilter.h>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QIntValidator>
#include <QSlider>
#include <QSpinBox>
#include <QDialogButtonBox>

#include <QPixmap>

#include <vector>
//#include <include/Camera/qcvfilter.h>

struct sQCvCameraSettings{

    int acquisitionRate;
    int iCurrentCamera;
    bool bUSE_SCALING;
    QString outputFile;
    bool bSTOP_CLEAR_BUFFER;

    sQCvCameraSettings():acquisitionRate(_CAM_NORMAL_REFRESH_TIME_MS),
                        iCurrentCamera(0),
                        bUSE_SCALING(true),
                        bSTOP_CLEAR_BUFFER(false),
                        outputFile("snapshot"){}


};

Q_DECLARE_METATYPE(sQCvCameraSettings)

class QCvCameraSettingsDlg :public QDialog
{
    Q_OBJECT

public:
    QCvCameraSettingsDlg(sQCvCameraSettings *s);
    sQCvCameraSettings getProperties();
    void setProperties(sQCvCameraSettings s);

signals:
    void updateSettings(sQCvCameraSettings s);


private:
    QString *strAcquisitionRateDisplay;
    QLabel *lAcquisitionRateLabel;
    QSlider *acquisitionRateEdit;
    QSpinBox *iCurrentCameraEdit;
    QCheckBox *scalingEdit;
    QLineEdit *filenameEdit;
    QDialogButtonBox *buttonBox;
    QCheckBox *clearBufferEdit;



private slots:
    void acquisitionRateUpdate(int number);


};

class QCvCamera : public QObject
{
    Q_OBJECT
public:
    QCvCamera(QMainWindow *parent);
    ~QCvCamera();
    //Getters
    virtual int getCameraAcquistionRate();
    virtual void getResolution(int channel, int *height, int *width); // why is this void?

    //Setters
    bool setCurrentCamera(int channel);
    void setCameraAcquisitionRate(int intervalMS);

    //Other
    bool takeSnapshot(QString filename);
    bool isRunning();
    void setOutputView(QGraphicsView *obj);
    void enableScaling(bool b);
    cv::Mat acquireImage();
    void start();
    void stop();
    void addSubMenu(QMenu* m);

    // Filter assignment
    void clearFilters();
    void appendFilter(QCvFilter *filter);

    // Threading
    void setThreadSafe(bool b);

signals:
    void imageReady(QImage);
    void imageReadyPixmap(QPixmap);
    void updateMessage(QString);
    void requestSettingsDlg(sQCvCameraSettings settings);

public slots:
    void takeSnapshot();
    void getSettingsDlg();
    void clearBuffer();
    void updateSettings(sQCvCameraSettings settings);
    void setPause(bool b);

private slots:
    virtual void refreshCamera();
    void toggleCamera();



protected:
    QMainWindow *ui;
    QGraphicsView *view;
    int _MAXCAMERAS;
    QTimer *timerCamera;
    int iCurrentCamera;
    int acquisitionRate;
    cv::VideoCapture vidCap;
    cv::Mat imgCaptured;
    cv::Mat croppedImage;

    sQCvCameraSettings s; //settings struct

    QImage qimgDisplay;
    QImage qimgScaledDisplay;
    bool bCAM_RUNNING;
    bool bPAUSE;

    //Menu Items/Actions
    QMenu *menu;
    QAction *action_Capture;
    QAction *action_Snapshot;
    QAction *action_Settings;
    QAction *action_ClearBuffer;

    QDir currentDir;

    QCvCameraSettingsDlg *settingsDlg;

    // Filter Setup
    std::vector<QCvFilter*> iFilters;

    // Threading Settings
    bool bTHREAD_SAFE;

  };

#endif // QCVCAMERA_H
