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
#ifndef PROPERTY_NODE_H_
#define PROPERTY_NODE_H_

#include <QWidget>
#include <QGridLayout>

#include <Core/Standard.h>
#include <Core/XML/XMLNode.h>
#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

class QCheckBox;

namespace Universe
{
    class Node;
    class World;
}


namespace QtToolbox
{
    class ComboBox;
    class SmartGroupBox;
    class CollapsibleWidget;
    class LineEdit;
}//namespace QtToolbox

namespace EPI
{
class PtyWidgetNode;
class EpiSingleSlidingDouble;


//------------------------------------------------------------------------------
class LM_API_EPI PtyNode : public PropertyNode
{
    //Q_OBJECT

    friend class PtyWidgetNode;

public:
    PtyNode(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNode, const Core::String& title = L"Node");
    

    virtual ~PtyNode();
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);
    virtual void updateData();
    virtual void updateProperty();
    virtual void updateChildren();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual bool isItMe(const Ptr<Universe::Node>& pNode);

    virtual const Ptr<Property>& getPtyTransform() const;
    virtual Ptr<Property>& getPtyTransform();

    const Ptr<Property>& getPtyTag() const;
    Ptr<Property>& getPtyTag();

    const Ptr<Property>& getPtyScript() const;

    virtual bool findPtyWithId(Ptr<Property>& pResult, int32 id);

    virtual void setUniverseNode(const Ptr<Universe::Node>& pNode);
    const Ptr<Universe::Node>& getUniverseNode() const;
    Ptr<Universe::Node>& getUniverseNode();

    virtual void addChild(Ptr<Property> pChild, int32 indexPos=-1);
    virtual void removeChild(Ptr<Property> pChild);
    virtual void removeAllChildren();
    virtual void removeChild(const int32 index);

    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);

    virtual bool isTransformable() const;

    Ptr<Universe::World>    getUniverseWorld() {return _pWorld;}

    virtual void            copyId(const Property& pSrc);
    virtual bool            isIntersecting  (const Core::Rayf& ray, float& distance);

    void setVisibleWidgetTransform(bool flag);
    void setVisibleWidgetTag(bool flag);
    void setVisibleWidgetShow(bool flag);
    void setVisibleWidgetLightmap(bool flag);

    virtual Core::AABoxf getBoundingBox();
    virtual void setVisible(bool flag);

    virtual void setName(const Core::String& name);
    virtual void internalRestart();
    virtual void internalStop();

    virtual String getIconFileName() {return L":/icons/ptyNode.png";}
protected:
    PtyNode(const PtyNode& other);


private:
    void setUniverseWorld(const Ptr<Universe::World>& pWorld) {_pWorld = pWorld;}

protected:
    Ptr<Core::XMLNode>      _pXMLforResurrect;
    Ptr<Property>           _pPtyT;
    Ptr<Property>           _pPtyTag;
    Ptr<Property>           _pPtyScript;
    bool                    _isVisible;
    bool                    _isStatic;
    int32                   _lightmapRatio;

private:
    Ptr<Universe::Node>     _pNode;
    Ptr<Universe::World>    _pWorld;
    bool                    _showWidgetTransform;
    bool                    _showWidgetTag;
    bool                    _showWidgetVisible;
    bool                    _showWidgetLightmap;
    String                  _ressourceName;
};


//------------------------------------------------------------------------------
class LM_API_EPI PtyNodeGroupMemoCam : public PtyNode
{
public:
    PtyNodeGroupMemoCam(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNode, const Core::String& title = L"MemoCam");
    virtual ~PtyNodeGroupMemoCam();
    virtual EPropertyClassName getCalssName() const {return PTY_GROUP_MEMOCAM;}
};

class LM_API_EPI PtyNodeEditableChildren : public PtyNode
{
    Q_OBJECT

public:
    PtyNodeEditableChildren(const Ptr<StateRecorder>& pStateRecorder, const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNode, const Core::String& title = L"NodeEditable");
    virtual ~PtyNodeEditableChildren();


    virtual bool isEditable() const {return true;}

    const Ptr<Property>& getPtySelection() const;
    Ptr<Property>& getPtySelection();

    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual bool findPtyWithId(Ptr<Property>& pResult, int32 id);

private Q_SLOTS:
    virtual void editingSelectionChanged() = 0;

private:
    Ptr<Property>   _pPtySelection;
};

class LM_API_EPI PtyNodeEditableContent : public PtyNode
{
    Q_OBJECT

public:
    PtyNodeEditableContent(const Ptr<StateRecorder>& pStateRecorder, const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNode, const Core::String& title = L"NodeEditable");
    virtual ~PtyNodeEditableContent();

    PtyNodeEditableContent(const PtyNodeEditableContent& other);

    virtual bool isEditable() const {return true;}

    const Ptr<Property>& getPtySelection() const;
    Ptr<Property>& getPtySelection();

    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual bool findPtyWithId(Ptr<Property>& pResult, int32 id);
    int32 findIndexWithId(int32 id);

    const Core::List<Ptr<Property> >&  getPtiesContentSelectable() const;
    Core::List<Ptr<Property> >&  getPtiesContentSelectable();

    void removePtyContent(int32 index);
    void removePtyContent(const Ptr<Property>& pPty);
    void insertPtyContent(const Ptr<Property>& pPty, int32 index = -1);


private Q_SLOTS:
    virtual void editingSelectionChanged() = 0;
    virtual void propertyContentChanged(const Property& pty) = 0;
Q_SIGNALS:
    void selectionChanged(const Core::List<Ptr<Property> >& pties);

private:
    Ptr<Property>   _pPtySelection;
    Core::List<Ptr<Property> >  ptiesContentSelectable;
};

//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetNode : public PropertyWidget
{
    Q_OBJECT

    friend class PtyNode;

public:
    PtyWidgetNode(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetNode();
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();
    virtual void collapse(bool flag);

private:
    Ptr<PropertyWidget>                 _pPWTransfrom;
    Ptr<PropertyWidget>                 _pPWTag;
   // Ptr<PropertyWidget>                 _pPWScript;
    QCheckBox*                          _pShow;
    QCheckBox*                          _pStatic;
    QtToolbox::ComboBox *               _pLightmapRatio;
    QGridLayout *                       _layout;
    QtToolbox::SmartGroupBox *          _groupBox;
    QtToolbox::CollapsibleWidget *      _pGroupScript;
    QtToolbox::CollapsibleWidget *      _pGroupInfo;
    QtToolbox::LineEdit *               _ressourceName;

private Q_SLOTS:
    void textScriptChanged();
    void textScriptLostFocus();
};
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //PROPERTY_NODE_H_
