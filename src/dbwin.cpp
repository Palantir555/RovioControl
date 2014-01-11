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

#include "dbwin.h"

DBWindow::DBWindow (QString dbWindowTitle, DatabaseControl *myDBC, RobotControl *myRC) :
    QDialog()
{
    Qt::WindowFlags flags = windowFlags(); //Get the enabled window flags
    flags |= Qt::WindowMinimizeButtonHint; //Add the minimize button
    setWindowFlags(flags);                 //Set the new modified window flags

    qDebug("In the DBWindow constructor");
    handleComboBox = 1;
    myDatabaseControl_p = myDBC;
    myRobotControl_p    = myRC;
    db_connection = new QSqlDatabase;
    *db_connection = myDatabaseControl_p->get_db();
    new_table_query = new QSqlQuery(*db_connection);
    if (!db_connection->isValid())
    {
        qDebug() << "The connection to the database is not valid";
        return;
    }
    this->setWindowTitle (dbWindowTitle);
    this->createShortcuts();
    this->createLayout();
    this->createWidgets();
    this->fill_comboTables();
}

DBWindow::~DBWindow (void)
{
    qDebug() << "In the DBWindow destructor";
    if (db_connection->isOpen())
        db_connection->close();
    if (db_connection)
        delete db_connection;
    if (new_table_query)
        delete new_table_query;

}

void DBWindow::createShortcuts (void)
{
    qDebug() << "DBWindow: creating the shortcuts";
    closeWindowShortcut = new QShortcut (QKeySequence::Close, this);
    connect (closeWindowShortcut, SIGNAL(activated()), this, SLOT(closeWindow()));
}

void DBWindow::createLayout (void)
{
    qDebug() << "DBWindow: creating the layout";
    superGrid   = new QGridLayout (this);
    topVBox     = new QVBoxLayout;
    topHBox0    = new QHBoxLayout;
    middleVBox  = new QVBoxLayout;
    bottomVBox  = new QVBoxLayout;
    bottomHBox0 = new QHBoxLayout;
    bottomHBox1 = new QHBoxLayout;
    bottomHBox2 = new QHBoxLayout;
    bottomHBox3 = new QHBoxLayout;
    bottomHBox4 = new QHBoxLayout;

    superGrid->addLayout (topVBox    ,0  ,0);
    superGrid->addLayout (middleVBox ,1  ,0);
    superGrid->addLayout (bottomVBox ,2  ,0);

    superGrid->setRowStretch (0 ,0);
    superGrid->setRowStretch (1 ,100);
    superGrid->setRowStretch (2 ,0);

    topVBox->addLayout (topHBox0);

    bottomVBox->addLayout (bottomHBox0, 0);
    bottomVBox->addLayout (bottomHBox1, 0);
    bottomVBox->addLayout (bottomHBox2, 0);
    bottomVBox->addLayout (bottomHBox3, 0);
    bottomVBox->addLayout (bottomHBox4, 0);

}

