#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <Omega7.h>
#include <omega7udp.h>
#include <QTimer>
#include <Eigen>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void getPose();


private:
    Ui::MainWindow *ui;
    Omega7 *myOmega7;
    Omega7Udp *pose;
    Omega7Udp *force;
    QTimer *udpUpdateTimer;
    QTimer *toggleUpdateTimer;

public slots:
    void update();
    void send();
    void updateToggle();
};

#endif // MAINWINDOW_H
