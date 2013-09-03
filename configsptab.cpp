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

#include "configsptab.h"

ConfigSPTab::ConfigSPTab(SerialPortControl *mySPC, QWidget *parent) :
    QWidget(parent)
{
    mySerialPortControl_p = mySPC;
    this->createLayout();
    this->createWidgets();
    this->listPorts();
    this->fillComboOpen();
    this->fillComboQuery();
    this->fillComboBaud();
    this->fillComboData();
    this->fillComboParity();
    this->fillComboStop();
    this->fillComboFlow();
}

//En serio? Este tochaco?? Algo estoy haciendo mal... Pointers mejor?:
void ConfigSPTab::createLayout (void)
{
    configGrid      = new QGridLayout;
    configHBoxA00   = new QHBoxLayout;
    configHBoxA01   = new QHBoxLayout;
    configHBoxA10   = new QHBoxLayout;
    configHBoxA11   = new QHBoxLayout;
    configHBoxA20   = new QHBoxLayout;
    configHBoxA21   = new QHBoxLayout;
    configHBoxA30   = new QHBoxLayout;
    configHBoxA31   = new QHBoxLayout;

    configHBoxB02   = new QHBoxLayout;
    configHBoxB03   = new QHBoxLayout;
    configHBoxB12   = new QHBoxLayout;
    configHBoxB13   = new QHBoxLayout;
    configHBoxB22   = new QHBoxLayout;
    configHBoxB23   = new QHBoxLayout;
    configHBoxB32   = new QHBoxLayout;
    configHBoxB33   = new QHBoxLayout;
    configHBoxB43   = new QHBoxLayout;

    configGrid->addLayout (configHBoxA00, 0, 0);
    configGrid->addLayout (configHBoxA01, 0, 1);
    configGrid->addLayout (configHBoxA10, 1, 0);
    configGrid->addLayout (configHBoxA11, 1, 1);
    configGrid->addLayout (configHBoxA20, 2, 0);
    configGrid->addLayout (configHBoxA21, 2, 1);
    configGrid->addLayout (configHBoxA30, 3, 0);
    configGrid->addLayout (configHBoxA31, 3, 1);

    configGrid->addLayout (configHBoxB02, 0, 2);
    configGrid->addLayout (configHBoxB03, 0, 3);
    configGrid->addLayout (configHBoxB12, 1, 2);
    configGrid->addLayout (configHBoxB13, 1, 3);
    configGrid->addLayout (configHBoxB22, 2, 2);
    configGrid->addLayout (configHBoxB23, 2, 3);
    configGrid->addLayout (configHBoxB32, 3, 2);
    configGrid->addLayout (configHBoxB33, 3, 3);
    configGrid->addLayout (configHBoxB43, 4, 3);

    configGrid->setColumnStretch (0, 40);
    configGrid->setColumnStretch (1, 10);
    configGrid->setColumnStretch (2, 10);
    configGrid->setColumnStretch (3, 40);

    configGrid->setContentsMargins(5, 0, 5, 0);
    configGrid->setVerticalSpacing(0);
}

