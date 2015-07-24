#include "camerapreview.h"

//cameraPreview::cameraPreview(QWidget *parent, Qt::WindowFlags flags)
//    : QDialog(parent, flags)
//{
//	ui.setupUi(this);
//	maxCameras=3;

//	// set ui selector max value
//	if (maxCameras>0){
//		ui.cameraSelector->setMaximum(maxCameras);
//	}
//	else {
//		ui.cameraSelector->setMaximum(0);
//	}

//	camTimer = new QTimer(this);

//	QApplication::connect(camTimer,SIGNAL(timeout()),this,SLOT(updateView()));
//	QApplication::connect(ui.okButton,SIGNAL(clicked()),this,SLOT(accept()));
//	QApplication::connect(ui.cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
//	QApplication::connect(this,SIGNAL(accepted()),this,SLOT(stopCapture()));
//	QApplication::connect(this,SIGNAL(rejected()),this,SLOT(stopCapture()));

//	scene = new QGraphicsScene();
//	vidCap.open(0);
//	camTimer->start(30);
//}

cameraPreview::cameraPreview(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    ui.setupUi(this);
    maxCameras=3;

    // set ui selector max value
    if (maxCameras>0){
        ui.cameraSelector->setMaximum(maxCameras);
    }
    else {
        ui.cameraSelector->setMaximum(0);
    }

    camTimer = new QTimer(this);

    QApplication::connect(camTimer,SIGNAL(timeout()),this,SLOT(updateView()));
    QApplication::connect(ui.okButton,SIGNAL(clicked()),this,SLOT(accept()));
    QApplication::connect(ui.cancelButton,SIGNAL(clicked()),this,SLOT(reject()));
    QApplication::connect(this,SIGNAL(accepted()),this,SLOT(stopCapture()));
    QApplication::connect(this,SIGNAL(rejected()),this,SLOT(stopCapture()));

    scene = new QGraphicsScene();
    vidCap.open(0);
    camTimer->start(30);
}

cameraPreview::~cameraPreview()
{
	camTimer->stop();
	vidCap.release();
}

// SLOTS ----------------------------------------------------------------------------
void cameraPreview::on_cameraSelector_valueChanged()
{
	camTimer->stop(); // stop capturing images

	vidCap.release();

	vidCap.open(ui.cameraSelector->value());
	camTimer->start(30);

}

void cameraPreview::updateView()
{
	vidCap.read(imgCaptured);

	if(imgCaptured.empty()==true){
		return;
	}
	else{
		cv::cvtColor(imgCaptured,imgCaptured,CV_BGR2RGB);
		QImage qimgDisplay((uchar*)imgCaptured.data,imgCaptured.cols,imgCaptured.rows,imgCaptured.step,QImage::Format_RGB888);
		double widthscale  = (qimgDisplay.width())/(ui.previewWindow->width());	   // the greater the number the more the width of the image must be reduced
		double heightscale = (qimgDisplay.height())/(ui.previewWindow->height());  // the greater the number the more the height of the image must be reduced
		if (heightscale<widthscale){
			qimgScaledDisplay = qimgDisplay.scaledToWidth(ui.previewWindow->width()-20);
		}
		else {
			qimgScaledDisplay = qimgDisplay.scaledToHeight(ui.previewWindow->height()-20);
		}
		scene->addPixmap(QPixmap::fromImage(qimgScaledDisplay));
		ui.previewWindow->setScene(scene);
	}
}

void cameraPreview::stopCapture()
{
	camTimer->stop();
	vidCap.release();
}

// METHODS -----------------------------------------------------------------------------
int cameraPreview::getSelectedCamera()
{
	return ui.cameraSelector->value();
}
