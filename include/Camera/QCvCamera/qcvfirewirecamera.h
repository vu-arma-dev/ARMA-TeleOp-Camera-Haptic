#ifndef QCVFIREWIRECAMERA_H
#define QCVFIREWIRECAMERA_H

#include <qcvcamera.h>
#include <Windows.h>
#include <1394Camera.h>
#include <QComboBox>
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
#include <QImage>
#include <QDebug>
#include <QTransform>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
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
#include <qcvfilter.h>

#define _FIREWIRE_CAM_NORMAL_REFRESH_TIME_MS 75

struct sQCvFirewireCameraSettings{

    int acquisitionRate;
    int iCurrentCamera;
    int iFrameIndex;
    bool bUSE_SCALING;
    QString outputFile;
    bool bSTOP_CLEAR_BUFFER;
    std::vector<int> vAvailableChannels;
    std::vector<int> vAvailableFrames;
    std::vector<QString> vAvailableChoices;
    sQCvFirewireCameraSettings():acquisitionRate(_FIREWIRE_CAM_NORMAL_REFRESH_TIME_MS),
                        iCurrentCamera(1),
                        iFrameIndex(3),
                        bUSE_SCALING(true),
                        bSTOP_CLEAR_BUFFER(false),
                        outputFile("snapshot"){}


};

Q_DECLARE_METATYPE(sQCvFirewireCameraSettings)

class QCvFirewireCameraSettingsDlg :public QDialog
{
    Q_OBJECT

public:
    QCvFirewireCameraSettingsDlg(sQCvFirewireCameraSettings *s);
    sQCvFirewireCameraSettings getProperties();
    void setProperties(sQCvFirewireCameraSettings s);



signals:
    void updateSettings(sQCvFirewireCameraSettings s);


private:
    sQCvFirewireCameraSettings settings;
    QString *strAcquisitionRateDisplay;
    QComboBox *channelListingBox;
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

class QCvFirewireCamera : public QObject
{
    Q_OBJECT

public:
    QCvFirewireCamera(QMainWindow *parent);
    ~QCvFirewireCamera();
    //Getters
    int getCameraAcquistionRate();
    void getResolution(int channel, int framerate, int *height, int *width); // why is this void?
    const QString getErrorMessage();
    //Setters
    bool setCurrentCamera(int channel,int framerate);
    void setCameraAcquisitionRate(int intervalMS);

    // Other
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
    void requestSettingsDlg(sQCvFirewireCameraSettings);

public slots:
    void takeSnapshot();
    void getSettingsDlg();
    void clearBuffer();
    void updateSettings(sQCvFirewireCameraSettings settings);

private slots:
    void refreshCamera();
    void toggleCamera();

private:
    QMainWindow *ui;
    QGraphicsView *view;
    QTimer *timerCamera;
    int acquisitionRate;
    cv::Mat imgCaptured;
    cv::Mat croppedImage;

    C1394Camera cmucam;
    IplImage *iplImage;
    unsigned long vHeight, vWidth;
    QString statusMsg;
    sQCvFirewireCameraSettings s;
    bool buildSettingsList();

    QImage qimgDisplay;
    QImage qimgScaledDisplay;
    bool bCAM_RUNNING;

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

#endif // QCVFIREWIRECAMERA_H
