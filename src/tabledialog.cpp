/**
 * Copyright 2014 Bhavyanshu Parasher
 * This file is part of "LightMd Editor".
 * "LightMd Editor" is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 * "LightMd Editor" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with "LightMd Editor".
 * If not, see http://www.gnu.org/licenses/.
 */

#include <QtGui>

#include <tabledialog.h>

 TableDialog::TableDialog(QWidget *parent)
     : QDialog(parent)
 {
     setStyleSheet("TableDialog {background:#302F2F;} QLabel {color:#FFFFFF; }");
     label1 = new QLabel(tr("Number of &rows:"));
     lineEdit1 = new QLineEdit;
     QRegExp rx("[1-9]\\d{0,0}");
     lineEdit1->setValidator(new QRegExpValidator(rx,this));
     label1->setBuddy(lineEdit1);

     label2 = new QLabel(tr("Number of &columns:"));
     lineEdit2 = new QLineEdit;
     lineEdit2->setValidator(new QRegExpValidator(rx,this));
     label2->setBuddy(lineEdit2);
     lineEdit2->setDisabled(true);

     addtableButton = new QPushButton(tr("&Add"));
     addtableButton->setDefault(true);
     addtableButton->setDisabled(true);

     cancelButton = new QPushButton(tr("&Cancel"));
     cancelButton->setAutoDefault(false);

     buttonBox = new QDialogButtonBox(Qt::Vertical);
     buttonBox->addButton(addtableButton, QDialogButtonBox::ActionRole);
     buttonBox->addButton(cancelButton, QDialogButtonBox::ActionRole);

     connect(addtableButton, SIGNAL(clicked()), this, SLOT(accept()));
     connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
     connect(lineEdit1, SIGNAL(textChanged(const QString &)), this, SLOT(check_line1(const QString &)));

     QHBoxLayout *topLeftLayout = new QHBoxLayout;
     topLeftLayout->addWidget(label1);
     topLeftLayout->addWidget(lineEdit1);
     topLeftLayout->addWidget(label2);
     topLeftLayout->addWidget(lineEdit2);

     QGridLayout *mainLayout = new QGridLayout;
     mainLayout->setSizeConstraint(QLayout::SetFixedSize);
     mainLayout->addLayout(topLeftLayout,0,0);
     mainLayout->addWidget(buttonBox,0,1);
     setLayout(mainLayout);

     setWindowTitle(tr("Table"));
 }


 void TableDialog::check_line1(const QString &str) {
     lineEdit2->setEnabled(!str.isEmpty());
     connect(lineEdit2, SIGNAL(textChanged(const QString &)), this, SLOT(check_line2(const QString &)));
 }

 void TableDialog::check_line2(const QString &str_new){
    addtableButton->setEnabled(!str_new.isEmpty());
 }

