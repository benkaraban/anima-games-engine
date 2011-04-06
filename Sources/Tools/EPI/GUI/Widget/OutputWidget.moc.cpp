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
#include "OutputWidget.moc.h"

#include <QTextEdit>
#include <QPushButton>
#include <QAction>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QStringList>
#include <QRegExp>
#include <QTextCharFormat>
#include <QVector>
#include <algorithm>
#include <QPlainTextEdit>

#include <QtToolBox/ComboBox.moc.h>
#include <QtToolBox/OutputWidgetLoggerStream.moc.h>

#include <EPI/Document/LuaOutput.moc.h>


namespace EPI
{

LM_ENUM_2 (OutputType, 
           OUTPUT_LOG, 
           OUTPUT_SCRIPT)

class LogSyntaxHighlighter : public QSyntaxHighlighter
{
public:
    LogSyntaxHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat _logInfFormat;
    QTextCharFormat _logWarFormat;
    QTextCharFormat _logErrFormat;
    QTextCharFormat _logDbgFormat;
    QTextCharFormat classFormat;
};


LogSyntaxHighlighter::LogSyntaxHighlighter(QTextDocument *parent) : 
    QSyntaxHighlighter(parent)
{
    _logInfFormat.setForeground( QBrush (QColor(30,30,30)));
    _logWarFormat.setForeground(Qt::darkCyan);
    _logErrFormat.setForeground(Qt::red);
    _logDbgFormat.setForeground(Qt::darkMagenta);

    _logInfFormat.setFontWeight(QFont::Bold);
    _logWarFormat.setFontWeight(QFont::Bold);
    _logErrFormat.setFontWeight(QFont::Bold);
    _logDbgFormat.setFontWeight(QFont::Bold);

    HighlightingRule rule;

    rule.pattern = QRegExp("\\bINF\\b");
    rule.format = _logInfFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegExp("\\bWAR\\b");
    rule.format = _logWarFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegExp("\\bERR\\b");
    rule.format = _logErrFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegExp("\\bDBG\\b");
    rule.format = _logDbgFormat;
    highlightingRules.append(rule);
}

void LogSyntaxHighlighter::highlightBlock(const QString &text)
{
    for (int32 ii=0; ii<highlightingRules.size(); ++ii)
    {
         HighlightingRule rule = highlightingRules[ii];
         QRegExp expression(rule.pattern);
         int index = text.indexOf(expression);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = text.indexOf(expression, index + length);
         }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QTextCharFormat OutputWidget::_sOutputMsgFormat;
QTextCharFormat OutputWidget::_sErrorMsgFormat;

OutputWidget::OutputWidget(QWidget* parent) :
    _writeScript(false),
    _writeLog(false)
{
    pLoggerStream = Ptr<QtToolbox::OutputWidgetLoggerStream> (new QtToolbox::OutputWidgetLoggerStream());
    connect(pLoggerStream.get(), SIGNAL(newText(const Core::String&)), this, SLOT(newLogMsg(const Core::String&)));

    Ptr<Core::TextOutputStream> tos ( new Core::TextOutputStream(pLoggerStream, true));
    Core::LoggerManager::getInstance().getDefaultLogger()->addStream(tos, L"loggerOutputWidget");
  //  Core::LoggerManager::getInstance().getDefaultLogger()->setDefaultMsgLevel(Core::L_DEBUG);
    setupUi();

    _activeOutput->selectIndex((int32)OUTPUT_SCRIPT);
    //_activeOutput->selectIndex((int32)OUTPUT_LOG);
    LogSyntaxHighlighter*   logSyntax = new LogSyntaxHighlighter(_pTextLog->document());
/*
    DBG << L"test ";
    ERR << L"test ";
    WAR << L"test ";
    INF << L"test ";
*/
    /*_pActiveOutputScript->setChecked(true);
    _pTextLog->setVisible(false);
    _pText->setVisible(true);*/
}
//-----------------------------------------------------------------------------
OutputWidget::~OutputWidget()
{
    Core::LoggerManager::getInstance().getDefaultLogger()->destroyStream(L"loggerOutputWidget");
}
//-----------------------------------------------------------------------------
void OutputWidget::setupUi()
{
    QGridLayout*    mainLayout = new QGridLayout(NULL);
        mainLayout->setContentsMargins(0,0,0,0);
        mainLayout->setSpacing(0);

    _pToolBar = new QWidget(this);
        QGridLayout* toolBarLayout = new QGridLayout(this);
        _pToolBar->setLayout(toolBarLayout);

    _activeOutput = new QtToolbox::ComboBox(this, "from: ");
        ADD_ITEM_COMBOBOX_LM_ENUM_NAMESPACE (_activeOutput, EPI, EPI::OutputType, 7);

    _pClear = new QPushButton(QIcon(":/icons/clearText.png"), "", this);
        _pClear->setFixedSize(40,20);
        _pClear->setFlat(true);

    _pText    = new QTextEdit(this);
        _pText->setReadOnly(true);
        _pText->setLineWrapMode(QTextEdit::NoWrap);

    _pTextLog    = new QPlainTextEdit(this);
        _pTextLog->setReadOnly(true);
        _pTextLog->setLineWrapMode(QPlainTextEdit::NoWrap);
        QBrush brush_script(QColor(0,0,0));
            QTextCharFormat format;
            format.setFont(QFont("Consolas", 8));
            format.setForeground(brush_script);
        _pTextLog->setCurrentCharFormat(format);
        _pTextLog->setBackgroundVisible(false);


    toolBarLayout->addWidget(_activeOutput,         0,  0, 1, 1,    Qt::AlignLeft);
    toolBarLayout->addWidget(_pClear,               0,  1, 1, 100,  Qt::AlignRight);


    mainLayout->addWidget(_pToolBar);
    mainLayout->addWidget(_pText);
    mainLayout->addWidget(_pTextLog);

    connect(_activeOutput,          SIGNAL(valueUpdated()), this, SLOT(activeOutputChanged()));
    connect(_pClear, SIGNAL(clicked()), this, SLOT(clearText()));

    ///////////
    QBrush brush_error(QColor(255,0,0));
    OutputWidget::_sErrorMsgFormat.setFont(QFont("Consolas", 8));
    OutputWidget::_sErrorMsgFormat.setForeground(brush_error);

    QBrush brush_output(QColor(20,20,20));
    OutputWidget::_sOutputMsgFormat.setFont(QFont("Consolas", 8));
    OutputWidget::_sOutputMsgFormat.setForeground(brush_output);
    /////////


    setLayout(mainLayout);
}
//-----------------------------------------------------------------------------
void OutputWidget::activeOutputChanged()
{
    EPI::OutputType type = (EPI::OutputType) _activeOutput->selectedIndex();

    if (type == OUTPUT_LOG)
    {
        _writeScript = false;
        _writeLog    = true;
    }
    else
    {
        _writeScript = true;
        _writeLog    = false;
    }

    _pTextLog->setVisible(_writeLog);
    _pText->setVisible(_writeScript);
}
//-----------------------------------------------------------------------------
void OutputWidget::newLogMsg(const Core::String& msg)
{
    _pTextLog->insertPlainText(String8(msg).c_str());
}
//-----------------------------------------------------------------------------
void OutputWidget::receiveLuaOutputMsg(const String& str)
{
    if (_writeScript == true)
    {
        _pText->setCurrentCharFormat(_sOutputMsgFormat);
        _pText->insertPlainText(String8(str+L"\n").c_str());
    }
}
//-----------------------------------------------------------------------------
void OutputWidget::receiveLuaErrorMsg(const String& str)
{
    if (_writeScript == true)
    {
        _pText->setCurrentCharFormat(_sErrorMsgFormat);
        _pText->insertPlainText(String8(str+L"\n").c_str());
    }
}
//-----------------------------------------------------------------------------
void OutputWidget::connectLuaOutput(const LuaOutput& luaOut)
{
    //TODO voir si on gere la sortie du script suivant le document Actif ou si tous documents
    connect (&luaOut, SIGNAL(newOutputMsg(const String& )), this, SLOT(receiveLuaOutputMsg(const String& )));
    connect (&luaOut, SIGNAL(newErrorMsg(const String& )), this, SLOT(receiveLuaErrorMsg(const String& )));
}
//-----------------------------------------------------------------------------
void OutputWidget::clearText()
{
    if (_writeLog == true)
    {
        _pTextLog->clear();
    }

    if (_writeScript == true)
    {
        _pText->clear();
    }
}
//-----------------------------------------------------------------------------
} // namespace EPI