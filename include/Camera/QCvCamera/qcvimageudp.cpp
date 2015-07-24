#include "qcvimageudp.h"

QCvImageUdp::QCvImageUdp(QObject *parent)
{
    setAddress("127.0.0.1",85000);
    menu = new QMenu("UDP Transmission");
}

QCvImageUdp::QCvImageUdp(QMainWindow *mw, QObject *parent)
{
    ui = mw;

    menu = ui->menuBar()->addMenu(tr("&Image Udp"));

    aSettings = new QAction(tr("&Settings"), this);
    menu->addAction(aSettings);
    QApplication::connect(aSettings,SIGNAL(triggered()),this,SLOT(getSettingsDialog()));

    aStartSending = new QAction(tr("S&tart Sending Images"), this);
    menu->addAction(aStartSending);
    QApplication::connect(aStartSending, SIGNAL(triggered()), this, SLOT(toggleStreaming()));
}

QCvImageUdp::~QCvImageUdp()
{

}



QMenu *QCvImageUdp::getMenu()
{
    return menu;
}

Mat QCvImageUdp::updateImage(Mat img)
{
    vector<unsigned char> buffer;
    vector<int> param;
    param.push_back(CV_IMWRITE_JPEG_QUALITY);
    param.push_back(70);
    if (imencode(".jpg",img,buffer,param))
    {
        qDebug()<< "Encoding successful "+QString::number(buffer.size())+" bytes written";
        int v = 0;
        /**
        foreach(unsigned char c, buffer){
            qDebug() <<c ;
            //if (v==9)
            if (v==10)
            {
                //qDebug() << "new image";
                break;
                break;
            }
            v++;
        }
        **/

        QUdpSocket u;
        bool written = u.writeDatagram((const char*)buffer.data(),buffer.size(),QHostAddress(s.ip),s.port);//QHostAddress(s.ip),s.port);
        //if (written) qDebug() << "written Datagram";
    }


    return img; // pass original image through filter unaltered
}

void QCvImageUdp:: printVector(unsigned char c)
{
    qDebug() << c;
}

void QCvImageUdp::getSettingsDialog()
{
    stream = false;
    QCvImgUdpDlg dlg(&s);

    if(dlg.exec() == QDialog::Accepted)
    {
        sQCvImageUdpSettings temp = dlg.getProperties();
        s = temp;

    }
}

QImage QCvImageUdp::getImage(vector <unsigned char> buf)
{
    Mat imgCaptured= imdecode(Mat(buf), CV_LOAD_IMAGE_COLOR);
    //loop through image filters in future
    /**
    QCvFilter *f;
    for (unsigned int i=0;i<iFilters.size();i++)
    {
       f = iFilters.at(i);
       cv::Mat temp = f->updateImage(imgCaptured);
       imgCaptured = temp;
    }
    **/
    QImage qimgDisplay((uchar*) imgCaptured.data, imgCaptured.cols, imgCaptured.rows, imgCaptured.step, QImage::Format_RGB888);

    return qimgDisplay;
}

void QCvImageUdp::setAddress(QString add, int p)
{
    s.ip=add;
    s.port=p;
}

QString QCvImageUdp::getAddress()
{
    return s.ip;
}

int QCvImageUdp::getPort()
{
    return s.port;
}

void QCvImageUdp::toggleStreaming()
{
    if (stream) stream = false;
    else stream = true;
}

/**void QCvImageUdp::connect()
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress(s.ip), s.port);
}**/


QCvImgUdpDlg::QCvImgUdpDlg(sQCvImageUdpSettings *s)
{
    this ->setWindowTitle("Settings");
    QFormLayout *form = new QFormLayout;

    //row 1 Address
    addressLabel = new QLabel;
    addressLabel->setText("Address");
    addressEdit = new QLineEdit;
    addressEdit->setText(s->ip);
    form->addRow(addressLabel, addressEdit);

    //row2 port
    portLabel = new QLabel;
    portLabel->setText("Port");
    portEdit= new QLineEdit;
    portEdit->setText(QString::number(s->port));
    form->addRow(portLabel, portEdit);

    //row 3 buttons
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox :: Cancel , Qt::Horizontal, this);
    form->addRow(buttonBox);
    this->setLayout(form);
    this->connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    this->connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

sQCvImageUdpSettings QCvImgUdpDlg::getProperties()
{
    sQCvImageUdpSettings p;
    p.ip = addressEdit->text();
    p.port = portEdit->text().toInt();
    return p;
}
