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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tabledialog.h"
#include "pmh_parser.h"

#include "findreplace/findreplacedialog.h"
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QFileSystemModel>
#include <QLabel>
#include <QMessageBox>
#include <QComboBox>
#include <QTextDocument>
#include <QInputDialog>
#include <QEvent>
#include <QCloseEvent>
#include <QFontDialog>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionWordWrap->setChecked(true);
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(slotCloseTab(int)));
    statusBar()->showMessage(tr("Ready"), 2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeSettings() {

    QSettings settings("LightMdEditor","LightMd");
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());

    if(te){
        QFont font = te->font();
        settings.setValue("font.family", font.family());
        settings.setValue("font.size", font.pointSize());
        settings.setValue("font.bold", font.bold());
        settings.setValue("font.italic", font.italic());
        //settings.setValue("font", font().toString());
    }
}

void MainWindow::readSettings() {

    QSettings settings("LightMdEditor","LightMd");
    fontFamily = settings.value("font.family", QString()).toString();
    fontSize = settings.value("font.size", 12).toInt();
    fontIsBold = settings.value("font.bold", false).toBool();
    fontIsItalic = settings.value("font.italic", false).toBool();
}


void MainWindow::closeEvent(QCloseEvent *eve)
{
    writeSettings();
    for(int i = 0; i< ui->tabWidget->count(); i++ )
    {
        qDebug() << ui->tabWidget->widget(i);
        QTextEdit* edit = qobject_cast<QTextEdit*>(ui->tabWidget->widget(i));
        if(edit) {
            //edit->setText("new");
            QString filname = ui->tabWidget->tabText(i);
            if(edit->document()->isModified()){

                QMessageBox msgBox;
                msgBox.setText("There are unsaved changes in "+filname+".");
                msgBox.setInformativeText("Do you want to save your changes?");
                msgBox.setStandardButtons(QMessageBox::Save |QMessageBox::Cancel | QMessageBox::Discard );
                msgBox.setDefaultButton(QMessageBox::Save);
                int ret = msgBox.exec();

                switch (ret) {
                  case QMessageBox::Save:
                      save();                   // Save and quit
                      on_actionExit_triggered();
                      break;
                  case QMessageBox::Discard:
                      eve->accept();            // Discard edited content
                      break;
                  case QMessageBox::Cancel:
                      eve->ignore();            // Cancel was clicked, do nothing
                      break;
                  default:
                      // should never be reached
                      break;
                }
            }
        }
    }

}

/**
 * @brief MainWindow::slotCloseTab - Close tab properly on pressing close button
 * @param index
 */
void MainWindow::slotCloseTab(int index)
{
    QTextEdit* edit = qobject_cast<QTextEdit*>(ui->tabWidget->widget(index));
    if(edit) {
        if(warnSave()==true){           //Before closing tab check if document is modified, if yes then ask to save.
            this->ui->tabWidget->removeTab(index);
            rollBackTab();
        }
    }
}

/**
 * @brief MainWindow::rollBackTab - Set focus on text field of previous tab (if exists)
 */
void MainWindow::rollBackTab()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    if(te){
        te->setFocus();
    }
    else
        return;
}

void MainWindow::documentWasModified()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    //int index = ui->tabWidget->currentIndex();

    if(te->document()->isModified()) {
        statusBar()->showMessage(tr("Document has been modified"), 2000);
    }
}


