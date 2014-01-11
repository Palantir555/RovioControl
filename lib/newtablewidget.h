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

#ifndef NEWTABLEWIDGET_H
#define NEWTABLEWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>

class NewTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NewTableWidget(QWidget *parent = 0);
    
private:
    QVBoxLayout *mainVBox;
    QHBoxLayout *tableNameHBox;
    QHBoxLayout *typeHBox;
    QHBoxLayout *fieldNameHBox;
    QHBoxLayout *addRemoveHBox;
    QHBoxLayout *readyHBox;
    QHBoxLayout *queryLbHBox;

    QFrame      *frLine;
    QLineEdit   *leTableName;
    QPushButton *btAddField;
    QPushButton *btRemoveField;
    QPushButton *btReady;
    QComboBox   *comboTypes;

    QLabel      *lbTypeId;
    QLineEdit   *leType;
    QLabel      *lbFieldId;
    QLineEdit   *leField;

    QLabel      *lbQuery;

    void createLayout   (void);
    void createWidgets  (void);
    void forgeQuery     (void);
    void fillComboTypes (void);
    void set_HLine      (QFrame *fr);
    void set_VLine      (QFrame *fr);
    bool typeIsValid    (QString str);

    QStringList *typesList;
    QStringList *namesList;

    QString *query;
    QString *otherType;

signals:
    void newTableReady (QString query_string);

public slots:
    void handleBtAddField   (void);
    void handleBtRemoveField(void);
    void myHideMethod       (void);
    void handleBtReady      (void);

private slots:
    void comboTypesIdxChanged (QString currentText);
};

#endif // NEWTABLEWIDGET_H
