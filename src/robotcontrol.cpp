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

#include "robotcontrol.h"

RobotControl::RobotControl(QObject *parent) :
    QObject(parent)
{
    qDebug("RobotControl: In the constructor");
    manualEnabled = 0;
    this->connected = 0;
    this->createWidgets();

    lstMovementNames << "RL" << "RR" << "N" << "S" << "E"
                     << "W" << "NE" << "NW" << "SE" << "SW";

    /*for (int i=0; i < lstMovementNames.size(); i++)
    {
        //this->setMovAverage(lstMovementNames[i], 100, 15);
        this->setMovAverage(lstMovementNames[i], 1, 1);
    }*/
    this->setMovAverage(lstMovementNames[0], 117, 10);
    this->setMovAverage(lstMovementNames[1], 89, 10);

    this->setMovAverage(lstMovementNames[2], 100, 14);
    this->setMovAverage(lstMovementNames[3], 100, 14);
    this->setMovAverage(lstMovementNames[4], 97, 14);
    this->setMovAverage(lstMovementNames[5], 96, 14);

    this->setMovAverage(lstMovementNames[6], 107, 14);
    this->setMovAverage(lstMovementNames[7], 105, 14);
    this->setMovAverage(lstMovementNames[8], 107, 14);
    this->setMovAverage(lstMovementNames[9], 106, 14);
}

RobotControl::~RobotControl (void)
{
    delete (networkManager);
    delete (image);
    delete (timVideoRefresh);
    delete (timMovementDelay);
    delete (lstEncodedCmds);
}

void RobotControl::createWidgets (void)
{
    networkManager  = new QNetworkAccessManager;
    image           = new QImage;
    timVideoRefresh = new QTimer;
    timMovementDelay= new QTimer;
    timStatus      = new QTimer;
    lstEncodedCmds  = new QStringList;

    followingPath = false;
    list_counter=0;
    timMovementDelay->setSingleShot(true);
    movementsNum=0;
    movementsCounter=0;
    lightStatus=0;
    timStatus      ->setSingleShot(false);
    timStatus      ->setInterval (STATUS_TIMER); //Check the status (battery, wifi, etc) every STATUS_TIMER msec

    this->set_url("http://192.168.1.20");   //Default address

    qDebug() << "About to load the default image";
    if(image->load(":/img/defaultImg.jpg"))
        qDebug() << "The image has been loaded successfully";
    else
        qDebug() << "Could not load the default image";

    connect (networkManager     ,SIGNAL(finished(QNetworkReply*))   ,this ,SLOT(replyFinished(QNetworkReply*)));
    connect (networkManager     ,SIGNAL(authenticationRequired (QNetworkReply*, QAuthenticator*))
                                    ,this ,SLOT(provideAuthentication (QNetworkReply*, QAuthenticator*)));
    connect (timVideoRefresh    ,SIGNAL(timeout())                  ,this ,SLOT(request_image()));
    connect (timMovementDelay   ,SIGNAL(timeout())                  ,this ,SLOT(timMovementDelayHandler()));
    connect (timStatus          ,SIGNAL(timeout())                  ,this ,SLOT(pollStatus()));
    connect (this               ,SIGNAL(movement_finished())        ,this ,SLOT(control_strlist()));
    connect (this               ,SIGNAL(straightSignal())           ,this ,SLOT(straight_move(void)));
    connect (this               ,SIGNAL(rotateSignal())             ,this ,SLOT(rotatory_move()));
}

/**************************************__<REPLIES HANDLER>__*************************************/
void RobotControl::replyFinished (QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) //Error handling done here instead of the SLOT
    {                                             //  because the SLOT errorHandler does not receive the QNetworkReply
        qDebug() << "RobotControl::replyFinished errored out:" << reply->errorString();
        return;
    }

    if (reply->property("type").toString() == "image")
        this->received_image(reply);
    else if (reply->property("type").toString() == "status")
    {
        this->readStatus(reply);
    }
    else if (reply->property("type").toString() == "movement")
    {
        //timMovementDelay->start();
        //emit movement_finished();
    }
    else
        qDebug() << "Unrecognized property 'type' in the reply:" << reply->property("type").toString();

    reply->deleteLater();
}
/**************************************__</REPLIES HANDLER>__************************************/

