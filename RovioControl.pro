# Copyright 2013 Juan Carlos Jiménez Caballero
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.



#indicar que tipo de destino se quiere compilar. En este caso una aplicacción
TEMPLATES = app
#Añadimos el icono de la aplicación:
ICON = images/rovio_top_view_3.icns

#indicar la lista de módulos cpp que forman la aplicación
SOURCES += \
    mainwindow.cpp \
    dbwin.cpp \
    configsptab.cpp \
    serialportcontrol.cpp \
    configdbtab.cpp \
    databasecontrol.cpp \
    robotcontrol.cpp \
    configrctab.cpp \
    newtablewidget.cpp
SOURCES += main.cpp

#indicar la lista de cabeceras propias que se van a usar
HEADERS += \
    mainwindow.h \
    dbwin.h \
    configsptab.h \
    serialportcontrol.h \
    configdbtab.h \
    databasecontrol.h \
    robotcontrol.h \
    configrctab.h \
    newtablewidget.h

include(qextserialport/src/qextserialport.pri)

CONFIG += extserialport

OTHER_FILES +=

FORMS +=

QT += sql network

RESOURCES += \
    resources.qrc


#Translation:

TRANSLATIONS = roviocontrol_sp.ts

CODECFORTR = ISO-8859-5
