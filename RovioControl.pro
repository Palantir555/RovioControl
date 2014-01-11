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
ICON = resources/images/rovio_top_view_3.icns

#indicar la lista de módulos cpp que forman la aplicación
SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/dbwin.cpp \
    src/configsptab.cpp \
    src/serialportcontrol.cpp \
    src/configdbtab.cpp \
    src/databasecontrol.cpp \
    src/robotcontrol.cpp \
    src/configrctab.cpp \
    src/newtablewidget.cpp

#indicar la lista de cabeceras propias que se van a usar
HEADERS += \
    lib/mainwindow.h \
    lib/dbwin.h \
    lib/configsptab.h \
    lib/serialportcontrol.h \
    lib/configdbtab.h \
    lib/databasecontrol.h \
    lib/robotcontrol.h \
    lib/configrctab.h \
    lib/newtablewidget.h

include(qextserialport/src/qextserialport.pri)

CONFIG += extserialport

OTHER_FILES +=

FORMS +=

QT += sql network

RESOURCES += \
    resources/resources.qrc


#Translation:

TRANSLATIONS = languages/roviocontrol_sp.ts

CODECFORTR = ISO-8859-5
