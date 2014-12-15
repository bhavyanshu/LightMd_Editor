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
#include "pmh_styleparser.h"

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

/**
 * @brief MainWindow::writeSettings - Writes settings to registry in win32 or writes to .config/LightMdEditor in *nix
 */
void MainWindow::writeSettings() {

    QSettings settings("LightMdEditor","LightMd");
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());

    if(te){
        QFont font = te->font();
        settings.setValue("font.family", font.family());
        settings.setValue("font.size", font.pointSize());
        settings.setValue("font.bold", font.bold());
        settings.setValue("font.italic", font.italic());
        settings.setValue("theme.style", themeSettingsSave);
        settings.setValue("theme.state", themeState);
    }
}

/**
 * @brief MainWindow::readSettings - Read registry settings in win32 or .config/LightMdEditor in *nix
 */
void MainWindow::readSettings() {

    QSettings settings("LightMdEditor","LightMd");
    fontFamily = settings.value("font.family", QString()).toString();
    fontSize = settings.value("font.size", 12).toInt();
    fontIsBold = settings.value("font.bold", false).toBool();
    fontIsItalic = settings.value("font.italic", false).toBool();
}

/**
 * @brief MainWindow::themeSettings - Specific to last used theme
 * @return QString type variable having theme.style name
 */
QString MainWindow::themeSettings() {
    QSettings settings("LightMdEditor","LightMd");
    themeSettingsSave = settings.value("theme.style").toString();
    themeState = settings.value("theme.state", false).toBool();
    return themeSettingsSave;
}

/**
 * @brief MainWindow::closeEvent - Handles the close event properly.
 * @param eve
 */
void MainWindow::closeEvent(QCloseEvent *eve)
{
    writeSettings();
    for(int i = 0; i< ui->tabWidget->count(); i++ )
    {
        //qDebug() << ui->tabWidget->widget(i);
        QPlainTextEdit* edit = qobject_cast<QPlainTextEdit*>(ui->tabWidget->widget(i));
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
    QPlainTextEdit* edit = qobject_cast<QPlainTextEdit*>(ui->tabWidget->widget(index));
    if(edit) {
        if(warnSave()==true){           //Before closing tab check if document is modified, if yes then ask to save.
            //qDebug() << filelocMap.value(index) << " Index of closing tab:" << index << "Current Index:" << ui->tabWidget->currentIndex();
            filelocMap.remove(index);
            this->ui->tabWidget->removeTab(index);
            if(index < ui->tabWidget->count()) // If closed tab is any but not the last tab in tabwidget
            {
                int i;
                 for(i=index;i<=ui->tabWidget->count();i++) {
                      //qDebug() << "Map Size:" << filelocMap.size() << "Index: " << i << " Value: " << filelocMap.value(i+1);
                      QString filename = filelocMap.value(i+1);
                      filelocMap.remove(i+1);
                      filelocMap.insert(i,filename);
                 }
                rollBackTab();
            }
            else if (index == ui->tabWidget->count()) //If closed tab is the last tab in tabwidget
            {
                QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
                if(te){
                    te->setFocus();
		    filelocMap.remove(index);
                }
                else
                    return;
            }
        }
    }
}

/**
 * @brief MainWindow::rollBackTab - Set focus on text field of previous tab (if exists)
 */
void MainWindow::rollBackTab()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    if(te){
        te->setFocus();
    }
    else
        return;
}

/**
 * @brief MainWindow::documentWasModified - Prints message to statusbar if text was modified
 */
void MainWindow::documentWasModified()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    //int index = ui->tabWidget->currentIndex();

    if(te->document()->isModified()) {
        statusBar()->showMessage(tr("Document has unsaved changes"), 2000);
    }
}

/**
 * @brief MainWindow::warnSave - Warns of Saving the file before quitting
 * @return bool value
 */
bool MainWindow::warnSave()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());

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

/**
 * @brief MainWindow::save - checks if saving the file first time or not
 * @return
 */
bool MainWindow::save()
{

    int index = ui->tabWidget->currentIndex();
    QString filename = ui->tabWidget->tabText(index);
    if (filename.compare("Untitled.md",Qt::CaseInsensitive)==0) { //That means file has never been saved before
        return saveAs(); //Save for the first time
    } else {
        //qDebug() << filelocMap.value(index);
        QString filename = filelocMap.value(index);
        return saveFile(filename);
    }
}

/**
 * @brief MainWindow::saveAs - Saves file for the first time
 * @return
 */
bool MainWindow::saveAs()
{
    QFileDialog dialog(this,
                       tr("Save File"),
                       QDir::currentPath(),
                       tr("Markdown (*.md)"));
    dialog.setDefaultSuffix(".md");
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (files.isEmpty())
        return false;

    return saveFile(files.at(0));
}

