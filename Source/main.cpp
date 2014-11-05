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
    a.setWindowIcon(QIcon("app.ico"));
    a.setOrganizationName("Bhavyanshu Parasher");
    a.setOrganizationDomain("https://bhavyanshu.me");
    a.setApplicationName("LightMd Editor");
    a.setApplicationVersion("1.0.0-beta");
    MainWindow w;
    w.readSettings();
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
