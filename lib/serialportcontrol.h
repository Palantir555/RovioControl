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

#ifndef SERIALPORTCONTROL_H
#define SERIALPORTCONTROL_H

#include <QObject>
#include <QIODevice>
#include <stdio.h>
#include <QDebug>

#include <qextserialenumerator.h>
#include "qextserialport.h"

#define BUFFER_SIZE 350

class SerialPortControl : public QObject
{
    Q_OBJECT

private:
    QextSerialPort *serialPort;
    QextSerialEnumerator *serialEnum;
    QString         port_name;
    BaudRateType    baud_rate;
    DataBitsType    data_bits;
    ParityType      parity_bits;
    StopBitsType    stop_bits;
    FlowType        flow_control;
    QIODevice::OpenMode open_mode;
    QextSerialPort::QueryMode query_mode;

    unsigned int STR_LENGTH;
    int lastH_value;
    int lastV_value;
    int lastR_value;
    int H_value; //horizontal
    int V_value; //Vertical
    int R_value; //rotation value
    int C_value; //Bottom push button
    int Z_value; //Top push button
    int CandZ_value; //Both buttons at the same time
    void processBuffer (void);

public:
    explicit SerialPortControl(QObject *parent = 0);
    ~SerialPortControl(void);
    //Setters:
    void set_portName   (QString portName);
    void set_baudRate   (BaudRateType baudRate);
    void set_dataBits   (DataBitsType dataBits);
    void set_parityBits (ParityType parityBits);
    void set_stopBits   (StopBitsType stopBits);
    void set_flowControl(FlowType flowControl);
    void set_openMode   (QIODevice::OpenMode openMode);
    void set_queryMode  (QextSerialPort::QueryMode queryMode);

    //Getters:
    bool isOpen (void);
    int get_HValue   (void);
    int get_VValue   (void);
    int get_RValue   (void);
    bool get_CValue  (void);
    bool get_ZValue  (void);
    bool get_CandZValue (void);

    //Orders:
    void portInit   (void);
    bool openPort   (void);
    void closePort  (void);
    void cleanBuffer(void);

    //Buffer:
    char buffer[BUFFER_SIZE];
    //Public and accessed directly... Hmm....

signals:
    void stringProcessed (void);
    void bufferModified (void);
    void portOpen (void);
    void portClosed (void);
    void portsListChanged (void);
    //void controlSystemOverride (void);

public slots:
    void onDataAvailable (void);
    void devicesListModified (void);
};

#endif // SERIALPORTCONTROL_H
