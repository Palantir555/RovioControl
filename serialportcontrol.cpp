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

#include "serialportcontrol.h"

SerialPortControl::SerialPortControl(QObject *parent) :
    QObject(parent)
{
    serialPort = new QextSerialPort(QextSerialPort::EventDriven, this);
    serialEnum = new QextSerialEnumerator;
    serialEnum->setUpNotifications(); //Only works in OSX and Win. For other OSs use the refresh button.
    strncpy (buffer, "", sizeof(char));

    //STR_LENGTH = 12;
    STR_LENGTH = 23;

    H_value = 0;
    V_value = 0;
    R_value = 0;
    //Inicializamos a los valores por defecto:
    this->set_baudRate (BAUD9600);
    this->set_dataBits (DATA_8);
    this->set_parityBits (PAR_NONE);
    this->set_stopBits (STOP_1);
    this->set_flowControl (FLOW_OFF);
    this->set_openMode (QIODevice::ReadWrite);

    connect (serialPort, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
    connect (serialEnum, SIGNAL(deviceDiscovered(QextPortInfo)) ,this ,SLOT(devicesListModified()));
    connect (serialEnum, SIGNAL(deviceRemoved(QextPortInfo))    ,this ,SLOT(devicesListModified()));
}

SerialPortControl::~SerialPortControl()
{
    delete serialPort;
}

void SerialPortControl::devicesListModified (void)
{
    qDebug("SerialPortContrl: devices list modified");
    emit portsListChanged();
}

void SerialPortControl::set_portName    (QString portName)
{
    this->port_name = portName;
}
void SerialPortControl::set_baudRate    (BaudRateType baudRate)
{
    this->baud_rate = baudRate;
}
void SerialPortControl::set_dataBits    (DataBitsType dataBits)
{
    this->data_bits = dataBits;
}
void SerialPortControl::set_parityBits  (ParityType parityBits)
{
    this->parity_bits = parityBits;
}
void SerialPortControl::set_stopBits    (StopBitsType stopBits)
{
    this->stop_bits = stopBits;
}
void SerialPortControl::set_flowControl (FlowType flowControl)
{
    this->flow_control = flowControl;
}
void SerialPortControl::set_openMode    (QIODevice::OpenMode openMode)
{
    this->open_mode = openMode;
}
void SerialPortControl::set_queryMode   (QextSerialPort::QueryMode queryMode)
{
    this->query_mode = queryMode;
}

void SerialPortControl::cleanBuffer (void)
{
    strncpy (buffer, "", 1);
    emit bufferModified();
}
void SerialPortControl::closePort (void)
{
    if (serialPort->isOpen())
    {
        serialPort->close();
        this->cleanBuffer();
        emit portClosed();
    }
    else
        qDebug("The port is already closed");
}
bool SerialPortControl::openPort (void)
{
    this->portInit();
    bool result = 0;
    result = serialPort->open(open_mode);
    if (result)
        emit portOpen();
    return(result);
}
bool SerialPortControl::isOpen (void)
{
    return(serialPort->isOpen());
}

void SerialPortControl::portInit (void)
{
    qDebug("Initializating the port");
/*testing tabs
    portName = comboEnumerator->QComboBox::itemText (idx);
*/
    if(serialPort->isOpen())
        serialPort->close ();
    serialPort->setPortName   (this->port_name);
    serialPort->setQueryMode  (QextSerialPort::EventDriven);  //Ya lo puse en el constructor. No varía.
    serialPort->setBaudRate   (this->baud_rate);
    serialPort->setDataBits   (this->data_bits);
    serialPort->setParity     (this->parity_bits);
    serialPort->setStopBits   (this->stop_bits);
    serialPort->setFlowControl(this->flow_control);
}

void SerialPortControl::onDataAvailable (void)
{
    QByteArray input;
    if(serialPort->isOpen())
        input = serialPort->readAll();
    if((strlen(buffer)+strlen(input.data())) < BUFFER_SIZE)    //Protección contra BoF (suficiente?)
        strcat(buffer, input.data());   //varName.data() convierte QByteArray a char*
    else   //Si buffer lleno -> asumimos cadenas extrañas -> reseteamos buffer.
    {
        qDebug("Buffer emptied");
        this->cleanBuffer();
    }
    emit bufferModified();
    this->processBuffer ();
}

/*
void SerialPortControl::processBuffer (void)    //String format: "$H0001V0001#"   [0001 == %04d]
{
    unsigned int i=0;
    //char string_to_check[STR_LENGTH];
    for (i=0; i<strlen(buffer); i++)
    {
        if ((buffer[i] == '$') && (strlen(buffer+i) >= STR_LENGTH) && (buffer[i+STR_LENGTH-1] == '#'))
        {
            lastH_value = H_value;
            lastV_value = V_value;
            //strncpy(string_to_check, STR_LENGTH, sizeof(char));
            //if (checksum_check (string_to_check))
            sscanf ((buffer+i), "$H%04dV%04d#", &H_value, &V_value);
            emit stringProcessed();
            strncpy (buffer, (buffer+i), strlen(buffer+i)+1);
            i=0;
        }
    }
}
*/
void SerialPortControl::processBuffer (void)
{
    unsigned int i=0;
    //char string_to_check[STR_LENGTH];
    for (i=0; i<strlen(buffer); i++)
    {
        if ((buffer[i] == '$') && (strlen(buffer+i) >= STR_LENGTH) && (buffer[i+STR_LENGTH-1] == '#'))
        {
            lastH_value = H_value;
            lastV_value = V_value;
            lastR_value = R_value;
            //strncpy(string_to_check, STR_LENGTH, sizeof(char));
            //if (checksum_check (string_to_check))
            sscanf ((buffer+i), "$H%04dV%04dR%04dC%1dZ%1dB%1d#",
                    &H_value, &V_value, &R_value, &C_value, &Z_value, &CandZ_value);
            emit stringProcessed();
            strncpy (buffer, (buffer+i), strlen(buffer+i)+1);
            i=0;
        }
    }
}

int SerialPortControl::get_HValue (void)
{
    return (H_value);
}

int SerialPortControl::get_VValue (void)
{
    return (V_value);
}

int SerialPortControl::get_RValue (void)
{
    return (R_value);
}

bool SerialPortControl::get_CValue (void)
{
    return ((bool)C_value);
}

bool SerialPortControl::get_ZValue (void)
{
    return ((bool)Z_value);
}

bool SerialPortControl::get_CandZValue (void)
{
    return ((bool)CandZ_value);
}
