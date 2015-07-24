#include "qcvimagestorage.h"

QCvImageStorage::QCvImageStorage(QObject *parent)
{
    this->setEnable(false);
    this->clearData();
    this->timer.start();
    this->constructMenu();

}

QCvImageStorage::~QCvImageStorage()
{

}

void QCvImageStorage::setMainWindow(QMainWindow *p)
{

}

Mat QCvImageStorage::updateImage(Mat img)
{
    if(this->isEnabled())
    {
        qDebug()<<"Recording!";
        sFrameData f;
        f.imgSize.height=img.rows;
        f.imgSize.width=img.cols;
        f.frame = img.clone();
        f.pose=buffer;
        f.time=timer.elapsed();
        vHistory.push_back(f);
    }
    return img;
}

void QCvImageStorage::clearData()
{
    vHistory.clear();
}

void QCvImageStorage::updateMenus()
{
    if (this->isEnabled())
    {
        action_ToggleEnable->setText("Disable");
    }
    else
    {
        action_ToggleEnable->setText("Enable");
    }
}

bool QCvImageStorage::saveData()
{
    emit pauseProcess(true);
    bool SUCCESS=false;
    if (vHistory.size()>0)
    {
        // Determine video file name
        QDir cDir(QDir::currentPath());
        QString searchName="iVideo*.avi";
        QStringList files=cDir.entryList(QStringList(searchName),QDir::Files | QDir::NoSymLinks);
        QString vname = "iVideo"+QString::number(files.count()+1)+".avi";
        // Designate file name
        QString fname = "iData"+QString::number(files.count()+1)+".csv";

        sFrameData temp = vHistory.at(0);

        cv::VideoWriter writer;

        // Open video file and text file
        int vtype = CV_FOURCC('M', 'S', 'V', 'C');
        qDebug()<< vtype;
        QFile fText(fname);
        if (fText.open(QIODevice::Append))
        {
            QTextStream out(&fText);
            if(writer.open(vname.toStdString(),vtype,15,temp.frame.size(),true))
            {
                // Begin writing loop
                for (int i=0; i<vHistory.size();i++)
                {
                    temp=vHistory.at(i);
                    Mat output;
                    cv::cvtColor(temp.frame,output,CV_BGR2RGB);
                    writer.write(output);
                    out<<temp.time<<","<<temp.pose.cameraDepth<<","<<temp.pose.q0<<","<<temp.pose.qx<<","<<temp.pose.qy<<","<<temp.pose.qz<<","<<temp.pose.wx<<","<<temp.pose.wy<<","<<temp.pose.wz<<"\n";
                }
                writer.release();

                SUCCESS=true;
            }
            else qDebug()<<"Error opening video file!";
            fText.close();
        }
        else qDebug()<<"Error opening text file!";
    }
    else qDebug()<<"No images saved!";


    emit pauseProcess(false);
    return SUCCESS;
}

void QCvImageStorage::updateBuffer(sPoseData s)
{
    buffer = s;
}


void QCvImageStorage::constructMenu()
{
    menu = new QMenu("Image Recording");
    action_Reset = new QAction(tr("&Reset Data"),this);
    connect(action_Reset,SIGNAL(triggered()),this,SLOT(clearData()));
    action_Save = new QAction(tr("&Save Video"),this);
    connect(action_Save,SIGNAL(triggered()),this,SLOT(saveData()));
    action_ToggleEnable = new QAction("Enable",this);
    connect(action_ToggleEnable,SIGNAL(triggered()),this,SLOT(toggleEnable()));
    connect(action_ToggleEnable,SIGNAL(triggered()),this,SLOT(updateMenus()));
    menu->addAction(action_Save);
    menu->addAction(action_Reset);
    menu->addAction(action_ToggleEnable);

}
