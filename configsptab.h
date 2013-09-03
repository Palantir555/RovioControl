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

#ifndef CONFIGSPTAB_H
#define CONFIGSPTAB_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>

#include "serialportcontrol.h"

class ConfigSPTab : public QWidget
{
    Q_OBJECT

public:
    ConfigSPTab (SerialPortControl *mySPC, QWidget *parent = 0);
    void createLayout(void);
    void createWidgets(void);
    void fillComboOpen (void);
    void fillComboQuery (void);
    void fillComboBaud (void);
    void fillComboData (void);
    void fillComboParity (void);
    void fillComboStop (void);
    void fillComboFlow (void);


private:
    SerialPortControl *mySerialPortControl_p;

    QGridLayout *configGrid;
    QHBoxLayout *configHBoxA00;
    QHBoxLayout *configHBoxA01;
    QHBoxLayout *configHBoxA10;
    QHBoxLayout *configHBoxA11;
    QHBoxLayout *configHBoxA20;
    QHBoxLayout *configHBoxA21;
    QHBoxLayout *configHBoxA30;
    QHBoxLayout *configHBoxA31;


    QHBoxLayout *configHBoxB02;
    QHBoxLayout *configHBoxB03;
    QHBoxLayout *configHBoxB12;
    QHBoxLayout *configHBoxB13;
    QHBoxLayout *configHBoxB22;
    QHBoxLayout *configHBoxB23;
    QHBoxLayout *configHBoxB32;
    QHBoxLayout *configHBoxB33;
    QHBoxLayout *configHBoxB43;

    QLabel      *lbPortComboId;
    QComboBox   *comboEnumerator;
    QPushButton *btRefreshPorts;

    QLabel      *lbOpenId;
    QComboBox   *comboOpen;

    QLabel      *lbQueryComboId;
    QComboBox   *comboQuery;

    QLabel      *lbBaudId;
    QComboBox   *comboBaud;

    QLabel      *lbDataId;
    QComboBox   *comboData;

    QLabel      *lbParityId;
    QComboBox   *comboParity;

    QLabel      *lbStopId;
    QComboBox   *comboStop;

    QLabel      *lbFlowId;
    QComboBox   *comboFlow;

    QPushButton *btConnectPort;
signals:

public slots:
    void listPorts(void);

    void sendTo_portNameSetter (int idx);
    void sendTo_openModeSetter(int idx);
    void sendTo_queryModeSetter(int idx);
    void sendTo_baudRateSetter(int idx);
    void sendTo_dataBitsSetter(int idx);
    void sendTo_parityBitsSetter(int idx);
    void sendTo_stopBitsSetter(int idx);
    void sendTo_flowControlSetter(int idx);

    void handleBtRefreshPorts(void);
    bool handleBtConnectPort(void);
};

#endif // CONFIGSPTAB_H
