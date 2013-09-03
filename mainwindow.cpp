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

#include "mainwindow.h"

TMainWindow::TMainWindow (const char *windowTitle)
    :QDialog()  // llamamos al constructor de la base
{
    Qt::WindowFlags flags = windowFlags(); //Get the enabled window flags
    flags |= Qt::WindowMinimizeButtonHint; //Add the minimize button
    setWindowFlags(flags);                 //Set the new modified window flags

    setWindowTitle(windowTitle);
    databaseWindow = NULL; //Initialized to NULL for the "if(!databaseWindow);"
    mySerialPortControl = new SerialPortControl;
    myDatabaseControl   = new DatabaseControl;
    myRobotControl      = new RobotControl;

    prev_CState = 0;
    prev_ZState = 0;
    prev_CandZState = 0;
    serialPortMovable=1;

    this->createShortcuts();
    this->createLayout();
    this->createWidgets();
    this->setConfigTabs();

    //this->showMaximized();
}
TMainWindow::~TMainWindow (void)
{
    if(mySerialPortControl->isOpen())
        mySerialPortControl->closePort();
    if(myDatabaseControl->connection_isOpen())
        myDatabaseControl->disconnect();
    if(databaseWindow)
        databaseWindow->close();
}

void TMainWindow::createShortcuts (void)
{
    closeWindowShortcut = new QShortcut (QKeySequence::Close, this);
    connect (closeWindowShortcut, SIGNAL(activated()), this, SLOT(handleBtExit()));
}

void TMainWindow::createLayout (void)
{
    qDebug ("Creating the layout");
    superGridLayout = new QGridLayout(this);
    configTabsVBox  = new QVBoxLayout;
    mainGridLayout  = new QGridLayout;
    leftHBox        = new QHBoxLayout;
    leftVBox        = new QVBoxLayout;
    middleVBox      = new QVBoxLayout;
    robotInfoHBox   = new QHBoxLayout;
    rightHBox       = new QHBoxLayout;
    rightVBox       = new QVBoxLayout;
    statusHBox      = new QHBoxLayout;
    HValueHBox      = new QHBoxLayout;
    VValueHBox      = new QHBoxLayout;
    RValueHBox      = new QHBoxLayout;

    leftHBoxVLine   = new QFrame; set_VLine(leftHBoxVLine);
    rightHBoxVLine  = new QFrame; set_VLine(rightHBoxVLine);

    superGridLayout ->addLayout (configTabsVBox ,0 ,0);
    superGridLayout ->addLayout (mainGridLayout ,1 ,0);
    superGridLayout ->addLayout (statusHBox     ,2 ,0);

    superGridLayout ->setRowStretch(0, 0);
    superGridLayout ->setRowStretch(1, 100);
    superGridLayout ->setRowStretch(2, 0);

    mainGridLayout ->addLayout (leftHBox   ,0 ,0 ,Qt::AlignLeft);
    leftHBox        ->addLayout (leftVBox);
    leftHBox        ->addWidget (leftHBoxVLine, 100);

    mainGridLayout ->addLayout (middleVBox ,0 ,1);
    middleVBox      ->addLayout (robotInfoHBox, 0); //To have the WiFi and Battery labels ON TOP of the image

    mainGridLayout ->addLayout (rightHBox  ,0 ,2 ,Qt::AlignRight);
    rightHBox       ->addWidget (rightHBoxVLine, 100);
    rightHBox       ->addLayout (rightVBox);

    mainGridLayout  ->setColumnStretch (0, 0);
    mainGridLayout  ->setColumnStretch (1, 100);
    mainGridLayout  ->setColumnStretch (2, 0);

    //<Not so elegant>
    superGridLayout ->setContentsMargins ( 0, 20,  0,  0);
    configTabsVBox  ->setContentsMargins (10, 10, 10, 10);
    leftHBox        ->setContentsMargins ( 0,  0,  0,  0);
    leftVBox        ->setContentsMargins (15,  0, 15,  0);
    middleVBox      ->setContentsMargins ( 0,  0,  0,  0);
    rightHBox       ->setContentsMargins ( 0,  0,  0,  0);
    rightVBox       ->setContentsMargins (15,  0, 15,  0);
    //</Not so elegant>

    configTabsVBox  ->setSpacing(0);
    leftHBox        ->setSpacing(0);
    rightHBox       ->setSpacing(5);

    //<layouts for the QGroupBoxes>
    straightVBox    = new QVBoxLayout;
    curvedVBox      = new QVBoxLayout;
    rotateVBox      = new QVBoxLayout;

    straightLenHBox = new QHBoxLayout;
    straightDirHBox = new QHBoxLayout;
    straightBtHBox  = new QHBoxLayout;

    curvedRadHBox   = new QHBoxLayout;
    curvedAngHBox   = new QHBoxLayout;
    curvedBtHBox    = new QHBoxLayout;

    rotateAngHBox   = new QHBoxLayout;
    rotateBtHBox    = new QHBoxLayout;
    //</layouts for the QGroupBoxes>

}

