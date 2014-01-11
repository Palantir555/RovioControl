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

#include "databasecontrol.h"

DatabaseControl::DatabaseControl(QObject *parent)
    :QObject(parent)
{
    this->connection_name = "myDBConnection";
    db      = new QSqlDatabase;
    query   = new QSqlQuery;

    qDebug() << "Available database drivers:" << QSqlDatabase::drivers();
    this->driver        = "QPSQL";
    this->host_name     = "host.com";
    this->port          = 5432;
    this->database_name = "db_name";
    this->username      = "user";
    this->password      = "pass";
}

DatabaseControl::~DatabaseControl(void)
{
    if(db)
        delete db;
}

bool    DatabaseControl::set_driver (QString db_driver)
{
    if (QSqlDatabase::isDriverAvailable(db_driver))
    {
        this->driver = db_driver;
        return (1);
    }
    else
    {
        qDebug() << "The selected driver is not available. Driver:" << db_driver;

        QMessageBox driverPopup;     //We want to ask for a confirmation to avoid accidental DROPs
        driverPopup.setIcon (QMessageBox::Warning);
        driverPopup.setText (tr("SQL driver not found"));
        QString informative_str = tr("The SQL driver driver you wanted to use is not available.\n Please install it and try again.");
        driverPopup.setInformativeText (informative_str);
        driverPopup.setStandardButtons (QMessageBox::Ok);
        driverPopup.setDefaultButton (QMessageBox::Ok);
        switch ( driverPopup.exec() ) //Do sth according to the confirmation box item selected by the user
        {
        case QMessageBox::Ok:
            break;
        default:
            break;
        }

        return (0);
    }
}
void    DatabaseControl::set_hostName (QString db_hostName)
{
    this->host_name = db_hostName;
}
void    DatabaseControl::set_databaseName (QString db_databaseName)
{
    this->database_name = db_databaseName;
}
void    DatabaseControl::set_username (QString db_userName)
{
    this->username = db_userName;
}
void    DatabaseControl::set_password (QString db_password)
{
    this->password = db_password;
}
void    DatabaseControl::set_port (int db_port)
{
    this->port = db_port;
}

QSqlDatabase DatabaseControl::get_db (void)
{
    return ( QSqlDatabase::database(connection_name) );
}
QString DatabaseControl::get_driver (void)
{
    return( db->driverName() );
}
QString DatabaseControl::get_hostName (void)
{
    return( this->host_name );
}
QString DatabaseControl::get_databaseName (void)
{
    return( this->database_name );
}
QString DatabaseControl::get_username (void)
{
    return( this->username );
}
QString DatabaseControl::get_password (void)
{
    return( this->password );
}
int     DatabaseControl::get_port (void)
{
    return( this->port );
}

QStringList DatabaseControl::get_tablesList (void)
{
    return( db->tables(QSql::Tables) );
}

QStringList DatabaseControl::get_tableRecord (const QString & table_name)
{
    QSqlRecord record;
    record = db->record(table_name);
    int number_of_fields = record.count();
    QStringList str_list;
    str_list.clear();
    for (int i=0; i < number_of_fields; i++)
    {
        str_list << record.fieldName(i);
    }
    return( str_list );
}

QSqlQuery DatabaseControl::get_retrievedColumn (QString query)
{
/*
    QString query_str = "SELECT " + field_name + " FROM " + table_name + " ORDER BY " + field_order;
    QStringList output_str;
    qDebug() << query_str;
    query->prepare(query_str);
    query->exec();
    int i=0;
    while (query->next())
    {
        output_str << query->value(i).toString();
        i++;
    }
    return ( output_str );
*/
    return ( db->exec(query) );
}

void DatabaseControl::configure_connection (void)
{
    if (db->isOpen())
        db->close();
    db->removeDatabase(connection_name);
    *db = QSqlDatabase::addDatabase(this->driver, connection_name);
    db->setHostName     (this->host_name);
    db->setPort         (this->port);
    db->setDatabaseName (this->database_name);
    db->setUserName     (this->username);
    db->setPassword     (this->password);
}

bool DatabaseControl::connect (void)
{
    if (db->isOpen())
        db->close();
    if (!db->isOpen())
    {
        this->configure_connection();
        qDebug("DatabaseControl::connect db->connected");
        return(db->open());
    }
    else
    {
        qDebug() << "The connection was already open";
        return(0);
    }
}

void DatabaseControl::disconnect (void)
{
    //db->removeDatabase(connection_name);
    db->close();
}

bool DatabaseControl::connection_isOpen (void)
{
    return (db->isOpen());
}
