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
#ifndef EPI_OUTPUT_WIDGET_H_
#define EPI_OUTPUT_WIDGET_H_

#include <QWidget>

#include <Core/Standard.h>
#include <Core/List.h>

#include <Core/IO/BufferStream.h>
#include <Core/IO/TextStream.h>
#include <QTextCharFormat>

class QTextEdit;
class QPlainTextEdit;


namespace QtToolbox
{
    class ComboBox;
    class OutputWidgetLoggerStream;
}

namespace EPI
{
class LuaOutput;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI OutputWidget : public QWidget
{
    Q_OBJECT

public:
    OutputWidget(QWidget* parent);
    virtual ~OutputWidget();
    void connectLuaOutput(const LuaOutput& luaOut);

protected:
    void setupUi();

private Q_SLOTS:
    void activeOutputChanged();

    void newLogMsg(const Core::String& msg);
    void receiveLuaOutputMsg(const String& str);
    void receiveLuaErrorMsg(const String& str);
    void clearText();
    
private:
    Ptr<QtToolbox::OutputWidgetLoggerStream>   pLoggerStream;

    QWidget *       _pToolBar;
    QPushButton *   _pClear;
    QTextEdit *     _pText;
    QPlainTextEdit *     _pTextLog;
    bool            _writeLog;
    bool            _writeScript;
    QtToolbox::ComboBox*       _activeOutput;

    static QTextCharFormat _sOutputMsgFormat;
    static QTextCharFormat _sErrorMsgFormat;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace EPI

#endif EPI_OUTPUT_WIDGET_H_