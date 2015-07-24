#include "qcvimpimagestorage.h"

QCvImpImageStorage::QCvImpImageStorage(QObject *parent) :
    QCvFilter(parent)
{
    this->setEnable(false);
    this->clearData();
    this->timer.start();
    this->constructMenu();

}

QCvImpImageStorage::~QCvImpImageStorage()
{

}

Mat QCvImpImageStorage::updateImage(Mat img)
{
    if(this->isEnabled())
    {
        qDebug()<<"Recording!";
        sImpFrameData f;
        f.imgSize.height=img.rows;
        f.imgSize.width=img.cols;
        f.frame = img.clone();
        f.time=timer.elapsed();
        vHistory.push_back(f);
        this->frame=this->frame+1;
    }
    return img;
}

bool QCvImpImageStorage::saveData()
{
    emit pauseProcess(true);
    bool SUCCESS=false;
    qDebug()<<"Entered save data function";
    if (vHistory.size()>0)
    {
        qDebug()<<vHistory.size()<<" frames recorded and "<<vImp.size()<<" impedance measurements taken";
        qDebug()<<"Detected video frames";
        // Determine video file name
        QDir cDir(QDir::currentPath());
        QString searchName="iVideo*.avi";
        QStringList files=cDir.entryList(QStringList(searchName),QDir::Files | QDir::NoSymLinks);
        QString vname = "iVideo"+QString::number(files.count()+1)+".avi";
        // Designate file name
        QString fname = "iData"+QString::number(files.count()+1)+".csv";
        qDebug()<<"test1";
        sImpFrameData temp = vHistory.at(0);
        sImpMeasurement m = vImp.at(0);
        cv::VideoWriter writer;
        qDebug()<<"Completed file definitions";
        // Open video file and text file
        int vtype = CV_FOURCC('M', 'S', 'V', 'C');
        qDebug()<< vtype;
        QFile fText(fname);
        if (fText.open(QIODevice::Append))
        {
            qDebug()<<"Opened text file";
            QTextStream out(&fText);
            if(writer.open(vname.toStdString(),vtype,15,temp.frame.size(),true))
            {
                qDebug()<<"Writing video...";
                // Begin writing loop
                for (int i=0; i<vHistory.size();i++)
                {
                    temp=vHistory.at(i);
                    Mat output;
                    cv::cvtColor(temp.frame,output,CV_BGR2RGB);
                    writer.write(output);

                }
                writer.release();
                for (int i=0;i<vImp.size();i++)
                {
                    m=vImp.at(i);
                    out<<m.frame<<","<<m.time<<","<<m.channel<<","<<m.impedance<<"\n";
                }

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

void QCvImpImageStorage::addMeasurement(sImpMeasurement p)
{
    p.frame=frame;
    vImp.push_back(p);
}

void QCvImpImageStorage::clearData()
{
    vHistory.clear();
    vImp.clear();
    frame=0;
}

void QCvImpImageStorage::updateMenus()
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

void QCvImpImageStorage::constructMenu()
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
