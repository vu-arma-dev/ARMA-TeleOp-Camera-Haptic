#include "qcvfirewirecamera.h"

QCvFirewireCamera::QCvFirewireCamera(QMainWindow *parent = 0)
{
    qRegisterMetaType<sQCvFirewireCameraSettings>();
    s.acquisitionRate = _FIREWIRE_CAM_NORMAL_REFRESH_TIME_MS;
    ui = parent;

    bCAM_RUNNING = false;
    //add label through a connection
    timerCamera = new QTimer(this);

    QApplication::connect(this->timerCamera,SIGNAL(timeout()),this,SLOT(refreshCamera()));

    //Creates menu bar and items
    menu = ui->menuBar()->addMenu(tr("&Firewire Camera"));

    action_Capture = new QAction(tr("Ca&pture"),this);
    menu->addAction(action_Capture);
    connect(action_Capture,SIGNAL(triggered()),this,SLOT(toggleCamera()));

    action_Snapshot = new QAction(tr("&Snapshot"), this);
    menu->addAction(action_Snapshot);
    connect(action_Snapshot, SIGNAL(triggered()), this, SLOT(takeSnapshot()));

    action_Settings = new QAction(tr("S&ettings"), this);
    menu->addAction(action_Settings);
    connect(action_Settings, SIGNAL(triggered()),this, SLOT(getSettingsDlg()));

    action_ClearBuffer = new QAction(tr("Clear &Buffer"), this);
    menu->addAction(action_ClearBuffer);
    connect(action_ClearBuffer, SIGNAL(triggered()), this, SLOT(clearBuffer()));


    clearFilters();

    bTHREAD_SAFE=false; // not thread safe by default
}

QCvFirewireCamera::~QCvFirewireCamera()
{
    //stops timer so that thread can be deactivated
    timerCamera->stop();
}

int QCvFirewireCamera::getCameraAcquistionRate()
{
    return s.acquisitionRate;
}

void QCvFirewireCamera::getResolution(int channel,int framerate, int *height, int *width)
{
    /*Gets the resolution of imgCaptured and
     * sets the memory value of height and width to
     * the witdth and height so that the user
     * can access the two separate value*/
    setCurrentCamera(channel,framerate);
    height[0] = vHeight;
    width[0] = vWidth;
}

const QString QCvFirewireCamera::getErrorMessage()
{
    return statusMsg;
}

bool QCvFirewireCamera::setCurrentCamera(int channel, int framerate)
{
    // NOTES
    // Video Format Mode (Frame rates)

    // 0 0: 160 x 120  color
    // 0 1: 320 x 240  color
    // 0 2: 640 x 480  color
    // 0 3: 640 x 480  color
    // 0 4: 640 x 480  mono
    // 0 5: 640 x 480  mono
    // 0 6: 640 x 480  mono
    // 1 0: 800 x 600  color (0, 1, 2, 3, 4)
    // 1 1: 800 x 600  color (2, 3)
    // 1 2: 800 x 600  mono
    // 1 3: 1024 x 786 color (0, 1, 2, 3)
    // 1 4: 1024 x 786 color (0, 1, 2)
    // 1 5: 1024 x 786 mono  (0, 1, 2, 3, 4)
    // 1 6: 800 x 600  mono
    // 1 7: 1024 x 786 mono  (0, 1, 2, 3)
    s.vAvailableChannels.clear();
    s.vAvailableFrames.clear();
    s.vAvailableChoices.clear();
    this->buildSettingsList();
    QString msg;
    bool bFLAG = false;
    if(cmucam.CheckLink() != CAM_SUCCESS)
    {
        msg="Link check failed";

    }
    else if(cmucam.InitCamera() != CAM_SUCCESS)
    {
        msg="Camera initialization failed";
    }
    else if(cmucam.SetVideoFormat(1) != CAM_SUCCESS)
    {

        msg.append("Video Format Failed\nAvailable Formats:\n");
        for (int i=0;i<8;i++)
        {
            if(cmucam.SetVideoFormat(i) == CAM_SUCCESS)
            {
                msg.append(QString::number(i)+"\n");
            }
        }

    }
    else if(cmucam.SetVideoMode(channel) != CAM_SUCCESS)
    {
        QString msg;
        msg.append("Video Mode Failed\nAvailable Modes:\n");
        for (int i=0;i<8;i++)
        {
            if(cmucam.SetVideoMode(framerate) == CAM_SUCCESS)
            {
                msg.append(QString::number(i)+"\n");
            }
        }

    }
    else if(cmucam.SetVideoFrameRate(3) != CAM_SUCCESS)
    {
        QString msg;
        msg.append("Video Frame Rate Failed\nAvailable Rates:\n");
        for (int i=0;i<8;i++)
        {
            if(cmucam.SetVideoFrameRate(i) == CAM_SUCCESS)
            {
                msg.append(QString::number(i)+"\n");
            }
        }
    }
    else if(cmucam.StartImageAcquisition() != CAM_SUCCESS)
    {
        msg="Image acquisition failed";
    }
    else
    {
        cmucam.GetVideoFrameDimensions(&vWidth,&vHeight);
        iplImage = cvCreateImage(cvSize(vWidth,vHeight),8,3);
        bFLAG=true;
        msg="Successfully initialized firewire camera!";
    }
    statusMsg = msg;
    return bFLAG;
}