bool MainWindow::warnSave()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());

    if (te->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Save File"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}


bool MainWindow::save()
{

    int index = ui->tabWidget->currentIndex();
    QString filename = ui->tabWidget->tabText(index);
    if (filename.compare("Untitled.md",Qt::CaseInsensitive)==0) { //That means file has never been saved before
        return saveAs(); //Save for the first time
    } else {
        return saveFile(filename);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (files.isEmpty())
        return false;

    return saveFile(files.at(0));
}

bool MainWindow::saveFile(const QString &fileName)
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << te->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    int index = ui->tabWidget->currentIndex();
    ui->tabWidget->setTabText(index,fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}


void MainWindow::loadFile(const QString &fileName)
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("LightMd"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    te->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    highlighter = new MkdSyntax(te->document(), 1000);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

/**
 * @brief MainWindow::selectedText - Gets the selected text
 * @return
 */
QString MainWindow::selectedText()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    return te->textCursor().selectedText();
}

/**
 * @brief MainWindow::replaceTextBy - Replaces text by a string
 * @param text
 */
void MainWindow::replaceTextBy( const QString& text )
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    QTextCursor cursor = te->textCursor();
    cursor.insertText(text);
    te->setTextCursor(cursor);
}

void MainWindow::insertAtCursor(const QString& text)
{
    replaceTextBy(text);
}

/**
 * Toolbar operations Begin
 */

/**
 * @brief MainWindow::on_actionBold_triggered - Action for Making text bold
 */
void MainWindow::on_actionBold_triggered()
{
    const QString select_text = selectedText();
    if(select_text.isEmpty()) {
        QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
        QTextCursor cursor = te->textCursor();
        cursor.insertText("** **");
        cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::MoveAnchor); //Move between Asterisks
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor); //Highlight that text must be added in between Asterisks
        te->setTextCursor(cursor);
    }
    else {
        replaceTextBy(QString("**%1**").arg(select_text));
    }
}

/**
 * @brief MainWindow::on_actionItalic_triggered - Action for making text italic
 */
void MainWindow::on_actionItalic_triggered()
{
    const QString select_text = selectedText();
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    if(select_text.isEmpty()) {
        QTextCursor cursor = te->textCursor();
        cursor.insertText("* *");
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor); //Move before Asterisk
        cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor); //Highlight that text must be added in between Asterisk
        te->setTextCursor(cursor);
    }
    else {
        replaceTextBy(QString("*%1*").arg(select_text));
    }
}

/**
 * @brief MainWindow::on_actionAnchor_Link_triggered - Action for adding anchor link
 */
void MainWindow::on_actionAnchor_Link_triggered()
{
    // Output should be in the form "[Title](URL "Title")"

    const QString title = QInputDialog::getText( this, QString::null, tr( "URL Title :" ) );
    if(title.isEmpty()) {
        return;
    }
    const QString url = QInputDialog::getText( this, QString::null, tr( "URL :" ) );
    if(url.isEmpty()) {
        return;
    }

    insertAtCursor(QString("[%1](%2 \"%1\")").arg(title).arg(url));
}

/**
 * @brief MainWindow::on_actionImage_triggered - Action for adding image
 */
void MainWindow::on_actionImage_triggered()
{
    // Output should be in the form "![Title](URL "Title")"

    const QString title = QInputDialog::getText( this, QString::null, tr( "URL Title :" ) );
    if(title.isEmpty()) {
        return;
    }
    const QString url = QInputDialog::getText( this, QString::null, tr( "URL :" ) );
    if(url.isEmpty()) {
        return;
    }

    insertAtCursor(QString("![%1](%2 \"%1\")").arg(title).arg(url));
}

void MainWindow::on_actionTable_triggered()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    QTextCursor cursor = te->textCursor();
    TableDialog tbd;
    int resultCode = tbd.exec();
    if(resultCode == QDialog::Accepted)
    {
        int i,j=0;
        const int rows = tbd.lineEdit1->text().toInt();
        const int column = tbd.lineEdit2->text().toInt();
        qDebug() << rows;
        qDebug() << column;
        for(i=0;i<rows;i++){
            for(j=0;j<column;j++){
                if(i==1){
                    cursor.insertText("|----"); //For Thead
                }
                else {
                    cursor.insertText("|    ");
                }
            }
            cursor.insertText("|\n");
        }
    }
    if(resultCode == QDialog::Rejected)
    {
        return;
    }
    tbd.close();
    if(!te->hasFocus()) {
        te->setFocus();
    }
}