//Possible alignments: Qt::AlignLeft, AlignRight, AlignHCenter,
//                     AlignJustify, AlignVCenter, AlignCenter
void TMainWindow::createWidgets (void)
{
    qDebug("Creating the widgets in the layout");
    frConfigHLine   = new QFrame; set_HLine(frConfigHLine);
    frSPInputHLine  = new QFrame; set_HLine(frSPInputHLine);
    configTabs      = new QTabWidget;
    btHideNShow     = new QPushButton (tr("Hide config tabs"));
    btManual        = new QPushButton;
    this->stopManualOverride(); //Sets the button text and enables software control
    btVideo         = new QPushButton (tr("Start video feed"));
    btSnapshot      = new QPushButton (tr("Take snapshot!"));
    btUpload        = new QPushButton (tr("Database"));
    btExit          = new QPushButton (tr("Exit"));

    strWifi         = new QString (tr("WiFi: "));
    lbWifi          = new QLabel (*strWifi + "-");
    strBattery      = new QString (tr("Battery: "));
    lbBattery       = new QLabel (*strBattery + "-");


    btPlayPath      = new QPushButton (tr("Play path"));
    btClearPath     = new QPushButton (tr("Clear path"));
    lbHValueId      = new QLabel (tr("Horizontal value:"));
    lbHValue        = new QLabel ("----");
    lbVValueId      = new QLabel (tr("Vertical value:"));
    lbVValue        = new QLabel ("----");
    lbRValueId      = new QLabel (tr("Rotation value:"));
    lbRValue        = new QLabel ("----");

    middleScrollArea= new QScrollArea;
    myImageLabel    = new QLabel;
    pixmap_image    = new QPixmap;
    resized_pixmapImage = new QPixmap;

    leStatusBar     = new QLineEdit;

    btManual    ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    btVideo     ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    btSnapshot  ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    btUpload    ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    btExit      ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    btManual    ->setAutoDefault (false);
    btVideo     ->setAutoDefault (false);
    btSnapshot  ->setAutoDefault (false);
    btExit      ->setAutoDefault (false);
    btUpload    ->setAutoDefault (false);
    btHideNShow ->setAutoDefault (false);

    leftVBox->addWidget (btManual);
    leftVBox->addWidget (btVideo);
    leftVBox->addWidget (btSnapshot);
    leftVBox->addWidget (btUpload);
    leftVBox->addWidget (btHideNShow);
    leftVBox->addStretch(100);
    leftVBox->addWidget (btExit);

      //Middle area:
    this->handleRobotDisconnected();  //By default, the robot is not connected
    lbBattery   ->setFrameStyle (QFrame::Panel | QFrame::Sunken);
    lbWifi      ->setFrameStyle (QFrame::Panel | QFrame::Sunken);
    robotInfoHBox->addStretch(45);
    robotInfoHBox->addWidget(lbBattery, 0, Qt::AlignRight);
    //robotInfoHBox->addStretch(10);
    robotInfoHBox->addWidget(lbWifi, 0, Qt::AlignRight);
    robotInfoHBox->addStretch(45);

    myImageLabel->setPixmap (*pixmap_image);
    myImageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    middleScrollArea->setWidget(myImageLabel);
    middleVBox->addWidget (middleScrollArea, 100);
    middleScrollArea->setFrameShape     (QFrame::NoFrame);
    middleScrollArea->setBackgroundRole (QPalette::NoRole);
    middleScrollArea->setWidgetResizable (true);
    middleScrollArea->setAlignment (Qt::AlignTop | Qt::AlignHCenter);

    //middleVBox      ->addLayout (robotInfoHBox, 0); //To have the WiFi and Battery labels under the image
      //</Middle area>

    HValueHBox->addStretch ();
    HValueHBox->addWidget (lbHValueId, 0, Qt::AlignRight);
    HValueHBox->addWidget (lbHValue, 0, Qt::AlignRight);
    VValueHBox->addStretch ();
    VValueHBox->addWidget (lbVValueId, 0, Qt::AlignRight);//
    VValueHBox->addWidget (lbVValue, 0, Qt::AlignRight);
    RValueHBox->addStretch ();
    RValueHBox->addWidget (lbRValueId, 0, Qt::AlignRight);
    RValueHBox->addWidget (lbRValue, 0, Qt::AlignRight);

    lbHValue->setFrameStyle (QFrame::Panel | QFrame::Sunken);
    lbVValue->setFrameStyle (QFrame::Panel | QFrame::Sunken);
    lbRValue->setFrameStyle (QFrame::Panel | QFrame::Sunken);
    lbHValue->setFixedWidth(lbHValue->sizeHint().width());
    lbVValue->setFixedWidth(lbHValue->sizeHint().width());
    lbRValue->setFixedWidth(lbHValue->sizeHint().width());
    //lbHValue->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    //lbVValue->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    //statusHBox->addWidget (leStatusBar); //Commented to hide it.
    statusHBox->addWidget (new QLabel);    //If leStatusBar is hidden, we need a bottom margin

    leStatusBar ->setFocusPolicy(Qt::NoFocus);
    lbHValue    ->setFocusPolicy(Qt::NoFocus);
    lbVValue    ->setFocusPolicy(Qt::NoFocus);
    lbRValue    ->setFocusPolicy(Qt::NoFocus);

    //<Setting the default image:>
    this->displayImage();
    //</Setting the default image>

    //<widgets for the QGroupBoxes>
    straightGBox    = new QGroupBox (tr("Straight movement:"));
    straightGBox->setLayout (straightVBox);
    curvedGBox      = new QGroupBox (tr("Curved movement:"));
    curvedGBox  ->setLayout (curvedVBox);
    rotateGBox      = new QGroupBox (tr("Rotatory movement:"));
    rotateGBox  ->setLayout (rotateVBox);

    straightVBox->setSpacing(0);
    curvedVBox  ->setSpacing(0);
    rotateVBox  ->setSpacing(0);

    lbStraightLen   = new QLabel (tr("Lenght: "));
    leStraightLen   = new QLineEdit ("1");
    lbStraightDir   = new QLabel (tr("Direction: "));
    comboStraightDir= new QComboBox;
    this->fillStraightDirCombo();
    btStraightSubmit= new QPushButton (tr("Submit"));
    straightVBox->addLayout (straightLenHBox);
    straightVBox->addLayout (straightDirHBox);
    straightVBox->addLayout (straightBtHBox);
    straightLenHBox ->addStretch();
    straightLenHBox ->addWidget (lbStraightLen);
    straightLenHBox ->addWidget (leStraightLen);
    straightDirHBox ->addStretch();
    straightDirHBox ->addWidget (lbStraightDir);
    straightDirHBox ->addWidget (comboStraightDir);
    straightBtHBox  ->addWidget (btStraightSubmit, 0, Qt::AlignHCenter);

    lbCurvedRad     = new QLabel (tr("Radius: "));
    leCurvedRad     = new QLineEdit ("1");
    lbCurvedAng     = new QLabel (tr("Angle: "));
    leCurvedAng     = new QLineEdit ("1");
    btCurvedSubmit  = new QPushButton (tr("Submit"));
    curvedVBox->addLayout (curvedRadHBox);
    curvedVBox->addLayout (curvedAngHBox);
    curvedVBox->addLayout (curvedBtHBox);
    curvedRadHBox->addStretch();
    curvedRadHBox->addWidget (lbCurvedRad, 0, Qt::AlignRight);
    curvedRadHBox->addWidget (leCurvedRad, 0, Qt::AlignRight);
    curvedAngHBox->addStretch();
    curvedAngHBox->addWidget (lbCurvedAng, 0, Qt::AlignRight);
    curvedAngHBox->addWidget (leCurvedAng, 0, Qt::AlignRight);
    curvedBtHBox ->addWidget (btCurvedSubmit, 0, Qt::AlignHCenter);

    lbRotateAng     = new QLabel (tr("Angle: "));
    leRotateAng     = new QLineEdit ("1");
    btRotateSubmit  = new QPushButton (tr("Submit"));
    rotateVBox->addLayout (rotateAngHBox);
    rotateVBox->addLayout (rotateBtHBox);
    rotateAngHBox->addStretch();
    rotateAngHBox   ->addWidget (lbRotateAng, 0, Qt::AlignRight);
    rotateAngHBox   ->addWidget (leRotateAng, 0, Qt::AlignRight);
    rotateBtHBox    ->addWidget (btRotateSubmit, 0, Qt::AlignHCenter);

    btStraightSubmit->setAutoDefault (false);
    btCurvedSubmit  ->setAutoDefault (false);
    btRotateSubmit  ->setAutoDefault (false);

        //It's never nice to use fixed sizes, but if I don't these look weird...
    leStraightLen ->setFixedWidth (comboStraightDir->sizeHint().width());
    leCurvedRad   ->setFixedWidth (comboStraightDir->sizeHint().width());
    leCurvedAng  ->setFixedWidth (comboStraightDir->sizeHint().width());
    leRotateAng     ->setFixedWidth (comboStraightDir->sizeHint().width());
        //</>

    rightVBox->addWidget (straightGBox);
    //rightVBox->addWidget (curvedGBox);
    rightVBox->addWidget (rotateGBox);
    //</widgets for the QGroupBoxes>

    btPlayPath ->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Minimum);
    btClearPath->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Minimum);
    btPlayPath ->setAutoDefault (false);
    btClearPath->setAutoDefault (false);
    rightVBox->addWidget (btPlayPath);
    rightVBox->addWidget (btClearPath);

    rightVBox->addStretch (100);
    rightVBox->addWidget(frSPInputHLine, 100);
    rightVBox->addLayout (HValueHBox);
    rightVBox->addLayout (VValueHBox);
    rightVBox->addLayout (RValueHBox);

    tmrSerialMovs = new QTimer;
    tmrSerialMovs->setSingleShot(true);

    qDebug ("StartupWindow: Connecting signals and slots");
    connect (mySerialPortControl    ,SIGNAL(stringProcessed())  ,this ,SLOT(handleStringProcessed()));
    connect (mySerialPortControl    ,SIGNAL(bufferModified())   ,this ,SLOT(echoBuffer()));
    connect (mySerialPortControl    ,SIGNAL(portClosed())       ,this ,SLOT(handlePortClosed()));
    connect (myRobotControl         ,SIGNAL(image_downloaded()) ,this ,SLOT(displayImage()));
    connect (myRobotControl         ,SIGNAL(statusRefreshed())  ,this ,SLOT(handleStatusRefreshed()));
    //connect (myRobotControl         ,SIGNAL(justConnected())    ,this ,SLOT(handleBtVideo())); //Automatically start vid
    //connect (myRobotControl         ,SIGNAL(justDisconnected()) ,this ,SLOT(handleBtVideo()));
    connect (myRobotControl         ,SIGNAL(justDisconnected()) ,this ,SLOT(handleRobotDisconnected()));
    connect (myRobotControl         ,SIGNAL(justConnected())    ,this ,SLOT(handleRobotConnected()));
    connect (btHideNShow            ,SIGNAL(released())         ,this ,SLOT(handleBtHideNShow()));
    connect (btManual               ,SIGNAL(released())         ,this ,SLOT(handleBtManual()));
    connect (btVideo                ,SIGNAL(released())         ,this ,SLOT(handleBtVideo()));
    connect (btSnapshot             ,SIGNAL(released())         ,this ,SLOT(handleBtSnapshot()));
    connect (btExit                 ,SIGNAL(released())         ,this ,SLOT(handleBtExit()));
    connect (btUpload               ,SIGNAL(released())         ,this ,SLOT(handleBtUpload()));
    connect (btStraightSubmit       ,SIGNAL(released())         ,this ,SLOT(handleBtStraightSubmit()));
    connect (leStraightLen          ,SIGNAL(returnPressed())    ,this ,SLOT(handleBtStraightSubmit()));
    connect (btRotateSubmit         ,SIGNAL(released())         ,this ,SLOT(handleBtRotateSubmit()));
    connect (leRotateAng            ,SIGNAL(returnPressed())    ,this ,SLOT(handleBtRotateSubmit()));
    connect (btPlayPath             ,SIGNAL(released())         ,this ,SLOT(handleBtPlayPath()));
    connect (btClearPath            ,SIGNAL(released())         ,this ,SLOT(handleBtClearPath()));
    connect (tmrSerialMovs          ,SIGNAL(timeout())          ,this ,SLOT(handleTmrSerialMovs()));
}