void QCvFirewireCamera::setCameraAcquisitionRate(int intervalMS)
{
    /*Resets the acquistion rate in milliseconds*/
    s.acquisitionRate = intervalMS;
}

bool QCvFirewireCamera::takeSnapshot(QString filename)
{
    /*This takes the image from the screen and saves
     * it with the desired filename*/
    if (!imgCaptured.empty())
    {
        qDebug() << "filename: " <<filename;
        cv::Mat output = acquireImage();
        cv::cvtColor(output,output,CV_BGR2RGB);
        if (bCAM_RUNNING)
        {

            timerCamera->stop();
            cv::imwrite(filename.toUtf8().constData(),output);

            timerCamera->start(s.acquisitionRate);
        }
        else
        {
            cv::imwrite(filename.toUtf8().constData(),output);
        }
    }
    return 0;
}

bool QCvFirewireCamera::isRunning()
{
    return bCAM_RUNNING;
}

void QCvFirewireCamera::setOutputView(QGraphicsView *obj)
{
    view=obj;
}

void QCvFirewireCamera::enableScaling(bool b)
{
    s.bUSE_SCALING=b;
}

Mat QCvFirewireCamera::acquireImage()
{
    /*Acquires the image from the camera and
     * converts color, returns the image*/
    cv::Mat image;

    cv::cvtColor(imgCaptured,image,CV_RGB2BGR);

    return image;
}

void QCvFirewireCamera::start()
{
    /*Starts timerCamera with the aquisitionRate
     * sets bCAM_RUNNING to true
     * and resets text in the menu*/
    timerCamera->start(s.acquisitionRate);
    bCAM_RUNNING=true;
    if(bCAM_RUNNING) action_Capture->setText(tr("&Stop Capture"));
}

void QCvFirewireCamera::stop()
{
    /* Stops timerCamera and sets bCAM_RUNNING to false,
     * resets text on the menu to "Capture*/
    timerCamera->stop();
    bCAM_RUNNING=false;
    if (!bCAM_RUNNING) action_Capture->setText(tr("&Capture"));
    if(s.bSTOP_CLEAR_BUFFER) clearBuffer();
}

void QCvFirewireCamera::addSubMenu(QMenu *m)
{
    menu->addMenu(m);
}

void QCvFirewireCamera::clearFilters()
{
    iFilters.clear();
}

void QCvFirewireCamera::appendFilter(QCvFilter *filter)
{
    iFilters.push_back(filter);
}

void QCvFirewireCamera::setThreadSafe(bool b)
{
    bTHREAD_SAFE=b;
}

void QCvFirewireCamera::takeSnapshot()
{
    /*
     * Automatically creates a fileName that is unique by taking the string
     * that was inserted and adds a number to the end to make it unique.
     */
        QString path = QDir::currentPath();
        currentDir = QDir(path);
        QStringList files;
        QString fileName = s.outputFile + "*";
        files = currentDir.entryList(QStringList(fileName),QDir::Files | QDir::NoSymLinks);
        fileName = s.outputFile + QString::number(files.count()+1) + ".jpg";
        takeSnapshot(fileName);

}

void QCvFirewireCamera::getSettingsDlg()
{
    /*Gets the settings dialog, sets it up beneath the current windo
     * then brigns it to the top. If the data is accepted, it will get the properties
     * from the boxes and reset the data in the sQCvCameraSettings s struct.
     * Otherwise it will throw it away. It then restarts the camera and resets the text in
     * the window.*/
    if (bTHREAD_SAFE)
    {
        emit requestSettingsDlg(s);
    }
    else
    {
        sQCvFirewireCameraSettings prev = s;
        bool bContinueRunning=this->isRunning();
        this->stop();
        bCAM_RUNNING = false;
        action_Capture->setText(tr("&Capture"));

        QCvFirewireCameraSettingsDlg dlg(&prev);

        if(dlg.exec() == QDialog::Accepted){
            sQCvFirewireCameraSettings temp = dlg.getProperties();
            s = temp;
            this->setCurrentCamera(s.iCurrentCamera,s.iFrameIndex);
            this->enableScaling(s.bUSE_SCALING);
        }
        if(bContinueRunning)
        {
            this->start();//s.acquisitionRate
            bCAM_RUNNING = true;
            if (bCAM_RUNNING) action_Capture->setText(tr("&Stop Capture"));
        }
    }
}

