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

#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <QtWidgets>
#include <QDialog>
#include "tabledialog.h"

 class TableDialog : public QDialog
 {
     Q_OBJECT

 public:
     TableDialog(QWidget *parent = 0);
     QLineEdit *lineEdit1;
     QLineEdit *lineEdit2;

private slots:
     void check_line1(const QString &str);
     void check_line2(const QString &str_new);

 private:
     QLabel *label1;
     QLabel *label2;
     QDialogButtonBox *buttonBox;
     QPushButton *addtableButton;
     QPushButton *cancelButton;
 };

#endif // TABLEDIALOG_H
