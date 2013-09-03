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

#ifndef CONFIGRCTAB_H
#define CONFIGRCTAB_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>

#include "robotcontrol.h"

class ConfigRCTab : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigRCTab(RobotControl *myRC, QWidget *parent = 0);
    
private:
    RobotControl *myRobotControl_p;

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
    QHBoxLayout *configHBoxB32;
    QHBoxLayout *configHBoxB33;
    QHBoxLayout *configHBoxB43;

    QLabel      *lbSpacer;

    QLabel      *lbUserId;
    QLineEdit   *leUser;

    QLabel      *lbPasswordId;
    QLineEdit   *lePassword;

    QLabel      *lbUrlId;
    QLineEdit   *leUrl;

    QLabel      *lbFrameRate;
    QComboBox   *comboFrameRate;
    QStringList slFrameRates;

    QLabel      *lbHeadPos;
    QComboBox   *comboHeadPos;

    QLabel      *lbLightStatus;
    QComboBox   *comboLightStatus;

    QPushButton *btConnectRobot;

    QTimer *tmrHead;

    void createLayout       (void);
    void createWidgets      (void);
    void fillComboFrameRate (void);
signals:
    
public slots:
    void handleBtConnect        (void);

private slots:
    void userSetter             (void);
    void passwordSetter         (void);
    void urlSetter              (void);
    void frameRateSetter        (void);
    void handleTimerHead        (void);
    void handleComboHeadPos     (int idx);
    void handleComboLightStatus (int idx);
    
};

#endif // CONFIGRCTAB_H