void QCvFirewireCamera::clearBuffer()
{
    /*Clears the screen to gray, size depends on
     * whether it was scaled or unscaled*/

    QPixmap *clear;
    if (s.bUSE_SCALING){
       clear = new QPixmap(qimgScaledDisplay.width(), qimgScaledDisplay.height());
    }
    else clear = new QPixmap(imgCaptured.cols, imgCaptured.rows);
    emit imageReady(clear->toImage());
}

void QCvFirewireCamera::updateSettings(sQCvFirewireCameraSettings settings)
{
    s=settings;
    this->setCurrentCamera(s.iCurrentCamera,s.iFrameIndex);
    qDebug()<<QString("Current camera and frame: %1 and %2").arg(s.iCurrentCamera).arg(s.iFrameIndex);
}

void QCvFirewireCamera::refreshCamera()
{
    /*Refresh camera gets the image captured and turns it into a cv::mat file
     * and does the scaling, if scaling is enabled.It then transmits qimgScaledDisplay
     * as through imageReady() */
    cmucam.AcquireImage();
    cmucam.getRGB((unsigned char*)(iplImage->imageData), (vHeight*vWidth*3));
    imgCaptured = Mat(iplImage,true);
    if(imgCaptured.empty()==true){
        qDebug() <<"no image found";
        return;
    }
    else
    {
        //loop through image filters in future
        QCvFilter *f;
        for (unsigned int i=0;i<iFilters.size();i++)
        {
           f = iFilters.at(i);
           cv::Mat temp = f->updateImage(imgCaptured);
           imgCaptured = temp;
        }
        QImage qimgDisplay((uchar*) imgCaptured.data, imgCaptured.cols, imgCaptured.rows, imgCaptured.step, QImage::Format_RGB888);

        //changes the output for scaling

        if(s.bUSE_SCALING )
        {
            // Jason's comment: the greater the number, the more of the width of the image must be reduced
            double widthScale = (qimgDisplay.width())/(view->width()); //what ist view?? do i need to initialize this??
            //Jason's comment: the greater the number the more the height of the image must be reduced
            double heightScale = (qimgDisplay.height())/(view->height());

            if (heightScale<widthScale)
            {
                qimgScaledDisplay = qimgDisplay.scaledToWidth(view->width()-10);
            }
            else qimgScaledDisplay = qimgDisplay.scaledToHeight(view->height()-10);

            //emits image to be dispalyed
            emit imageReady(qimgScaledDisplay);
        }

        else
        {
            emit imageReady(qimgDisplay.copy());
        }
    }
}

void QCvFirewireCamera::toggleCamera()
{
    /*Toggles the camera on and off
     * using the start() and stop() functions*/
    if (bCAM_RUNNING)
    {
        stop();
    }

    else
    {
        start();

    }
}


bool QCvFirewireCamera::buildSettingsList()
{

    if (!cmucam.IsInitialized())
    {
        if(cmucam.CheckLink() != CAM_SUCCESS)
        {
            return false;
        }
        else if(cmucam.InitCamera() != CAM_SUCCESS)
        {
            return false;
        }
    }
    // Build available cameras structure
    cmucam.StopImageAcquisition();
    QString desc;
    for (int i=0;i<8;i++)
    {
        if(cmucam.SetVideoFormat(i) == CAM_SUCCESS)
        {
            for (int j=0;j<8;j++)
            {
                if(cmucam.SetVideoMode(j) == CAM_SUCCESS)
                {
                    s.vAvailableChannels.push_back(i);
                    s.vAvailableFrames.push_back(j);
                    unsigned long h,w;
                    cmucam.GetVideoFrameDimensions(&w,&h);
                    unsigned short depth;
                    cmucam.GetVideoDataDepth(&depth);

                    desc = QString("Channel %1, Framerate Mode %2, %3 x %4, %5 bit").arg(i).arg(j).arg(w).arg(h).arg(depth);
                    s.vAvailableChoices.push_back(desc);
                }
            }

        }
    }
    return true;
}