void RobotControl::readStatus (QNetworkReply *reply)
{
    QString status(reply->readAll());
    QString status_mod = status;
    bool okWifi, okBat, okCharging, okHeadPos;
    int bat;
    int wifi;
    int charging;
    int headPos;

    status_mod.remove(0, status_mod.indexOf("wifi_ss=")+QString("wifi_ss=").length());
    status_mod.truncate(status_mod.indexOf("|"));
    wifi = status_mod.toInt(&okWifi, 10);

    status_mod = status;
    status_mod.remove(0, status_mod.indexOf("battery=")+QString("battery=").length());
    status_mod.truncate(status_mod.indexOf("|"));
    bat = status_mod.toInt(&okBat, 10);

    status_mod = status;
    status_mod.remove(0, status_mod.indexOf("charging=")+QString("charging=").length());
    status_mod.truncate(status_mod.indexOf("|"));
    charging = status_mod.toInt(&okCharging, 10);

    status_mod = status;
    status_mod.remove(0, status_mod.indexOf("head_position=")+QString("head_position=").length());
    status_mod.truncate(status_mod.indexOf("|"));
    headPos = status_mod.toInt(&okBat, 10);

    if (okWifi == true)
    {
        // 254 ------>100  |
        // wifi------> x   |>  x= (wifi * 100) / 254
        this->wifiStrength = (wifi*100)/254;
    }
    else
        qDebug() << "The WiFi strength level received is not valid: " << status_mod;

    if (okBat == true)
    {
        bat -= 100; //There's a 100 offset, according to the API
        // 27 ------>100  |
        // bat------> x   |>  x = (bat*100)/27
        this->batteryLevel = (bat*100)/27;
    }
    else
        qDebug() << "The battery level received is not valid: " << status_mod;

    if ((okCharging == true) && (charging >= 0) && (charging <= 80))
    {
        // 80 == charging, [0-79] == not charging, according to the API
        if (charging == 80)
            this->chargingStatus = true;
        else
            this->chargingStatus = false;
    }
    else
        qDebug() << "The battery level received is not valid: " << status_mod;

    if (okHeadPos == true)
    {
        if (headPos >= 180)
            this->headPosition = "BOT";
        else if ((headPos < 180) && (headPos >= 100))
            this->headPosition = "MID";
        else if (headPos < 100)
            this->headPosition = "TOP";
        else
            qDebug() << "The head position received is not valid: " << headPos;
    }

    if (okWifi && okBat && okCharging)
        emit statusRefreshed();
}

/**************************************__<VIDEO CONTROL>__***************************************/
void RobotControl::request_image (void)
{
    QString url_to_send = QString(robot_url.toString() + "/Jpeg/CamImg" + QString::number(imgNumber) + ".jpg");
    this->image_url.setUrl(url_to_send);
    imgNumber++;
    if (imgNumber >= 1000)    // Rovio's API documentation recommends to use only 4 digits in the number.
        imgNumber=0;
    QNetworkRequest img_request;
    img_request.setUrl (image_url);
    img_request.setPriority(QNetworkRequest::LowPriority);
    //img_request.setOriginatingObject(img_qobj);
    QNetworkReply *image_reply = networkManager->get (img_request);
    image_reply->setProperty("type", "image");

    connect (image_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
             SLOT(errorHandler(QNetworkReply::NetworkError)));
}

