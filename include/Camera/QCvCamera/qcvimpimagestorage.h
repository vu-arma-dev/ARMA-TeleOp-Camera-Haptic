#ifndef QCVIMPIMAGESTORAGE_H
#define QCVIMPIMAGESTORAGE_H

#include <qcvfilter.h>
#include <opencv.hpp>
#include <QTime>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>

using namespace cv;
using namespace std;

struct sImpMeasurement{
    int frame;
    double time;
    double channel;
    double impedance;

    sImpMeasurement(): time(0), channel(1),impedance(28000),frame(0){}
};

struct sImpFrameData{
    Size imgSize;
    Mat frame; // uninitialized frame data
    int time; // time using ms counter
};

typedef vector<sImpFrameData> camHist;
typedef vector<sImpMeasurement> impHist;

Q_DECLARE_METATYPE(sImpMeasurement)

class QCvImpImageStorage : public QCvFilter
{
    Q_OBJECT
public:
    explicit QCvImpImageStorage(QObject *parent = 0);
    ~QCvImpImageStorage();
    void setMainWindow(QMainWindow *p);
    Mat updateImage(Mat img);


signals:

public slots:
    bool saveData();
    void addMeasurement(sImpMeasurement p);
    void clearData();
    void updateMenus();

private:
    camHist vHistory;
    impHist vImp;
    QTime timer;
    int frame;
    // Menu Item
    void constructMenu();
    QAction *action_Reset;
    QAction *action_Save;
    QAction *action_ToggleEnable;



};

#endif // QCVIMPIMAGESTORAGE_H
