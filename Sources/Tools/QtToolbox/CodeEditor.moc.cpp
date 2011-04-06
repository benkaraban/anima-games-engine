/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * Jérémie Comarmond, Didier Colin.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <QtToolbox/CodeEditor.moc.h>
#include <QtGui>
#include <QTextCursor>

namespace QtToolbox
{
QString tabStr ("    ");
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
 {
     setStyleSheet("QPlainTextEdit {background-color: rgb(230,230,230);} ");

     lineNumberArea = new LineNumberArea(this);

     connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
     connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
     connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

     updateLineNumberAreaWidth(0);
     highlightCurrentLine();
     setLineWrapMode(QPlainTextEdit::NoWrap);
 }
//-----------------------------------------------------------------------------
 int CodeEditor::lineNumberAreaWidth()
 {
     int digits = 1;
     int max = qMax(1, blockCount());
     while (max >= 10) {
         max /= 10;
         ++digits;
     }

     int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

     return space;
 }
//-----------------------------------------------------------------------------
 void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
 {
     setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
 }
//-----------------------------------------------------------------------------
 void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
 {
     if (dy)
         lineNumberArea->scroll(0, dy);
     else
         lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

     if (rect.contains(viewport()->rect()))
         updateLineNumberAreaWidth(0);
 }
//-----------------------------------------------------------------------------
 void CodeEditor::resizeEvent(QResizeEvent *e)
 {
     QPlainTextEdit::resizeEvent(e);

     QRect cr = contentsRect();
     lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
 }
//-----------------------------------------------------------------------------
 void CodeEditor::highlightCurrentLine()
 {
     QList<QTextEdit::ExtraSelection> extraSelections;

     if (!isReadOnly()) {
         QTextEdit::ExtraSelection selection;

         QColor lineColor = QColor(Qt::yellow).lighter(160);

         selection.format.setBackground(lineColor);
         selection.format.setProperty(QTextFormat::FullWidthSelection, true);
         selection.cursor = textCursor();
         selection.cursor.clearSelection();
         extraSelections.append(selection);
     }

     setExtraSelections(extraSelections);
 }
//-----------------------------------------------------------------------------
void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                          Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
void CodeEditor::indentSelection()
{
    QTextCursor cur = textCursor ();

    cur.beginEditBlock();

    QTextBlock selectedBlock = cur.block();

    if (cur.hasSelection() == false)
        cur.insertText(tabStr);
    else
    {
        QTextCursor cur = textCursor();
        QTextCharFormat format = cur.blockCharFormat ();


        int a = cur.anchor();
        int p = cur.position();
        
        if (a>p) {int32 t = a; a=p; p=t;}

        int memoA = a;
        int memoP  = p;
        
        

        cur.setPosition(a);
        cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
        a = cur.position();

  

        // on va au debut de la ligne
        cur.setPosition(a);
        cur.setPosition(p, QTextCursor::KeepAnchor);

        // on recupere le text
        QString str = cur.selection().toPlainText();
        // on split le text par ligne
        QStringList list = str.split("\n");

        //on insert tabStr au debut de chaque ligne
        for (int i = 0; i < list.count(); i++)
            list[i].insert(0,tabStr);
            
        //on place le nouveau text
        str=list.join("\n");
        cur.removeSelectedText();
        cur.insertText(str);
       
        //on reselectionne le text
        cur.setPosition(memoA+tabStr.size(), QTextCursor::MoveAnchor);
        cur.setPosition(memoP+tabStr.size()*list.count(), QTextCursor::KeepAnchor);
        cur.setBlockCharFormat (format);
        setTextCursor(cur);
    }
    cur.endEditBlock();
}
//-----------------------------------------------------------------------------
void CodeEditor::desindentSelection()
{
    QTextCursor cur = textCursor ();

    cur.beginEditBlock();

    QTextBlock selectedBlock = cur.block();

    if (cur.hasSelection() == false)
    {
        QTextCursor cur = textCursor();

        cur.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor, 1);
        cur.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor, 1);
        cur.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, 1);


        QString str = cur.selection().toPlainText();
        if (str.size() <= tabStr.size())
        {
            cur.removeSelectedText();
        }
        else
        {
            if (str.size() % tabStr.size() == 0)
            {
                cur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, str.size() - tabStr.size());
            }
            else
            {
                cur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, str.size() - (str.size() % tabStr.size()));
            }
            
            cur.removeSelectedText();
        }
    }
    else
    {
        QTextCursor cur = textCursor();
        QTextCharFormat format = cur.blockCharFormat ();

     

        int a = cur.anchor();
        int p = cur.position();
        
        if (a>p) {int32 t = a; a=p; p=t;}

        int memoA = a;
        int memoP  = p;
        
        

        cur.setPosition(a);
        cur.movePosition(QTextCursor::StartOfBlock,QTextCursor::MoveAnchor);
        a = cur.position();

  

        // on va au debut de la ligne
        cur.setPosition(a);
        cur.setPosition(p, QTextCursor::KeepAnchor);

        // on recupere le text
        QString str = cur.selection().toPlainText();
        // on split le text par ligne
        QStringList list = str.split("\n");

      
        int32 startmovePos = 0;
        int32 endmovepos = 0;
        for (int i = 0; i < list.count(); i++)
        {
            if (list[i].left(tabStr.size()) == tabStr)
            {
                list[i].remove(0, tabStr.size());
                if (i==0)
                {
                    startmovePos = tabStr.size();
                }
                else
                {
                    endmovepos +=tabStr.size();
                }
            }
        }

            
        //on place le nouveau text
        str=list.join("\n");
        cur.removeSelectedText();
        cur.insertText(str);
       
        //on reselectionne le text
        cur.setPosition(a, QTextCursor::MoveAnchor);
        cur.setPosition(p-startmovePos-endmovepos, QTextCursor::KeepAnchor);
        cur.setBlockCharFormat (format);
        setTextCursor(cur);
    }
    cur.endEditBlock();
}
//-----------------------------------------------------------------------------
void CodeEditor::keyPressEvent(QKeyEvent * e)
{
    if(e->key() == Qt::Key_Tab && e->modifiers() == Qt::NoModifier)
    {
        indentSelection();
        e->accept();
    }
    else if(e->key() == Qt::Key_Backtab)
    {
        desindentSelection();
        e->accept();
    }
    else
    {
        QPlainTextEdit::keyPressEvent(e);
    }
}
//-----------------------------------------------------------------------------
void CodeEditor::keyReleaseEvent(QKeyEvent * e)
{
    QPlainTextEdit::keyPressEvent(e);
}
//-----------------------------------------------------------------------------
void CodeEditor::focusOutEvent ( QFocusEvent * event )
{
    emit lostFocus();
    QPlainTextEdit::focusOutEvent(event);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace QtToolbox
