#ifndef CAMERAPREVIEW_H
#define CAMERAPREVIEW_H

#include <QDialog>
#include "ui_camerapreview.h"
#include <qtimer.h>
#include <opencv.hpp>
#include <highgui.h>

class cameraPreview : public QDialog
{
	Q_OBJECT

public:
    cameraPreview(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~cameraPreview();

	int getSelectedCamera();

protected:
	QTimer *camTimer;

private slots:
	void on_cameraSelector_valueChanged();
	void updateView();
	void stopCapture();

private:
	Ui::cameraPreviewClass ui;
	
	int maxCameras;

	cv::VideoCapture vidCap;
	cv::Mat imgCaptured;

	QGraphicsScene* scene;
	QImage qimgDisplay;
	QImage qimgScaledDisplay;
};

#endif // CAMERAPREVIEW_H
