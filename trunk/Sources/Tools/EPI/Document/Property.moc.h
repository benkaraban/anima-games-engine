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
#ifndef Property_H_
#define Property_H_

#include <QObject>
#include <Core/Standard.h>
#include <EPI/Document/Properties/Itransformable.h>
#include <EPI/Document/IEditable.h>

class QWidget;

namespace Core
{
    struct Rayf;
}

namespace Universe
{
    class Node;
    class World;
}

namespace EPI
{
//-----------------------------------------------------------------------------
class PropertyWidget;
class PropertyWidgetDataProxy;
struct ImportInfo;
//-----------------------------------------------------------------------------
LM_ENUM_3(EPropertyType,
          DOCUMENT_EDITION,
          DOCUMENT_DESCRIPTION,
          CONTENT)

LM_ENUM_18(EPropertyClassName,
          PROPERTY_CLASS,
          PTYNODE_CAMERA_CLASS,
          PTYNODE_PLAYER_POSITION,
          PTYNODE_INSTANCE,
          PTY_TRANSFORM,
          PTY_TRANSFORM_NODE,
          PTY_MATERIAL,
          PTY_GROUP_MEMOCAM,
          PTYNODE_GIZMO,
          PTYNODE_SOUND,
          PTYNODE_WATER_PATCH,
          PTYNODE_WATER_LAYER,
          PTYNODE_DECAL,
          PTYNODE_POINT_LIGHT,
          PTYNODE_SKELETON,
          PTYNODE_TRAIL,
          PTYNODE_EMITTER,
          PTYNODE_TERRAIN)






LM_ENUM_3(EPtyFlagType,
          FLAG_PTY_NODE,
          FLAG_PTY_EDITABLE_CONTENT,
          FLAG_PTY_TRANSFORM)
//-----------------------------------------------------------------------------
/**
* Defini l'interface de base des proprietes du document
*/
class LM_API_EPI Property : public QObject, public Core::Object, public ITransformable, public IEditable
{
    Q_OBJECT

protected:
    Property(const Property& other);
    Property(const EPropertyType type);
    Property(const EPropertyType type, const Core::String name, bool isSelectable = false, bool isDeletable = true);

public:
    virtual ~Property();
    
    virtual bool isRepresented() const {return false;} // si la pty dispose d'une representation
    virtual void setRepresentationVisible(bool flag) {}

    //Constructeur de copie virtuel - on recupere une copie sans les enfants
    virtual Ptr<Property>   clone   () const = 0;
    //on ne copie que le contenue de la property pas les enfant
    virtual void            copy    (const Ptr<Property>& pSrc);

    int32                   getId () const;
    void                    setId (int32 id);
    virtual void            copyId(const Property& pSrc) {setId(pSrc.getId());}

    // accesseur a la variable stockant l'id du document sur lequel porte la propriete
    virtual void            setDocId   (const int32 docId);
    const int32&            getDocId   () const;               

    virtual bool            isItMe          (const Ptr<Universe::Node>& pNode);
    virtual void            updateData();
    virtual void            updateProperty();
    virtual void            resurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual bool            isIntersecting  (const Core::Rayf& ray, float& distance);
    //recherche dans les pty child et les pty membre de la pty la pty ayant l'id specifie
    virtual bool            findPtyWithId(Ptr<Property>& pResult, int32 id);

    bool                    isSelectable    () const;
    bool                    isDeletable     () const;
    virtual bool            isEditable      () const;
    virtual bool            isTransformable () const;
    void                    setSelectable   (bool isSelectable);
    void                    setDeletable    (bool flag);
    virtual bool            isLeaf () const {return false;}

    EPropertyType           getType         () const;
    virtual EPropertyClassName getCalssName() const {return PROPERTY_CLASS;}
    const Core::String&     getName         () const;
    virtual             void     setName(const Core::String& name);
    virtual             void     setSelected(bool isSelected) {}

    virtual void                    addChild        (Ptr<Property> pChild, int32 indexPos=-1);  //insert l'enfant a la position indexPos de la liste d'enfant
    virtual void                    removeChild     (Ptr<Property> pChild);
    virtual void                    removeChild     (const int32 index);
    virtual void                    removeAllChildren();
    virtual int32                   getNbChildren   () const;
    virtual const Ptr<Property>&    getChild        (const int32 index) const;
    virtual Ptr<Property>&          getChild        (const int32 index);
    virtual const Property*         getParent       () const;
    virtual Property*               getParent       ();
    virtual void                    setParent       (Property* parent); 

    Ptr<PropertyWidget> createPropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    void launchSignalProeprtyChanged(bool callParent=false);

    virtual const Ptr<Property>&    getPtyTransform() const;
    virtual Ptr<Property>&          getPtyTransform();
    virtual const Ptr<Property>&    getPtySelection() const;
    virtual Ptr<Property>&          getPtySelection();

    int32 getFlags();
    void addFlag(EPtyFlagType  flag);
    bool hasFlag(EPtyFlagType flag) const;

    virtual String toString() const;

    virtual void restart() {}
    virtual void internalRestart() {}

    virtual void stop() {}
    virtual void internalStop() {}

    virtual String getIconFileName() {return L":/icons/refresh.png";}

    virtual bool beforeUndo() {return true;}
    virtual bool beforeRedo() {return true;}

protected:
    virtual void internalResurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);
    virtual void internalCopy(const Ptr<Property>& pSrc) = 0;
    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

private:
    Property& operator=( const Property& other );  //on desactive l'operateur d'assignation


Q_SIGNALS:
    void generate(const Ptr<ImportInfo> & pImportInfo);
    void propertyChanged();

private:
    class implementation;
	implementation* const _impl;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_EPI void                         cloneWithChild          (Ptr<Property>& pPtyDst, const Property& ptySrc, const bool copyId = false);
LM_API_EPI bool                         ptiesAreDeletable       (const Core::List<Ptr<Property> >& pties);
LM_API_EPI bool                         ptiesAreInstance        (const Core::List<Ptr<Property> >& pties);
LM_API_EPI Core::List<Ptr<Property> >   removePropertyChildren  (Core::List<Ptr<Property> >& pties);
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI

#endif Property_H_

