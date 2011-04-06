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
#ifndef EPI_PROPERTY_SCRIPT_H_
#define EPI_PROPERTY_SCRIPT_H_

#include <QWidget>
#include <QGridLayout>
#include <QTextCharFormat>

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>
#include <EPI/Document/LuaOutput.moc.h>

#include <EPI/Document/WorldScriptableDocScript.h>

class QTextEdit;
class QLineEdit;
class QLabel;

namespace QtToolbox
{
    class SmartGroupBox;
    class LuaSyntaxHighlighter;
    class CodeEditor;
}//namespace QtToolbox

namespace Universe
{
    class ITask;
}//namespace Universe

namespace EPI
{
class PtyWidgetScript;
class CmdLineEventFilter;

//-----------------------------------------------------------------------------
class LM_API_EPI PtyScript : public PropertyNode
{
    friend class PtyWidgetScript;

public:
    PtyScript(const Ptr<IWorldScriptable>& pWorldScriptable, const Core::String& title = L"Script");
    PtyScript(const PtyScript& other);
    virtual ~PtyScript();

    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    virtual void updateData();
    virtual void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    void setScript(const Core::String8& script);
    Core::String8 getScript() const;

    const Ptr<IWorldScriptable>&    getWorldScriptable() const;

    const Ptr<LuaOutput>&       getLuaOutput() const;
    //Ptr<LuaOutput>&             getLuaOutput();

    void runInline(const String8  & source, const String & name = String(L"<console>"));

    virtual String getIconFileName() {return L":/icons/ptyScript.png";}

    QtToolbox::CodeEditor*  getCodeEditor() {return _pText;}
private:
    QtToolbox::CodeEditor*              _pText;
    QtToolbox::LuaSyntaxHighlighter *   _pLuaHighlighter;
    Ptr<IWorldScriptable>                _pWorldScriptable;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetScript : public PropertyWidget
{
    Q_OBJECT

    friend class PtyScript;

public:
    PtyWidgetScript(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetScript();

    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);

    void setupUi();

    void retrievePreviousCmdLine();
    void retrieveNextCmdLine();

private:
  //  void initializeLuaOutput();
    virtual void 	resizeEvent  ( QResizeEvent * event );
    
private Q_SLOTS:
    void runCommandLine();
 //   void receiveLuaOutputMsg(const String& str);
 //   void receiveLuaErrorMsg(const String& str);
    void scriptTextChanged();
    void scriptExecutionStart();
    void scriptExecutionStop();
    void textScriptChanged();
private:
    QVBoxLayout * _layout;
    //QtToolbox::SmartGroupBox *  _groupBox;

   // QTextEdit*  _pLuaOutput;
    QLineEdit*  _pCommandLine;
    //QLabel*     _pLabelLuaOutput;
    QLabel*     _pLabelCommandLine;

    static QTextCharFormat _sOutputMsgFormat;
    static QTextCharFormat _sErrorMsgFormat;
    static QTextCharFormat _sCmdMsgFormat;

    CmdLineEventFilter* _filterCmdLine;
    int32               _posMemoCmd;
    String              _memoCurrentCmd;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif /*EPI_PROPERTY_SCRIPT_H_*/
