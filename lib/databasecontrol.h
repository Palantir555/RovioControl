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

#ifndef DATABASECONTROL_H
#define DATABASECONTROL_H

#include <QObject>

#include <QtSql>
#include <QSqlDriver>
#include <QMessageBox>


class DatabaseControl : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseControl(QObject *parent = 0);
    ~DatabaseControl(void);

    bool set_driver       (QString db_driver);
    void set_hostName     (QString db_hostName);
    void set_databaseName (QString db_databaseName);
    void set_username     (QString db_userName);
    void set_password     (QString db_password);
    void set_port         (int db_port);

    QSqlDatabase get_db      (void);
    QString get_driver       (void);
    QString get_hostName     (void);
    QString get_databaseName (void);
    QString get_username     (void);
    QString get_password     (void);
    int get_port             (void);
    QStringList get_tablesList      (void);
    QStringList get_tableRecord     (const QString & table_name);
    QSqlQuery get_retrievedColumn  (QString query=QString());

    void configure_connection (void);
    bool connect    (void);
    void disconnect (void);
    bool connection_isOpen (void);

private:
    QSqlDatabase *db;
    QSqlQuery    *query;

    QString connection_name;
    QString driver;
    QString host_name;
    QString database_name;
    QString username;
    QString password;
    int     port;

signals:
    
public slots:
    
};

#endif // DATABASECONTROL_H