void TMainWindow::setConfigTabs (void)
{
    myRCTab = new ConfigRCTab(myRobotControl);
    mySPTab = new ConfigSPTab(mySerialPortControl);
    myDBTab = new ConfigDBTab(myDatabaseControl);
    configTabs->addTab(myRCTab  ,tr("Robot"));
    configTabs->addTab(mySPTab  ,tr("Serial Port"));
    configTabs->addTab(myDBTab  ,tr("Database"));

    //configTabsVBox->setMargin(0);
    configTabsVBox->addWidget(configTabs);
    configTabsVBox->addWidget(frConfigHLine, 100);
}

void TMainWindow::fillStraightDirCombo (void)
{
    // Must keep the order of the directions.
    // Otherwise, change handleBtStraightSubmit.
    comboStraightDir->addItem (tr("North"));
    comboStraightDir->addItem (tr("South"));
    comboStraightDir->addItem (tr("East"));
    comboStraightDir->addItem (tr("West"));
    comboStraightDir->addItem (tr("North-East"));
    comboStraightDir->addItem (tr("North-West"));
    comboStraightDir->addItem (tr("South-East"));
    comboStraightDir->addItem (tr("South-West"));
}

void TMainWindow::handleBtUpload (void)
{
    btUpload->setEnabled(false);
    qDebug("In handleBtUpload");
    if (!databaseWindow)
    {
        myDBTab->handleBtConnect ();
        databaseWindow = new DBWindow(tr("Database control"), myDatabaseControl, myRobotControl);
    }
    if (databaseWindow)
    {
        databaseWindow->show ();
        databaseWindow->raise ();
        databaseWindow->activateWindow ();
    }
    btUpload->setEnabled(true);
}

