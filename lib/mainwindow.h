/*****************************************************************************
Copyright 2013 Juan Carlos Jiménez Caballero

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*****************************************************************************/

#ifndef TMAINWINDOW_H
#define TMAINWINDOW_H
#include <QDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QMenu>
#include <QKeySequence>
#include <QShortcut>
#include <QComboBox>
#include <QMessageBox>
#include <QPixmap>
#include <QtNetwork>
#include <QUrl>
#include <QTableWidget>
#include <QTimer>
#include <QImage>
#include <QResizeEvent>
#include <QScrollArea>
#include <QToolButton>
#include <QFrame>
#include <QIcon>
#include <QGroupBox>
#include <QDesktopServices>

#include <qextserialenumerator.h>
#include "qextserialport.h"
#include "dbwin.h"
#include "configsptab.h"
#include "configdbtab.h"
#include "configrctab.h"
#include "serialportcontrol.h"
#include "databasecontrol.h"
#include "robotcontrol.h"

class TMainWindow : public QDialog
{
    Q_OBJECT

public:
    TMainWindow (const char *windowTitle);
    ~TMainWindow(void);
    SerialPortControl *mySerialPortControl;
    DatabaseControl   *myDatabaseControl;
    RobotControl      *myRobotControl;

    void set_HLine(QFrame *fr);
    void set_VLine(QFrame *fr);

private:
    DBWindow *databaseWindow;  //The window that controls the database

  //<Separator lines>
    QFrame *leftHBoxVLine;
    QFrame *rightHBoxVLine;
    QFrame *frConfigHLine;
    QFrame *frSPInputHLine;
  //</Separator lines>

  //<shortcuts>
    void createShortcuts (void);
    QShortcut *closeWindowShortcut; //In this window, it will close the app
  //</shortcuts>

  //<Layouts and tabs>
    void createLayout (void);
    QGridLayout *superGridLayout;

    QVBoxLayout *configTabsVBox;
    void setConfigTabs (void);
    QTabWidget  *configTabs;
    ConfigRCTab *myRCTab;
    ConfigSPTab *mySPTab;
    ConfigDBTab *myDBTab;

    QGridLayout *mainGridLayout;
    QHBoxLayout *leftHBox;
    QVBoxLayout *leftVBox;
    QVBoxLayout *middleVBox;
    QHBoxLayout *robotInfoHBox;
    QScrollArea *middleScrollArea;  //QScrollArea porque facilita el resize
    QHBoxLayout *rightHBox;
    QVBoxLayout *rightVBox;

    QHBoxLayout *HValueHBox;
    QHBoxLayout *VValueHBox;
    QHBoxLayout *RValueHBox;

    QHBoxLayout *statusHBox;

    //<for the QGroupBoxes>
    QVBoxLayout *straightVBox;  //Hay que añadir control de dirección y speed
    QVBoxLayout *curvedVBox;
    QVBoxLayout *rotateVBox;

    QHBoxLayout *straightLenHBox;
    QHBoxLayout *straightDirHBox;
    QHBoxLayout *straightBtHBox;

    QHBoxLayout *curvedRadHBox;
    QHBoxLayout *curvedAngHBox;
    QHBoxLayout *curvedBtHBox;

    QHBoxLayout *rotateAngHBox;
    QHBoxLayout *rotateBtHBox;
    //</for the QGroupBoxes>
  //</Layouts and tabs>

  //<Widgets>
    void createWidgets (void);
    //QToolButton *btHideNShow;
    QPushButton *btHideNShow;
    QPushButton *btManual;
    QPushButton *btVideo;
    QPushButton *btExit;
    QPushButton *btUpload;
    QPushButton *btSnapshot;

    QLabel      *lbWifi;
    QLabel      *lbBattery;
    QString     *strWifi;
    QString     *strBattery;

    QLabel      *lbHValueId;
    QLabel      *lbHValue;
    QLabel      *lbVValueId;
    QLabel      *lbVValue;
    QLabel      *lbRValueId;
    QLabel      *lbRValue;

    QLineEdit   *leStatusBar;
        //<GroupBoxes>
    QGroupBox   *straightGBox;
    QGroupBox   *curvedGBox;
    QGroupBox   *rotateGBox;

    QLabel      *lbStraightLen;
    QLineEdit   *leStraightLen;
    QLabel      *lbStraightDir;
    QComboBox   *comboStraightDir;
    QPushButton *btStraightSubmit;
    void fillStraightDirCombo (void);

    QLabel      *lbCurvedRad;
    QLineEdit   *leCurvedRad;
    QLabel      *lbCurvedAng;
    QLineEdit   *leCurvedAng;
    QPushButton *btCurvedSubmit;

    QLabel      *lbRotateAng;
    QLineEdit   *leRotateAng;
    QPushButton *btRotateSubmit;
        //</GroupBoxes>
    QPushButton *btPlayPath;
    QPushButton *btClearPath;
  //</Widgets>

    //network stuff:
    void readOnlineImage (void);
    QLabel *myImageLabel;
    QPixmap *pixmap_image;
    QPixmap *resized_pixmapImage;
  //<Resize>
    void resizeEvent (QResizeEvent *event);
    void forceResizeEvent (void);
  //</Resize>

  //<Misc>
    bool prev_CState;
    bool prev_ZState;
    bool prev_CandZState;
    QTimer *tmrSerialMovs;
    bool serialPortMovable; //Stores if "manual override" mode is enabled or not
  //</Misc>
private slots:
    void handleBtExit           (void);

public slots:
    void handleBtUpload         (void);
    void echoBuffer             (void);
    void handleBtHideNShow      (void);
    void displayImage           (void);
    void handleStringProcessed  (void);
    void handleBtManual         (void);
    void startManualOverride    (void);
    void stopManualOverride     (void);
    void handleBtVideo          (void);
    void handleBtSnapshot       (void);
    void handlePortClosed       (void);
    void handleBtStraightSubmit (void);
    void handleBtRotateSubmit   (void);
    void handleBtPlayPath       (void);
    void handleBtClearPath      (void);
    void handleTmrSerialMovs    (void);
    void handleStatusRefreshed  (void);
    void handleRobotDisconnected(void);
    void handleRobotConnected   (void);
};
#endif // TMAINWINDOW_H