/**
 * @brief MainWindow::on_actionCode_Block_triggered - Action for adding code block
 */
void MainWindow::on_actionCode_Block_triggered()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    QTextCursor cursor = te->textCursor();
    cursor.insertText(QString("\n```language\n```"));
    cursor.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    te->setTextCursor(cursor);
}

/**
 * @brief MainWindow::on_actionBlockquote_triggered - Action for adding blockquote
 */
void MainWindow::on_actionBlockquote_triggered()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    const QString select_text = selectedText();
    if(select_text.isEmpty()) {
        QTextCursor cursor = te->textCursor();
        cursor.insertText("\n>");
        te->setTextCursor(cursor);
    }
    else {
        replaceTextBy(QString("\n>%1").arg(select_text));
    }
}

void MainWindow::on_noarg()
{
    on_actionNew_triggered();
}

/**
 * @brief MainWindow::on_actionNew_triggered - Action for creating new file
 */
void MainWindow::on_actionNew_triggered()
{
    QTextEdit *textEdit_field = new QTextEdit();
    QFont font = QFont (fontFamily);

    font.setPointSize(fontSize);
    font.setBold(fontIsBold);
    font.setItalic(fontIsItalic);
    textEdit_field->setFont(font);
    ui->tabWidget->addTab(textEdit_field,"Untitled.md");
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget->setCurrentWidget(textEdit_field);
    textEdit_field->setFocus();
    highlighter = new MkdSyntax(textEdit_field->document(), 1000);
    on_actionFocus_Mode_triggered(); //if user opens new tab then set Focus mode on new tab as well.
    connect(textEdit_field->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()),Qt::UniqueConnection);
}

void MainWindow::on_actionSave_triggered()
{
    save();
}

void MainWindow::on_argOpenFile(const QString &fileName)
{
    QTextEdit *textEdit_field = new QTextEdit();
    QFont font = QFont(fontFamily);
    font.setPointSize(fontSize);
    font.setBold(fontIsBold);
    font.setItalic(fontIsItalic);
    textEdit_field->setFont(font);
    if (!fileName.isEmpty()) {
        ui->tabWidget->addTab(textEdit_field,fileName);
        ui->tabWidget->setTabsClosable(true);
        ui->tabWidget->setCurrentWidget(textEdit_field);
        textEdit_field->setFocus();
        loadFile(fileName);
    }
    on_actionFocus_Mode_triggered();
    connect(textEdit_field->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()),Qt::UniqueConnection);
}


void MainWindow::on_actionOpen_triggered()
{

        QString fileName = QFileDialog::getOpenFileName(this);
        QTextEdit *textEdit_field = new QTextEdit();
        QFont font = QFont(fontFamily);
        font.setPointSize(fontSize);
        font.setBold(fontIsBold);
        font.setItalic(fontIsItalic);
        textEdit_field->setFont(font);
        if (!fileName.isEmpty()) {
            ui->tabWidget->addTab(textEdit_field,fileName);
            ui->tabWidget->setTabsClosable(true);
            ui->tabWidget->setCurrentWidget(textEdit_field);
            textEdit_field->setFocus();
            loadFile(fileName);
        }
        on_actionFocus_Mode_triggered();
        connect(textEdit_field->document(), SIGNAL(contentsChanged()),
                this, SLOT(documentWasModified()),Qt::UniqueConnection);
}


/**
 * @brief MainWindow::on_actionExit_triggered - Action for exiting app properly
 */
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionFont_triggered()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    te->setFont(QFontDialog::getFont(0, te->font()));
    writeSettings();
    readSettings();
}

void MainWindow::on_actionWordWrap_triggered()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    if(ui->actionWordWrap->isChecked()) {
        te->setWordWrapMode(QTextOption::WordWrap);
    }
    else
        te->setWordWrapMode(QTextOption::NoWrap);
}


