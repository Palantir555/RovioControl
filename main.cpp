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

#include <QApplication>
#include <QObject>
#include <QTranslator>
#include "qextserialport.h"

#include "mainwindow.h"
#include "dbwin.h"
#include "serialportcontrol.h"
#include "robotcontrol.h"

TMainWindow *StartupWindow;
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator translator;
    translator.load("roviocontrol_sp");
    app.installTranslator(&translator);

    StartupWindow = new TMainWindow("Rovio control");
    StartupWindow->show();

    return app.exec();
}