void DBWindow::createWidgets (void)
{
    qDebug("DBWindow: creating the widgets");
    strOrder        = new QString ("cmd_order");
    strCommand      = new QString ("command");
    strDir          = new QString ("direction");
    strDist         = new QString ("distance");
    strMovsNum      = new QString ("movements");
    currentTableName   = new QString;
    currentTableFields = new QStringList;
    fieldsTable     = new QTableWidget;
    tablesList      = new QStringList;
    frTopDelimiter  = new QFrame; set_HLine(frTopDelimiter);
    frBotDelimiter  = new QFrame; set_HLine(frBotDelimiter);
    myNewTableWidget= new NewTableWidget;
    comboTables     = new QComboBox;
    btDropTable     = new QPushButton (tr("Drop this table"));
    btCreateTable   = new QPushButton (tr("Create new table"));
    lbOrder         = new QLabel (tr(" Cmd_order:"));
    leOrder         = new QLineEdit;
    lbCommand       = new QLabel (tr("Command:"));
    leCommand       = new QLineEdit;
    btInsertCmd     = new QPushButton (tr("Insert"));
    leCustomQuery   = new QLineEdit   (tr("Execute your own query"));
    btSubmitQuery   = new QPushButton (tr("Submit"));
    btAddTableToPath= new QPushButton (tr("Add this table to the path"));
    btImportTuning  = new QPushButton (tr("Import tuning values"));

    topHBox0->addWidget (comboTables    ,0 ,Qt::AlignLeft);
    topHBox0->addWidget (btCreateTable  ,0 ,Qt::AlignLeft);
    topHBox0->addStretch ();
    topHBox0->addWidget (btDropTable    ,0 ,Qt::AlignRight);
    topVBox->addWidget (frTopDelimiter);
    myNewTableWidget->myHideMethod();
    topVBox->addWidget (myNewTableWidget);

    middleVBox->addWidget (fieldsTable);

    this->hideInsertCommand();  //By default, the widgets to insert a cmd are hidden.
    bottomHBox1->addWidget (lbOrder ,0);
    bottomHBox1->addWidget (leOrder ,10);
    bottomHBox1->addWidget (lbCommand ,0);
    bottomHBox1->addWidget (leCommand ,15);
    bottomHBox1->addWidget (btInsertCmd ,0);

    bottomHBox2->addWidget (leCustomQuery ,100 ,Qt::AlignBottom);
    bottomHBox2->addWidget (btSubmitQuery ,0   ,Qt::AlignBottom | Qt::AlignRight);
    bottomHBox3->addWidget (frBotDelimiter);
    bottomHBox4->addWidget (btAddTableToPath ,0 ,Qt::AlignLeft);
    bottomHBox4->addStretch(100);
    bottomHBox4->addWidget (btImportTuning ,0);

    leCustomQuery->setSizePolicy (QSizePolicy::Ignored ,QSizePolicy::Fixed);

    btDropTable     ->setAutoDefault (false);
    btCreateTable   ->setAutoDefault (false);
    btSubmitQuery   ->setAutoDefault (false);
    btInsertCmd     ->setAutoDefault (false);
    btAddTableToPath->setAutoDefault (false);
    btImportTuning  ->setAutoDefault (false);

    btAddTableToPath->setEnabled (false);
    btImportTuning  ->setEnabled (false);

    connect (comboTables        ,SIGNAL(activated(QString)) ,this
             ,SLOT(comboTables_handleIndexChanged(QString)));
    connect (btCreateTable      ,SIGNAL(released())         ,this
             ,SLOT(handleBtCreateTable()));
    connect (btDropTable        ,SIGNAL(released())         ,this
             ,SLOT(handleBtDropTable()));
    connect (leCustomQuery      ,SIGNAL(returnPressed())    ,this
             ,SLOT(handleBtSubmit()));
    connect (btSubmitQuery      ,SIGNAL(released())         ,this
             ,SLOT(handleBtSubmit()));
    connect (myNewTableWidget   ,SIGNAL(newTableReady(QString)) ,this
             ,SLOT(handleTableReady(QString)));
    connect (btAddTableToPath   ,SIGNAL(released())         ,this
             ,SLOT(handleBtAddToPath()));
    connect (btImportTuning     ,SIGNAL(released())         ,this
             ,SLOT(handleBtImportTuning()));
    connect (btInsertCmd        ,SIGNAL(released())         ,this
             ,SLOT(handleBtInsertCmd()));
    connect (leOrder            ,SIGNAL(returnPressed())    ,this
             ,SLOT(handleBtInsertCmd()));
}

void DBWindow::fill_comboTables (void)
{
    handleComboBox = false;
    comboTables->setEnabled(false);
    tablesList->clear();
    *tablesList = myDatabaseControl_p->get_tablesList ();
    comboTables->clear();
    comboTables->addItems (*tablesList);
    comboTables->setEnabled(true);
    handleComboBox = true;
    this->comboTables_handleIndexChanged (comboTables->currentText());
}

void DBWindow::hideInsertCommand (void)
{
    lbOrder         ->hide();
    leOrder         ->hide();
    lbCommand       ->hide();
    leCommand       ->hide();
    btInsertCmd     ->hide();
}

void DBWindow::showInsertCommand (void)
{
    lbOrder         ->show();
    leOrder         ->show();
    lbCommand       ->show();
    leCommand       ->show();
    btInsertCmd     ->show();
}

void DBWindow::hideTuningValues (void)
{

}

void DBWindow::showTuningValues (void)
{

}

void DBWindow::comboTables_handleIndexChanged (QString table_name)
{
    if (!handleComboBox)
    {
        qDebug() << "handleComboBox is disabled. Sth messing with the tables QComboBox?";
        return;
    }
    if (!db_connection->isOpen())
    {
        qDebug() << "The connection to the database is not open";
        return;
    }
    currentTableName->clear();
    *currentTableName = table_name;

    QSqlQuery   query (*db_connection);
    query.prepare("SELECT * FROM " + *currentTableName + " ORDER BY 1");
    if (!this->execute_query(&query))
        return;
    this->fill_table(&query);
}

void DBWindow::handleBtSubmit (void)
{
    QSqlQuery query (*db_connection);
    query.prepare( leCustomQuery->text() );

    if (!this->execute_query(&query))
        return;

    if (query.isSelect())
        this->fill_table(&query);
    else
        this->comboTables_handleIndexChanged(comboTables->currentText());
    leCustomQuery->setText (tr("Query submitted"));
}


