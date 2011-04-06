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
#include "PtyScript.moc.h"
#include <QVBoxLayout>

#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QSizePolicy>

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/LuaSyntaxHighlighter.h>
#include <QtToolbox/CodeEditor.moc.h>

#include <Universe/World.h>

namespace EPI
{
//-----------------------------------------------------------------------------
class CmdLineEventFilter : public QObject
{
public:
    CmdLineEventFilter(PtyWidgetScript* pPtyW, QObject* pParent = 0) : QObject(pParent), _pPtyW(pPtyW) {}
    virtual bool eventFilter(QObject* pWatched, QEvent* pEvent)
    {
        if (pEvent->type() == QEvent::KeyRelease)
        {
            QKeyEvent *keyEvent = (QKeyEvent*)(pEvent);
            
            if (keyEvent->key() == Qt::Key_Up)
            {
                _pPtyW->retrievePreviousCmdLine();
                return true;
            }

            if (keyEvent->key() == Qt::Key_Down)
            {
                _pPtyW->retrieveNextCmdLine();
                return true;
            }
           
        }

        return false;
    }

private:
    PtyWidgetScript*  _pPtyW;
};
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyScript::PtyScript(const Ptr<IWorldScriptable>& pWorldScriptable, const Core::String& title):
    PropertyNode(title, true, false, CONTENT),
    _pWorldScriptable(pWorldScriptable),
    _pText(NULL),
    _pLuaHighlighter(NULL)
{
    QBrush brush_script(QColor(0,0,0));
    QTextCharFormat format;
    format.setFont(QFont("Consolas", 8));
    format.setForeground(brush_script);


    _pText = new QtToolbox::CodeEditor(NULL);

    _pText->setCurrentCharFormat(format);
    _pText->setBackgroundVisible(false);
    _pLuaHighlighter = new QtToolbox::LuaSyntaxHighlighter(_pText->document());

    updateProperty();
}
//-----------------------------------------------------------------------------
PtyScript::PtyScript(const PtyScript& other) : 
    PropertyNode(other),
    _pWorldScriptable(other._pWorldScriptable),
    _pText(NULL),
    _pLuaHighlighter(NULL)
{
    QBrush brush_script(QColor(0,0,0));
    QTextCharFormat format;
    format.setFont(QFont("Consolas", 8));
    format.setForeground(brush_script);


    _pText = new QtToolbox::CodeEditor(NULL);

    _pText->setCurrentCharFormat(format);
    _pText->setBackgroundVisible(false);
    _pLuaHighlighter = new QtToolbox::LuaSyntaxHighlighter(_pText->document());
}
//-----------------------------------------------------------------------------
PtyScript::~PtyScript()
{
    delete _pLuaHighlighter;
    delete _pText;
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyScript::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetScript> pPW (new PtyWidgetScript(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyScript::updateData()
{

}
//-----------------------------------------------------------------------------
void PtyScript::updateProperty()
{
    emit propertyChanged();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyScript::clone() const
{
    Ptr<PtyScript> ret (new PtyScript( *this ));
    
    QString text = _pText->toPlainText();
    ret->_pText->setPlainText( text );

    return ret;
}
//-----------------------------------------------------------------------------
void PtyScript::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyScript> pPty = LM_DEBUG_PTR_CAST<PtyScript>(pSrc);

    _pWorldScriptable = pPty->_pWorldScriptable;
    QString text = pPty->_pText->toPlainText();
    _pText->setPlainText( text );


    _pText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    updateData();
}
//-----------------------------------------------------------------------------
void PtyScript::setScript(const Core::String8& script)
{
    _pText->setPlainText(script.c_str());
    updateData();
}
//-----------------------------------------------------------------------------
const Ptr<IWorldScriptable>& PtyScript::getWorldScriptable() const
{
    return _pWorldScriptable;
}
//-----------------------------------------------------------------------------
const Ptr<LuaOutput>& PtyScript::getLuaOutput() const
{
    return getWorldScriptable()->getLuaOutput();
}
/*//-----------------------------------------------------------------------------
Ptr<LuaOutput>& PtyScript::getLuaOutput()
{
    return getWorldScriptable()->getLuaOutput();
}*/
//-----------------------------------------------------------------------------
void PtyScript::runInline(const String8  & source, const String & name)
{
    getLuaOutput()->pushBackCmd(String(source));
    getWorldScriptable()->runScript(source, name);
    //Sleep(1);
    getWorldScriptable()->stopScript();
}
//-----------------------------------------------------------------------------
Core::String8 PtyScript::getScript() const
{
    return Core::String8(_pText->toPlainText().toStdString().c_str());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QTextCharFormat PtyWidgetScript::_sOutputMsgFormat;
QTextCharFormat PtyWidgetScript::_sErrorMsgFormat;
QTextCharFormat PtyWidgetScript::_sCmdMsgFormat;


PtyWidgetScript::PtyWidgetScript(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent),
    _posMemoCmd(0)
{
    setupUi();


    ///////////
    QBrush brush_error(QColor(255,0,0));
    PtyWidgetScript::_sErrorMsgFormat.setFont(QFont("Consolas", 8));
    PtyWidgetScript::_sErrorMsgFormat.setForeground(brush_error);

    QBrush brush_output(QColor(20,20,20));
    PtyWidgetScript::_sOutputMsgFormat.setFont(QFont("Consolas", 8));
    PtyWidgetScript::_sOutputMsgFormat.setForeground(brush_output);

    QBrush brush_cmd(QColor(0, 0, 40));
    QFont fontCmd ("Consolas", 8, QFont::Bold);
    fontCmd.setBold(true);
    PtyWidgetScript::_sCmdMsgFormat.setFont(fontCmd);
    PtyWidgetScript::_sCmdMsgFormat.setForeground(brush_cmd);
    /////////


}
//-----------------------------------------------------------------------------
PtyWidgetScript::~PtyWidgetScript()
{ 
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());

    //_groupBox->removeWidget(pPtyScript->_pText);
    _layout->removeWidget(pPtyScript->_pText);
    pPtyScript->_pText->setParent(NULL);
    delete _filterCmdLine;
    //delete pLuaHighlighter;
}
//-----------------------------------------------------------------------------
void PtyWidgetScript::setupUi()
{
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());

    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);


  /*  _pLabelLuaOutput = new QLabel("Lua Output", this);
    _pLuaOutput = new QTextEdit(this);
    _pLuaOutput->setReadOnly(true);
    _pLuaOutput->setCurrentCharFormat(_sOutputMsgFormat);
*/
    _pLabelCommandLine = new QLabel("Cmd :", this);
    _pCommandLine = new QLineEdit(this);
    _pCommandLine->setFont(QFont("Consolas", 8));

    QWidget*     pWidgetCmdLine = new QWidget(this);
    QGridLayout* pLayoutCmdLine = new QGridLayout(pWidgetCmdLine);
    pLayoutCmdLine->setContentsMargins(0, 0, 0, 0);
    pLayoutCmdLine->addWidget(_pLabelCommandLine, 0, 0);
    pLayoutCmdLine->addWidget(_pCommandLine, 0, 1);
    pWidgetCmdLine->setLayout(pLayoutCmdLine);
    _filterCmdLine = new CmdLineEventFilter(this, this);
    pWidgetCmdLine->installEventFilter(_filterCmdLine);

    _layout->addWidget(pPtyScript->_pText, 10);
 //   _layout->addWidget(_pLabelLuaOutput, 0, Qt::AlignTop);
//    _layout->addWidget(_pLuaOutput, 0, Qt::AlignTop);
    _layout->addWidget(pWidgetCmdLine, 0, Qt::AlignTop);

    setLayout(_layout);


    connect(pPtyScript->_pText, SIGNAL(textChanged ()), this, SLOT(textScriptChanged()));
    //connect(pPtyScript->_pText, SIGNAL(lostFocus ()), this, SLOT(textScriptChanged()));

    connect (_pCommandLine, SIGNAL(editingFinished()), this, SLOT(runCommandLine()));
   // connect (pPtyScript->getLuaOutput().get(), SIGNAL(newOutputMsg(const String& )), this, SLOT(receiveLuaOutputMsg(const String& )));
 //   connect (pPtyScript->getLuaOutput().get(), SIGNAL(newErrorMsg(const String& )), this, SLOT(receiveLuaErrorMsg(const String& )));
    connect (_pCommandLine, SIGNAL(editingFinished()), this, SLOT(runCommandLine()));

    connect (pPtyScript->getWorldScriptable().get(), SIGNAL(scriptStarted()), this, SLOT(scriptExecutionStart()));
    connect (pPtyScript->getWorldScriptable().get(), SIGNAL(scriptStopped()), this, SLOT(scriptExecutionStop()));

    //_pLuaOutput->setFixedHeight(100);
    
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


  //  initializeLuaOutput();
    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetScript::readProperty()
{
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());

    //_pText->setPlainText(pPtyScript->_script.c_str());
}
//-----------------------------------------------------------------------------
void PtyWidgetScript::writeProperty(QWidget* pWidget)
{
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());
}
//-----------------------------------------------------------------------------
void PtyWidgetScript::runCommandLine()
{
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());
    String cmd = String(_pCommandLine->text().toStdString().c_str());