void ConfigSPTab::createWidgets (void)
{
    lbPortComboId   = new QLabel (tr("Port:"));
    comboEnumerator = new QComboBox;
    btRefreshPorts  = new QPushButton;

    lbOpenId        = new QLabel(tr("Open mode:"));
    comboOpen       = new QComboBox;

    lbQueryComboId  = new QLabel(tr("Query mode:"));
    comboQuery      = new QComboBox;

    lbBaudId        = new QLabel(tr("Baud Rate:"));
    comboBaud       = new QComboBox;

    lbDataId        = new QLabel(tr("Data bits:"));
    comboData       = new QComboBox;

    lbParityId      = new QLabel(tr("Parity bits:"));
    comboParity     = new QComboBox;

    lbStopId        = new QLabel(tr("Stop bits:"));
    comboStop       = new QComboBox;

    lbFlowId        = new QLabel(tr("Flow control:"));
    comboFlow       = new QComboBox;

    btConnectPort   = new QPushButton (tr("Connect"));

    btRefreshPorts->setSizePolicy  (QSizePolicy::Minimum, QSizePolicy::Minimum);
    QPixmap refresh_icon(":/img/refresh.png");
    btRefreshPorts->setIcon (QIcon(refresh_icon));
    //btRefreshPorts->resize (refresh_icon.size());
    btRefreshPorts->setAutoDefault (false);
    btConnectPort ->setAutoDefault (false);

    configHBoxA00->addStretch (100);        //Add stretch->add label->add combo->add stretch
    configHBoxA00->addWidget  (lbPortComboId);
    configHBoxA01->addWidget  (comboEnumerator);
    configHBoxA01->addWidget  (btRefreshPorts);//, 0, Qt::AlignRight);
    configHBoxA01->addStretch (100);

    configHBoxA10->addStretch (100);
    configHBoxA10->addWidget  (lbOpenId);
    configHBoxA11->addWidget  (comboOpen);
    configHBoxA11->addStretch (100);

    configHBoxA20->addStretch (100);
    configHBoxA20->addWidget  (lbQueryComboId);
    configHBoxA21->addWidget  (comboQuery);
    configHBoxA21->addStretch (100);

    configHBoxA30->addStretch (100);
    configHBoxA30->addWidget  (lbBaudId);
    configHBoxA31->addWidget  (comboBaud);
    configHBoxA31->addStretch (100);

    configHBoxB02->addStretch (100);
    configHBoxB02->addWidget  (lbDataId);
    configHBoxB03->addWidget  (comboData);
    configHBoxB03->addStretch (100);

    configHBoxB12->addStretch (100);
    configHBoxB12->addWidget  (lbParityId);
    configHBoxB13->addWidget  (comboParity);
    configHBoxB13->addStretch (100);

    configHBoxB22->addStretch (100);
    configHBoxB22->addWidget  (lbStopId);
    configHBoxB23->addWidget  (comboStop);
    configHBoxB23->addStretch (100);

    configHBoxB32->addStretch (100);
    configHBoxB32->addWidget  (lbFlowId);
    configHBoxB33->addWidget  (comboFlow);
    configHBoxB33->addStretch (100);

    configHBoxB43->addWidget  (btConnectPort);
    configHBoxB43->addStretch (100);

    setLayout(configGrid);//DO NOT FORGET!! If the widgets dont appear in the tab, check setLayout(layout)
    connect (mySerialPortControl_p ,SIGNAL(portsListChanged()) ,this ,SLOT(listPorts()));
    connect (comboEnumerator    ,SIGNAL(activated(int)) ,this ,SLOT(sendTo_portNameSetter(int)));
    connect (comboOpen          ,SIGNAL(activated(int)) ,this ,SLOT(sendTo_openModeSetter(int)));
    connect (comboQuery         ,SIGNAL(activated(int)) ,this ,SLOT(sendTo_queryModeSetter(int)));
    connect (comboBaud          ,SIGNAL(activated(int)) ,this ,SLOT(sendTo_baudRateSetter(int)));
    connect (comboData          ,SIGNAL(activated(int)) ,this ,SLOT(sendTo_dataBitsSetter(int)));
    connect (comboParity        ,SIGNAL(activated(int)) ,this ,SLOT(sendTo_parityBitsSetter(int)));
    connect (comboStop          ,SIGNAL(activated(int)) ,this ,SLOT(sendTo_stopBitsSetter(int)));
    connect (comboFlow          ,SIGNAL(activated(int)) ,this ,SLOT(sendTo_flowControlSetter(int)));
    connect (btRefreshPorts     ,SIGNAL(released())     ,this ,SLOT(handleBtRefreshPorts()));
    connect (btConnectPort      ,SIGNAL(released())     ,this ,SLOT(handleBtConnectPort()));
}

