/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef DOCUMENTBASE_H_
#define DOCUMENTBASE_H_

#include <QObject>
#include <Core/Standard.h>
#include <EPI/Document/IWorldScriptable.moc.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class QAction;
class QMimeData;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace Core
{
    struct Orientationf;
    struct Vector3f;
    template <class TElement> class List;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace Universe
{
    class World;
    class Node;
    class NodeCamera;
    class NodeListener;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace EPI
{
class PtyDocEdition;
class PyDocumentInformation;
class StateRecorder;
class DocumentRenderer;
class GuiDescription;
class Property;
class PropertyNode;
struct ImportInfo;
class LuaOutput;
class IDocumentWidget;
class PtyNodeCamera;
class PtyNodeInstance;
class GuiDocument;
class IWorldScriptable;
//-----------------------------------------------------------------------------
LM_ENUM_12(DocumentType,
          DOC_UNKNOW,
          DOC_PARTICLES,
          DOC_MATERIAL,
          DOC_MODEL,
          DOC_TEXTURE,
          DOC_TERRAIN,
          DOC_WORLD,
          DOC_SCRIPT,
          DOC_GROUP,
          DOC_SPLINE,
          DOC_PIC_BANK,
          DOC_TEST)
//-----------------------------------------------------------------------------
LM_ENUM_5 (EMemoCameraID,
           MEMO_CAM_1,
           MEMO_CAM_2,
           MEMO_CAM_3,
           MEMO_CAM_4,
           MEMO_CAM_5)
//-----------------------------------------------------------------------------
/**
*  La classe mere de tous documents. Elle nous permet de centraliser les operations communent a chaque document
* Un document est edite par un editeur. Les donnees stockee par le document le sont sous forme d'arbre.
* Toute donnee herite de la classe property.
* La racine de l'arbre de property est _propertyRoot.
* Un document stocke deux type de donnee. les informations sur le document (titre, etc ...), et le contenu du document
*
* _propertyRoot
*       |
*       |- DocumentInformation
*       |- Content
*
* Pour permettre des action d'undo/redo un document stocke egalement son historique sous forme d'etat, grace au membre stateRecorder. 
* 
*/
class  LM_API_EPI DocumentBase : public QObject, public Core::Object
{
    Q_OBJECT

protected:
    DocumentBase(GuiDocument& guiGoc, const DocumentType type, const Ptr<DocumentRenderer>& pDocRdr, bool initWorldAnimation=false);

public:
    virtual ~DocumentBase();

    GuiDocument& getGuiDocument() const;

    int32                                           getID();
    const DocumentType&                             getType()  const;
    const String&                                   getTitle() const;
    virtual Core::Orientationf                      getDefaultCameraOrientation() const;
    virtual Core::Vector3f                          getDefaultCameraTargetPosition() const;
    
    virtual const Ptr<StateRecorder>&               getStateRecorder() const;
    virtual const Ptr<DocumentRenderer>&            getDocumentRenderer() const;
    virtual const Ptr<Universe::World>&             getWorld() const;
    virtual const Ptr<Universe::World>&             getWorldDocument() const;
    virtual const Ptr<Universe::World>&             getWorldInfoContent() const;
    virtual Ptr<Universe::World>                    getWorldDeco();
    virtual void                                    setWorldDeco(const Ptr<Universe::World>& pWorld);
    virtual const Ptr<GuiDescription>&              getGuiDescription() const;
    
    virtual bool                            isClosable() const;
    virtual void                            setClosable(bool closable);
    virtual bool                            isModified() const;
    virtual void                            setModified(bool modified);
    virtual bool                            canClose(bool userInteraction) const;

    virtual void                            deleteSelectedProperties();
    virtual void                            renameCurrentProperty();
    virtual void                            duplicateSelectedProperties();
    virtual void                            addChildrenToCurrentProperty(Core::List<Ptr<Property> > pties);

    //permet de recuperer la racine des donnes stockee par le document
    virtual Ptr<PropertyNode>               getPropertyRoot() const;

    //pour acceder au noeud stokant les information du document
    virtual const Ptr<Property>&            getPyDocumentInformation() const;
    virtual Ptr<PtyDocEdition>              getPropertyDocumentEdition() const;

    //pour acceder au noeud stockant le contenu du document
    virtual const Ptr<Property>&            getPropertyDocumentContent() const;
    virtual const Ptr<Property>&            getPropertyDocumentContentStart() const;
    virtual const Ptr<Property>&            getPropertyDocumentContentRoot() const;

    virtual Ptr<Property>           getPtyForEdition() const;


    // pour selectionner une propriete
    virtual void                                selectProperty(Ptr<Property> p);
    virtual void                                selectProperty(int32 id);
    virtual void                                selectProperties(const Core::List<int32>& id);
    virtual void                                selectAllProperties();
    virtual void                                deselectAllProperties();

    virtual const Core::List<Ptr<Property> >&   getSelectedProperties() const;
    virtual Ptr<Property>                       getProperty(int32 id) const;
    virtual void                                getProperty(const Core::List<int32>& ids, Core::List<Ptr<Property> >& pties);
    virtual Ptr<Property>                       getDefaultProperty() const;

   
    virtual void runStopScript();
    virtual bool isEditing();
    virtual void setEditing(bool edit);

    // permet de connecter la propriété au slot de génération de données
    void setGenerationProperty(Ptr<Property> pProperty);

    virtual int32 getPropertyIdFromUniverseNode(const Ptr<Universe::Node>& pNode);


    virtual QList<QAction*>&    getActionsCreateProperty();

    virtual Ptr<IDocumentWidget> createDocumentWidget(QWidget* parent) const;

    virtual const Core::List<Ptr<PtyNodeCamera> >& getAllCameraContent() const;

    virtual Core::List<Ptr<Property> > getAllGizmoContent() const;

    virtual Ptr<PtyNodeCamera>& getCameraContent(int32 ptyID) const;

    virtual bool dropInViewportIsPossible() const;
    virtual bool dropThisFormatIsAuthorized(const QMimeData* mimeData, QString& format) const;
    virtual void dropThisFile(const QMimeData* mimeData, QString& format);
    QStringList& getFormatDropable();

    virtual void exportCurrentPtyAsGroup();

    virtual Ptr<Universe::NodeCamera> getCameraEdition() const;

    virtual Ptr<Property>   getActivePlayerPosition();
    virtual void addContent(const Ptr<Property>& pPty);
    virtual void removeContent(const Property& pty);
    virtual void removeContent(const Core::List<Ptr<Property> >& pties);
    void    initializeIdContent();

    virtual void escape();

    virtual void setMemoCamera(const Ptr<Universe::NodeCamera>& pCam, const EMemoCameraID& id);
    virtual void getMemoCamera(Ptr<Universe::NodeCamera> pCam, const EMemoCameraID& id);
    virtual void convertInstance(Core::List<Ptr<Property> > pties);

    virtual Ptr<Universe::NodeListener> getDocumentSoundListener();

    virtual Ptr<IWorldScriptable>  getWorldScriptable() const;

    virtual void restart();
    virtual void stop();

    void activeDesactiveIHM();
private:
    void travelInitializeIdContent(const Ptr<Property>& pPty);
    Ptr<PtyNodeCamera> createPropertyMemoCamera(const EMemoCameraID& id);

    bool ptiesSelectedBeforeUndo();
    bool ptiesSelectedBeforeRedo();

protected:
    void setGuiDescription(const Ptr<GuiDescription>& pGuiDesc);
    void createProperty(Ptr<Property> newPty, Ptr<Property> ptyParent);

public Q_SLOTS:
    virtual void         activeProperty(int32 id);
    void                 generateInfo(const Ptr<ImportInfo> & pImportInfo);
    virtual void         contentIsModified();
    void                 emitDocumentContentUpdated() {emit documentContentUpdated();}
    void                 editIsModified();

    virtual void        undo();
    virtual void        redo();
    virtual void        save();
    virtual void        saveAs();

    //Méthodes appelées par le drag&drop dans le WorldViewport
    void createPropertyTerrain(const Core::String & fileName);
    void createPropertyNodeInstance(const Core::String & fileName, Ptr<PtyNodeInstance>& pPtyNodeI);
    void createGizmo(const Ptr<Property>& pPty);

private Q_SLOTS:
    virtual void currentSelectionChanged();
    void createPropertyWaterPatch();
    void createPropertyTerrain();
    void createPropertyDecal();
    void createPropertyNodeEmitter();
    void createPropertyStorm();
    void createPropertyNodeGroup();
    void createPropertyNodePointLight();
    void createPropertyNodeTrail();
    void createPropertyNodeSound();

Q_SIGNALS:
    void generate(const Ptr<ImportInfo> & pImportInfo);
    void currentSelectionChanged(const Core::List<Ptr<Property> >&  pties);
    void documentContentUpdated();
    void runStopScriptChanged();
    void editingChange(bool isEditing);

private:
    class implementation;
	implementation* const _impl;
};
//-----------------------------------------------------------------------------
LM_API_EPI QIcon documentTypeToIcon(DocumentType type);
//-----------------------------------------------------------------------------
}  // namespace EPI

#endif /*DOCUMENTBASE_H_*/