/**
 * @brief MainWindow::saveFile - Actually saves the file
 * @param fileName
 * @return
 */
bool MainWindow::saveFile(const QString &fileName)
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
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

    //filelocList.append(fileName);
    QFileInfo fileInfo(file);
    QString tabname = fileInfo.fileName();
    if(tabname.length() > 30) {
        tabname.truncate(30);
        tabname += QLatin1String("..");
    }
    int index = ui->tabWidget->currentIndex();
    filelocMap.insert(index, fileName);
    //qDebug() << filelocMap.value(index);
    ui->tabWidget->setTabText(index,tabname);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

/**
 * @brief MainWindow::loadFile - Loads the file in respective QPlainTextEdit
 * @param fileName
 */
void MainWindow::loadFile(const QString &fileName)
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
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

    int index = ui->tabWidget->currentIndex();
    filelocMap.insert(index, fileName);
    //qDebug() << filelocMap.value(index) << "at index:" << index << "New Size:" << filelocMap.size();
    highlighter = new HGMarkdownHighlighter(te->document(), 1000);
    if(themeState==false) {
        QString styleFilePath = ":/styles/markdown-dark.style";
        highlighter->getStylesFromStylesheet(styleFilePath, te);
    }
    else {
        QString styleFilePath = ":/styles/markdown-light.style";
        highlighter->getStylesFromStylesheet(styleFilePath, te);
    }
    highlighter->highlightNow();
    highlighter->parseAndHighlightNow();
    statusBar()->showMessage(tr("File loaded"), 2000);
}

/**
 * @brief MainWindow::selectedText - Gets the selected text
 * @return
 */
QString MainWindow::selectedText()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    return te->textCursor().selectedText();
}

/**
 * @brief MainWindow::replaceTextBy - Replaces text by a string
 * @param text
 */
void MainWindow::replaceTextBy( const QString& text )
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
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
        QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
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
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
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

/**
 * @brief MainWindow::on_actionTable_triggered - Generates table syntax for markdown
 */
void MainWindow::on_actionTable_triggered()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    QTextCursor cursor = te->textCursor();
    TableDialog tbd;
    int resultCode = tbd.exec();
    if(resultCode == QDialog::Accepted)
    {
        int i,j=0;
        const int rows = tbd.lineEdit1->text().toInt();
        const int column = tbd.lineEdit2->text().toInt();
        //qDebug() << rows;
        //qDebug() << column;
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
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
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
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
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

/**
 * @brief MainWindow::on_argOpenFile - Called when there are no command line arguments passed. Ex.- ~$ LightMdEditor filename.md
 * @param fileName - Pass the argument which is the name of the markdown file.
 */
void MainWindow::on_argOpenFile(const QString &fileName)
{
    QPlainTextEdit *textEdit_field = new QPlainTextEdit();
    QFont font = QFont(fontFamily);
    QFile file(fileName);
    QFileInfo fileInfo(file);
    font.setPointSize(fontSize);
    font.setBold(fontIsBold);
    font.setItalic(fontIsItalic);
    textEdit_field->setFont(font);

    QString tabname = fileInfo.fileName();
    if(tabname.length() > 30) {
        tabname.truncate(30);
        tabname += QLatin1String("..");
    }

    if (!fileName.isEmpty()) {
        ui->tabWidget->addTab(textEdit_field,tabname);
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
 * @brief MainWindow::on_noarg - Called when there are no command line arguments passed. Ex.- ~$ LightMdEditor
 */
void MainWindow::on_noarg()
{
    on_actionNew_triggered();
}

/**
 * @brief MainWindow::on_actionNew_triggered - Action for creating new file
 */
void MainWindow::on_actionNew_triggered()
{
    QPlainTextEdit *textEdit_field = new QPlainTextEdit();
    QFont font = QFont (fontFamily);

    QString tabname = "Untitled.md";

    font.setPointSize(fontSize);
    font.setBold(fontIsBold);
    font.setItalic(fontIsItalic);
    textEdit_field->setFont(font);
    ui->tabWidget->addTab(textEdit_field,tabname);
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget->setCurrentWidget(textEdit_field);
    int index = ui->tabWidget->currentIndex();
    filelocMap.insert(index, tabname);
    //qDebug() << filelocMap.value(index) << "at index:" << index;
    textEdit_field->setFocus();
    highlighter = new HGMarkdownHighlighter(textEdit_field->document());
    if(themeState==false) {
        QString styleFilePath = ":/styles/markdown-dark.style";
        highlighter->getStylesFromStylesheet(styleFilePath, textEdit_field);
    }
    else {
        QString styleFilePath = ":/styles/markdown-light.style";
        highlighter->getStylesFromStylesheet(styleFilePath, textEdit_field);
    }
    highlighter->highlightNow();
    highlighter->parseAndHighlightNow();
    on_actionFocus_Mode_triggered(); //if user opens new tab then set Focus mode on new tab as well.
    connect(textEdit_field->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()),Qt::UniqueConnection);
}

void MainWindow::on_actionSave_triggered()
{
    save();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open Markdown File"),
                QDir::currentPath(),
                tr("Markdown (*.md *.markdown *.mdown *.mkdn *.mkd *.mdwn *.mdtxt *.mdtext);;All files (*.*)"));
    QFile file(fileName);
    QMapIterator<int, QString> ix(filelocMap); //First check if file is  already opened up in any of the tabs
    bool propened=false; //we will just assume that it's not opened
    while (ix.hasNext()) {
         ix.next();
         if(ix.value() == fileName) {
	     //qDebug() << "Value :" << ix.value() << "File is open" << fileName;
             propened = true; //File is pr opened, now switch to that tab
	     break; //we don't wanna check the whole QMap and waste time.
         }
     }
    if(propened==false) {
    QPlainTextEdit *textEdit_field = new QPlainTextEdit();
    QFont font = QFont(fontFamily);
    QFileInfo fileInfo(file);
    font.setPointSize(fontSize);
    font.setBold(fontIsBold);
    font.setItalic(fontIsItalic);
    textEdit_field->setFont(font);

    QString tabname = fileInfo.fileName();
    if(tabname.length() > 30) {
        tabname.truncate(30);
        tabname += QLatin1String("..");
    }
    if (!fileName.isEmpty()) {
        ui->tabWidget->addTab(textEdit_field,tabname);
        ui->tabWidget->setTabsClosable(true);
        ui->tabWidget->setCurrentWidget(textEdit_field);
        textEdit_field->setFocus();
        loadFile(fileName);
    }
    on_actionFocus_Mode_triggered();
    connect(textEdit_field->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()),Qt::UniqueConnection);
    }
    else {
        int index;
        index = filelocMap.key(fileName);
        QPlainTextEdit* edit = qobject_cast<QPlainTextEdit*>(ui->tabWidget->widget(index));
        ui->tabWidget->setCurrentIndex(index);
        statusBar()->showMessage(tr("File is already open"), 2000);
        if(edit) {
            edit->setFocus();
        }
    }
}


