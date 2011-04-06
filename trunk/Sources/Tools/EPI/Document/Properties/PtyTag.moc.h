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
#ifndef PROPERTY_TAG_H_
#define PROPERTY_TAG_H_

#include <QWidget>

#include <Core/Standard.h>
#include <Universe/Node.h>

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>
 


class QGridLayout;
class QListWidget;
class QPushButton;

namespace QtToolbox
{
    class CollapsibleWidget;
}//namespace QtToolbox

namespace EPI
{
class ListTagEventFilter;
class PtyWidgetTag;
//-----------------------------------------------------------------------------
class LM_API_EPI PtyTag : public PropertyNode
{
    friend class PtyWidgetTag;

public:
    PtyTag(const Ptr<Universe::Node> pNode, const Core::String& title=L"Tag");
    virtual ~PtyTag();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    virtual void updateData();
    virtual void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    void setUniverseNode(const Ptr<Universe::Node>& pNode);
    const Ptr<Universe::Node>& getUniverseNode() const;
    
private:
    Ptr<Universe::Node> _pNode;
    Core::Set<String>   _tags;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetTag : public PropertyWidget
{
    Q_OBJECT

    friend class PtyTag;

public:
    PtyWidgetTag(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetTag();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();

    void renameCurrentTag();
    void deleteSelectedTags();
    virtual void collapse(bool flag);
public Q_SLOTS:
    void addTag();

private:
    QGridLayout              *  _pLayout;
    QtToolbox::CollapsibleWidget *  _pGoupBox;
    QListWidget              *  _listTag;
    ListTagEventFilter       *  _filter;
    QWidget                  *  _buttonBar;
    QPushButton              *  _pAddTag;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI

#endif //PROPERTY_TAG_H_
