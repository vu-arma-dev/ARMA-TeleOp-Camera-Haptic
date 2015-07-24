
#include "qcvcamera.h"


QCvCamera::QCvCamera(QMainWindow *parent = 0)
{
    qRegisterMetaType<sQCvCameraSettings>();
    s.acquisitionRate = _CAM_NORMAL_REFRESH_TIME_MS;
    ui = parent;
    bCAM_RUNNING = false;
    bPAUSE=false;
    //add label through a connection
    timerCamera = new QTimer(this);

    QApplication::connect(timerCamera,SIGNAL(timeout()),this,SLOT(refreshCamera()));

    //Creates menu bar and items
    menu = ui->menuBar()->addMenu(tr("&Camera"));

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

    vidCap.open(s.iCurrentCamera);
    clearFilters();

    bTHREAD_SAFE=false; // not thread safe by default
}

QCvCamera::~QCvCamera()
{
    //stops timer so that thread can be deactivated
    timerCamera->stop();
}


//GETTERS


int QCvCamera::getCameraAcquistionRate(){
    return s.acquisitionRate;
}

void QCvCamera::getResolution(int channel, int *height, int *width)
{
    /*Gets the resolution of imgCaptured and
     * sets the memory value of height and width to
     * the witdth and height so that the user
     * can access the two separate value*/
    setCurrentCamera(channel);
    cv::Mat imgCaptured = acquireImage();
    height[0] = imgCaptured.rows;
    width[0] = imgCaptured.cols;
}

//SETTERS
bool QCvCamera:: setCurrentCamera(int channel)
{
    /*Changes the camera in use to the channel
     * specified in the argument.*/
    s.iCurrentCamera = channel;
    vidCap.release();

    timerCamera->stop();
    vidCap.open(s.iCurrentCamera);

    return 1;
}

void QCvCamera:: setCameraAcquisitionRate(int intervalMS)
{
    /*Resets the acquistion rate in milliseconds*/
    s.acquisitionRate = intervalMS;
}


void QCvCamera::setOutputView(QGraphicsView *obj)
{
    view=obj;
}

void QCvCamera::enableScaling(bool b)
{
    s.bUSE_SCALING=b;
}

//OTHERS
cv::Mat QCvCamera::acquireImage()
{
    /*Acquires the image from the camera and
     * converts color, returns the image*/
    cv::Mat image;

    cv::cvtColor(imgCaptured,image,CV_RGB2BGR);

    return image;
}

bool QCvCamera::takeSnapshot(QString filename)
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
void QCvCamera::takeSnapshot()
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

void QCvCamera::getSettingsDlg()
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
        sQCvCameraSettings prev = s;
        bool bContinueRunning=this->isRunning();
        this->stop();
        bCAM_RUNNING = false;
        action_Capture->setText(tr("&Capture"));

        QCvCameraSettingsDlg dlg(&prev);

        if(dlg.exec() == QDialog::Accepted){
            sQCvCameraSettings temp = dlg.getProperties();
            s = temp;
            this->setCurrentCamera(s.iCurrentCamera);
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

bool QCvCamera::isRunning()
{
    return bCAM_RUNNING;
}

void QCvCamera::clearBuffer()
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

void QCvCamera::updateSettings(sQCvCameraSettings settings)
{
    s=settings;
    this->setCurrentCamera(s.iCurrentCamera);
}

void QCvCamera::setPause(bool b)
{
    bPAUSE=b;
    if (this->isRunning())
    {
        if (bPAUSE)
        {
            this->timerCamera->stop();
        }
        else
        {
            this->timerCamera->start(this->acquisitionRate);
        }
    }
}


//SLOTS
void QCvCamera::refreshCamera()
{
    /*Refresh camera gets the image captured and turns it into a cv::mat file
     * and does the scaling, if scaling is enabled.It then transmits qimgScaledDisplay
     * as through imageReady() */
    vidCap.read(imgCaptured);
    if(imgCaptured.empty()==true){
        qDebug() <<"no image found";
        return;
    }
    else
    {
        cv::Mat imgCaptured = acquireImage();
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

        if(s.bUSE_SCALING)
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
        else emit imageReady(qimgDisplay.copy());
    }
}

void QCvCamera::stop()
{
    /* Stops timerCamera and sets bCAM_RUNNING to false,
     * resets text on the menu to "Capture*/
    timerCamera->stop();
    bCAM_RUNNING=false;
    if (!bCAM_RUNNING) action_Capture->setText(tr("&Capture"));
    if(s.bSTOP_CLEAR_BUFFER) clearBuffer();
}

void QCvCamera::addSubMenu(QMenu *m)
{
    menu->addMenu(m);
}

void QCvCamera::clearFilters()
{
    iFilters.clear();
}

void QCvCamera::appendFilter(QCvFilter *filter)
{
    iFilters.push_back(filter);
}

void QCvCamera::setThreadSafe(bool b)
{
    bTHREAD_SAFE=b;
}

void QCvCamera::start()
{
    /*Starts timerCamera with the aquisitionRate
     * sets bCAM_RUNNING to true
     * and resets text in the menu*/
    timerCamera->start(s.acquisitionRate);
    bCAM_RUNNING=true;
    if(bCAM_RUNNING) action_Capture->setText(tr("&Stop Capture"));
}

void QCvCamera::toggleCamera()
{
    /*Toggles the camera on and off
     * using the start() and stop() functions*/
    qDebug ()<< "entered toggleCamera";
    if (bCAM_RUNNING)
    {
        stop();
    }

    else
    {
        start();

    }

}



QCvCameraSettingsDlg::QCvCameraSettingsDlg(sQCvCameraSettings *s)
{
    /*Creates a dialog box for user to input settings*/

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
    iCurrentCameraEdit = new QSpinBox(this);
    form->addRow("Current Camera: ", iCurrentCameraEdit);
    iCurrentCameraEdit->setValue(s->iCurrentCamera);

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

sQCvCameraSettings QCvCameraSettingsDlg::getProperties()
{
    /*This will get the properties from the settings
    dialog box and insert them into the struct*/
    sQCvCameraSettings s;
    s.acquisitionRate= acquisitionRateEdit->value();
    s.iCurrentCamera= iCurrentCameraEdit->value();
    s.bUSE_SCALING = scalingEdit->checkState();
    s.bSTOP_CLEAR_BUFFER = clearBufferEdit->checkState();
    s.outputFile = filenameEdit->text();
    return s;
}

void QCvCameraSettingsDlg::setProperties(sQCvCameraSettings s)
{
    /*Sets the values in the settings dialog box for
     * user viewing */

    iCurrentCameraEdit->setMinimum(0);
    iCurrentCameraEdit->setMaximum(10);
    acquisitionRateEdit->setValue(s.acquisitionRate);
    iCurrentCameraEdit->setValue(s.iCurrentCamera);
    scalingEdit->setChecked(s.bUSE_SCALING);
    clearBufferEdit->setChecked(s.bSTOP_CLEAR_BUFFER);
    filenameEdit->setText(s.outputFile);
}

void QCvCameraSettingsDlg::acquisitionRateUpdate(int number)
{
    /*Updates the acquisition rate label in the settings dialog*/
    lAcquisitionRateLabel->setText(""+QString::number(1000.0/(double)number)+"FPS  ("+QString::number(number)+" ms) ");
}