/**
 * @brief MainWindow::on_actionExit_triggered - Action for exiting app properly
 */
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

/**
 * @brief MainWindow::on_actionFont_triggered - Helper dialog to change font. Reads all sys fonts.
 */
void MainWindow::on_actionFont_triggered()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    te->setFont(QFontDialog::getFont(0, te->font()));
    writeSettings();
    readSettings();
}

/**
 * @brief MainWindow::on_actionWordWrap_triggered - Enable/Disable word wrap
 */
void MainWindow::on_actionWordWrap_triggered()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    if(ui->actionWordWrap->isChecked()) {
        te->setWordWrapMode(QTextOption::WordWrap);
    }
    else
        te->setWordWrapMode(QTextOption::NoWrap);
}

/**
 * @brief MainWindow::on_actionFind_triggered - Find dialog to search for text
 */
void MainWindow::on_actionFind_triggered()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    m_findDialog = new FindDialog();
    QPalette dark;
    dark.setColor(QPalette::Text, Qt::white);
    m_findDialog->setModal(false);
    m_findDialog->setTextEdit(te);
    m_findDialog->setStyleSheet("FindDialog {background:#302F2F; color:#FFFFFF;} QLabel {color:#FFFFFF; } QGroupBox {color:#FFFFFF;} QRadioButton, QCheckBox{background:#302F2F; color:#FFFFFF; font:bold;} QRadioButton:hover, QCheckBox:hover{background:#2D81FF; color:#000000; font:bold;}");
    m_findDialog->show();
}

/**
 * @brief MainWindow::on_actionFind_Replace_triggered - Find replace dialog to search and replace text
 */
void MainWindow::on_actionFind_Replace_triggered()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    m_findReplaceDialog = new FindReplaceDialog();
    m_findReplaceDialog->setStyleSheet("FindReplaceDialog {background:#302F2F; color:#FFFFFF;} QLabel {color:#FFFFFF; } QGroupBox {color:#FFFFFF;} QRadioButton, QCheckBox{background:#302F2F; color:#FFFFFF; font:bold;} QRadioButton:hover, QCheckBox:hover{background:#2D81FF; color:#000000; font:bold;}");
    m_findReplaceDialog->setModal(false);
    m_findReplaceDialog->setTextEdit(te);
    m_findReplaceDialog->show();
}

/**
 * @brief MainWindow::on_actionFull_Screen_triggered - Checks whether running app in fullscreen or not when F11 pressed
 */
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

/**
 * @brief MainWindow::on_actionFocus_Mode_triggered - Checks for state of focus mode
 */