void TMainWindow::handleBtExit (void)   //close the app
{
    qDebug ("Exit button released");
    if(mySerialPortControl->isOpen())
        mySerialPortControl->closePort();
    if(myDatabaseControl->connection_isOpen())
        myDatabaseControl->disconnect();
    if(databaseWindow)
        databaseWindow->close();
    this->close();
}

// <Resizing stuff>
void TMainWindow::displayImage (void)
{
    *pixmap_image = QPixmap::fromImage(myRobotControl->get_image());    //Cargamos la image como pixmap
    *resized_pixmapImage = pixmap_image->scaled (middleScrollArea->size().width()-2,
                                                 middleScrollArea->size().height()-2, Qt::KeepAspectRatio); //resize it
    myImageLabel->setPixmap(*resized_pixmapImage);                      //display it

    //Para establecer el tamaño mínimo en unas dimensiones concretas (la resolución mínima a la que transmite el rovio):
    middleScrollArea->setMinimumWidth (176);
    middleScrollArea->setMinimumHeight (144);

}

void TMainWindow::resizeEvent(QResizeEvent *event=NULL)
{
    if (!pixmap_image->isNull())
    {
        *pixmap_image = QPixmap::fromImage(myRobotControl->get_image());
        *resized_pixmapImage = pixmap_image->scaled (middleScrollArea->size().width(),
                                                     middleScrollArea->size().height(), Qt::KeepAspectRatio);
        myImageLabel->setPixmap(*resized_pixmapImage);
    }
    event->ignore();
}

