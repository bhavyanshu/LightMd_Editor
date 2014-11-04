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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QTextEdit>

#include "finddialog.h"
#include "findreplacedialog.h"
#include "pmh_definitions.h"
#include "highlighter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void on_argOpenFile(const QString &fileName);
    void on_noarg();

    void readSettings();


private slots:
    void on_actionBold_triggered();

    void on_actionItalic_triggered();

    void on_actionAnchor_Link_triggered();

    void on_actionImage_triggered();

    void on_actionCode_Block_triggered();

    void on_actionBlockquote_triggered();

    void on_actionExit_triggered();

    void on_actionContact_triggered();

    void slotCloseTab(int index);

    void rollBackTab();

    void on_actionOpen_triggered();

    void documentWasModified();

    bool save();

    bool saveAs();

    void on_actionSave_triggered();

    void on_actionFont_triggered();

    void on_actionFind_triggered();

    void on_actionFind_Replace_triggered();

    void on_actionWordWrap_triggered();

    void on_actionTable_triggered();

    void on_actionNew_triggered();

    void on_actionFull_Screen_triggered();

    void on_actionFocus_Mode_triggered();

    void highlightCurrentLine();

private:
    void closeEvent(QCloseEvent *eve);
    void keyPressEvent(QKeyEvent *e);
    QString selectedText();
    void replaceTextBy(const QString& text);
    void insertAtCursor(const QString& text);
    void setCurrentFile(const QString &fileName);

    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);

    bool warnSave();
    void writeSettings();


    QString fontFamily;
    int fontSize;
    bool fontIsBold;
    bool fontIsItalic;
    FindDialog *m_findDialog;
    FindReplaceDialog *m_findReplaceDialog;
    MkdSyntax *highlighter;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
