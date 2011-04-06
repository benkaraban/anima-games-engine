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
#ifndef EPI_PROPERTY_DOCUMENT_GROUP_H_
#define EPI_PROPERTY_DOCUMENT_GROUP_H_

#include <QWidget>
#include <QGridLayout>

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

#include <Universe/NodeGroup.h>

namespace QtToolbox
{
    class SmartGroupBox;
    class ComboBox;
    class LineEdit;
}//namespace QtToolbox

namespace EPI
{
class PtyWidgetDocGroup;

//-----------------------------------------------------------------------------
class LM_API_EPI PtyDocGroup : public PropertyNode
{
    friend class PtyWidgetDocGroup;

public:
    PtyDocGroup(const Ptr<Universe::NodeGroup> pNodeG,
                const Core::String& title = L"DocGroup");
    virtual ~PtyDocGroup();

    virtual Ptr<PropertyWidget> internalCreatePropertyWidget    (const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void                internalCopy                    (const Ptr<Property>& pSrc);
    virtual Ptr<Property>       clone                           () const;
    virtual void                updateData();
            void                updateProperty();

    virtual void addChild(Ptr<Property> pChild, int32 indexPos=-1);

    const Core::String& getWorldDecoPath() const;
    Ptr<Universe::NodeGroup> getNodeGroup() const;
    void setNodeGroup(const Ptr<Universe::NodeGroup>& node);


private:
    String                      _worldDeco;
    Ptr<Universe::NodeGroup>    _pNodeG;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetDocGroup : public PropertyWidget
{
    friend class PtyDocGroup;

public:
    PtyWidgetDocGroup(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetDocGroup();

    void readProperty   ();
    void writeProperty  (QWidget* pWidget);
    void setupUi        ();

private:
    QGridLayout *               _layout;
    QtToolbox::SmartGroupBox *  _groupBox;

    QtToolbox::LineEdit *       _worldDeco;
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif /*EPI_PROPERTY_DOCUMENT_GROUP_H_*/
