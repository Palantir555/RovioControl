#include "Wire.h"       //Libreria I2C
#include "WiiChuck.h"   //Libreria WiiChuck

WiiChuck chuck = WiiChuck();  //Instancia de WiiChuck

int joyH = 0;
int joyV = 0;
int btC = 0;
int prev_btC = 0;
int btZ = 0;
int prev_btZ = 0;
int accelX = 0;

char str[19];

int C_justPressed;
int Z_justPressed;
int CandZ_justPressed;

void setup()
{
    Serial.begin(9600);    //Iniciamos comunicacion serie
    chuck.begin();         //Iniciamos WiiChuck
}

void loop()
{
    C_justPressed = 0;
    Z_justPressed = 0;
    CandZ_justPressed = 0;
    chuck.update();
    
    joyH = (int)chuck.readJoyX();
    joyV = (int)chuck.readJoyY();
    btC = (int)chuck.cPressed();
    btZ = (int)chuck.zPressed();
    accelX = (int)chuck.readAccelX();

    if (prev_btC == 0 && btC == 1)
        C_justPressed = 1;
    if (prev_btZ == 0 && btZ == 1)
        Z_justPressed = 1;

    //If both of them are pressed at the same time:
    if (((prev_btC == 0) || (prev_btZ == 0)) && ((btC == 1) && (btZ == 1)))  
        CandZ_justPressed = 1;

    sprintf (str, "$H%+04dV%+04dR%+04dU%1dD%1dL%1d#", 
        joyH, joyV, accelX, C_justPressed, Z_justPressed, CandZ_justPressed);
    Serial.print (str);  //send the string

    delay(100);
    prev_btC = btC;
    prev_btZ = btZ;
}