void MainWindow::on_actionFind_triggered()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    m_findDialog = new FindDialog();
    QPalette dark;
    dark.setColor(QPalette::Text, Qt::white);
    m_findDialog->setModal(false);
    m_findDialog->setTextEdit(te);
    m_findDialog->setStyleSheet("FindDialog {background:#302F2F; color:#FFFFFF;} QLabel {color:#FFFFFF; } QGroupBox {color:#FFFFFF;} QRadioButton, QCheckBox{background:#302F2F; color:#FFFFFF; font:bold;} QRadioButton:hover, QCheckBox:hover{background:#2D81FF; color:#000000; font:bold;}");
    m_findDialog->show();
}

void MainWindow::on_actionFind_Replace_triggered()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    m_findReplaceDialog = new FindReplaceDialog();
    m_findReplaceDialog->setStyleSheet("FindReplaceDialog {background:#302F2F; color:#FFFFFF;} QLabel {color:#FFFFFF; } QGroupBox {color:#FFFFFF;} QRadioButton, QCheckBox{background:#302F2F; color:#FFFFFF; font:bold;} QRadioButton:hover, QCheckBox:hover{background:#2D81FF; color:#000000; font:bold;}");
    m_findReplaceDialog->setModal(false);
    m_findReplaceDialog->setTextEdit(te);
    m_findReplaceDialog->show();
}


void MainWindow::on_actionFull_Screen_triggered()
{
    if(isFullScreen()) {
        this->setWindowState(Qt::WindowMaximized);
    }
    else if(!isFullScreen()) {
       this->setWindowState(Qt::WindowFullScreen);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
  QWidget::keyPressEvent(e);
  if((e->key() == Qt::Key_F11 || e->key() == Qt::Key_Escape)  && isFullScreen()) { //Press Esc or F11 to exit
      this->setWindowState(Qt::WindowMaximized);
  }
  else if(e->key() == Qt::Key_F11 && !isFullScreen()) {
     this->setWindowState(Qt::WindowFullScreen);
  }
}


void MainWindow::on_actionFocus_Mode_triggered()
{
    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    if(ui->actionFocus_Mode->isChecked()){
        connect(te, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    }
    else {
        disconnect(te, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
        QList<QTextEdit::ExtraSelection> extraSelections;//empty list
        te->setExtraSelections(extraSelections);
    }
}

void MainWindow::highlightCurrentLine() {

    QTextEdit *te = qobject_cast<QTextEdit*>(ui->tabWidget->currentWidget());
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QColor lineColor = QColor(Qt::black).lighter(160);
    QTextCharFormat format;

    format.setBackground(lineColor);
    lineColor.setAlpha(40);
    selection.format = format;
    selection.format.setProperty(QTextFormat::FullWidthSelection,true);
    selection.cursor = te->textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
    te->setExtraSelections(extraSelections);
}

/**
 * @brief MainWindow::on_actionContact_triggered - Contact information
 */
void MainWindow::on_actionContact_triggered()
{
    QMessageBox msgBox;
    msgBox.setStyleSheet("QMessageBox {background-color: #302F2F;}");
    msgBox.setWindowTitle("Contact Developer");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText("<p style=\"color:#FFFFFF\">"+QApplication::applicationName()
                   + " Version " + QApplication::applicationVersion()+"<br>Developer : Bhavyanshu Parasher<br> Email : <a style=\"color:#5C67FF\" href=\"mailto:ask@bhavyanshu.me\">ask@bhavyanshu.me</a><br>Website : <a style=\"color:#5C67FF\" href=\"https://bhavyanshu.me\">https://bhavyanshu.me</a><br> Project Webpage : <a style=\"color:#5C67FF\" href=\"https://github.com/bhavyanshu/LightMd_Editor\">https://github.com/bhavyanshu/LightMd_Editor</a></p>");
    msgBox.exec();
}

/**
 * Toolbar operations End
 */
