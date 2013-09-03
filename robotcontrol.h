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

#ifndef ROBOTCONTROL_H
#define ROBOTCONTROL_H

#include <QObject>
#include <QPixmap>
#include <QList>
#include <QtNetwork>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#define MANUAL_DELAY 20
#define STR_DELAY 200
#define ROT_DELAY 150
#define STATUS_TIMER 2000
class RobotControl : public QObject
{
    Q_OBJECT

public:
    RobotControl (QObject *parent = 0);
    ~RobotControl (void);

    QString get_url (void);
    QImage get_image (void);

    void set_url (QString robotAddress);  //To configure the required URLs

    void set_videoFreq (int freq);
    void startVideoFeed (void);
    void stopVideoFeed (void);

    bool openConnection (void);
    void closeConnection (void);

    bool isVideoON (void);
    bool isConnected (void);
    bool isFollowingPath (void);
    void set_followPath (bool opt);

    void clearCmdsList (void);

    void set_headPosition   (int position);
    void set_lightStatus    (bool status);
    void set_user           (QString usr);
    void set_pass           (QString pwd);
    bool get_lightStatus    (void);
    int  get_battery        (void);
    int  get_wifiStrength   (void);
    bool get_chargingStatus (void); //Ret if the robot is charging(1) or not(0)
    QString get_headPosition(void);

    bool appendToPath (QString cmd); //receives an encoded command
    void executePath (void);
    bool executeNowAndContinuePath (QString cmd);
    bool executeNowAndStopPath (QString cmd);
    bool manualEnabled;
    void setMovAverage (QString dir, int dist, int movsNum);

private:
    bool connected;
    bool followingPath;
    QString headPosition;
    bool lightStatus;
    void received_image (QNetworkReply *reply);
    void createWidgets  (void);
    void readStatus     (QNetworkReply *reply);


    QNetworkAccessManager *networkManager;
    //QAuthenticator *networkAuth;
    QImage *image;

    QString authUser;
    QString authPass;

    QUrl robot_url;
    QUrl image_url;
    QUrl movement_url;
    QUrl status_url;

    int videoRefreshFreq;
    QTimer *timVideoRefresh;
    QTimer *timMovementDelay;
    /******* <Movement Variables> *******/
    int movementsNum;       //Number of movements in the current command
                            //    for example: send "go forward" [3] times

    float floatMovementsNum;
    QList<QString> lstMovementNames;
    int RLavgDist; int RLavgMovNum;
    int RRavgDist; int RRavgMovNum;
    int NavgDist; int NavgMovNum;
    int SavgDist; int SavgMovNum;
    int EavgDist; int EavgMovNum;
    int WavgDist; int WavgMovNum;
    int NEavgDist; int NEavgMovNum;
    int NWavgDist; int NWavgMovNum;
    int SEavgDist; int SEavgMovNum;
    int SWavgDist; int SWavgMovNum;

    int movementsCounter;   //Movement I'm performing in the current cmd
    QString movementDirection;
    QString currentMovement;

    /******* </Movement Variables> *******/

    void request_movement (QString command);

    int imgNumber; // To keep track of the frames being received
    int last_imgNumber;
    unsigned long int dropped_frames;
    unsigned long int total_frames;//testeo. estadistica.

    int     batteryLevel; //Between 0 and 100: Remaining battery
    int     wifiStrength; //Between 0 and 100: WiFi signal strength
    bool    chargingStatus;
    QTimer *timStatus;

    QStringList *lstEncodedCmds;
    int list_counter;//tst
    void transToUrl (QString encodedCmd); //We pass it an encoded CMD and it
    //Translates it into the appropiate URL and executes it, using the
    //variables movementsNum and movementDirection along the way.
    //It does NOT use strList, but when a movement is finished and the
    //movement_finished signal is emitted, control_strList() is called.
    //That's why we use the followingPath variable: to bypass
    //control_strList()


signals:
    void image_downloaded   (void);
    void movement_finished  (void);
    void straightSignal     (void);
    void rotateSignal       (void);
    void justConnected      (void);
    void justDisconnected   (void);
    void statusRefreshed    (void);

public slots:
    void control_strlist (void);//tst
    void request_image (void);
    void replyFinished (QNetworkReply *reply);
    void errorHandler (QNetworkReply::NetworkError errorCode);

    void straight_move  (void);
    void rotatory_move  (void);
    void provideAuthentication (QNetworkReply *rep, QAuthenticator *auth);

private slots:
    void timMovementDelayHandler (void);
    void pollStatus (void);
};

#endif // ROBOTCONTROL_H