void MainWindow::on_actionFocus_Mode_triggered()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    if(ui->actionFocus_Mode->isChecked()){
        connect(te, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    }
    else {
        disconnect(te, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
        QList<QTextEdit::ExtraSelection> extraSelections;//empty list
        te->setExtraSelections(extraSelections);
    }
}

/**
 * @brief MainWindow::highlightCurrentLine - Implements focus mode by highlighting current line using cursor position
 */
void MainWindow::highlightCurrentLine() {

    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QColor lineColor;
    if(themeState==false) {
        lineColor = QColor(Qt::black).lighter(160);
    }
    else {
        lineColor = QColor(238,232,213);
    }
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
 * @brief MainWindow::on_switchTheme - Switches the icons in the toolbar depending on theme to resolve "dark over dark"/"light over light" conflict.
 * @param themeName
 */
void MainWindow::on_switchTheme(const QString &themeName)
{
    QString image_path = ":/images/theme_"+themeName+"/";

    QAction *act_1 = qobject_cast<QAction*>(ui->actionBold);
    QAction *act_2 = qobject_cast<QAction*>(ui->actionItalic);
    QAction *act_3 = qobject_cast<QAction*>(ui->actionAnchor_Link);
    QAction *act_4 = qobject_cast<QAction*>(ui->actionBlockquote);
    QAction *act_5 = qobject_cast<QAction*>(ui->actionCode_Block);
    QAction *act_6 = qobject_cast<QAction*>(ui->actionImage);
    QAction *act_7 = qobject_cast<QAction*>(ui->actionTable);
    act_1->setIcon(QIcon(image_path+"bold.png"));
    act_2->setIcon(QIcon(image_path+"italic.png"));
    act_3->setIcon(QIcon(image_path+"link.png"));
    act_4->setIcon(QIcon(image_path+"blockquote.png"));
    act_5->setIcon(QIcon(image_path+"codeblock.png"));
    act_6->setIcon(QIcon(image_path+"image.png"));
    act_7->setIcon(QIcon(image_path+"table.png"));

    QAction *act_8 = qobject_cast<QAction*>(ui->actionCut);
    QAction *act_9 = qobject_cast<QAction*>(ui->actionCopy);
    QAction *act_10 = qobject_cast<QAction*>(ui->actionPaste);
    act_8->setIcon(QIcon(image_path+"cut.png"));
    act_9->setIcon(QIcon(image_path+"copy.png"));
    act_10->setIcon(QIcon(image_path+"paste.png"));

    QAction *act_11 = qobject_cast<QAction*>(ui->actionNew);
    QAction *act_12 = qobject_cast<QAction*>(ui->actionOpen);
    QAction *act_13 = qobject_cast<QAction*>(ui->actionSave);
    act_11->setIcon(QIcon(image_path+"new.png"));
    act_12->setIcon(QIcon(image_path+"edit.png"));
    act_13->setIcon(QIcon(image_path+"save.png"));

    QAction *act_14 = qobject_cast<QAction*>(ui->actionFind);
    QAction *act_15 = qobject_cast<QAction*>(ui->actionFind_Replace);
    QAction *act_16 = qobject_cast<QAction*>(ui->actionFont);
    act_14->setIcon(QIcon(image_path+"find.png"));
    act_15->setIcon(QIcon(image_path+"findreplace.png"));
    act_16->setIcon(QIcon(image_path+"font.png"));

}

/**
 * @brief MainWindow::on_actionTheme_Toggle_triggered - Actual implementation of changing the theme.
 */
void MainWindow::on_actionTheme_Toggle_triggered()
{
    QPlainTextEdit *te = qobject_cast<QPlainTextEdit*>(ui->tabWidget->currentWidget());
    //qDebug() << themeState;
    if(themeState==true)
    {
        themeSettingsSave = "dark";
        on_switchTheme("dark"); //ThemeName dark means light icons will be set
        QFile styleFile_dark(":/styles/dark.qss");
        styleFile_dark.open(QFile::ReadOnly);
        QString style(styleFile_dark.readAll());
        styleFile_dark.close();
        QString styleFilePath = ":/styles/markdown-dark.style";
        highlighter->getStylesFromStylesheet(styleFilePath, te);
        highlighter->highlightNow();
        highlighter->parseAndHighlightNow();
        this->setStyleSheet(style);
        themeState=false;
    }
    else
    {
        themeSettingsSave = "light";
        on_switchTheme("light"); //ThemeName light means dark icons will be set
        QFile styleFile_light(":/styles/light.qss");
        styleFile_light.open(QFile::ReadOnly);
        QString style(styleFile_light.readAll());
        styleFile_light.close();
        QString styleFilePath = ":/styles/markdown-light.style";
        highlighter->getStylesFromStylesheet(styleFilePath, te);
        highlighter->highlightNow();
        highlighter->parseAndHighlightNow();
        this->setStyleSheet(style);
        themeState=true;
    }
}
