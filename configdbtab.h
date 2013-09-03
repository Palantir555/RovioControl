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

#ifndef CONFIGDBTAB_H
#define CONFIGDBTAB_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include "databasecontrol.h"

#include <QtSql>

class ConfigDBTab : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigDBTab(DatabaseControl *myDBC, QWidget *parent = 0);

private:
    DatabaseControl *myDatabaseControl_p;

    QGridLayout *configGrid;
    QHBoxLayout *configHBoxA00;
    QHBoxLayout *configHBoxA01;
    QHBoxLayout *configHBoxA10;
    QHBoxLayout *configHBoxA11;
    QHBoxLayout *configHBoxA20;
    QHBoxLayout *configHBoxA21;

    QHBoxLayout *configHBoxB02;
    QHBoxLayout *configHBoxB03;
    QHBoxLayout *configHBoxB12;
    QHBoxLayout *configHBoxB13;
    QHBoxLayout *configHBoxB22;
    QHBoxLayout *configHBoxB23;
    QHBoxLayout *configHBoxB33;

    QLabel      *lbDriverId;
    QComboBox   *comboDriver;

    QLabel      *lbHostId;
    QLineEdit   *leHost;

    QLabel      *lbPortId;
    QLineEdit   *lePort;

    QLabel      *lbNameId;
    QLineEdit   *leName;

    QLabel      *lbUserId;
    QLineEdit   *leUser;

    QLabel      *lbPasswordId;
    QLineEdit   *lePassword;

    QPushButton *btConnectDB;

    void createLayout(void);
    void createWidgets(void);
    void fillComboDriver (void);
    void fillLabels_defaultValues (void);
signals:
    
public slots:
    void sendTo_driverSetter (int idx);
    void sendTo_hostSetter (void);
    void sendTo_portSetter (void);
    void sendTo_nameSetter (void);
    void sendTo_userSetter (void);
    void sendTo_passwordSetter (void);
    void handleBtConnect (void);
};

#endif // CONFIGDBTAB_H