void TMainWindow::forceResizeEvent (void)
{
    QResizeEvent *resizeEvent = new QResizeEvent (this->size(), this->size());
    QCoreApplication::postEvent(this, resizeEvent);
}
// </Resizing stuff>

void TMainWindow::handleStringProcessed (void)
{
    int JOY_NEG = -40;
    int JOY_POS = 40;
    int ACC_NEG = -150;
    int ACC_POS = 150;
    int HValue = 0;
    int VValue = 0;
    int RValue = 0;
    bool CState = 0;
    bool ZState = 0;
    bool CandZState = 0;

    HValue = mySerialPortControl->get_HValue();
    VValue = mySerialPortControl->get_VValue();
    RValue = mySerialPortControl->get_RValue();
    CState = mySerialPortControl->get_CValue();
    ZState = mySerialPortControl->get_ZValue();
    CandZState = mySerialPortControl->get_CandZValue();
    lbHValue->setText (QString::number(HValue));
    lbVValue->setText (QString::number(VValue));
    lbRValue->setText (QString::number(RValue));
    //leStatusBar->setText (QString(mySerialPortControl->buffer));

    if (myRobotControl->isFollowingPath())
        return; // Return if the manual override is not enabled.

    if ((CandZState == 1) && (prev_CandZState == false))
    {
        if (myRobotControl->get_lightStatus())
            myRobotControl->executeNowAndStopPath("L0");
        else
            myRobotControl->executeNowAndStopPath("L1");
    }
    else if ((CState == true) && (prev_CState == false))
    {
        if (myRobotControl->get_headPosition() == "BOT")
            myRobotControl->executeNowAndStopPath("HMID");
        else if (myRobotControl->get_headPosition() == "MID")
            myRobotControl->executeNowAndStopPath("HTOP");
    }
    else if ((ZState == true) && (prev_ZState == false))
    {
        if (myRobotControl->get_headPosition() == "TOP")
            myRobotControl->executeNowAndStopPath("HMID");
        else if (myRobotControl->get_headPosition() == "MID")
            myRobotControl->executeNowAndStopPath("HBOT");
    }
    prev_CState = CState;
    prev_ZState = ZState;
    prev_CandZState = CandZState;

    if (serialPortMovable != 1)
        return;

    if ((HValue >= JOY_POS) && (VValue >= JOY_POS))
        myRobotControl->executeNowAndStopPath("NE1"); //North-East
    else if ((JOY_NEG < HValue) && (HValue < JOY_POS) && (VValue > JOY_POS))
        myRobotControl->executeNowAndStopPath("N1");  //North
    else if ((HValue < JOY_NEG) && (VValue > JOY_POS))
        myRobotControl->executeNowAndStopPath("NW1"); //North-West

    else if ((HValue < JOY_NEG) && (JOY_NEG < VValue) && (VValue < JOY_POS))
        myRobotControl->executeNowAndStopPath("W1");  //West
    else if ((HValue > JOY_POS) && (JOY_NEG < VValue) && (VValue < JOY_POS))
        myRobotControl->executeNowAndStopPath("E1");  //East

    else if ((HValue < JOY_NEG) && (VValue < JOY_NEG))
        myRobotControl->executeNowAndStopPath("SW1"); //South-West
    else if ((JOY_NEG < HValue) && (HValue < JOY_POS) && (VValue < JOY_NEG))
        myRobotControl->executeNowAndStopPath("S1");  //South
    else if ((HValue > JOY_POS) && (VValue < JOY_NEG))
        myRobotControl->executeNowAndStopPath("SE1"); //South-East

    else if (RValue >= ACC_POS) //"else if" because straight + rotate messes up the movement of the robot.
        myRobotControl->executeNowAndStopPath("RR1"); //Rotate right
    else if (RValue <= ACC_NEG)
        myRobotControl->executeNowAndStopPath("RL1"); //Rotate left

    serialPortMovable = 0;
    tmrSerialMovs->start(50);

}

