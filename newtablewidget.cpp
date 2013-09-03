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

#include "newtablewidget.h"

NewTableWidget::NewTableWidget(QWidget *parent) :
    QWidget(parent)
{
    query = new QString("");

    this->createLayout();
    this->createWidgets();
    this->forgeQuery();
}

void NewTableWidget::createLayout (void)
{
    mainVBox        = new QVBoxLayout(this);
    tableNameHBox   = new QHBoxLayout;
    typeHBox        = new QHBoxLayout;
    fieldNameHBox   = new QHBoxLayout;
    addRemoveHBox   = new QHBoxLayout;
    readyHBox       = new QHBoxLayout;
    queryLbHBox     = new QHBoxLayout;

    mainVBox->addLayout (tableNameHBox);
    mainVBox->addLayout (fieldNameHBox);
    mainVBox->addLayout (typeHBox);
    mainVBox->addLayout (addRemoveHBox);
    mainVBox->addLayout (readyHBox);
    mainVBox->addLayout (queryLbHBox);

    mainVBox->setContentsMargins(5 ,0 ,5 ,0);
}

void NewTableWidget::createWidgets (void)
{
    typesList   = new QStringList;
    namesList   = new QStringList;
    otherType   = new QString ("Other...");

    frLine      = new QFrame; set_HLine(frLine);
    mainVBox->addWidget (frLine);

    leTableName = new QLineEdit(tr("Table name"));
    tableNameHBox->addWidget(leTableName, 100, Qt::AlignCenter);
    leTableName->setSizePolicy (QSizePolicy::Ignored ,QSizePolicy::Fixed);

    lbFieldId       = new QLabel(tr("Field's name:"));
    leField         = new QLineEdit;
    comboTypes      = new QComboBox; this->fillComboTypes();
    lbTypeId        = new QLabel(tr("Field's type:"));
    leType          = new QLineEdit; leType->hide(); //Hidden because the default is the comboTypes
    btAddField      = new QPushButton(tr("+"));
    btRemoveField   = new QPushButton(tr("-"));
    btReady         = new QPushButton(tr("Submit the query"));
    lbQuery         = new QLabel;

    int max_lb_width=0;
    if (lbTypeId->sizeHint().width() > max_lb_width)
        max_lb_width = lbTypeId->sizeHint().width();
    if (lbFieldId->sizeHint().width() > max_lb_width)
        max_lb_width = lbFieldId->sizeHint().width();
    lbTypeId ->setFixedWidth (max_lb_width);
    lbFieldId->setFixedWidth (max_lb_width);

    fieldNameHBox   ->addWidget (lbFieldId);
    fieldNameHBox   ->addWidget (leField ,100);
    typeHBox        ->addWidget (lbTypeId   ,0  ,Qt::AlignLeft);
    typeHBox        ->addWidget (comboTypes ,5  ,Qt::AlignLeft);
    typeHBox        ->addWidget (leType ,95);
    addRemoveHBox   ->addStretch ();
    addRemoveHBox   ->addWidget (btRemoveField  ,0  ,Qt::AlignLeft);
    addRemoveHBox   ->addWidget (btAddField     ,0  ,Qt::AlignLeft);
    addRemoveHBox   ->addStretch ();
    readyHBox       ->addWidget (btReady        ,0  ,Qt::AlignHCenter);
    queryLbHBox     ->addWidget (lbQuery);

    lbQuery->setFrameStyle (QFrame::Panel | QFrame::Sunken);

    btAddField      ->setAutoDefault (false);
    btRemoveField   ->setAutoDefault (false);
    btReady         ->setAutoDefault (false);

    connect (leType         ,SIGNAL(returnPressed())    ,this ,SLOT(handleBtAddField()));
    connect (leField        ,SIGNAL(returnPressed())    ,this ,SLOT(handleBtAddField()));
    connect (comboTypes     ,SIGNAL(currentIndexChanged(QString)) ,this ,SLOT(comboTypesIdxChanged(QString)));
    connect (btAddField     ,SIGNAL(released())         ,this ,SLOT(handleBtAddField()));
    connect (btRemoveField  ,SIGNAL(released())         ,this ,SLOT(handleBtRemoveField()));
    connect (btReady        ,SIGNAL(released())         ,this ,SLOT(handleBtReady()));
}

