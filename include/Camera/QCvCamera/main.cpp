#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
/*int main(int argc, char *argv[])
{
QApplication a(argc, argv);
QGraphicsView* w = new QGraphicsView();
QGraphicsScene *scn = new QGraphicsScene( w );
scn->setSceneRect( w->rect() );
w->setScene( scn );
w->setFixedSize( 400, 400 );
QPixmap pix( "c:\\pix_jpg.jpg" );
scn->addPixmap( pix );
w->show();
a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
return a.exec();
}
*/
