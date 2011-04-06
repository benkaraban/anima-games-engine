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
#ifndef EPI_PROPERTY_DOCUMENT_SCRIPT_H_
#define EPI_PROPERTY_DOCUMENT_SCRIPT_H_

#include <QWidget>
#include <QGridLayout>


#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>
#include <EPI/Document/WorldScriptableDocScript.h>

namespace Renderer
{
    class IRenderer;
}

namespace Core
{
    class VirtualFileSystem;
}

namespace QtToolbox
{
    class SmartGroupBox;
    class ComboBox;
}//namespace QtToolbox

namespace EPI
{
class PtyWidgetMaterial;
class PtyWidgetDocScript;
class GuiDocument;

//-----------------------------------------------------------------------------
class LM_API_EPI PtyDocScript : public PropertyNode
{
    Q_OBJECT

    friend class PtyWidgetDocScript;

public:
    PtyDocScript(GuiDocument& guiDoc, const Ptr<DocumentRenderer>& pDocR, const Core::String& title = L"DocScript");
    virtual ~PtyDocScript();

    virtual Ptr<PropertyWidget> internalCreatePropertyWidget    (const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void                internalCopy                    (const Ptr<Property>& pSrc);
    virtual Ptr<Property>       clone                           () const;

    virtual void                updateData();
    virtual void                updateProperty();
    String                      getTagGamePlay() {return _tagGameplay;}
    String                      setTagGamePlay(const String& str) {return _tagGameplay=str;}

    const Ptr<Property>& getPtyScript();
    const String& getWorldXMLPath() {return _worldXMLPath;}
    void setWorldXMLPath(const String& str) {_worldXMLPath=str;}

Q_SIGNAL
    void worldChanged();

private:
    Ptr<WorldScriptableForDocScript>       _pWorldA;
    String                                 _worldXMLPath;
    String                                 _tagGameplay;
    GuiDocument&                           _guiDoc;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetDocScript : public PropertyWidget
{
    Q_OBJECT

    friend class PtyDocScript;

public:
    PtyWidgetDocScript(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetDocScript();
    void setupUi        ();

    void readProperty   ();
    void writeProperty  (QWidget* pWidget);

private Q_SLOTS:
    void worldXMLChange();

private:
    QVBoxLayout *               _layout;
    QWidget*                    _Test;
    QtToolbox::LineEdit *       _worldXML;
    QtToolbox::LineEdit *       _tagGameplay;
    //QtToolbox::SmartGroupBox *  _groupBox;


    Ptr<PropertyWidget>         _pPtyWidgetScript;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif /*EPI_PROPERTY_DOCUMENT_SCRIPT_H_*/