void TMainWindow::handleTmrSerialMovs (void)
{
    serialPortMovable = 1;
}

void TMainWindow::handlePortClosed (void)
{
    this->stopManualOverride();
    lbHValue->setText ("----");
    lbVValue->setText ("----");
    lbRValue->setText ("----");
}

void TMainWindow::handleBtHideNShow (void)
{
    if (configTabs->isHidden())
    {
        configTabs->show();
        btHideNShow->setText (tr("Hide config tabs"));
    }
    else
    {
        configTabs->hide();
        btHideNShow->setText (tr("Show config tabs"));
    }
    this->forceResizeEvent();
    /*
    QResizeEvent *resizeEvent = new QResizeEvent (this->size(), this->size());
    QCoreApplication::postEvent(this, resizeEvent);
    */
}

void TMainWindow::handleBtManual (void)
{
    btManual->setEnabled (false);
    if ( !mySerialPortControl->isOpen() ) //If the port is closed
    {
        if ( !mySPTab->handleBtConnectPort ()) //  Try to open it.
        {
            btManual->setEnabled (true);
            return;                            //  If we could not connect the port, return.
        }
    }

    if (myRobotControl->isFollowingPath())
        this->startManualOverride();
    else
        this->stopManualOverride();

    btManual->setEnabled (true);
}

void TMainWindow::startManualOverride (void)
{
    myRobotControl->set_followPath(0);
    btManual->setText (tr("Software control"));
}

