#ifndef QCVCAMERACALIBRATION_H
#define QCVCAMERACALIBRATION_H

#include <qcvfilter.h>
#include <QObject>
#include <QMainWindow>
#include <core/core.hpp>
#include <imgproc/imgproc.hpp>
#include <calib3d/calib3d.hpp>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QIntValidator>
#include <QDialogButtonBox>
#include <QImage>
#include <QTime>

#include <QMenu>
#include <QAction>
#include <QMenuBar>

#include <Conversions/QtOpenCvConversions.hpp>

#define _DEFAULT_CHECKER_ROWS 4
#define _DEFAULT_CHECKER_COLS 3
#define _DEFAULT_SAMPLE_INTERVAL 500
#define _DEFAULT_SQUARE_SIZE_MM 10
#define _DEFAULT_MIN_NUM_SAMPLES 10

using namespace std;

struct sCvCalibSettings{
    cv::Size boardSize;
    bool bTRACKING;
    bool bCORRECTION_ON;
    bool bCALIBRATED;
    QString outputFileName;
    int timeBetweenCalibSamples;
    float squareSize; // using mm for length measure
    unsigned int minNumSamples;

    sCvCalibSettings():bTRACKING(false),
                       bCORRECTION_ON(false),
                       bCALIBRATED(false),
                       timeBetweenCalibSamples(_DEFAULT_SAMPLE_INTERVAL),
                       squareSize(_DEFAULT_SQUARE_SIZE_MM),
                       minNumSamples(_DEFAULT_MIN_NUM_SAMPLES),
                       outputFileName("cameracalib.xml"){}
};

class QCvCameraCalibrationDlg : public QDialog
{
    Q_OBJECT

public:
    explicit QCvCameraCalibrationDlg(sCvCalibSettings *s);
    sCvCalibSettings getProperties();
    void setProperties(sCvCalibSettings s);

private:
    QLineEdit *rowsEdit;
    QLineEdit *colsEdit;
    QCheckBox *trackingCheckBox;
    QCheckBox *correctionCheckBox;
    QDialogButtonBox *buttonBox;


};

class QCvCameraCalibration : public QCvFilter
{
    Q_OBJECT


public:
    QCvCameraCalibration(QObject *parent = 0);
    ~QCvCameraCalibration();
    const bool isCalibrated();
    bool setCheckerboardDimensions(int rows, int cols);
    Mat updateImage(Mat img);
    QMenu* getMenu();

signals:

    void statusUpdate(QString);


public slots:
    void getSettingsDlg();
    void resetData();
    void calibrate();
    void load();

private:
    QMainWindow *ui;
    void initialize();
    void calcBoardCornerPositions(cv::Size boardSize, float squareSize, vector<cv::Point3f>& corners);
    void saveCameraParams();
    bool loadCameraParams(QString fid);
    sCvCalibSettings s; //settings

    // Calibration data
    QTime *time;
    vector<vector<cv::Point2f>> imagePoints;
    vector<cv::Point3f> cornerPoints;
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    vector<cv::Mat> rvecs;
    vector<cv::Mat> tvecs;
    cv::Size imgSize;
    double rmsError;
    double reprojectionError;

    // Dialogs
    QCvCameraCalibrationDlg *settingsDlg;

    // Menu Item
    QAction *action_Settings;
    QAction *action_Reset;
    QAction *action_Calibrate;
    QAction *action_Load;

};

#endif // QCVCAMERACALIBRATION_H