bool DBWindow::execute_query  (QSqlQuery *query)
{
    QSqlQuery *query_p = query;

    //Check for previous errors in the query (e.g. In the QSqlQuery::prepare method)
    if (query_p->lastError().isValid())
    {
        qDebug() << "The DB is going to be queried:" << query_p->lastQuery();
        qDebug() << "    These are the previous errors in the query:";
        qDebug() << "----------<ERROR>----------";
        qDebug() << query_p->lastError();
        qDebug() << "----------</ERROR>----------";
    }

    if( !db_connection->isOpen() )
    {
        qDebug() << "The query was not executed successfully";
        qDebug() << "    Your connection to the database seems to be closed";
        qDebug() << "    Connection error:" << db_connection->lastError().text();
        QMessageBox *connection_problem = new QMessageBox;
        connection_problem->setText            (tr("The query could not be executed successfully"));
        connection_problem->setInformativeText (tr("Your connection to the database seems to be closed. Try restarting it."));
        connection_problem->setIcon            (QMessageBox::Warning);
        connection_problem->exec               ();
        return (0);
    }
    else if (!query_p->exec())
    {
        qDebug() << "The query was not executed successfully";
        qDebug() << "DBWindow::execute_query -> The query could not be executed.";
        qDebug() << "    Invalid query?:"   << query_p->lastQuery();
        qDebug() << "    Error:";
        qDebug() << "----------<ERROR>----------";
        qDebug() << query_p->lastError().text();
        qDebug() << "----------</ERROR>----------";
        QMessageBox *query_not_executed = new QMessageBox;
        query_not_executed->setText            (tr("The query could not be executed successfully"));
        query_not_executed->setInformativeText (tr("Check your query and try again"));
        query_not_executed->setIcon            (QMessageBox::Warning);
        query_not_executed->exec               ();
        return (0);
    }
    return (1);
}

void DBWindow::fill_table (QSqlQuery *query)
{
    QSqlQuery *query_p = query;
    int i=0;
    int n=0;

    fieldsTable->clear();
    fieldsTable->setRowCount(0);
    fieldsTable->setColumnCount(0);

    //<Write the columns titles>
    QSqlRecord  record = query_p->record();
    fieldsTable->setColumnCount (record.count());
    currentTableFields->clear();
    for (i=0; i < record.count(); i++)
        *currentTableFields << record.fieldName(i);
    fieldsTable->setHorizontalHeaderLabels ( *currentTableFields );
    //</Write the columns titles>
    //<Fill the table>
    while (query_p->next())
    {
        fieldsTable->setRowCount (fieldsTable->rowCount() + 1);
        for (i=0; i<record.count(); i++)
        {
            QString str = query_p->value(i).toString();
            fieldsTable->setCellWidget(n, i, new QLabel(str));
        }
        n++;
    }
    //</Fill the table>
    //<Hide or show the Widgets to insert rows>
    if (currentTableFields->contains(*strOrder, Qt::CaseInsensitive) &&
            currentTableFields->contains(*strCommand, Qt::CaseInsensitive))
    {
        btAddTableToPath->setEnabled(true);
        btImportTuning->setEnabled(false);
        this->showInsertCommand();
        this->hideTuningValues();
    }
    else if (currentTableFields->contains(*strDir, Qt::CaseInsensitive) &&
             currentTableFields->contains(*strDist, Qt::CaseInsensitive) &&
             currentTableFields->contains(*strMovsNum, Qt::CaseInsensitive))
    {
         btImportTuning->setEnabled(true);
         btAddTableToPath->setEnabled(false);
         this->showTuningValues();
         this->hideInsertCommand();
    }
    else
    {
        btAddTableToPath->setEnabled(false);
        btImportTuning  ->setEnabled(false);
        this->hideInsertCommand();
        this->hideTuningValues();
    }
}

void DBWindow::handleBtCreateTable (void)
{
    if (myNewTableWidget->isVisible())
    {
        myNewTableWidget->myHideMethod();
        btCreateTable   ->setText(tr("Create new table"));
    }
    else
    {
        myNewTableWidget->show();
        btCreateTable   ->setText(tr("Abort table creation"));
    }
}