void RobotControl::received_image (QNetworkReply *reply)
{
    if (!this->timVideoRefresh->isActive())
        return; //The user might stop the video to see sth... can't let it continue a single frame.
    int new_imageId;
    QString new_imageId_str;
    int pos=0;
    pos = reply->url().toString().indexOf(QRegExp("[0-9]{1,4}.jpg"));       //find the image number
    new_imageId_str = reply->url().toString().mid(pos,4);                   //get a 4 chars string with the number
    new_imageId_str = new_imageId_str.left( new_imageId_str.indexOf('.') ); //get rid of the additional chars
    new_imageId = new_imageId_str.toInt();                                  //convert to int
    total_frames++; //testeo. estadistica.

    if ((total_frames % 100) == 0) //testeo. estadistica.
    {
        qDebug() << "   Total Frames ==" << total_frames << "|| Frames dropped within the last hundred:" << dropped_frames;
        dropped_frames=0;
    }

    if (new_imageId == 999)
        last_imgNumber = 0;
    if ((new_imageId >= last_imgNumber) && (new_imageId < last_imgNumber+50))  //Give it a margin to avoid huge voids
    {
        if (new_imageId >= last_imgNumber+100)
            this->startVideoFeed();
        if (new_imageId >= last_imgNumber+50)
            return;
        last_imgNumber = new_imageId;
        if (last_imgNumber == 999)
            last_imgNumber = 0;
    }
    else
    {
        dropped_frames++;
        if (dropped_frames == 100)
        {
            qDebug() << "100% dropped frame in the last 100 frames. Restarting the video feed...";
            this->startVideoFeed();
        }
        /*qDebug() << "Frame discarded because it was not in order. Frames dropped:" << dropped_frames
                 << "--ID:" << new_imageId << "--LastID:" << last_imgNumber << "--Received num:" << total_frames;*/
        return;
    }

    QByteArray image_data = reply->readAll();
    if(!this->image->loadFromData(image_data))
        qDebug() << "RobotControl: The image has not been loaded successfully from the QByteArray";
    else
        emit image_downloaded();
}
/**************************************__</VIDEO CONTROL>__**************************************/

/***************************************__<AUTHENTICATION>__**************************************/
void RobotControl::set_user (QString usr)
{
    authUser = usr;
}

void RobotControl::set_pass (QString pwd)
{
    authPass = pwd;
}

void RobotControl::provideAuthentication (QNetworkReply *rep, QAuthenticator *auth)
{
    qDebug() << "Providing authentication";
    auth->setUser (authUser);
    auth->setPassword (authPass);
}

/**************************************__</AUTHENTICATION>__**************************************/

void RobotControl::errorHandler (QNetworkReply::NetworkError errorCode)
{
    errorCode = QNetworkReply::NoError;
    //errors are handled in the replyFinished method for the sake of simplicity
}

void RobotControl::set_url (QString robotAddress)
{
    qDebug() << "RobotControl: Setting the required URLs based on the root:" << robotAddress;
    this->robot_url.setUrl(robotAddress);
    //this->image_url.setUrl(robot_url.toString() + "/Jpeg/CamImg.jpg");
}

QString RobotControl::get_url (void)
{
    return(this->robot_url.toString());
}
QImage RobotControl::get_image (void)
{
    return(*image);
}

void RobotControl::set_videoFreq (int freq)
{
    this->videoRefreshFreq = freq;
}

void RobotControl::startVideoFeed (void)
{
    qDebug() << "Starting the video feed...";
    imgNumber      = 0;
    last_imgNumber = 0;
    dropped_frames = 0;
    total_frames   = 0;//test. statistics.

    this->timVideoRefresh->setInterval(1000 / videoRefreshFreq); // (1k ms / freq fps)
    this->timVideoRefresh->start();
}

void RobotControl::stopVideoFeed (void)
{
    qDebug() << "Shutting the video feed...";
    timVideoRefresh->stop();
}

bool RobotControl::openConnection (void)
{
    //this->startVideoFeed();
    headPosition = "not_available"; //not yet.
    this->pollStatus();
    timStatus->start();
    this->connected = 1;
    emit justConnected();
    return (true);
}

void RobotControl::closeConnection (void)
{
    timStatus->stop();
    if (this->isVideoON())
        this->stopVideoFeed();
    this->connected = 0;
    emit justDisconnected();
}

bool RobotControl::isConnected (void)
{
    return (this->connected);
}

bool RobotControl::isVideoON (void)
{
    return (timVideoRefresh->isActive());
}

bool RobotControl::isFollowingPath (void)
{
    return (this->followingPath);
}

void RobotControl::set_followPath (bool opt)
{
    this->followingPath = opt;
}


                         /************************\
                        |  Robot physical control  |
\*-------------------------------------------------------------------------*/
void RobotControl::request_movement (QString command)
{
    QString url_to_send = QString(robot_url.toString() + command);
    //qDebug() << "Sending the following URL:" << url_to_send; // For debugging purposes
    this->movement_url.setUrl(url_to_send);
    QNetworkRequest move_request;
    move_request.setUrl (movement_url);
    move_request.setPriority (QNetworkRequest::HighPriority);
    //img_request.setOriginatingObject(img_qobj);
    QNetworkReply *move_reply = networkManager->get (move_request);
    move_reply->setProperty("type", "movement");

    connect (move_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
             SLOT(errorHandler(QNetworkReply::NetworkError)));
}

        /********_<Translate order to URLs>_********/
