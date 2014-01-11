#ifndef TUPLOADSWINDOW_H
#define TUPLOADSWINDOW_H
#include <QDialog>
#include <QPushButton>
#include <QGridLayout>

#include "dbwindow.cpp"

class TUploadsWindow : public QDialog
{
    Q_OBJECT;

private:
    QGridLayout *grid;
    QPushButton *button;
    void DB_Init(void);
public:
    TUploadsWindow(void);
};

#endif // TUPLOADSWINDOW_H
