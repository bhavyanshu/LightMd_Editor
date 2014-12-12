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
#include <QApplication>
#include <QSettings>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args = a.arguments();
    a.setWindowIcon(QIcon(":/lightmdeditor.ico"));
    a.setOrganizationName("Bhavyanshu Parasher");
    a.setOrganizationDomain("https://bhavyanshu.me");
    a.setApplicationName("LightMd Editor");
    a.setApplicationVersion("1.0-3");
    MainWindow w;
    w.readSettings();
    QString theme;
    theme = w.themeSettings();
    QFile styleFile;
    if(theme.isEmpty()) {
        styleFile.setFileName(":/styles/dark.qss"); //If app is run the first time
    }
    else {
        styleFile.setFileName(":/styles/"+theme+".qss"); //Get theme name from last save settings
        w.on_switchTheme(theme);
    }

    styleFile.open(QFile::ReadOnly);
    QString style(styleFile.readAll());
    styleFile.close();
    w.setStyleSheet(style);

    w.showMaximized();
    w.show();
    if (args.count() > 1) { //Opening a file
        w.on_argOpenFile(args[1]);
    }
    else { //Gonna create a new file
        w.on_noarg();
    }
    return a.exec();
}
