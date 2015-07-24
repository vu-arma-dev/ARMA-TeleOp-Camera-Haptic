#ifndef QCVIMAGESTORAGE_H
#define QCVIMAGESTORAGE_H

#include <qcvfilter.h>
#include <opencv.hpp>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>

using namespace cv;
using namespace std;

struct sPoseData{
    double cameraDepth;
    double q0;
    double qx;
    double qy;
    double qz;
    double wx;
    double wy;
    double wz;

    sPoseData():cameraDepth(0),
                q0(1),
                qx(0),
                qy(0),
                qz(0),
                wx(0),
                wy(0),
                wz(0){}
};

struct sFrameData{
    Size imgSize;
    Mat frame; // uninitialized frame data
    int time; // time using ms counter
    sPoseData pose;

};

typedef vector<sFrameData> camHist;

Q_DECLARE_METATYPE(sPoseData)

class QCvImageStorage : public QCvFilter
{
    Q_OBJECT
public:
    explicit QCvImageStorage(QObject *parent = 0);
    ~QCvImageStorage();
    void setMainWindow(QMainWindow *p);
    Mat updateImage(Mat img);



signals:

public slots:
    bool saveData();
    void updateBuffer(sPoseData p);
    void clearData();
    void updateMenus();

private:
    camHist vHistory;
    QTime timer;

    // Menu Item
    void constructMenu();
    QAction *action_Reset;
    QAction *action_Save;
    QAction *action_ToggleEnable;
    sPoseData buffer;



};

#endif // QCVIMAGESTORAGE_H