void RobotControl::transToUrl (QString encodedCmd)
{
    movementsCounter = 0;
    int currentDist = 0;
    encodedCmd = encodedCmd.toUpper();  //transform the QString to Upper case
    //Aquí habría que validar que es una string válida con una expresión regular.
    //si no lo es: return;
    if (encodedCmd.at(0) == 'H') //If head position: "HTOP" || "HMID" || "HBOT"
    {
        if (encodedCmd.right(3) == "TOP")
            this->set_headPosition(2);
        if (encodedCmd.right(3) == "MID")
            this->set_headPosition(1);
        if (encodedCmd.right(3) == "BOT")
            this->set_headPosition(0);
    }
    else if (encodedCmd.at(0) == 'L') //If light status: "L1" || "L0"
    {
        if (encodedCmd.right(1).toInt() == 1)
            this->set_lightStatus(1);
        if (encodedCmd.right(1).toInt() == 0)
            this->set_lightStatus(0);
    }
    else if (encodedCmd.at(0) == 'R')    //If rotatory: "RLXXX" || "RRXXX"
    {
        currentMovement = QString("ROT");
        if (encodedCmd.at(1) == QChar('L'))
        {
            movementDirection = QString("L");
            currentDist = encodedCmd.right(encodedCmd.length()-2).toInt();
            floatMovementsNum = (currentDist * RLavgMovNum) / RLavgDist;
        }
        if (encodedCmd.at(1) == 'R')
        {
            movementDirection = "R";
            currentDist = encodedCmd.right(encodedCmd.length()-2).toInt();
            floatMovementsNum = (currentDist * RRavgMovNum) / RRavgDist;
        }
        if((floatMovementsNum < 1) && (floatMovementsNum >= 0))
            movementsNum = 1;
        else
            movementsNum = floatMovementsNum;
        this->rotatory_move ();
    }
    else if (encodedCmd.at(0) == 'N')
    {
        currentMovement = "STR";
        if (encodedCmd.at(1) == 'E')
        {
            movementDirection = "NE";
            currentDist = encodedCmd.right(encodedCmd.length()-2).toInt();
            floatMovementsNum = (currentDist * NEavgMovNum) / NEavgDist;
        }
        else if (encodedCmd.at(1) == 'W')
        {
            movementDirection = "NW";
            currentDist = encodedCmd.right(encodedCmd.length()-2).toInt();
            floatMovementsNum = (currentDist * NWavgMovNum) / NWavgDist;
        }
        else
        {
            movementDirection = "N";
            currentDist = encodedCmd.right(encodedCmd.length()-1).toInt();
            floatMovementsNum = (currentDist * NavgMovNum) / NavgDist;
        }
        if((floatMovementsNum < 1) && (floatMovementsNum >= 0))
            movementsNum = 1;
        else
            movementsNum = floatMovementsNum;
        this->straight_move();
    }
    else if (encodedCmd.at(0) == 'S')
    {
        currentMovement = "STR";
        if (encodedCmd.at(1) == 'E')
        {
            movementDirection = "SE";
            currentDist = encodedCmd.right(encodedCmd.length()-2).toInt();
            floatMovementsNum = (currentDist * SEavgMovNum) / SEavgDist;
        }
        else if (encodedCmd.at(1) == 'W')
        {
            movementDirection = "SW";
            currentDist = encodedCmd.right(encodedCmd.length()-2).toInt();
            floatMovementsNum = (currentDist * SWavgMovNum) / SWavgDist;
        }
        else
        {
            movementDirection = "S";
            currentDist = encodedCmd.right(encodedCmd.length()-1).toInt();
            floatMovementsNum = (currentDist * SavgMovNum) / SavgDist;
        }
        if((floatMovementsNum < 1) && (floatMovementsNum >= 0))
            movementsNum = 1;
        else
            movementsNum = floatMovementsNum;
        this->straight_move();
    }
    else if (encodedCmd.at(0) == 'E')
    {
        currentMovement = "STR";
        movementDirection = "E";
        currentDist = encodedCmd.right(encodedCmd.length()-1).toInt();
        floatMovementsNum = (currentDist * EavgMovNum) / EavgDist;
        if((floatMovementsNum < 1) && (floatMovementsNum >= 0))
            movementsNum = 1;
        else
            movementsNum = floatMovementsNum;
        this->straight_move();
    }
    else if (encodedCmd.at(0) == 'W')
    {
        currentMovement = "STR";
        movementDirection = "W";
        currentDist = encodedCmd.right(encodedCmd.length()-1).toInt();
        floatMovementsNum = (currentDist * WavgMovNum) / WavgDist;
        if((floatMovementsNum < 1) && (floatMovementsNum >= 0))
            movementsNum = 1;
        else
            movementsNum = floatMovementsNum;
        this->straight_move();
    }
    else
    {
        qDebug() << "ERROR: Command not recognized. Skipping it...";
        list_counter++;
        emit movement_finished();
    }
}

