
/*
 * CodeQuery
 * Copyright (C) 2013 ruben2020 https://github.com/ruben2020/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>
#include <QtDesigner/QDesignerExportWidget>
#include "fileviewer.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;
class fileviewer;

#ifdef NO_QDESIGNER_WIDGET_EXPORT
class CodeEditor : public QPlainTextEdit
#else
class QDESIGNER_WIDGET_EXPORT CodeEditor : public QPlainTextEdit
#endif
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void highlightLine(int linenum = 0);
    fileviewer *m_fileviewer;

protected:
    /*
// OTher events that we'd be able to override then get a callback on
virtual void 	changeEvent ( QEvent * e )
virtual void 	contextMenuEvent ( QContextMenuEvent * event )
virtual void 	dragEnterEvent ( QDragEnterEvent * e )
virtual void 	dragLeaveEvent ( QDragLeaveEvent * e )
virtual void 	dragMoveEvent ( QDragMoveEvent * e )
virtual void 	dropEvent ( QDropEvent * e )
virtual void 	focusInEvent ( QFocusEvent * e )
virtual bool 	focusNextPrevChild ( bool next )
virtual void 	focusOutEvent ( QFocusEvent * e )
virtual void 	inputMethodEvent ( QInputMethodEvent * e )
virtual QVariant 	inputMethodQuery ( Qt::InputMethodQuery property ) const
virtual void 	keyPressEvent ( QKeyEvent * e )
virtual void 	keyReleaseEvent ( QKeyEvent * e )
virtual void 	mouseDoubleClickEvent ( QMouseEvent * e )
virtual void 	mouseMoveEvent ( QMouseEvent * e )
virtual void 	mousePressEvent ( QMouseEvent * e )
virtual void 	mouseReleaseEvent ( QMouseEvent * e )
virtual void 	paintEvent ( QPaintEvent * e )
virtual void 	resizeEvent ( QResizeEvent * e )
virtual void 	scrollContentsBy ( int dx, int dy )
virtual void 	showEvent ( QShowEvent * )
virtual void 	wheelEvent ( QWheelEvent * e )
*/
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
    int m_HighlightedLineNumber;
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor)
    {
        codeEditor = editor;
    }

    QSize sizeHint() const
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event)
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};


#endif

