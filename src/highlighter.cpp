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
#include "highlighter.h"

MkdSyntax::MkdSyntax(QTextDocument *parent,int interv) : QObject(parent)
{
    syntaxStyle = NULL;
    childThread = NULL;
    cached_elements = NULL;
    waitInterval = interv;
    timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(interv);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    document = parent;
    connect(document, SIGNAL(contentsChange(int,int,int)),
            this, SLOT(handleContentsChange(int,int,int)));

    this->parse();
}

void MkdSyntax::setStyles(QVector<HighlightingStyle> &styles)
{
    this->syntaxStyle = &styles;
}


#define STY(type, format) styles->append((HighlightingStyle){type, format})
void MkdSyntax::setDefaultStyles()
{
    QVector<HighlightingStyle> *styles = new QVector<HighlightingStyle>();

    QTextCharFormat headers; headers.setForeground(QBrush(QColor(108,113,196)));
    STY(pmh_H1, headers);

    STY(pmh_H2, headers);

    STY(pmh_H3, headers);

    STY(pmh_H4, headers);

    STY(pmh_H5, headers);

    STY(pmh_H6, headers);

    QTextCharFormat hrule; hrule.setForeground(QBrush(QColor(255,255,69)));
    hrule.setBackground(QBrush(QColor(48,47,47)));
    STY(pmh_HRULE, hrule);

    QTextCharFormat list; list.setForeground(QBrush(QColor(247,187,0)));
    STY(pmh_LIST_BULLET, list);
    STY(pmh_LIST_ENUMERATOR, list);

    QTextCharFormat link; link.setForeground(QBrush(Qt::darkCyan));
    link.setBackground(QBrush(QColor(48,47,47)));
    STY(pmh_LINK, link);
    STY(pmh_AUTO_LINK_URL, link);
    STY(pmh_AUTO_LINK_EMAIL, link);

    QTextCharFormat code; code.setForeground(QBrush(QColor(250,115,115)));
    code.setBackground(QBrush(QColor(82,81,81)));
    STY(pmh_CODE, code);
    STY(pmh_VERBATIM, code);

    QTextCharFormat emph; emph.setForeground(QBrush(QColor(52,255,69)));
    emph.setFontItalic(true);
    STY(pmh_EMPH, emph);

    QTextCharFormat strong; strong.setForeground(QBrush(QColor(52,255,69)));
    strong.setFontWeight(QFont::Bold);
    STY(pmh_STRONG, strong);

    QTextCharFormat comment; comment.setForeground(QBrush(Qt::gray));
    STY(pmh_COMMENT, comment);

    QTextCharFormat blockquote; blockquote.setForeground(QBrush(QColor(255,48,48)));
    STY(pmh_BLOCKQUOTE, blockquote);

    QTextCharFormat image; image.setForeground(QBrush(Qt::darkCyan));
    image.setBackground(QBrush(QColor(48,47,47)));
    STY(pmh_IMAGE, image);

    QTextCharFormat ref; ref.setForeground(QBrush(QColor(213,178,178)));
    STY(pmh_REFERENCE, ref);

    this->setStyles(*styles);
}

void MkdSyntax::clearFormatting()
{
    QTextBlock block = document->firstBlock();
    while (block.isValid()) {
        block.layout()->clearAdditionalFormats();
        block = block.next();
    }
}

void MkdSyntax::highlight()
{
    if (cached_elements == NULL) {
        return;
    }
    if (syntaxStyle == NULL)
        this->setDefaultStyles();

    this->clearFormatting();

    for (int i = 0; i < syntaxStyle->size(); i++)
    {
        HighlightingStyle style = syntaxStyle->at(i);
        pmh_element *elem_cursor = cached_elements[style.type];
        while (elem_cursor != NULL)
        {
            if (elem_cursor->end <= elem_cursor->pos) {
                elem_cursor = elem_cursor->next;
                continue;
            }
            int startBlockNum = document->findBlock(elem_cursor->pos).blockNumber();
            int endBlockNum = document->findBlock(elem_cursor->end).blockNumber();
            for (int j = startBlockNum; j <= endBlockNum; j++)
            {
                QTextBlock block = document->findBlockByNumber(j);
                QTextLayout *layout = block.layout();
                QList<QTextLayout::FormatRange> list = layout->additionalFormats();
                int blockpos = block.position();
                QTextLayout::FormatRange r;
                r.format = style.format;

                if (j == startBlockNum) {
                    r.start = elem_cursor->pos - blockpos;
                    r.length = (startBlockNum == endBlockNum)
                                ? elem_cursor->end - elem_cursor->pos
                                : block.length() - r.start;
                } else if (j == endBlockNum) {
                    r.start = 0;
                    r.length = elem_cursor->end - blockpos;
                } else {
                    r.start = 0;
                    r.length = block.length();
                }

                list.append(r);
                layout->setAdditionalFormats(list);
            }

            elem_cursor = elem_cursor->next;
        }
    }

    document->markContentsDirty(0, document->characterCount());
}

void MkdSyntax::parse()
{
    if (childThread != NULL && childThread->isRunning()) {
        parsePending = true;
        return;
    }

    QString content = document->toPlainText();
    QByteArray ba = content.toLatin1();
    char *content_cstring = strdup((char *)ba.data());

    if (childThread != NULL)
        delete childThread;
    childThread = new WorkerThread();
    childThread->content = content_cstring;
    connect(childThread, SIGNAL(finished()), this, SLOT(threadFinished()));
    parsePending = false;
    childThread->start();
}

void MkdSyntax::threadFinished()
{
    if (parsePending) {
        this->parse();
        return;
    }

    if (cached_elements != NULL)
        pmh_free_elements(cached_elements);
    cached_elements = childThread->result;
    childThread->result = NULL;

    this->highlight();
}

void MkdSyntax::handleContentsChange(int position, int charsRemoved,
                                                 int charsAdded)
{
    if (charsRemoved == 0 && charsAdded == 0)
        return;
    timer->stop();
    timer->start();
}

void MkdSyntax::timerTimeout()
{
    this->parse();
}

WorkerThread::~WorkerThread()
{
    if (result != NULL)
        pmh_free_elements(result);
    free(content);
}
void WorkerThread::run()
{
    if (content == NULL)
        return;
    pmh_markdown_to_elements(content, pmh_EXT_NONE, &result);
}
