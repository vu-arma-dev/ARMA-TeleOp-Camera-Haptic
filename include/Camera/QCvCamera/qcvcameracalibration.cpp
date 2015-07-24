#include "qcvcameracalibration.h"
#include <QDebug>

QCvCameraCalibration::QCvCameraCalibration(QObject *parent)
{

    // Initialize States
    initialize();

    // Initialize Menu
    menu = new QMenu("Calibration");
    action_Settings = new QAction(tr("Calibration &Settings"),this);
    connect(action_Settings,SIGNAL(triggered()),this,SLOT(getSettingsDlg()));
    action_Reset = new QAction(tr("&Reset Data"),this);
    connect(action_Reset,SIGNAL(triggered()),this,SLOT(resetData()));
    action_Calibrate = new QAction(tr("&Generate Calibration"),this);
    connect(action_Calibrate,SIGNAL(triggered()),this,SLOT(calibrate()));
    action_Load = new QAction(tr("&Load Calibration"),this);
    connect(action_Load,SIGNAL(triggered()),this,SLOT(load()));
    menu->addAction(action_Settings);
    menu->addAction(action_Reset);
    menu->addAction(action_Calibrate);
    menu->addAction(action_Load);
    settingsDlg = new QCvCameraCalibrationDlg(&s);


}

QCvCameraCalibration::~QCvCameraCalibration()
{

}

const bool QCvCameraCalibration::isCalibrated()
{
    return s.bCALIBRATED;
}

bool QCvCameraCalibration::setCheckerboardDimensions(int rows, int cols)
{
    s.boardSize.height=rows;
    s.boardSize.width=cols;
    if (s.boardSize.height==rows && s.boardSize.width==cols) return true;
    else return false;
}

Mat QCvCameraCalibration::updateImage(Mat img)
{
    if (this->isEnabled())
    {
        // Track checker board pattern if display requested
        imgSize.height = img.rows;
        imgSize.width = img.cols;
        // Chessboard detection
        if (s.bTRACKING)
        {
            bool found;
            std::vector<cv::Point2f> pts;
            found = cv::findChessboardCorners(img,s.boardSize,pts,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
            if (found)
            {
                cv::Mat matGray; // memory reserved for a grayscale image
                cv::cvtColor(img,matGray,CV_RGB2GRAY); // convert the current image to grayscale
                cv::cornerSubPix(matGray,pts,cv::Size(11,11),cv::Size(-1,-1),cv::TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1)); // improve point location accuracy
                cv::drawChessboardCorners(img,s.boardSize,cv::Mat(pts),found);
                // Save points if sufficient time has passed
                if (time->elapsed()>s.timeBetweenCalibSamples)
                {

                    imagePoints.push_back(pts);
                    QString msg = QString::number(imagePoints.size())+" calibration poses collected with "+QString::number(pts.size());
                    time->restart();
                    emit statusUpdate(msg);
                    qDebug()<< imagePoints.size() <<" sample taken!";
                }
            }
        }
        // Image Correction
        if (s.bCALIBRATED )//&& s.bCORRECTION_ON)
        {
            /* TODO: Add remapping functionality
             *
             */
            cv::Mat temp = img.clone();
            cv::undistort(temp,img,cameraMatrix,distCoeffs);
            qDebug()<<"applied correction to image";
        }

    }
    return img;
}

QMenu *QCvCameraCalibration::getMenu()
{
    return menu;
}

void QCvCameraCalibration::getSettingsDlg()
{
    sCvCalibSettings prev = s;
    settingsDlg->setProperties(prev);
    settingsDlg->show();
    settingsDlg->activateWindow();
    settingsDlg->raise();
    this->setEnable(false);
    if (settingsDlg->exec() == QDialog::Accepted)
    {
        sCvCalibSettings temp = settingsDlg->getProperties();
        if ((s.boardSize.height==temp.boardSize.height)&&(s.boardSize.width==temp.boardSize.width)) s= temp;
        else
        {
            this->resetData();
            s=temp;
        }
    }
    this->setEnable(true);
}

void QCvCameraCalibration::resetData()
{
    s.bCALIBRATED=false;
    s.bCORRECTION_ON=false;
    imagePoints.clear();
}

void QCvCameraCalibration::calibrate()
{
    this->setEnable(false);
    QString msg;
    if (imagePoints.size() >= s.minNumSamples)
    {
        // compute board geometry
        vector<vector<cv::Point3f> > objectPoints(1);
        calcBoardCornerPositions(s.boardSize, s.squareSize, objectPoints[0]);
        objectPoints.resize(imagePoints.size(),objectPoints[0]);
        rmsError = calibrateCamera(objectPoints, imagePoints, imgSize, cameraMatrix,
                                    distCoeffs, rvecs, tvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);

        msg = "updated object points successfully, RMS = "+QString::number(rmsError);
        s.bCALIBRATED=true;
        saveCameraParams();
    }
    else
    {
        msg = "Insufficient number of samples taken!";
    }
    emit statusUpdate(msg);
    this->setEnable(true);
}