void ConfigSPTab::listPorts (void)
{
    int current_index=0;
    char arduino_serialPort_name[] = "usbserial";
    QString arduinoMega_SP_name = "usbmodem";
    comboEnumerator->clear ();
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    foreach (QextPortInfo port, ports)
    {
        if (!port.portName.isEmpty())
            comboEnumerator->addItem(port.portName);
        //strstr() returns NULL if the second string isn't found in the first one
        if (port.portName.contains(arduino_serialPort_name) && !mySerialPortControl_p->isOpen())
        {
            comboEnumerator->setCurrentIndex(current_index);
            this->sendTo_portNameSetter(current_index);
        }
        if (port.portName.contains(arduinoMega_SP_name) && !mySerialPortControl_p->isOpen())
        {
            comboEnumerator->setCurrentIndex(current_index);
            this->sendTo_portNameSetter(current_index);
        }
        current_index++;
    }
}
void ConfigSPTab::fillComboOpen (void)
{
    comboOpen->addItem(tr("Read"));
    comboOpen->addItem(tr("Write"));
    comboOpen->addItem(tr("Read & Write"));
    comboOpen->setCurrentIndex(2);

    this->sendTo_openModeSetter(comboOpen->currentIndex());
}
void ConfigSPTab::fillComboQuery (void)
{
    comboQuery->addItem(tr("Event driven"));
    comboQuery->setCurrentIndex(0);

    this->sendTo_queryModeSetter(comboQuery->currentIndex());
}
void ConfigSPTab::fillComboBaud (void)
{
    comboBaud->addItem("110");
    comboBaud->addItem("300");
    comboBaud->addItem("600");
    comboBaud->addItem("1200");
    comboBaud->addItem("2400");
    comboBaud->addItem("4800");
    comboBaud->addItem("9600");
    comboBaud->addItem("19200");
    comboBaud->addItem("38400");
    comboBaud->addItem("57600");
    comboBaud->addItem("115200");
    comboBaud->setCurrentIndex (6);

    this->sendTo_baudRateSetter(comboBaud->currentIndex());
}
void ConfigSPTab::fillComboData (void)
{
    comboData->addItem("5");
    comboData->addItem("6");
    comboData->addItem("7");
    comboData->addItem("8");
    comboData->setCurrentIndex (3);

    this->sendTo_dataBitsSetter(comboData->currentIndex());
}
void ConfigSPTab::fillComboParity (void)
{
    comboParity->addItem("Space");
    comboParity->addItem("None");
    comboParity->addItem("Even");
    comboParity->addItem("Odd");
    comboParity->setCurrentIndex (1);

    this->sendTo_parityBitsSetter(comboParity->currentIndex());
}
void ConfigSPTab::fillComboStop (void)
{
    comboStop->addItem("1");
    comboStop->addItem("2");
    comboStop->setCurrentIndex(0);

    this->sendTo_stopBitsSetter(comboStop->currentIndex());
}
void ConfigSPTab::fillComboFlow (void)
{
    comboFlow->addItem("OFF");
    comboFlow->addItem("Hardware");
    comboFlow->setCurrentIndex(0);

    this->sendTo_flowControlSetter(comboFlow->currentIndex());
}