QCvFirewireCameraSettingsDlg::QCvFirewireCameraSettingsDlg(sQCvFirewireCameraSettings *s)
{
    /*Creates a dialog box for user to input settings*/

    // Copy settings

    settings.acquisitionRate = s->acquisitionRate;
    settings.bSTOP_CLEAR_BUFFER = s->bSTOP_CLEAR_BUFFER;
    settings.bUSE_SCALING = s->bUSE_SCALING;
    settings.iCurrentCamera = s->iCurrentCamera;
    settings.iFrameIndex = s->iFrameIndex;
    settings.outputFile = s->outputFile;
    settings.vAvailableChannels = s->vAvailableChannels;
    settings.vAvailableChoices = s->vAvailableChoices;
    settings.vAvailableFrames = s->vAvailableFrames;


    this ->setWindowTitle("Settings");
    QFormLayout *form = new QFormLayout;
    lAcquisitionRateLabel = new QLabel;


    //Acquisition Rate: Row 1
    acquisitionRateEdit = new QSlider(Qt::Horizontal, this);
    acquisitionRateEdit->setMinimum(10);
    acquisitionRateEdit->setMaximum(1000);
    connect(acquisitionRateEdit,SIGNAL(valueChanged(int)),this, SLOT(acquisitionRateUpdate(int)));
    int number;
    QString strAcquisitionRateDisplay;
    number = s->acquisitionRate;
    strAcquisitionRateDisplay = QString::number(s->acquisitionRate);
    lAcquisitionRateLabel->setText(strAcquisitionRateDisplay);
    lAcquisitionRateLabel->setFixedWidth(150);
    form->addRow(lAcquisitionRateLabel, acquisitionRateEdit);
    acquisitionRateEdit->setValue(s->acquisitionRate);

    //iCurrentCamera: Row 2
    channelListingBox = new QComboBox(this);
    for (unsigned int i =0;i<s->vAvailableChoices.size();i++)
    {
        channelListingBox->addItem(s->vAvailableChoices.at(i));
    }
    form->addRow("Current Camera: ", channelListingBox);

    //Scaling: Row 3
    scalingEdit = new QCheckBox(this);
    form->addRow("Scaling: ", scalingEdit);
    scalingEdit->setChecked(s->bUSE_SCALING);

    //Clear Buffer after Stop: Row 4
    clearBufferEdit = new QCheckBox(this);
    form->addRow("Clear Buffer after Stop: ", clearBufferEdit);
    clearBufferEdit->setChecked(s->bSTOP_CLEAR_BUFFER);

    //Snapshot Output FileName: Row 5
    filenameEdit = new QLineEdit(this);
    form->addRow("File Name: ", filenameEdit);
    filenameEdit->setText(s->outputFile);

    //Cancel and Okay Buttons: Row 6
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox :: Cancel , Qt::Horizontal, this);
    form->addRow(buttonBox);
    this->setLayout(form);
    this->connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    this->connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


}

sQCvFirewireCameraSettings QCvFirewireCameraSettingsDlg::getProperties()
{
    /*This will get the properties from the settings
    dialog box and insert them into the struct*/
    settings.acquisitionRate= acquisitionRateEdit->value();
    settings.iCurrentCamera= 1;
    settings.iFrameIndex = 3;
    int x = channelListingBox->currentIndex();
    int c = settings.vAvailableChannels.at(x);

    settings.iCurrentCamera= c;
    settings.iFrameIndex = settings.vAvailableFrames.at(x);
    settings.bUSE_SCALING = scalingEdit->checkState();
    settings.bSTOP_CLEAR_BUFFER = clearBufferEdit->checkState();
    settings.outputFile = filenameEdit->text();
    return settings;
}

void QCvFirewireCameraSettingsDlg::setProperties(sQCvFirewireCameraSettings s)
{
    /*Sets the values in the settings dialog box for
     * user viewing */
    settings=s;
    acquisitionRateEdit->setValue(s.acquisitionRate);

    scalingEdit->setChecked(s.bUSE_SCALING);
    clearBufferEdit->setChecked(s.bSTOP_CLEAR_BUFFER);
    filenameEdit->setText(s.outputFile);
}



void QCvFirewireCameraSettingsDlg::acquisitionRateUpdate(int number)
{
    /*Updates the acquisition rate label in the settings dialog*/
    lAcquisitionRateLabel->setText(""+QString::number(1000.0/(double)number)+"FPS  ("+QString::number(number)+" ms) ");
}
