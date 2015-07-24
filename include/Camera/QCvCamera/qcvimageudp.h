#ifndef QCVIMAGEUDP_H
#define QCVIMAGEUDP_H

#include <qcvfilter.h>
#include <QByteArray>
#include <QDataStream>
#include <opencv2/core/core.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QDebug>
#include <QUdpSocket>
#include <QApplication>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>


using namespace cv;
using namespace std;

struct sQCvImageUdpSettings{
    QString ip;
    int port;

    sQCvImageUdpSettings():ip("127.0.0.1"),port(15000){}
};
Q_DECLARE_METATYPE(sQCvImageUdpSettings)


class QCvImgUdpDlg : public QDialog
{
    Q_OBJECT

public:
    //explicit QCvImageUdpDlg(sCvCalibSettings *s);
    explicit QCvImgUdpDlg(sQCvImageUdpSettings *s);
    sQCvImageUdpSettings getProperties();
    void setProperties(sQCvImageUdpSettings s);
    //void setProperties(sCvCalibSettings s);

private:
    QLineEdit *addressEdit;
    QLineEdit *portEdit;
    QLabel *addressLabel;
    QLabel *portLabel;
    QDialogButtonBox *buttonBox;
    /**
    QLineEdit *rowsEdit;
    QLineEdit *colsEdit;
    QCheckBox *trackingCheckBox;
    QCheckBox *correctionCheckBox;
    QDialogButtonBox *buttonBox;
    **/

};

class QCvImageUdp : public QCvFilter
{
    Q_OBJECT
public:
    explicit QCvImageUdp(QObject *parent = 0);
    explicit QCvImageUdp(QMainWindow *mw, QObject *parent = 0);
    ~QCvImageUdp();
    void setMainWindow(QMainWindow *p);
    QMenu* getMenu();
    Mat updateImage(Mat img);
    QImage getImage(vector<unsigned char> buf);

    void setAddress(QString add, int p);
    QString getAddress();
    int getPort();
    void connect();
    bool stream;


    QUdpSocket *udpSocket;

    void printVector(unsigned char c);
private:
    sQCvImageUdpSettings s;

public slots:
    void getSettingsDialog();
    void toggleStreaming();

protected:
    QMainWindow *ui;

    QMenu *menu;
    QAction *aSettings;
    QAction *aStartSending;

};

#endif // QCVIMAGEUDP_H