void RobotControl::setMovAverage (QString dir, int dist, int movsNum)
{
    if (dir == "RL")
    {
        RLavgDist = dist; RLavgMovNum = movsNum;
    }
    else if (dir == "RR")
    {
        RRavgDist = dist; RRavgMovNum = movsNum;
    }
    else if (dir == "N")
    {
        NavgDist = dist; NavgMovNum = movsNum;
    }
    else if (dir == "S")
    {
        SavgDist = dist; SavgMovNum = movsNum;
    }
    else if (dir == "E")
    {
        EavgDist = dist; EavgMovNum = movsNum;
    }
    else if (dir == "W")
    {
        WavgDist = dist; WavgMovNum = movsNum;
    }
    else if (dir == "NE")
    {
        NEavgDist = dist; NEavgMovNum = movsNum;
    }
    else if (dir == "NW")
    {
        NWavgDist = dist; NWavgMovNum = movsNum;
    }
    else if (dir == "SE")
    {
        SEavgDist = dist; SEavgMovNum = movsNum;
    }
    else if (dir == "SW")
    {
        SWavgDist = dist; SWavgMovNum = movsNum;
    }
    else
        qDebug() << "Direction not recognized in RobotControl::setMovAverage";
}

        /********_</Translate order to URLs>_********/

void RobotControl::straight_move (void)
{
    QString str;
    if (movementsCounter < movementsNum)
    {
        if (movementDirection == "N")
            str = "/rev.cgi?Cmd=nav&action=18&drive=1&speed=1";
        else if (movementDirection == "S")
            str = "/rev.cgi?Cmd=nav&action=18&drive=2&speed=1";
        else if (movementDirection == "E")
            str = "/rev.cgi?Cmd=nav&action=18&drive=4&speed=1";
        else if (movementDirection == "W")
            str = "/rev.cgi?Cmd=nav&action=18&drive=3&speed=1";
        else if (movementDirection == "NE")
            str = "/rev.cgi?Cmd=nav&action=18&drive=8&speed=1";
        else if (movementDirection == "NW")
            str = "/rev.cgi?Cmd=nav&action=18&drive=7&speed=1";
        else if (movementDirection == "SE")
            str = "/rev.cgi?Cmd=nav&action=18&drive=10&speed=1";
        else if (movementDirection == "SW")
            str = "/rev.cgi?Cmd=nav&action=18&drive=9&speed=1";
        else
            qDebug() << "Direction not recognized in RobotControl::straight_move";

        this->request_movement (str);
        if (followingPath)
            timMovementDelay->start(STR_DELAY);
        else
            timMovementDelay->stop();
            //timMovementDelay->start(MANUAL_DELAY);
    }
    else
    {
        currentMovement.clear();
        list_counter++;
        emit movement_finished();
        return;
    }
    movementsCounter++;
}

void RobotControl::rotatory_move (void)
{
    QString str;
    if (movementsCounter < movementsNum)
    {
        if (movementDirection == "R")
            str = "/rev.cgi?Cmd=nav&action=18&drive=6&speed=7";
        else if (movementDirection == "L")
            str = "/rev.cgi?Cmd=nav&action=18&drive=5&speed=7";
        else
            qDebug() << "Direction not recognized for a rotatory movement";
        this->request_movement (str);
        if (followingPath)
            timMovementDelay->start(ROT_DELAY);
        else
            timMovementDelay->stop();
           // timMovementDelay->start(MANUAL_DELAY);
    }
    else
    {
        currentMovement.clear();
        list_counter++;
        emit movement_finished();
        return;
    }
    movementsCounter++;
    //this->control_strlist();
}