void TMainWindow::stopManualOverride (void)
{
    myRobotControl->set_followPath(1);
    btManual->setText (tr("Manual override"));
}

void TMainWindow::handleBtVideo (void)
{
    if (!myRobotControl->isConnected())
        myRCTab->handleBtConnect();
    if (!myRobotControl->isVideoON())
    {
        myRobotControl->startVideoFeed();
        btVideo->setText(tr("Stop video feed"));
    }
    else if (myRobotControl->isVideoON())
    {
        myRobotControl->stopVideoFeed();
        btVideo->setText(tr("Start video feed"));
    }
}

void TMainWindow::handleBtSnapshot (void)
{
    QDateTime dt = QDateTime::currentDateTime();
    QImage img;
    QString path = QDesktopServices::storageLocation
                    (QDesktopServices::DesktopLocation);
    path.append("/Rovio-Snapshot_" + dt.toString("d.MM.yyyy  hh:mm:ss") + ".jpeg");

    img = myRobotControl->get_image();
    if (img.save(path, "JPEG", 50))
        qDebug() << "Snapshot successfuly saved";
    else
        qDebug() << "ERROR: Snapshot not saved";
}

void TMainWindow::handleStatusRefreshed (void)
{
    if (myRobotControl->get_chargingStatus() == true) //If the robot is charging...
        lbBattery->setText(*strBattery + tr("Docked")); //Don't display the % because the Rovio does not read it
    else
        lbBattery->setText(*strBattery + QString::number(myRobotControl->get_battery()) + "\%");
    lbWifi->setText(*strWifi + QString::number(myRobotControl->get_wifiStrength()) + "\%");
}

void TMainWindow::handleRobotDisconnected (void)
{
    lbBattery   ->hide();
    lbWifi      ->hide();
    this->forceResizeEvent();
}

void TMainWindow::handleRobotConnected (void)
{
    lbBattery   ->show();
    lbWifi      ->show();
    this->forceResizeEvent();
}
                                /**************\
\******************************|******Misc:*****|****************************/
void TMainWindow::set_HLine (QFrame *fr)
{
    fr->setFrameShape (QFrame::HLine);
    fr->setFrameShadow(QFrame::Sunken);
}

void TMainWindow::set_VLine(QFrame *fr)
{
    fr->setFrameShape (QFrame::VLine);
    fr->setFrameShadow(QFrame::Sunken);
}

void TMainWindow::echoBuffer(void)
{
    leStatusBar->setText(QString(mySerialPortControl->buffer));
}

                             /*********************\
\***************************|*****Movement BTs:*****|************************/
void TMainWindow::handleBtStraightSubmit (void)
{
    int len=0;
    unsigned int len_abs=0;
    len = leStraightLen->text().toInt();
    QString direction;
    if (comboStraightDir->currentIndex() == 0)
        direction = "N";
    else if (comboStraightDir->currentIndex() == 1)
        direction = "S";
    else if (comboStraightDir->currentIndex() == 2)
        direction = "E";
    else if (comboStraightDir->currentIndex() == 3)
        direction = "W";
    else if (comboStraightDir->currentIndex() == 4)
        direction = "NE";
    else if (comboStraightDir->currentIndex() == 5)
        direction = "NW";
    else if (comboStraightDir->currentIndex() == 6)
        direction = "SE";
    else if (comboStraightDir->currentIndex() == 7)
        direction = "SW";
    else
        qDebug() << "Unrecognized index in comboStraightDir... What?";
    if (len >= 0)
        len_abs = len;
    else if (len < 0)
        len_abs = -len; //Just in case...
    myRobotControl->appendToPath(direction+QString::number(len_abs));
}

void TMainWindow::handleBtRotateSubmit (void)
{
    int ang=0;
    unsigned int ang_abs=0;
    ang = leRotateAng->text().toInt();
    if (ang >= 0)
    {
        ang_abs = ang;
        myRobotControl->appendToPath("RR"+QString::number(ang_abs));
    }
    else if (ang < 0)
    {
        ang_abs = -ang;
        myRobotControl->appendToPath("RL"+QString::number(ang_abs));
    }
    else
        return;
}

void TMainWindow::handleBtPlayPath (void)
{
    myRobotControl->executePath();
}

void TMainWindow::handleBtClearPath (void)
{
    myRobotControl->clearCmdsList();
}


                                /**************\
\******************************|*****Tests:*****|****************************/