void ConfigSPTab::sendTo_portNameSetter (int idx)
{
    QString name_of_the_port = comboEnumerator->QComboBox::itemText (idx);
    mySerialPortControl_p->set_portName(name_of_the_port);
}
void ConfigSPTab::sendTo_openModeSetter (int idx)
{
    QextSerialPort::OpenMode Open_Mode;
    switch (idx)
    {
    case 0: Open_Mode = QIODevice::ReadOnly; break;
    case 1: Open_Mode = QIODevice::WriteOnly; break;
    case 2: Open_Mode = QIODevice::ReadWrite; break;
    default: Open_Mode = QIODevice::ReadOnly; break;
    }
    mySerialPortControl_p->set_openMode(Open_Mode);
}
void ConfigSPTab::sendTo_queryModeSetter (int idx)
{
    switch (idx)
    {
    case 0:  mySerialPortControl_p->set_queryMode(QextSerialPort::EventDriven); break;
    default: mySerialPortControl_p->set_queryMode(QextSerialPort::EventDriven); break;
    }
}
void ConfigSPTab::sendTo_baudRateSetter (int idx)
{
    BaudRateType Baud_Rate;
    switch (idx)
    {
    case 0: Baud_Rate = BAUD110; break;
    case 1: Baud_Rate = BAUD300; break;
    case 2: Baud_Rate = BAUD600; break;
    case 3: Baud_Rate = BAUD1200; break;
    case 4: Baud_Rate = BAUD2400; break;
    case 5: Baud_Rate = BAUD4800; break;
    case 6: Baud_Rate = BAUD9600; break;
    case 7: Baud_Rate = BAUD19200; break;
    case 8: Baud_Rate = BAUD38400; break;
    case 9: Baud_Rate = BAUD57600; break;
    case 10: Baud_Rate = BAUD115200; break;
    default: Baud_Rate = BAUD9600; break;
    }
    mySerialPortControl_p->set_baudRate(Baud_Rate);
}
void ConfigSPTab::sendTo_dataBitsSetter (int idx)
{
    DataBitsType Data_Bits;
    switch (idx)
    {
    case 0: Data_Bits = DATA_5; break;
    case 1: Data_Bits = DATA_6; break;
    case 2: Data_Bits = DATA_7; break;
    case 3: Data_Bits = DATA_8; break;
    default: Data_Bits = DATA_8; break;
    }
    mySerialPortControl_p->set_dataBits(Data_Bits);
}
void ConfigSPTab::sendTo_parityBitsSetter (int idx)
{
    ParityType Parity_Bits;
    switch (idx)
    {
    case 0: Parity_Bits = PAR_SPACE; break;
    case 1: Parity_Bits = PAR_NONE; break;
    case 2: Parity_Bits = PAR_EVEN; break;
    case 3: Parity_Bits = PAR_ODD; break;
    default: Parity_Bits = PAR_NONE; break;
    }
    mySerialPortControl_p->set_parityBits(Parity_Bits);
}
void ConfigSPTab::sendTo_stopBitsSetter (int idx)
{
    StopBitsType Stop_Bits;
    switch (idx)
    {
    case 0: Stop_Bits = STOP_1; break;
    case 1: Stop_Bits = STOP_2; break;
    default: Stop_Bits = STOP_1; break;
    }

    mySerialPortControl_p->set_stopBits(Stop_Bits);
}
void ConfigSPTab::sendTo_flowControlSetter (int idx)
{
    FlowType Flow_Type;
    switch (idx)
    {
    case 0: Flow_Type = FLOW_OFF; break;
    case 1: Flow_Type = FLOW_HARDWARE; break;
    default: Flow_Type = FLOW_OFF; break;
    }
    mySerialPortControl_p->set_flowControl(Flow_Type);
}

void ConfigSPTab::handleBtRefreshPorts (void)
{
    this->listPorts();
}

bool ConfigSPTab::handleBtConnectPort (void)
{
    btConnectPort->setEnabled (false);
    QMessageBox *unableToConnectMessage = new QMessageBox;
    if (mySerialPortControl_p->isOpen() == false)
    {
        if(mySerialPortControl_p->openPort() == true)
        {
            qDebug ("Port open");
            btConnectPort->setText ("Disconnect");
        }
        else
        {
            qDebug ("Failed to open the port");
            unableToConnectMessage->setText (tr("Unable to open the connection to the Serial Port"));
            unableToConnectMessage->setInformativeText
                    (tr("Try a different port or check your connections and configuration"));
            unableToConnectMessage->setIcon (QMessageBox::Warning);
            unableToConnectMessage->exec();
        }
    }
    else
    {
        mySerialPortControl_p->closePort();

        btConnectPort->setText ("Connect");
    }
    mySerialPortControl_p->cleanBuffer();
    btConnectPort->setEnabled (true);
    return (mySerialPortControl_p->isOpen());
}
