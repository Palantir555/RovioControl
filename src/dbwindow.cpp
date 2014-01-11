#include "dbwindow.h"

TUploadsWindow::TUploadsWindow(void)
    :QDialog()
{
    qDebug("In the TUploadsWindow constructor");
    setWindowTitle("Database control");
    qDebug("1");
    grid = new QGridLayout (this);
    button = new QPushButton("ojete");
    grid->addWidget (button, 0, 0, Qt::AlignLeft);
}
