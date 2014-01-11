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

#include "configrctab.h"

ConfigRCTab::ConfigRCTab(RobotControl *myRC, QWidget *parent) :
    QWidget(parent)
{
    myRobotControl_p = myRC;
    this->createLayout();
    this->createWidgets();
}

void ConfigRCTab::createLayout (void)
{
    configGrid      = new QGridLayout;
    configHBoxA00   = new QHBoxLayout;
    configHBoxA01   = new QHBoxLayout;
    configHBoxA10   = new QHBoxLayout;
    configHBoxA11   = new QHBoxLayout;
    configHBoxA20   = new QHBoxLayout;
    configHBoxA21   = new QHBoxLayout;

    configHBoxB02   = new QHBoxLayout;
    configHBoxB03   = new QHBoxLayout;
    configHBoxB12   = new QHBoxLayout;
    configHBoxB13   = new QHBoxLayout;
    configHBoxB22   = new QHBoxLayout;
    configHBoxB23   = new QHBoxLayout;
    configHBoxB32   = new QHBoxLayout;
    configHBoxB33   = new QHBoxLayout;
    configHBoxB43   = new QHBoxLayout;
    /*
                COLUMN A                        COLUMN B
      -------------  ---------------   ------------- ---------------
    |      A00      |      A01      |||     B02     |      B03      |
    |---------------|---------------|||-------------|---------------|
    |---------------|---------------|||-------------|---------------|
    |---------------|---------------|||-------------|---------------|
    |---------------|---------------|||-------------|---------------|
           AX0             AX1              BX2            BX3
    */
    configGrid->addLayout (configHBoxA00, 0, 0);
    configGrid->addLayout (configHBoxA01, 0, 1);
    configGrid->addLayout (configHBoxA10, 1, 0);
    configGrid->addLayout (configHBoxA11, 1, 1);
    configGrid->addLayout (configHBoxA20, 2, 0);
    configGrid->addLayout (configHBoxA21, 2, 1);

    configGrid->addLayout (configHBoxB02, 0, 2);
    configGrid->addLayout (configHBoxB03, 0, 3);
    configGrid->addLayout (configHBoxB12, 1, 2);
    configGrid->addLayout (configHBoxB13, 1, 3);
    configGrid->addLayout (configHBoxB22, 2, 2);
    configGrid->addLayout (configHBoxB23, 2, 3);
    configGrid->addLayout (configHBoxB32, 3, 2);
    configGrid->addLayout (configHBoxB33, 3, 3);
    configGrid->addLayout (configHBoxB43, 4, 3);

    configGrid->setColumnStretch (0, 20);
    configGrid->setColumnStretch (1, 25);
    configGrid->setColumnStretch (2, 5);
    configGrid->setColumnStretch (3, 45);

    configGrid->setContentsMargins(5, 0, 5, 0);
    configGrid->setVerticalSpacing(0);
}

void ConfigRCTab::createWidgets (void)
{
    lbSpacer        = new QLabel("");
    lbUserId        = new QLabel (tr("Username:"));
    leUser          = new QLineEdit;

    lbPasswordId    = new QLabel (tr("Password:"));
    lePassword      = new QLineEdit;

    lbUrlId         = new QLabel (tr("URL:"));
    leUrl           = new QLineEdit(myRobotControl_p->get_url());

    lbFrameRate     = new QLabel (tr("Video Frame Rate:"));
    comboFrameRate  = new QComboBox;

    lbHeadPos       = new QLabel (tr("Head Position:"));
    comboHeadPos    = new QComboBox;

    lbLightStatus   = new QLabel (tr("Light:"));
    comboLightStatus= new QComboBox;
    btConnectRobot  = new QPushButton (tr("Connect"));

    tmrHead         = new QTimer();

    btConnectRobot->setAutoDefault (false);
    lePassword->setEchoMode (QLineEdit::Password);
    //QLineEdit::PasswordEchoOnEdit shows the password uncover until you leave the box

    comboHeadPos->addItem (tr("Bottom"));
    comboHeadPos->addItem (tr("Middle"));
    comboHeadPos->addItem (tr("Top"));
    comboHeadPos->setCurrentIndex (0);

    comboLightStatus->addItem (tr("OFF"));
    comboLightStatus->addItem (tr("ON"));
    comboLightStatus->setCurrentIndex (0);

    configHBoxA00->addStretch (100);
    configHBoxA00->addWidget  (lbUserId);
    configHBoxA01->addWidget  (leUser);

    configHBoxA10->addStretch (100);
    configHBoxA10->addWidget  (lbPasswordId);
    configHBoxA11->addWidget  (lePassword);

    configHBoxA20->addStretch (100);
    configHBoxA20->addWidget  (lbSpacer);
    configHBoxA21->addWidget  (lbSpacer);

    configHBoxB02->addStretch (100);
    configHBoxB02->addWidget  (lbUrlId);
    configHBoxB03->addWidget  (leUrl);

    configHBoxB12->addStretch (100);
    configHBoxB12->addWidget  (lbFrameRate);
    configHBoxB13->addWidget  (comboFrameRate);
    configHBoxB13->addStretch (100);

    configHBoxB22->addStretch (100);
    configHBoxB22->addWidget  (lbHeadPos);
    configHBoxB23->addWidget  (comboHeadPos);
    configHBoxB23->addStretch (100);

    configHBoxB32->addStretch (100);
    configHBoxB32->addWidget  (lbLightStatus);
    configHBoxB33->addWidget  (comboLightStatus);
    configHBoxB33->addStretch (100);

    configHBoxB43->addWidget  (btConnectRobot);
    configHBoxB43->addStretch (100);

    setLayout(this->configGrid);

    tmrHead->setSingleShot(true);
    this->fillComboFrameRate();
    //Init the default values:
    this->leUser    ->setText (QString("admin"));
    this->lePassword->setText (QString("password"));
    this->userSetter();
    this->passwordSetter();

    connect (this->comboHeadPos     ,SIGNAL(activated(int)) ,this
             ,SLOT(handleComboHeadPos(int)));

    connect (this->comboLightStatus ,SIGNAL(activated(int)) ,this
             ,SLOT(handleComboLightStatus(int)));

    connect (this->btConnectRobot   ,SIGNAL(released())               ,this
             ,SLOT(handleBtConnect()));
    connect (this->tmrHead          ,SIGNAL(timeout())      ,this
             ,SLOT(handleTimerHead()));
}