void RobotControl::set_headPosition (int position)
{
    QString str;
    if (position == 0)
    {
        str = "/rev.cgi?Cmd=nav&action=18&drive=12";
        this->headPosition = "BOT";
    }
    if (position == 1)
    {
        str = "/rev.cgi?Cmd=nav&action=18&drive=13";
        this->headPosition = "MID";
    }
    if (position == 2)
    {
        str = "/rev.cgi?Cmd=nav&action=18&drive=11";
        this->headPosition = "TOP";
    }
    this->request_movement(str);
    list_counter++;
    emit movement_finished();
    //Position 0 is down, position 1 is middle, position 2 is top
}

void RobotControl::set_lightStatus (bool status)
{
    QString str;
    if (status == 1)
        str = "/rev.cgi?Cmd=nav&action=19&LIGHT=1";
    else
        str = "/rev.cgi?Cmd=nav&action=19&LIGHT=0";
    lightStatus = status;
    this->request_movement(str);
    list_counter++;
    emit movement_finished();
    //1 is ON, 0 is OFF.
}

void RobotControl::control_strlist (void)
{
    if (followingPath == false)
        return;
    qDebug() << "followPath is enabled. list_counter =" << QString::number(list_counter)
             << "||| and list.size =" << QString::number(lstEncodedCmds->size());
    if (list_counter < lstEncodedCmds->size())
    {
        this->transToUrl(lstEncodedCmds->at(list_counter));
    }
    else
    {
        qDebug("Movements list finished");
        this->followingPath = false;
        list_counter=0;
        return;
    }
}

void RobotControl::timMovementDelayHandler (void)
{
    if (currentMovement.isEmpty())
        return;
    else if (currentMovement == "STR")
        emit straightSignal();
    else if (currentMovement == "ROT")
        emit rotateSignal();
}


                        /*****************\
\****************************__<API>__****************************/
bool RobotControl::appendToPath (QString cmd)
{
    qDebug() << "Appending cmd to the path:" << cmd;
    lstEncodedCmds->append(cmd);
    return(true);
}

bool RobotControl::executeNowAndContinuePath (QString cmd)
{
    timMovementDelay->stop();
    this->followingPath = true;
    QString left_str;
    QString new_str;
    int left_movements = 0;
    if (movementsNum != 0)
    {
        left_movements = movementsNum - (movementsCounter);
        left_str = movementDirection + QString::number(left_movements);
        qDebug() << "*******Command left to finish the actual movement:" << left_str;
        lstEncodedCmds->insert (list_counter+1, cmd);
        if (left_movements > 0)
            lstEncodedCmds->insert (list_counter+2, left_str);
        list_counter++;
        emit movement_finished();
        return(true);
    }
    else
    {
        qDebug() << "*******No commands in the path.";
        this->executeNowAndStopPath(cmd);
    }
    return(1);
}

bool RobotControl::executeNowAndStopPath (QString cmd)
{
    this->followingPath = false;
    this->clearCmdsList();
    movementsCounter=0;
    movementsNum=1;
    this->transToUrl(cmd);

    return(true);
}

void RobotControl::executePath (void)
{
    this->followingPath = true;
    list_counter=0;
    this->control_strlist();
}

void RobotControl::clearCmdsList (void)
{
    this->lstEncodedCmds->clear();
    list_counter=0;
}

bool RobotControl::get_lightStatus (void)
{
    return (this->lightStatus);
}

int RobotControl::get_battery (void)
{
    return (this->batteryLevel);
}

int RobotControl::get_wifiStrength (void)
{
    return (this->wifiStrength);
}

bool RobotControl::get_chargingStatus (void)
{
    return (this->chargingStatus);
}

void RobotControl::pollStatus (void)
{
    QString url_to_send = QString(robot_url.toString() + "/rev.cgi?Cmd=nav&action=1");
    this->status_url.setUrl(url_to_send);
    QNetworkRequest status_request;
    status_request.setUrl (status_url);
    status_request.setPriority (QNetworkRequest::LowPriority);
    QNetworkReply *status_reply = networkManager->get (status_request);
    status_reply->setProperty("type", "status");

    connect (status_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
             SLOT(errorHandler(QNetworkReply::NetworkError)));
}

QString RobotControl::get_headPosition (void)
{
    return (this->headPosition);
}
