#include "omega7udp.h"

//Omega7Udp::Omega7Udp(bool force, QObject *parent) : QObject(parent)
Omega7Udp::Omega7Udp(bool force, QMainWindow *parent) :QMainWindow(parent)
{
    sendReceiveDataEnabled = false;
    s.port = 20000; //default port number
    s.address = "192.168.1.169"; //default address
    socket = new QUdpSocket(this);
    ui = parent;
    if (force){
        s.force = true;
        s.pose = false;
    }
    else{
       s.force=false;
       s.pose = true;
    }


    //setup toolbar
    if (s.force) menu = ui->menuBar()->addMenu(tr("&Force Udp"));
    if(s.pose) menu = ui->menuBar()->addMenu(tr("&Pose Udp"));

    aSettings = new QAction(tr("&Settings"), this);
    menu->addAction(aSettings);
    QApplication::connect(aSettings, SIGNAL(triggered()),this, SLOT(getSettingsDialog()));

    aConnect = new QAction(tr("&Connect"), this);
    menu->addAction(aConnect);
    QApplication::connect(aConnect, SIGNAL(triggered()), this, SLOT(connect()));
}

Omega7Udp::~Omega7Udp()
{

}

int Omega7Udp::getPortNumber()
{
    return s.port;
}

QString Omega7Udp::getAddress()
{
    return s.address;
}

void Omega7Udp::setPortNumber(int number)
{
    s.port = number;
}

void Omega7Udp::setAddress(QString string)
{
    s.address = string;
}



void Omega7Udp::setForces(double sFX, double sFY, double sFZ, double sFG)
{
    fx = sFX;
    fy = sFY;
    fz = sFZ;
    fg = sFG;
}

void Omega7Udp::getForces(double &gFX, double &gFY, double &gFZ, double &gFG)
{
    gFX = fx;
    gFY = fy;
    gFZ = fz;
    gFG = fg;
}

void Omega7Udp::setPose(double posG, double posX, double posY, double posZ, double radW, double radX, double radY, double radZ)
{
    pg = posG;
    px = posX;
    py = posY;
    pz = posZ;

    q[0] = radW;
    q[1] = radX;
    q[2] = radY;
    q[3] = radZ;
}

void Omega7Udp::getPose(double &posG, double &posX, double &posY, double &posZ)
{
    posG = pg;
    posX=px;
    posY = py;
    posZ = pz;
}

void Omega7Udp::getOrientArray(double (&returnArray)[4])
{
    returnArray[0] = q[0];
    returnArray[1] = q[1];
    returnArray[2] = q[2];
    returnArray[3] = q[3];
}


bool Omega7Udp::connect()
{
    quint64 connected = socket->bind(QHostAddress(s.address), s.port);
    qDebug() <<"connected";
    sendReceiveDataEnabled = true;

    if (s.force)
    {
        QApplication::connect(socket, SIGNAL(readyRead()), this, SLOT(recieveForce()));
    }

    if(s.pose)
    {
        QApplication::connect(socket, SIGNAL(readyRead()), this, SLOT(recievePose()));
    }

    if (connected == -1) return false;
    else return true;
}

bool Omega7Udp::recieve()
{
    QByteArray datagram;
    qint64 readDatagramRecieve;
    do{
        datagram.resize(socket->pendingDatagramSize());
        readDatagramRecieve = socket->readDatagram(datagram.data(),datagram.size());
    }while(socket->hasPendingDatagrams());

    //int size = datagram.size();


    //if (size <= 32) recieveForce(datagram);
    //else if (size <= 56) recievePose(datagram);
    return true;

}

bool Omega7Udp::sendPose()
{
    QByteArray datagram;
    qint64 writeDatagramSendPose;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_8);
    out.setByteOrder(QDataStream::LittleEndian);
    double i= 1;
    out<<i<<pg<<px<<py<<pz<<q[0]<<q[1]<<q[2]<<q[3];

    writeDatagramSendPose = socket->writeDatagram(datagram, datagram.size(), QHostAddress(s.address),s.port);
    if (writeDatagramSendPose == -1) return false;
    else return true;
}

bool Omega7Udp::recievePose(QByteArray datagram)
{
    QDataStream in(&datagram,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);
    in.setByteOrder(QDataStream::LittleEndian);
    double i;
    in>>i>>pg>>px>>py>>pz>>q[0]>>q[1]>>q[2]>>q[3];
    //TODO send to ROS
    return true;
}

