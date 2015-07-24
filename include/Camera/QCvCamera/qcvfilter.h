#ifndef QCVFILTER_H
#define QCVFILTER_H

#include <QObject>
#include <QtCore>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

using namespace cv;

class QCvFilter : public QObject
{
    Q_OBJECT
public:
    explicit QCvFilter(QObject *parent = 0);
    ~QCvFilter();

signals:
    void statusUpdate(QString msg);
    void pauseProcess(bool b);
public slots:
    void toggleEnable();
    void setEnable(bool flag);
public:
    virtual Mat updateImage(Mat img);
    virtual QMenu* getMenu();
    bool isEnabled();

protected:
    bool bENABLE;
    QMenu* menu;

private:



};

#endif // QCVFILTER_H
