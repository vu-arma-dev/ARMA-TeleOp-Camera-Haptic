#include "cvcamera.h"
#include <QDebug>

/*TODO:
Finish making this object thread safe:
1) camera selection dialog will need to be moved outside of the class
2) selected camera updates require signal transmission
3) eventually opencv filtering is needed will subclass off of this class
*/

cvCamera::cvCamera(QMainWindow *parent = 0)
{
    ui = parent;
    iCurrentCamera = 1;
    bCAM_RUNNING = false;
    bUSE_SCALING = true; // default to using scaled image

    scene = new QGraphicsScene();

    timerCamera = new QTimer(this);

    iFilters.clear();

    QApplication::connect(timerCamera,SIGNAL(timeout()),this,SLOT(refreshCamera()));

    // Initialize menus

    action_Camera_Select = new QAction(tr("&Select Camera"),this);
    connect(action_Camera_Select,SIGNAL(triggered()),this,SLOT(openCamPreviewDlg()));
    action_Capture = new QAction(tr("&Capture"),this);
    connect(action_Capture,SIGNAL(triggered()),this,SLOT(toggleCamera()));
    action_Snapshot = new QAction(tr("&Snapshot"),this);
    connect(action_Snapshot,SIGNAL(triggered()),this,SLOT(takeSnapshot()));

    menu=ui->menuBar()->addMenu(tr("&Test Camera"));
    menu->addAction(action_Camera_Select);
    menu->addAction(action_Capture);
    menu->addAction(action_Snapshot);
    vidCap.open(iCurrentCamera);
    action_Capture->setEnabled(true);
}

cvCamera::~cvCamera()
{
    timerCamera->stop(); // timers must stop or thread cannot be terminated
}

void cvCamera::setOutputView(QGraphicsView *obj)
{
    view=obj;
}


void cvCamera::snapshot(QString fname)
{
    cv::Mat output;
    cv::cvtColor(imgCaptured,output,CV_RGB2BGR);

    if (bCAM_RUNNING)
    {
        timerCamera->stop();
        cv::imwrite(fname.toUtf8().constData(),output);
        timerCamera->start(_CAM_NORMAL_REFRESH_TIME_MS);
    }
    else
    {
        cv::imwrite(fname.toUtf8().constData(),output);
    }
}

void cvCamera::openCamPreviewDlg()
{
    timerCamera->stop();
    vidCap.release();
    bCAM_RUNNING = false;
    action_Capture->setText(tr("&Capture"));
    emit requestCamSelection();
}

void cvCamera::updateCurrentCamera(int id)
{
    iCurrentCamera = id;
    vidCap.release();
    timerCamera->stop();
    vidCap.open(iCurrentCamera);
    action_Capture->setEnabled(true);

}

void cvCamera::refreshCamera()
{
    vidCap.read(imgCaptured);
    if(imgCaptured.empty()==true){
        return;
    }
    else{

        cv::cvtColor(imgCaptured,imgCaptured,CV_BGR2RGB);
        // Filter loop
        QCvFilter *f;
        for (unsigned int i=0;i<iFilters.size();i++)
        {
            f = iFilters.at(i);
            cv::Mat temp = f->updateImage(imgCaptured);
            imgCaptured = temp;
        }


        QImage qimgDisplay((uchar*)imgCaptured.data,imgCaptured.cols,imgCaptured.rows,imgCaptured.step,QImage::Format_RGB888);
        if (bUSE_SCALING)
        {
            double widthscale  = (qimgDisplay.width())/(view->width());	   // the greater the number the more the width of the image must be reduced
            double heightscale = (qimgDisplay.height())/(view->height());  // the greater the number the more the height of the image must be reduced
            if (heightscale<widthscale){
                qimgScaledDisplay = qimgDisplay.scaledToWidth(view->width()-10);
            }
            else {
                qimgScaledDisplay = qimgDisplay.scaledToHeight(view->height()-10);
            }
            emit imageReady(qimgScaledDisplay);

        }
        else
        {
            emit imageReady(qimgDisplay);
        }

    }
}

void cvCamera::toggleCamera()
{
    if (bCAM_RUNNING)
    {
        timerCamera->stop();
        bCAM_RUNNING=false;
        action_Capture->setText(tr("&Capture"));
        action_Snapshot->setEnabled(false);
    }
    else
    {

        timerCamera->start(_CAM_NORMAL_REFRESH_TIME_MS);
        bCAM_RUNNING=true;
        action_Capture->setText(tr("&Stop Capture"));
        action_Snapshot->setEnabled(true);
    }
}

void cvCamera::takeSnapshot()
{
    QString fname="snapshot.jpg";
    cv::Mat output;
    cv::cvtColor(imgCaptured,output,CV_RGB2BGR);

    if (bCAM_RUNNING)
    {
        timerCamera->stop();
        cv::imwrite(fname.toUtf8().constData(),output);
        timerCamera->start(_CAM_NORMAL_REFRESH_TIME_MS);
    }
    else
    {
        cv::imwrite(fname.toUtf8().constData(),output);
    }
}

bool cvCamera::isRunning()
{
    return bCAM_RUNNING;
}

void cvCamera::enableScaling(bool b)
{
    bUSE_SCALING = b;
}

void cvCamera::clearFilters()
{
    iFilters.clear();
}

void cvCamera::appendFilter(QCvFilter *filter)
{
    iFilters.push_back(filter);
}