bool Omega7Udp::recievePose()
{
    QByteArray datagram;
    qint64 readDatagramRecieve;
    do{
        datagram.resize(socket->pendingDatagramSize());
        readDatagramRecieve = socket->readDatagram(datagram.data(),datagram.size());
        /**
        if (readDatagramRecieve == -1 ) qDebug()<< "unable to read pose datagram";
        else qDebug() << "able to read pose datagram"; **/

    }while(socket->hasPendingDatagrams());


    QDataStream in(&datagram,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);
    in.setByteOrder(QDataStream::LittleEndian);
    double i;
    in>>i>>pg>>px>>py>>pz>>q[0]>>q[1]>>q[2]>>q[3];
    //qDebug()<< QString::number(px) + ", " + QString::number(py) + ", " + QString::number(pz);
    //TODO send to ROS
    return true;
}

bool Omega7Udp::sendForces()
{
    QByteArray datagram;
    QDataStream out(&datagram, QIODevice::WriteOnly);
    qint64 writeDatagramSendForce;
    out.setVersion(QDataStream::Qt_4_8);
    out.setByteOrder(QDataStream::LittleEndian);
    //TODO Need to get these variables for ROS

    out <<fx<<fy<<fz<<fg;
    writeDatagramSendForce = socket->writeDatagram(datagram, datagram.size(), QHostAddress(s.address), s.port);
    if (writeDatagramSendForce == -1) return false;
    else return true;
}

bool Omega7Udp::recieveForce(QByteArray datagram)
{

    double incomingX,incomingY,incomingZ,incomingG;
    QDataStream in(&datagram,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);
    in.setByteOrder(QDataStream::LittleEndian);
    in >> incomingX >> incomingY >> incomingZ >> incomingG;
    setForces(incomingX,incomingY,incomingZ,incomingG);

    return true;
}

bool Omega7Udp::recieveForce()
{
    QByteArray datagram;
    qint64 readDatagramRecieve;
    do{
        datagram.resize(socket->pendingDatagramSize());
        readDatagramRecieve = socket->readDatagram(datagram.data(),datagram.size());
        //if (readDatagramRecieve == -1 ) qDebug()<< "unable to read force datagram";
        //else qDebug() << "able to read force datagram";
    }while(socket->hasPendingDatagrams());

    double incomingX,incomingY,incomingZ,incomingG;
    QDataStream in(&datagram,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_4_8);
    in.setByteOrder(QDataStream::LittleEndian);
    in >> incomingX >> incomingY >> incomingZ >> incomingG;
    setForces(incomingX,incomingY,incomingZ,incomingG);
    return true;
}

void Omega7Udp::getSettingsDialog()
{
    sOmega7UdpSettings prev = s;
    Omega7UdpSettingsDialog dlg (&prev);

    if(dlg.exec() == QDialog::Accepted)
    {
        sOmega7UdpSettings temp = dlg.getProperties();
        s = temp;
        //if connected, get make okay button go away
        //or if connected, disconnect if okay is pressed
    }


}





Omega7UdpSettingsDialog::Omega7UdpSettingsDialog(sOmega7UdpSettings *s)
{
    this ->setWindowTitle("Settings");
    QFormLayout *form = new QFormLayout(this);


    poseForceLabel = new QLabel;
    blankLabel = new QLabel;
    portLabel = new QLabel;
    addressLabel = new QLabel;
    blankLabel2 = new QLabel;

    addressEdit = new QLineEdit;
    portEdit = new QLineEdit;


    // Pose and Force Row 1
    if(s->pose) poseForceLabel->setText("Pose");
    if(s->force) poseForceLabel->setText("Force");
    form->addRow(blankLabel, poseForceLabel);\


    // Address Row 2

    addressLabel->setText("Address");
    addressEdit->setText(s->address);
    form->addRow(addressLabel, addressEdit);

    //Port row 3
    portLabel->setText("Port");
    portEdit->setText(QString::number(s->port));
    form->addRow(portLabel, portEdit);

    //Buttons row 4

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox :: Cancel , Qt::Horizontal, this);
    form->addRow(buttonBox);
    this->setLayout(form);
    this->connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    this->connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));


}

sOmega7UdpSettings Omega7UdpSettingsDialog::getProperties()
{
    sOmega7UdpSettings s;
    s.address= addressEdit->text();
    s.port = portEdit->text().toInt();


    return s;
}

void Omega7UdpSettingsDialog::setProperties(sOmega7UdpSettings s)
{
    addressEdit->setText(s.address);
    portEdit->setText(QString::number(s.port));

}