void ConfigRCTab::fillComboFrameRate (void)
{
    double i=0;
    slFrameRates.clear();
    for (i=1; i<=32; i++)
        slFrameRates.append(QString("%1 fps").arg(QString::number(i)));

    comboFrameRate->addItems(slFrameRates);
    comboFrameRate->setCurrentIndex(12-1); //Default is 12 FPS
}

void ConfigRCTab::userSetter(void)
{
    myRobotControl_p->set_user(leUser->text());
}

void ConfigRCTab::passwordSetter(void)
{
    myRobotControl_p->set_pass(lePassword->text());
}

void ConfigRCTab::urlSetter(void)
{
    myRobotControl_p->set_url (leUrl->text());
}

void ConfigRCTab::frameRateSetter (void)
{
    myRobotControl_p->set_videoFreq(comboFrameRate->currentIndex()+1);  //+1 because there's no 0 in the list
}

void ConfigRCTab::handleComboHeadPos (int idx)
{
    QString cmd;
    cmd.clear();
    //if isFollowingPath -> executenowandcontinuepath
    //else executenowandstoppath
    //Para ello, dentro del switch seleccionamos la QString, y después elegimos cómo enviarlo
    switch (idx)
    {
    case 0:
        cmd = "HBOT";
        break;
    case 1:
        cmd = "HMID";
        break;
    case 2:
        cmd = "HTOP";
        break;
    default:
        cmd = "HBOT";
    }
    if(myRobotControl_p->isFollowingPath())
        myRobotControl_p->executeNowAndContinuePath(cmd);
    else
        myRobotControl_p->executeNowAndStopPath(cmd);
}

void ConfigRCTab::handleComboLightStatus(int idx)
{
    QString cmd;
    switch (idx)
    {
    case 0:
        cmd = "L0";
        break;
    case 1:
        cmd = "L1";
        break;
    default:
        cmd = "L0";
    }
    if(myRobotControl_p->isFollowingPath())
        myRobotControl_p->executeNowAndContinuePath(cmd);
    else
        myRobotControl_p->executeNowAndStopPath(cmd);
}

void ConfigRCTab::handleBtConnect(void)
{
    if (!myRobotControl_p->isConnected())
    {
        this->userSetter();
        this->passwordSetter();
        this->urlSetter();
        this->frameRateSetter();
        myRobotControl_p->openConnection();
        btConnectRobot->setText (tr("Disconnect"));
        tmrHead->start(100);
    }
    else
    {
        myRobotControl_p->closeConnection();
        btConnectRobot->setText (tr("Connect"));
    }
}

void ConfigRCTab::handleTimerHead (void)
{
    QStringList headPositions;
    headPositions << "BOT" << "MID" << "TOP";
    QString currentHeadPosition;

    currentHeadPosition = myRobotControl_p->get_headPosition();
    if (currentHeadPosition == "not_available")
    {
        tmrHead->start(100);
        return;
    }
    comboHeadPos->setCurrentIndex(headPositions.indexOf(currentHeadPosition));
}