void DBWindow::handleBtDropTable (void)
{
    QString dropTable_string="";
    QSqlQuery dropTable_query(*db_connection);
    dropTable_string.append ("DROP TABLE " + this->comboTables->currentText() + ";");

    QMessageBox confirmationPopup;     //We want to ask for a confirmation to avoid unwanted DROPs
    confirmationPopup.setIcon (QMessageBox::Warning);
    confirmationPopup.setText (tr("Are you sure about dropping that table?"));
    QString informative_str = tr("Confirm that you want to execute the following SQL query: ");
    informative_str.append(dropTable_string);
    confirmationPopup.setInformativeText (informative_str);
    confirmationPopup.setStandardButtons (QMessageBox::Cancel | QMessageBox::Ok);
    confirmationPopup.setDefaultButton (QMessageBox::Cancel);
    switch ( confirmationPopup.exec() ) //Do sth according to the confirmation box item selected by the user
    {
    case QMessageBox::Ok:
        dropTable_query.exec (dropTable_string);
        this->fill_comboTables();
        break;
    case QMessageBox::Cancel:
        break;
    default:
        break;
    }
}

void DBWindow::handleTableReady (QString table_query_str)
{
    const QString *ntq = new QString(table_query_str);
    qDebug() << "About to execute the query";
    new_table_query->exec(*ntq);
    this->fill_comboTables();
    this->handleBtCreateTable(); //This will hide the widget and change the button's text
}

void DBWindow::handleBtAddToPath (void)
{
    //Not strictly necessary, because btAddTableToPath is disabled in that case. Left for debugging.
    if (!currentTableFields->contains(*strOrder, Qt::CaseInsensitive)
            || !currentTableFields->contains(*strCommand, Qt::CaseInsensitive))
    {
        qDebug() << "The current table does not have the 'cmd_order' and 'command' fields";
    }

    QSqlQuery   query (*db_connection);
    QString     strQuery = QString ("SELECT %1 FROM %2 ORDER BY %3")
                            .arg(*strCommand).arg(*currentTableName).arg(*strOrder);
    query.prepare(strQuery);
    if (!this->execute_query(&query))
        return;
    this->appendTableToPath(&query);
}

bool DBWindow::appendTableToPath (QSqlQuery *query)
{
    QSqlQuery *query_p = query;
    bool result=1;
    while (query_p->next())
    {
        result = myRobotControl_p->appendToPath( query_p->value(0).toString());
    }
    return(result);
}

void DBWindow::handleBtImportTuning (void)
{
    /*
    if (!currentTableFields->contains(*strDir, Qt::CaseInsensitive) ||
            !currentTableFields->contains(*strDist, Qt::CaseInsensitive) ||
            !currentTableFields->contains(*strMovsNum, Qt::CaseInsensitive))
    {
        qDebug() << "The current table does not have the 'direction', 'distance' and 'movements' fields";
    }
    */

    QSqlQuery   query (*db_connection);
    QString     strQuery = QString ("SELECT * FROM %1 ORDER BY %2")
                            .arg(*currentTableName).arg(*strDir);
    query.prepare(strQuery);
    if (!this->execute_query(&query))
        return;
    this->importTuningValues(&query);

}

void DBWindow::importTuningValues (QSqlQuery *query)
{
    QSqlQuery *query_p = query;
    int dirFieldNum  = 0;
    int distFieldNum = 0;
    int movsFieldNum = 0;
    while (query_p->next())
    {
        dirFieldNum = query_p->record().indexOf(*strDir);
        distFieldNum= query_p->record().indexOf(*strDist);
        movsFieldNum= query_p->record().indexOf(*strMovsNum);
        myRobotControl_p->setMovAverage (query_p->value(dirFieldNum).toString(),
                 query_p->value(distFieldNum).toInt(),
                 query_p->value(movsFieldNum).toInt());
    }
}

void DBWindow::handleBtInsertCmd (void)
{
    QSqlQuery query (*db_connection);
    QString strQuery = QString("INSERT INTO %1 (%2, %3) VALUES (%4, '%5');")
            .arg(*currentTableName).arg(*strOrder).arg(*strCommand).arg(leOrder->text()).arg(leCommand->text());
    query.prepare( strQuery );

    if (!this->execute_query(&query))
        return;

    this->comboTables_handleIndexChanged(comboTables->currentText());
    leOrder     ->clear();
    leCommand   ->clear();
}

void DBWindow::closeWindow (void)
{
    qDebug() << "DBWindow closed";
    this->close ();
}

/****************************************************\
\*****************__Miscellaneous:__*****************/
void DBWindow::set_HLine (QFrame *fr)
{
    fr->setFrameShape (QFrame::HLine);
    fr->setFrameShadow(QFrame::Sunken);
}

void DBWindow::set_VLine(QFrame *fr)
{
    fr->setFrameShape (QFrame::VLine);
    fr->setFrameShadow(QFrame::Sunken);
}
