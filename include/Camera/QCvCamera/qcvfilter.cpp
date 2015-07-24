#include "qcvfilter.h"

QCvFilter::QCvFilter(QObject *parent) :
    QObject(parent)
{
}

QCvFilter::~QCvFilter()
{

}

Mat QCvFilter::updateImage(Mat img)
{
    // ADD CODE TO MODIFY IMAGE HERE
    qDebug()<<"Entered filter image update";
    return img;
}

QMenu *QCvFilter::getMenu()
{
    return menu;
}

void QCvFilter::setEnable(bool flag)
{
    bENABLE = flag;
}

void QCvFilter::toggleEnable()
{
    if(this->isEnabled()) this->setEnable(false);
    else this->setEnable(true);
}

bool QCvFilter::isEnabled()
{
    return bENABLE;
}
