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

#include "configdbtab.h"

ConfigDBTab::ConfigDBTab(DatabaseControl *myDBC, QWidget *parent) :
    QWidget(parent)
{
    myDatabaseControl_p = myDBC;
    this->createLayout();
    this->createWidgets();
    this->fillComboDriver();
    this->fillLabels_defaultValues();
}

void ConfigDBTab::createLayout (void)
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
    configHBoxB33   = new QHBoxLayout;

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
    configGrid->addLayout (configHBoxB33, 3, 3);

    configGrid->setColumnStretch (0, 5);
    configGrid->setColumnStretch (1, 45);
    configGrid->setColumnStretch (2, 5);
    configGrid->setColumnStretch (3, 45);

    configGrid->setContentsMargins(5, 0, 5, 0);
    configGrid->setVerticalSpacing(0);
}

void ConfigDBTab::createWidgets (void)
{
    lbDriverId      = new QLabel (tr("Driver:"));
    comboDriver     = new QComboBox;

    lbHostId        = new QLabel (tr("Host name:"));
    leHost          = new QLineEdit;

    lbPortId        = new QLabel (tr("Server port:"));
    lePort          = new QLineEdit;

    lbNameId        = new QLabel (tr("Database name:"));
    leName          = new QLineEdit;

    lbUserId        = new QLabel (tr("User:"));
    leUser          = new QLineEdit;

    lbPasswordId    = new QLabel (tr("Password:"));
    lePassword      = new QLineEdit;

    btConnectDB    = new QPushButton (tr("Connect"));

    btConnectDB->setAutoDefault (false);
    lePassword->setEchoMode (QLineEdit::Password);
    //QLineEdit::PasswordEchoOnEdit shows the password uncover until you leave the box

    configHBoxA00->addStretch (100);        //Add stretch->add label->add combo->add stretch
    configHBoxA00->addWidget  (lbDriverId);
    configHBoxA01->addWidget  (comboDriver);
    configHBoxA01->addStretch (100);

    configHBoxA10->addStretch (100);
    configHBoxA10->addWidget  (lbHostId);
    configHBoxA11->addWidget  (leHost);

    configHBoxA20->addStretch (100);
    configHBoxA20->addWidget  (lbPortId);
    configHBoxA21->addWidget  (lePort);

    configHBoxB02->addStretch (100);
    configHBoxB02->addWidget  (lbNameId);
    configHBoxB03->addWidget  (leName);

    configHBoxB12->addStretch (100);
    configHBoxB12->addWidget  (lbUserId);
    configHBoxB13->addWidget  (leUser);

    configHBoxB22->addStretch (100);
    configHBoxB22->addWidget  (lbPasswordId);
    configHBoxB23->addWidget  (lePassword);

    configHBoxB33->addWidget  (btConnectDB);
    configHBoxB33->addStretch (100);

    setLayout(this->configGrid);

    connect (this->comboDriver,SIGNAL(activated(int))       ,this, SLOT(sendTo_driverSetter(int)));
    connect (this->leHost     ,SIGNAL(editingFinished())    ,this, SLOT(sendTo_hostSetter()));
    connect (this->lePort     ,SIGNAL(editingFinished())    ,this, SLOT(sendTo_portSetter()));
    connect (this->leName     ,SIGNAL(editingFinished())    ,this, SLOT(sendTo_nameSetter()));
    connect (this->leUser     ,SIGNAL(editingFinished())    ,this, SLOT(sendTo_userSetter()));
    connect (this->lePassword ,SIGNAL(editingFinished())    ,this, SLOT(sendTo_passwordSetter()));
    connect (this->btConnectDB,SIGNAL(released())           ,this, SLOT(handleBtConnect()));
}

void ConfigDBTab::fillComboDriver (void)
{
    comboDriver->addItem("QPSQL");
    comboDriver->setCurrentIndex(0);

    this->sendTo_driverSetter(comboDriver->currentIndex());
}

void ConfigDBTab::sendTo_driverSetter (int idx)
{
    switch (idx)
    {
    case 0:  myDatabaseControl_p->set_driver ("QPSQL"); break;  //set_driver devuelve un bool para ver si el driver
    default: myDatabaseControl_p->set_driver ("QPSQL"); break;  //      estaba disponible.
    }                                                           //Usarlo cuando haya más drivers.
}
void ConfigDBTab::sendTo_hostSetter (void)
{
    myDatabaseControl_p->set_hostName (leHost->text());
}
void ConfigDBTab::sendTo_portSetter (void)
{
    myDatabaseControl_p->set_port (lePort->text().toInt()); //converted toInt
}
void ConfigDBTab::sendTo_nameSetter (void)
{
    myDatabaseControl_p->set_databaseName (leName->text());
}
void ConfigDBTab::sendTo_userSetter (void)
{
    myDatabaseControl_p->set_username (leUser->text());
}
void ConfigDBTab::sendTo_passwordSetter (void)
{
    myDatabaseControl_p->set_password(lePassword->text());
}

void ConfigDBTab::fillLabels_defaultValues (void)
{
    leHost      ->setText (myDatabaseControl_p->get_hostName());
    lePort      ->setText (QString::number(myDatabaseControl_p->get_port()));
    leName      ->setText (myDatabaseControl_p->get_databaseName());
    leUser      ->setText (myDatabaseControl_p->get_username());
    lePassword  ->setText (myDatabaseControl_p->get_password());
}

void ConfigDBTab::handleBtConnect (void)
{
    QMessageBox *unableToConnectMessage = new QMessageBox;
    if (myDatabaseControl_p->connection_isOpen() == false)  //If the DB is disconnected...
    {
        btConnectDB->setEnabled (false);
        if(myDatabaseControl_p->connect() == true)          //    ...connect it
        {
            qDebug ("Connection to the DB open");
            btConnectDB->setText ("Disconnect");
        }
        else
        {
            qDebug ("Failed to connect to the DB");
            unableToConnectMessage->setText (tr("Unable to open the connection to the DB"));
            unableToConnectMessage->setInformativeText (tr("Check your connections and configuration"));
            unableToConnectMessage->setIcon (QMessageBox::Warning);
            unableToConnectMessage->exec();
        }
        btConnectDB->setEnabled (true);
    }
    else
    {
        btConnectDB->setEnabled (false);
        myDatabaseControl_p->disconnect();
        btConnectDB->setText ("Connect");
        btConnectDB->setEnabled (true);
    }
}
