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


SOURCES += main.cpp\
           mainwindow.cpp \
           pmh_parser.c \
           highlighter.cpp \
           tabledialog.cpp

HEADERS  += mainwindow.h \
            pmh_parser.h \
            pmh_definitions.h \
            highlighter.h \
            tabledialog.h

FORMS    += mainwindow.ui

RESOURCES += \
    LightMdEditor.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib -lqtfindreplacedialog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib -lqtfindreplacedialog
else:unix: LIBS += -L$$PWD/lib/ -lqtfindreplacedialog

DEPENDPATH += $$PWD/lib
INCLUDEPATH += ./dialogs
QTFINDREPLACE_LIB = libqtfindreplacedialog.*
contains(COMPILER, msvc) {
    QTFINDREPLACE_LIB = qtfindreplacedialog.lib
}

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libqtfindreplacedialog.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libqtfindreplacedialogd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/qtfindreplacedialog.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/qtfindreplacedialogd.lib
else:unix: PRE_TARGETDEPS += $$PWD/lib/libqtfindreplacedialog.a

macx {
    ICON = macx/lightmd.cns
    QMAKE_INFO_PLIST = macx/info.plist
    QMAKE_LFLAGS += -F$${DESTDIR}/$${TARGET}.app/Contents/Frameworks
}
else:win32 {
    RC_FILE = win32/lmd.rc
}
else:unix {
    target.path = $${INSTALL_PREFIX}/bin
    icons.path = $${INSTALL_PREFIX}/share/icons/scalable/apps
    icons.files = x11/icons/scalable/lmd.svg
    icons16.path = $${INSTALL_PREFIX}/share/icons/hicolor/16x16/apps
    icons16.files = x11/icons/16x16/lmd.png
    icons32.path = $${INSTALL_PREFIX}/share/icons/hicolor/32x32/apps
    icons32.files = x11/icons/32x32/lmd.png
    icons48.path = $${INSTALL_PREFIX}/share/icons/hicolor/48x48/apps
    icons48.files = x11/icons/48x48/lmd.png
    icons128.path = $${INSTALL_PREFIX}/share/icons/hicolor/128x128/apps
    icons128.files = x11/icons/128x128/lmd.png
    icons256.path = $${INSTALL_PREFIX}/share/icons/hicolor/256x256/apps
    icons256.files = x11/icons/256x256/lmd.png
    desktop.path = $${INSTALL_PREFIX}/share/applications
    desktop.files = x11/lmd.desktop
    INSTALLS += target \
        icons \
        icons16 \
        icons32 \
        icons48 \
        icons128 \
        icons256 \
        desktop
}