    disconnect (pPtyScript->getWorldScriptable().get(), SIGNAL(scriptStarted()), this, SLOT(scriptExecutionStart()));
    disconnect (pPtyScript->getWorldScriptable().get(), SIGNAL(scriptStopped()), this, SLOT(scriptExecutionStop()));

    if (pPtyScript->getWorldScriptable()->isRunning() == false)
    {
        _posMemoCmd = 0;
        if (cmd!=L"")
        {/*
            _pLuaOutput->setCurrentCharFormat(_sCmdMsgFormat);
            Core::String cmdConsol = L" > " + cmd;
            _pLuaOutput->append(Core::String8(cmdConsol).c_str());
*/
            pPtyScript->runInline(Core::String8(cmd), cmd);
            _pCommandLine->clear();
        }
    }

    connect (pPtyScript->getWorldScriptable().get(), SIGNAL(scriptStarted()), this, SLOT(scriptExecutionStart()));
    connect (pPtyScript->getWorldScriptable().get(), SIGNAL(scriptStopped()), this, SLOT(scriptExecutionStop()));
}
//-----------------------------------------------------------------------------
/*void PtyWidgetScript::receiveLuaOutputMsg(const String& str)
{
    _pLuaOutput->setCurrentCharFormat(_sOutputMsgFormat);
    _pLuaOutput->append(Core::String8(str).c_str());
}
//-----------------------------------------------------------------------------
void PtyWidgetScript::receiveLuaErrorMsg(const String& str)
{
    _pLuaOutput->setCurrentCharFormat(_sErrorMsgFormat);
    _pLuaOutput->append(Core::String8(str).c_str());
}*/
//-----------------------------------------------------------------------------
void PtyWidgetScript::retrievePreviousCmdLine()
{
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());

    if (_posMemoCmd==0)
    {
        _memoCurrentCmd = String(_pCommandLine->text().toStdString().c_str());
    }

    _posMemoCmd++;
    if (_posMemoCmd>pPtyScript->getLuaOutput()->getNbCmd()) _posMemoCmd = pPtyScript->getLuaOutput()->getNbCmd();

    _pCommandLine->clear();
    String cmd = pPtyScript->getLuaOutput()->getCmd(pPtyScript->getLuaOutput()->getNbCmd() - _posMemoCmd);
    _pCommandLine->setText(Core::String8(cmd).c_str());
}
//-----------------------------------------------------------------------------
void PtyWidgetScript::retrieveNextCmdLine()
{
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());

    _posMemoCmd--;
    if (_posMemoCmd<0) _posMemoCmd = 0;
    _pCommandLine->clear();

    if (_posMemoCmd>0)
    {
        String cmd = pPtyScript->getLuaOutput()->getCmd(pPtyScript->getLuaOutput()->getNbCmd() - _posMemoCmd);
        _pCommandLine->setText(Core::String8(cmd).c_str());
    }
    else
    {
        _pCommandLine->setText(Core::String8(_memoCurrentCmd).c_str());
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetScript::scriptTextChanged()
{/*
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());
    pPtyScript->_script = Core::String8(_pText->toPlainText().toStdString().c_str());*/
}
//-----------------------------------------------------------------------------
/*void PtyWidgetScript::initializeLuaOutput()
{
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());
    Ptr<LuaOutput> pLuaOut = pPtyScript->getLuaOutput();


    for (int32 ii=0; ii<pLuaOut->getNbExecutionMemo(); ++ii)
    {
        const LuaOutput::luaExecutionMemo& memo = pLuaOut->getExecutionMemo(ii);

        _pLuaOutput->setCurrentCharFormat(_sCmdMsgFormat);
        Core::String8 cmdConsol = " > " + Core::String8(memo.name);
        _pLuaOutput->append(cmdConsol.c_str());

        for (int32 iOut=0; iOut<memo.luaOutputs.size(); ++iOut)
        {
            if (memo.luaOutputs[iOut].type == LUA_ERROR)
            {
                receiveLuaErrorMsg(memo.luaOutputs[iOut].msg);
            }
            else
            {
                receiveLuaOutputMsg(memo.luaOutputs[iOut].msg);
            }
        }
    }
}*/
//-----------------------------------------------------------------------------
void PtyWidgetScript::scriptExecutionStart()
{
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());

    _pCommandLine->setDisabled(true);
    
    pPtyScript->_pText->setStyleSheet(
        "QFrame                    { background-color: rgb(230,130,130); }"
        );
    pPtyScript->_pText->setDisabled(true);
}
//-----------------------------------------------------------------------------
void PtyWidgetScript::scriptExecutionStop()
{
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());

    _pCommandLine->setDisabled(false);
    pPtyScript->_pText->setDisabled(false);

    pPtyScript->_pText->setStyleSheet(
        "QFrame                    { background-color: rgb(230,230,230); }"
        );
}
//-----------------------------------------------------------------------------
void PtyWidgetScript::resizeEvent (QResizeEvent * event)
{/*
    int32 w = width();
    int32 h = height();
    
    Ptr<PtyScript> pPtyScript = LM_DEBUG_PTR_CAST<PtyScript>(getDataProxy()->getProperty());

    //int32 outHeight = _pLuaOutput->height();
    int32 cmdHeight = _pCommandLine->height();

    pPtyScript->_pText->resize(w, h-outHeight-cmdHeight-_pLabelLuaOutput->height()-_pLabelCommandLine->height());
*/
    QWidget::resizeEvent(event);
}
//-----------------------------------------------------------------------------
void PtyWidgetScript::textScriptChanged()
{
    emit propertyModified();
}
//-----------------------------------------------------------------------------

} // namespace EPI