void QCvCameraCalibration::load()
{
    loadCameraParams("calib.xml");
}

void QCvCameraCalibration::initialize()
{
    // Initialize Parameters
    s.boardSize.height = _DEFAULT_CHECKER_ROWS;
    s.boardSize.width = _DEFAULT_CHECKER_COLS;

    // Initial Camera Values
    cameraMatrix = cv::Mat::eye(3,3,CV_64F);
    cameraMatrix.at<double>(0,0)=1.0; // used for fixed aspect ratio
    distCoeffs = cv::Mat::zeros(8,1,CV_64F);
    rmsError = 10000000;
    reprojectionError = 10000000;

    // Boolean state variables
    this->setEnable(true);

    // Start time tracking
    time = new QTime();
    time->start();
    imagePoints.clear();
}

void QCvCameraCalibration::calcBoardCornerPositions(cv::Size boardSize, float squareSize, vector<cv::Point3f> &corners)
{
    corners.clear();


      for( int i = 0; i < boardSize.height; ++i )
      {
        for( int j = 0; j < boardSize.width; ++j )
        {
            corners.push_back(cv::Point3f(float( j*squareSize ), float( i*squareSize ), 0));
        }

      }
}

// Print camera parameters to the output file
void QCvCameraCalibration::saveCameraParams()
{
    cv::FileStorage fs( "calib.xml", cv::FileStorage::WRITE );

    fs << "image_Width" << imgSize.width;
    fs << "image_Height" << imgSize.height;
    fs << "board_Width" << s.boardSize.width;
    fs << "board_Height" << s.boardSize.height;
    fs << "square_Size" << s.squareSize;
    fs << "Camera_Matrix" << cameraMatrix;
    fs << "Distortion_Coefficients" << distCoeffs;
}

bool QCvCameraCalibration::loadCameraParams(QString fid)
{
    cv::FileStorage fs("calib.xml", cv::FileStorage::READ); // Read the settings
    if (!fs.isOpened())
    {
        emit statusUpdate("Load failed");
        return false;
    }
    fs["image_Width"] >> imgSize.width;
    fs["image_Height"] >> imgSize.height;
    fs["board_Width"] >> s.boardSize.width;
    fs["board_Height"] >> s.boardSize.height;
    fs["square_Size"] >> s.squareSize;



    fs["Camera_Matrix"] >> cameraMatrix;
    fs["Distortion_Coefficients"] >> distCoeffs;
    fs.release();                                         // close Settings file
    emit statusUpdate("Load successful");
    s.bCALIBRATED=true;
    return true;
}


QCvCameraCalibrationDlg::QCvCameraCalibrationDlg(sCvCalibSettings *s)
{
    this->setWindowTitle("Calibration Settings");
    QFormLayout *form = new QFormLayout();
    QIntValidator *v = new QIntValidator(1,20,this);
    rowsEdit = new QLineEdit(QString::number(s->boardSize.height),this);
    rowsEdit->setValidator(v);
    form->addRow("Number of rows:",rowsEdit);
    colsEdit = new QLineEdit(QString::number(s->boardSize.width),this);
    colsEdit->setValidator(v);
    form->addRow("Number of collumns:",colsEdit);
    trackingCheckBox = new QCheckBox(this);
    trackingCheckBox->setChecked(s->bTRACKING);
    form->addRow("Enable Tracking:",trackingCheckBox);
    correctionCheckBox = new QCheckBox(this);
    correctionCheckBox->setChecked(s->bCORRECTION_ON);
    form->addRow("Correct Distortion:",correctionCheckBox);
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, this);
    form->addRow(buttonBox);
    this->setLayout(form);
    this->connect(buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
    this->connect(buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
}

sCvCalibSettings QCvCameraCalibrationDlg::getProperties()
{
    sCvCalibSettings s;
    s.boardSize.height=rowsEdit->text().toInt();
    s.boardSize.width=colsEdit->text().toInt();
    s.bTRACKING=trackingCheckBox->isChecked();
    s.bCORRECTION_ON=correctionCheckBox->isChecked();
    return s;
}

void QCvCameraCalibrationDlg::setProperties(sCvCalibSettings s)
{
    rowsEdit->setText(QString::number(s.boardSize.height));
    colsEdit->setText(QString::number(s.boardSize.width));
    trackingCheckBox->setChecked(s.bTRACKING);
    correctionCheckBox->setChecked(s.bCORRECTION_ON);
}


