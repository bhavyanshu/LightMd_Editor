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

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QTextCharFormat>
#include <QThread>

extern "C" {
#include "pmh_parser.h"
}

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class WorkerThread : public QThread
{
public:
    ~WorkerThread();
    void run();
    char *content;
    pmh_element **result;
};

struct HighlightingStyle
{
    pmh_element_type type;
    QTextCharFormat format;
};


class MkdSyntax : public QObject
{
    Q_OBJECT

public:
    MkdSyntax(QTextDocument *parent = 0, int aWaitInterval = 2000);
    void setStyles(QVector<HighlightingStyle> &styles);
    int waitInterval;

protected:

private slots:
    void handleContentsChange(int position, int charsRemoved, int charsAdded);
    void threadFinished();
    void timerTimeout();

private:
    WorkerThread *childThread;

    bool parsePending;
    pmh_element **cached_elements;
    QVector<HighlightingStyle> *syntaxStyle;
    QTimer *timer;
    QTextDocument *document;

    void clearFormatting();
    void highlight();
    void parse();
    void setDefaultStyles();
};

#endif
