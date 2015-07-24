#ifndef OMEGA7_H
#define OMEGA7_H

#include <stdio.h>
#include <math.h>
#include <qstring.h>
#include <dhdc.h>
#include <omega7udp.h>
#include <QObject>
#include <QUdpSocket>

class Omega7
{
public:
	Omega7(int ID);
	~Omega7(void);
	bool refreshDevice(int ID);
	QString getDeviceName();
	bool getPosition(double *x,double *y, double *z);
	void getGripperAngleDeg(double *g);
	void getOrientationDeg(double *rx,double *ry, double *rz);
	void getOrientationRad(double *rx,double *ry, double *rz);
	void getPositionAndOrientation(double *x,double *y, double *z,double *g,double q[4]);
	void setForce();
	void setForce(double fx, double fy, double fz,double fg);
	void setConstantForce(double fx, double fy, double fz,double fg);
	void getAppliedForce(double f[4]);
	bool isLeftHanded();
    bool close();
	

protected:

private:
	int iChannel, iDeviceID;
	unsigned short serialnum;
	bool bIS_LEFT;
	bool bDEVICE_ERROR;
	double px,py,pz,Rx,Ry,Rz;  // position variables
	double R[3][3]; // orientation matrix
	double appliedForce[4]; // force to be applied by haptic device: defined as fx, fy, fz, fgripper

    QString name;
};

#endif // OMEGA7_H

