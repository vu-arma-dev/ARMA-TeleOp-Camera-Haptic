#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qcvcamera.h>
#include <QMainWindow>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QCvCamera *cam;
    QGraphicsScene *scene;
    QGraphicsView *view;
private slots:
    void updateDisplay(QImage img);

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
