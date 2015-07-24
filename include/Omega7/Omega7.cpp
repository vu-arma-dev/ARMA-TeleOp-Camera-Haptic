#include "Omega7.h"

Omega7::Omega7(int ID)
{
	iChannel=ID;
	refreshDevice(ID);
	for (int i=0;i<4;i++)
	{
		appliedForce[i]=0;
    }
}

Omega7::~Omega7(void)
{
	dhdClose(iChannel);
}

bool Omega7::refreshDevice(int ID)
{
	dhdEnableForce(DHD_ON);
	iDeviceID=dhdOpenID(ID);
	bDEVICE_ERROR=(iDeviceID<0);
	bIS_LEFT=dhdIsLeftHanded(ID);
	bDEVICE_ERROR=(dhdGetSerialNumber(&serialnum,ID)<0);
	if (!bDEVICE_ERROR) 
	{
		dhdSetForceAndGripperForce(0.0,0.0,0.0,0.0,0);
	}
	return bDEVICE_ERROR;
}

QString Omega7::getDeviceName()
{
	if (bIS_LEFT) name="SN: "+QString::number(serialnum)+" (left-handed)";
	else name="SN: "+QString::number(serialnum)+" (right-handed)";
	return name;
}

bool Omega7::getPosition(double *x,double *y, double *z)
{
	if(dhdGetPosition(x,y,z,iChannel)<0)
	{
		return false;
	}
	else
	{
		return true;	
	};		
}

void Omega7::getGripperAngleDeg(double *g)
{
	dhdGetGripperAngleDeg(g,iChannel);
}

void Omega7::getOrientationDeg(double *rx,double *ry, double *rz)
{
	dhdGetOrientationDeg(rx,ry,rz,iChannel);
}

void Omega7::getOrientationRad(double *rx,double *ry, double *rz)
{
	dhdGetOrientationRad(rx,ry,rz,iChannel);
}

void Omega7::setForce()
{
	dhdSetForceAndGripperForce(appliedForce[0],appliedForce[1],appliedForce[2],appliedForce[3],iChannel);
}

void Omega7::setForce(double fx, double fy, double fz, double fg)
{
	dhdSetForceAndGripperForce(fx,fy,fz,fg,iChannel);
}

void Omega7::setConstantForce(double fx, double fy, double fz, double fg)
{
	appliedForce[0]=fx;
	appliedForce[1]=fy;
	appliedForce[2]=fz;
	appliedForce[3]=fg;
	this->setForce();
}

void Omega7::getPositionAndOrientation(double *x, double *y, double *z,double *g,double *q)
{
	dhdGetPositionAndOrientationFrame(x, y, z, R,iChannel);
	dhdGetGripperAngleRad(g,iChannel);

	// Rot to Quat written by Haoran Yu
	double tr,S;
	tr = R[0][0] + R[1][1] + R[2][2];

	if (tr > 0) { 
	  S = sqrt(tr+1.0) * 2; // S=4*q[0] 
	  q[0] = 0.25 * S;
	  q[1] = (R[2][1] - R[1][2]) / S;
	  q[2] = (R[0][2] - R[2][0]) / S; 
	  q[3] = (R[1][0] - R[0][1]) / S; 
	} else if ((R[0][0] > R[1][1])&(R[0][0] > R[2][2])) { 
	  S = sqrt(1.0 + R[0][0] - R[1][1] - R[2][2]) * 2; // S=4*q[1] 
	  q[0] = (R[2][1] - R[1][2]) / S;
	  q[1] = 0.25 * S;
	  q[2] = (R[0][1] + R[1][0]) / S; 
	  q[3] = (R[0][2] + R[2][0]) / S; 
	} else if (R[1][1] > R[2][2]) { 
	  S = sqrt(1.0 + R[1][1] - R[0][0] - R[2][2]) * 2; // S=4*q[2]
	  q[0] = (R[0][2] - R[2][0]) / S;
	  q[1] = (R[0][1] + R[1][0]) / S; 
	  q[2] = 0.25 * S;
	  q[3] = (R[1][2] + R[2][1]) / S; 
	} else { 
	  S = sqrt(1.0 + R[2][2] - R[0][0] - R[1][1]) * 2; // S=4*q[3]
	  q[0] = (R[1][0] - R[0][1]) / S;
	  q[1] = (R[0][2] + R[2][0]) / S;
	  q[2] = (R[1][2] + R[2][1]) / S;
	  q[3] = 0.25 * S;
	}

}

void Omega7::getAppliedForce(double f[4])
{
	for (int i=0;i<4;i++)
	{
		f[i]=appliedForce[i];
	}
}

bool Omega7::isLeftHanded()
{
    return bIS_LEFT;
}

bool Omega7::close()
{
    int check;
    check=dhdClose(iChannel);
    if (check==0)
        return true;
    else
        return false;

}
