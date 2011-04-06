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
#ifndef PROPERTY_DOC_WORLD_H_
#define PROPERTY_DOC_WORLD_H_

#include <EPI/DocumentRenderer.h>
#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/World.h>

namespace QtToolbox
{
    class SmartGroupBox;
}//namespace QtToolbox

class QGridLayout;
namespace EPI
{
class PtyWidgetDocWorld;
class GuiDocument;

//-----------------------------------------------------------------------------
class LM_API_EPI PtyDocWorld : public PropertyNode
{
    friend class PtyWidgetDocWorld;

public:
    PtyDocWorld(GuiDocument& guiDoc, const Ptr<Universe::World>& pWorld, const Ptr<DocumentRenderer> & pDocumentRenderer, const Core::String& title = L"World");
    virtual ~PtyDocWorld();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent = 0);
    
    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual void updateData();
    void updateProperty();

    
    /*
private:
    void parseNodes(const Ptr<Universe::Node> & pNode, PropertyNode & ptyNode);
*/
private:
    Ptr<Universe::World>    _pWorld;
    Ptr<DocumentRenderer>   _pDocumentRenderer;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetDocWorld : public PropertyWidget
{
    friend class PtyDocWorld;

public:
    PtyWidgetDocWorld(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetDocWorld();

    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);

    void setupUi();

private:
    QGridLayout * _layout;
    QtToolbox::SmartGroupBox *          _groupBox;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //PROPERTY_DOC_WORLD_H_
