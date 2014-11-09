#-------------------------------------------------
# Copyright 2014 Bhavyanshu Parasher
# This file is part of "LightMd Editor".
# "LightMd Editor" is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
# "LightMd Editor" is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with "LightMd Editor".
# If not, see http://www.gnu.org/licenses/.
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LightMdEditor
TEMPLATE = app
CONFIG += warn_on

DESTDIR = ../bin
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
unix:OBJECTS_DIR = ../build/o/unix
win32:OBJECTS_DIR = ../build/o/win32
macx:OBJECTS_DIR = ../build/o/mac

SOURCES += main.cpp\
           mainwindow.cpp \
           pmh_parser.c \
           highlighter.cpp \
           tabledialog.cpp \
    findreplace/finddialog.cpp \
    findreplace/findform.cpp \
    findreplace/findreplacedialog.cpp \
    findreplace/findreplaceform.cpp \
    pmh_styleparser.c

HEADERS  += mainwindow.h \
            pmh_parser.h \
            pmh_definitions.h \
            highlighter.h \
            tabledialog.h \
    findreplace/finddialog.h \
    findreplace/findform.h \
    findreplace/findreplacedialog.h \
    findreplace/findreplaceform.h \
    findreplace/findreplace_global.h \
    pmh_styleparser.h

FORMS    += mainwindow.ui \
    findreplace/findreplacedialog.ui \
    findreplace/findreplaceform.ui

RESOURCES += \
    LightMdEditor.qrc

macx {
    ICON = macx/lightmd.cns
    QMAKE_INFO_PLIST = macx/info.plist
    QMAKE_LFLAGS += -F$${DESTDIR}/$${TARGET}.app/Contents/Frameworks
}
else:win32 {
    RC_FILE = win32/lmd.rc
}