void NewTableWidget::handleBtAddField (void)
{
    QString type;
    QString name = leField->text();
    if ( comboTypes->currentText() == otherType)
        type = leType->text();
    else
        type = comboTypes->currentText();

    if ( (!this->typeIsValid(type)) || (leField->text().isEmpty()))
        return;
    typesList->append(type);
    namesList->append(leField->text());
    leField ->clear();
    leType  ->clear();
    leField ->setFocus(); //put the focus in the first QLineEdit
    this->forgeQuery();
}

void NewTableWidget::handleBtRemoveField (void)
{
    if ((!typesList->isEmpty()) && (!namesList->isEmpty()))
    {
        typesList->removeLast ();
        namesList->removeLast ();
        this->forgeQuery();
    }
    else
    {
        qDebug("One or both of the lists is/are already empty");
        this->hide();
    }
}

void NewTableWidget::forgeQuery (void)
{
    if (typesList->count() != namesList->count())
    {
        qDebug("Weird error: The count of arguments in the typesList and the one of namesList is not the same");
        return;
    }
    QString *query_beginning = new QString("CREATE TABLE ");
    QString *query_end       = new QString(");");
    query = query_beginning;
    query->append( leTableName->text() + " (");
    int i=0;
    for (i=0; i < (typesList->count() -1); i++) //Tras el último type no hay coma, así que por eso el " - 1"
    {
        if (!typeIsValid( typesList->at(i) ))
        {
            return;//error out #!# hay que poner una función que lance un pop-up o algo
        }
        query->append(namesList->at(i) + " ");
        query->append(typesList->at(i) + ", ");
    }
    if (typesList->count() >= 1)
    {
        if (!typeIsValid( typesList->at( typesList->count() -1 ) ))
        {
            return;//error out #!# hay que poner una función que lance un pop-up o algo
        }
        query->append(namesList->at( namesList->count()-1 ) + " ");
        query->append(typesList->at( typesList->count()-1 ));
    }
    query->append(query_end);
    lbQuery->setText(*query);
}

void NewTableWidget::handleBtReady (void)
{
    //this->handleBtAddField();    //Para añadir a la query los fields que estan escritos pero no añadidos
    this->forgeQuery();
    qDebug() << "New table query:" << *query;
    emit newTableReady (*query);
    //this->myHideMethod(); // Commented bcause the signal handler will hide the widget
                            //    AND change the text of the button that shows/hides it.
}

bool NewTableWidget::typeIsValid (QString str)
{   // #!# Necesito una lista de tipos válidos y mejorar esta función. esto es provisional.
    str.clear(); //remove once this method does something
    return(true);
}

void NewTableWidget::myHideMethod (void)
{
    leTableName ->setText("Table_name");
    leType      ->clear();
    leField     ->clear();
    lbQuery     ->clear();

    typesList->clear();
    namesList->clear();

    this->hide();
}

void NewTableWidget::fillComboTypes (void)
{
    comboTypes->addItem ("INTEGER");
    comboTypes->addItem ("VARCHAR(100)");
    comboTypes->addItem ("DATETIME");
    comboTypes->addItem ("FLOAT");
    comboTypes->addItem (*otherType);
}

void NewTableWidget::comboTypesIdxChanged (QString currentText)
{
    if (currentText == *otherType)
    {
        leType->show();
    }
    else
    {
        if (leType->isVisible())
            leType->hide();
    }

}

/****************************************************\
\*****************__Miscellaneous:__*****************/
void NewTableWidget::set_HLine (QFrame *fr)
{
    fr->setFrameShape (QFrame::HLine);
    fr->setFrameShadow(QFrame::Sunken);
}

void NewTableWidget::set_VLine(QFrame *fr)
{
    fr->setFrameShape (QFrame::VLine);
    fr->setFrameShadow(QFrame::Sunken);
}
