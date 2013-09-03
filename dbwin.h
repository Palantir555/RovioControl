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
#ifndef DBWIN_H
#define DBWIN_H

#include <stddef.h>
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QShortcut>
#include <QComboBox>
#include <QTableWidget>
#include <QTableView>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QMessageBox>

#include <QtSql>
#include <QStringList>

#include "databasecontrol.h"
#include "newtablewidget.h"
#include "robotcontrol.h"


class DBWindow : public QDialog
{
    Q_OBJECT

public:
    DBWindow(QString dbWindowTitle, DatabaseControl *myDBC,
             RobotControl *myRC);
    ~DBWindow(void);

private:
    void createShortcuts(void);
    void createLayout   (void);
    void createWidgets  (void);
    void set_HLine      (QFrame *fr);
    void set_VLine      (QFrame *fr);
    void fill_comboTables  (void);
    void fill_table     (QSqlQuery *query);
    void hideInsertCommand (void);
    void showInsertCommand (void);
    void hideTuningValues (void);
    void showTuningValues (void);
    bool execute_query  (QSqlQuery *query); //1=noError 0=error
    bool appendTableToPath (QSqlQuery *query);
    void importTuningValues (QSqlQuery *query);

    RobotControl    *myRobotControl_p;
    DatabaseControl *myDatabaseControl_p;
    QSqlDatabase    *db_connection;
    QSqlQuery       *new_table_query;

    QStringList *tablesList;
    QString     *currentTableName;
    QStringList *currentTableFields; //Will contain the field's names
    QString     *strOrder;  //Name of the field with the order of cmds
    QString     *strCommand;//Name of the field with the cmds
    QString     *strDir;
    QString     *strDist;
    QString     *strMovsNum;

    bool handleComboBox;

    //<shortcuts>
    QShortcut *closeWindowShortcut;
    //</shortcuts>

    //<layouts>
    QGridLayout *superGrid;
    QVBoxLayout *topVBox;
    QHBoxLayout *topHBox0;
    QVBoxLayout *middleVBox;
    QVBoxLayout *bottomVBox;
    QHBoxLayout *bottomHBox0;
    QHBoxLayout *bottomHBox1;
    QHBoxLayout *bottomHBox2;
    QHBoxLayout *bottomHBox3;
    QHBoxLayout *bottomHBox4;
    //</layouts>

    QComboBox   *comboTables;
    QPushButton *btDropTable;
    QPushButton *btCreateTable;
    QFrame      *frTopDelimiter;
    NewTableWidget *myNewTableWidget;

    QTableWidget    *fieldsTable;

    QLabel      *lbOrder;
    QLineEdit   *leOrder;
    QLabel      *lbCommand;
    QLineEdit   *leCommand;
    QPushButton *btInsertCmd;

    QLineEdit   *leCustomQuery;
    QPushButton *btSubmitQuery;
    QFrame      *frBotDelimiter;

    QPushButton *btAddTableToPath;
    QPushButton *btImportTuning;


signals:
    
public slots:

private slots:
    void closeWindow        (void);
    void comboTables_handleIndexChanged (QString table_name);
    void handleBtSubmit     (void);
    void handleBtDropTable  (void);
    void handleBtCreateTable (void);
    void handleTableReady   (QString table_query_str);
    void handleBtAddToPath  (void);
    void handleBtInsertCmd  (void);
    void handleBtImportTuning (void);
};

#endif // DBWIN_H
