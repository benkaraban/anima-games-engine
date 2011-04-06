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
#include <EPI/Document/DocumentBase.moc.h>

#include <QAction>
#include <QMessageBox>
#include <QIcon>
#include <QInputDialog>

#include <Core/VFS/VFSDirectMapping.h>
#include <Packing/TexCompressionOnTheFly.h>

#include <EPI/ApplicationBase.moc.h>
#include <EPI/DocumentRenderer.h>
#include <EPI/ImportInfo/ImportInfo.moc.h>
#include <EPI/Document/GuiDescription.h>
#include <EPI/Document/Property.moc.h>
#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/Properties/PtyNodeCamera.h>
#include <EPI/Document/Properties/PtyGizmo.h>
#include <EPI/Document/Properties/PtyNodeStorm.moc.h>
#include <EPI/Document/Properties/PtyDocInformation.moc.h>
#include <EPI/Document/Properties/PtyDocEdition.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <EPI/Document/PropertySelection.moc.h>
#include <EPI/Document/IDocumentWidget.h>
#include <Universe/World.h>

#include <EPI/Document/Properties/PtyNodeInstance.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyNodeEmitter.moc.h>
#include <EPI/Document/Properties/PtyNodeTrail.h>
#include <EPI/Document/Properties/PtyNodePointLight.h>
#include <EPI/Document/Properties/PtyWaterPatch.h>
#include <EPI/Document/Properties/PtyTerrain.moc.h>
#include <EPI/Document/Properties/PtyNodeDecal.moc.h>
#include <EPI/Document/Properties/PtyNodeSound.moc.h>
#include <EPI/Document/WorldScriptableDocBase.h>

#include <QtToolbox/VFSDialog.moc.h>
#include <EPI/Document/Tools.h>


namespace EPI
{
Ptr<PtyNodeCamera>   pPtyCameraBidon = null;

struct  IdContentType
{
    void clear() {idPtiesTerrain.clear(); idPtiesWithRepresentation.clear(); idPtiesPlayersPos.clear(); idPtiesCameraContent.clear(); idPtiesGizmo.clear(); idPtyGroupMemoCamera=-1;}
    Core::List<int32>   idPtiesTerrain;
    Core::List<int32>   idPtiesPlayersPos;
    Core::List<int32>   idPtiesCameraContent;
    Core::List<int32>   idPtiesGizmo;
    Core::List<int32>   idPtiesWithRepresentation;
    int32               idPtyGroupMemoCamera;
    Core::List<Ptr<PtyNodeCamera> >   ptiesCameraContent;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class  DocumentBase::implementation 
{
public:
    implementation(GuiDocument& gGoc, const DocumentType type, const Ptr<DocumentRenderer>& pDocRdr);
    ~implementation();

    int32                               id;
    DocumentType                        type;
    Ptr<StateRecorder>                  pStateRecorder;
    Ptr<PropertyNode>                   propertyRoot;

    Ptr<DocumentRenderer>               pDocRdr;
    
    Ptr<Universe::World>                pWorld;
    Ptr<Universe::World>                pWorldInfo;   //overlay pour les information sur le contenu 
    Ptr<GuiDescription>                 pGuiDesc;

    bool                                isClosable;
    bool                                isModified;
    bool                                isEditing;

    QList <QAction*>                    actionsCreateProperty;
 

    QStringList                         formatDropable;
    GuiDocument&                        guiGoc;

    IdContentType                       idContent;

     Ptr<WorldScriptableForDocBase>     pWorldA;
     bool                               showIHM;
private:
    // counter de document permettant d'affecter l'identifiant
    static int32 countDocument;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// TODO : constante temporaire a virer
const int32 TEMP_DOCUMENT_INFORMATION = 0;
const int32 TEMP_DOCUMENT_CONTENT     = 1;
const int32 TEMP_DOCUMENT_EDITION     = 2;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int32 DocumentBase::implementation::countDocument = 0;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DocumentBase::implementation::implementation(GuiDocument& gGoc, const DocumentType type, const Ptr<DocumentRenderer>& pDocRdr)
:   guiGoc(gGoc),
    type(type),
    propertyRoot(Ptr<PropertyNode>(new PropertyNode(L"root"))),
    id(countDocument++),
    pDocRdr(pDocRdr),
    pGuiDesc(null),
    isClosable(true),
    isModified(false),
    isEditing(true),
    pWorldA(null),
    showIHM(true)
{
}
//-----------------------------------------------------------------------------
DocumentBase::implementation::~implementation()
{
    Ptr<StateRecorder> test = pStateRecorder;
    Ptr<PropertyNode> testPR = propertyRoot;
    Ptr<Universe::World> testpWorld = pWorld;
    Ptr<Universe::World> testpWorldInfo = pWorldInfo;
    Ptr<WorldScriptableForDocBase> testpWorldA = pWorldA;

    pStateRecorder->kill();
    pStateRecorder = null;
    propertyRoot   = null;

    pWorld         = null;
    pWorldInfo     = null;
    pGuiDesc       = null;
    pWorldA        = null;

    pDocRdr        = null;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int32 ptyTravelgetPropertyIdFromUniverseNode(const Ptr<PropertyNode>& pPty, const Ptr<Universe::Node>& pNode)
{
    int32 res = -1;

    if (pPty->isItMe(pNode) == true)
    {
        res = pPty->getId();
    }
    else
    {
        for (int32 iChild=0; iChild<pPty->getNbChildren(); ++iChild)
        {
            res = ptyTravelgetPropertyIdFromUniverseNode(LM_DEBUG_PTR_CAST<PropertyNode> (pPty->getChild(iChild)), pNode);

            if (res != -1) break;
        }
    }

    return res;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DocumentBase::DocumentBase(GuiDocument& guiGoc, const DocumentType type, const Ptr<DocumentRenderer>& pDocRdr, bool initWorldAnimation) :
    _impl(new implementation(guiGoc, type, pDocRdr))
{
    LOG_FCT_NAME

    _impl->pStateRecorder = Ptr<StateRecorder>(new StateRecorder()), 

    _impl->propertyRoot->setDocId(_impl->id);


    _impl->propertyRoot->addChild(Ptr<Property> (new PyDocumentInformation(L"")));
    _impl->propertyRoot->addChild(Ptr<Property> (new PropertyNode(L"Contains")));
    _impl->propertyRoot->addChild(Ptr<Property> (new PtyDocEdition(L"DocEdit", getPropertyDocumentContent(), _impl->pStateRecorder)));
    
    
    Ptr<Universe::IOnTheFlyTextureGenerator> pTexGen(new Packing::OnTheFlyTextureGenerator());

    _impl->pWorld = Ptr<Universe::World>( new Universe::World(pDocRdr->getRenderer(),  pDocRdr->getVFS(), null, pDocRdr->getAudioDevice()));
    _impl->pWorld->getRessourcesPool()->setOnTheFlyTextureGenerator(pTexGen);
    _impl->pWorld->setFogColor(Core::Vector4f(0.2f, 0.2f, 0.25f, 0.f));
    _impl->pWorld->getNodeListener()->setGain(0.f);

    _impl->pWorldInfo = Ptr<Universe::World>( new Universe::World(pDocRdr->getRenderer(),  pDocRdr->getVFS(), null, pDocRdr->getAudioDevice()));
    _impl->pWorldInfo->getRessourcesPool()->setOnTheFlyTextureGenerator(pTexGen);
    _impl->pWorldInfo->getNodeListener()->setGain(0.f);
    setGuiDescription(Ptr<GuiDescription> (new GuiDescription()));


    if (initWorldAnimation==true)
    {
        _impl->pWorldA = Ptr<WorldScriptableForDocBase> (new WorldScriptableForDocBase(_impl->pDocRdr, _impl->pWorld));
    }

    connect(getPropertyDocumentEdition()->getPtySelection().get(), SIGNAL(selectionChanged()),
            this, SLOT(currentSelectionChanged()));

        Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (getPropertyDocumentEdition());
        Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyDocEdit->getPtySelection());
    connect(pPtySelection.get(), SIGNAL(selectionChanged()), this, SLOT(editIsModified()));
}
//-----------------------------------------------------------------------------
GuiDocument& DocumentBase::getGuiDocument() const
{
    return _impl->guiGoc;
}
//-----------------------------------------------------------------------------
DocumentBase::~DocumentBase()
{
    LOG_FCT_NAME

    delete _impl;
}
//-----------------------------------------------------------------------------
int32 DocumentBase::getID()
{
    LOG_FCT_NAME

    return _impl->id;
}
//-----------------------------------------------------------------------------
const DocumentType& DocumentBase::getType()  const
{
    LOG_FCT_NAME

    return _impl->type;
}
//-----------------------------------------------------------------------------
const String& DocumentBase::getTitle() const
{
    LOG_FCT_NAME

    Ptr<PyDocumentInformation> pPinfo = LM_DEBUG_PTR_CAST<PyDocumentInformation>(getPyDocumentInformation());
    return pPinfo->getTitle();
}
//-----------------------------------------------------------------------------
Core::Orientationf DocumentBase::getDefaultCameraOrientation() const
{
    LOG_FCT_NAME

    return Core::Orientationf (Core::deg2rad(0), Core::deg2rad(0), Core::deg2rad(0));
}
//-----------------------------------------------------------------------------
Core::Vector3f DocumentBase::getDefaultCameraTargetPosition() const
{
    LOG_FCT_NAME

    return Core::Vector3f(Core::ZERO);
}
//-----------------------------------------------------------------------------
const Ptr<StateRecorder>& DocumentBase::getStateRecorder() const
{
    LOG_FCT_NAME

    return _impl->pStateRecorder;
}
//-----------------------------------------------------------------------------
const Ptr<DocumentRenderer>& DocumentBase::getDocumentRenderer() const
{
    LOG_FCT_NAME

    return _impl->pDocRdr;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::World>& DocumentBase::getWorld() const
{
    LOG_FCT_NAME

    return _impl->pWorld;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::World>& DocumentBase::getWorldDocument() const
{
    return _impl->pWorld;
}
//-----------------------------------------------------------------------------
Ptr<Universe::World> DocumentBase::getWorldDeco()
{
    LOG_FCT_NAME

    return null;
}
//-----------------------------------------------------------------------------
void DocumentBase::setWorldDeco(const Ptr<Universe::World>& pWorld)
{
    LOG_FCT_NAME
}
//-----------------------------------------------------------------------------
const Ptr<GuiDescription>& DocumentBase::getGuiDescription() const
{
    LOG_FCT_NAME

    return _impl->pGuiDesc;
}
//-----------------------------------------------------------------------------
bool DocumentBase::isClosable() const
{
    LOG_FCT_NAME

    return _impl->isClosable;
}
//-----------------------------------------------------------------------------
void DocumentBase::setClosable(bool closable)
{
    LOG_FCT_NAME

    _impl->isClosable = closable;
}
//-----------------------------------------------------------------------------
bool DocumentBase::isModified() const
{
    LOG_FCT_NAME

    return _impl->isModified;
}
//-----------------------------------------------------------------------------
void DocumentBase::setModified(bool modified)
{
    LOG_FCT_NAME

    _impl->isModified = modified;
}
//-----------------------------------------------------------------------------
void DocumentBase::deleteSelectedProperties()
{
    LOG_FCT_NAME

    Core::List<Ptr<Property> > ptiesSelected = getSelectedProperties();

    if (ptiesAreDeletable(ptiesSelected) == true)
    {
        selectProperty(getDefaultProperty());

        removeChildrenRecording(getStateRecorder(), ptiesSelected);
        
        removeContent(ptiesSelected);
    }
    emit documentContentUpdated();
}
//-----------------------------------------------------------------------------
void DocumentBase::renameCurrentProperty()
{
    const Core::List<Ptr<Property> >&   selectedPties = getSelectedProperties();

    if (selectedPties.size() == 1)
    {
        String text = selectedPties[0]->getName();
        QString name = QInputDialog::getText(NULL, "Name", 
                 "Enter new name", QLineEdit::Normal, StrToQStr(text));
        name = name.simplified();

        if (!name.isEmpty())
        {
            selectedPties[0]->setName(QStrToStr(name));

            emit documentContentUpdated();
            contentIsModified();
        }
    }
}
//-----------------------------------------------------------------------------
void DocumentBase::duplicateSelectedProperties()
{
    LOG_FCT_NAME

    Core::List<Ptr<Property> > ptiesSelected = getSelectedProperties();

    ptiesSelected = removePropertyChildren(ptiesSelected);

    Core::List<Ptr<IState> > undoStates;
    Core::List<Ptr<IState> > redoStates;

    getStateRecorder()->startRecording();

    Core::List<Ptr<Property> > propertiesForCancelRecording;
    Core::List<Ptr<Property> > propertiesDuplicated;

    for(int32 ii=0; ii<ptiesSelected.size(); ++ii)
    {
        if (ptiesSelected[ii]->getParent() != NULL)
        {
            Ptr<Property> pPtyClone;
            cloneWithChild(pPtyClone, *ptiesSelected[ii], false);
            pPtyClone->resurrect(getWorldDocument(), getWorldInfoContent());
            pPtyClone->setName(ptiesSelected[ii]->getName() + L"_copie");
            ptiesSelected[ii]->getParent()->addChild(pPtyClone);
            
            undoStates.push_back( Ptr<IState> ((IState*) new StatePropertyDestroy (*pPtyClone)) );
            redoStates.push_back( Ptr<IState> ((IState*) new StatePropertyCreated (*pPtyClone)) );
            propertiesForCancelRecording.push_back(pPtyClone);

            addContent(pPtyClone);
            propertiesDuplicated.push_back(pPtyClone);
        }
    }

    

    if (undoStates.size()>0)
    {
        //on selectionn les nouvelles properties 
        Ptr<PropertySelection> pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>  (getPropertyDocumentEdition()->getPtySelection());

        Ptr<IState> pUndoStatePtyS (new StatePropertyChanged (*pPtyS));
        pPtyS->deselectAll();
        pPtyS->select(propertiesDuplicated);
        Ptr<IState> pRedoStatePtyS (new StatePropertyChanged (*pPtyS));

        undoStates.push_back(pUndoStatePtyS);
        redoStates.push_back(pRedoStatePtyS);

        getStateRecorder()->getCurrentChangeSet()->recordUndoStates(undoStates);
        getStateRecorder()->getCurrentChangeSet()->recordRedoStates(redoStates);

        getStateRecorder()->stopRecording();
    }
    else
    {
        getStateRecorder()->cancelRecording(propertiesForCancelRecording);
    }

    emit documentContentUpdated();
}
//-----------------------------------------------------------------------------
void DocumentBase::addChildrenToCurrentProperty(Core::List<Ptr<Property> > pties)
{
    LOG_FCT_NAME

    Core::List<Ptr<Property> > ptiesSelected = getSelectedProperties();

    ptiesSelected = removePropertyChildren(ptiesSelected);

    Property* pParent;
    if (ptiesSelected.size() > 0)
    {
        pParent = ptiesSelected[0]->getParent();
    }
    else
    {
        pParent = const_cast<Property*>(getPropertyDocumentContentStart().get());
    }

    Core::List<Ptr<Property> > newProperties;

    getStateRecorder()->startRecording();
    Core::List<Ptr<IState> > undoStates;
    Core::List<Ptr<IState> > redoStates;

    for (int32 ii=0; ii<pties.size(); ++ii)
    {
        Ptr<Property> pChild;
        cloneWithChild(pChild, *pties[ii]);
        pChild->resurrect(getWorldDocument(), getWorldInfoContent());
        pChild->setName(pChild->getName() + L"_copie");
        addChildRecording       (getStateRecorder(), pChild, *pParent);

        addContent(pChild);
        newProperties.push_back(pChild);
    }

    if (newProperties.size()>0)
    {
        //on selectionn les nouvelles properties 
        Ptr<PropertySelection> pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>  (getPropertyDocumentEdition()->getPtySelection());

        Ptr<IState> pUndoStatePtyS (new StatePropertyChanged (*pPtyS));
        pPtyS->deselectAll();
        pPtyS->select(newProperties);
        Ptr<IState> pRedoStatePtyS (new StatePropertyChanged (*pPtyS));

        undoStates.push_back(pUndoStatePtyS);
        redoStates.push_back(pRedoStatePtyS);

        getStateRecorder()->getCurrentChangeSet()->recordUndoStates(undoStates);
        getStateRecorder()->getCurrentChangeSet()->recordRedoStates(redoStates);

        getStateRecorder()->stopRecording();
    }
    else
    {
        getStateRecorder()->cancelRecording(newProperties);
    }

    emit documentContentUpdated();
}
//-----------------------------------------------------------------------------
Ptr<PropertyNode> DocumentBase::getPropertyRoot() const
{
    LOG_FCT_NAME

    return _impl->propertyRoot;
}
//-----------------------------------------------------------------------------
const Ptr<Property>& DocumentBase::getPyDocumentInformation() const
{
    LOG_FCT_NAME

    return _impl->propertyRoot->getChild(TEMP_DOCUMENT_INFORMATION);
}
//-----------------------------------------------------------------------------
Ptr<PtyDocEdition> DocumentBase::getPropertyDocumentEdition() const
{
    LOG_FCT_NAME

    return LM_DEBUG_PTR_CAST<PtyDocEdition> (_impl->propertyRoot->getChild(TEMP_DOCUMENT_EDITION));
}
//-----------------------------------------------------------------------------
const Ptr<Property>& DocumentBase::getPropertyDocumentContent() const
{
    LOG_FCT_NAME

    return _impl->propertyRoot->getChild(TEMP_DOCUMENT_CONTENT);
}
//-----------------------------------------------------------------------------
const Ptr<Property>& DocumentBase::getPropertyDocumentContentStart() const
{
    LOG_FCT_NAME

    return _impl->propertyRoot->getChild(TEMP_DOCUMENT_CONTENT);
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentBase::getPtyForEdition() const
{
    LOG_FCT_NAME

    return null;
}
//-----------------------------------------------------------------------------
void DocumentBase::selectProperty(Ptr<Property> p)
{
    LOG_FCT_NAME

    Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (getPropertyDocumentEdition());
    Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyDocEdit->getPtySelection());

    if (p!=null)    pPtySelection->select(p);
    else            pPtySelection->deselectAll();
}
//-----------------------------------------------------------------------------
void DocumentBase::selectProperty(int32 id)
{
    LOG_FCT_NAME

    Ptr<Property> pPy = getProperty(id);
    selectProperty(pPy);    
}
//-----------------------------------------------------------------------------
void DocumentBase::selectAllProperties()
{
    Ptr<PropertySelection>  pPtySelection;
    Ptr<Property>           pPtyEdit = getPtyForEdition();

    if (pPtyEdit!=null)
    {
        pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyEdit->getPtySelection());
    }
    else
    {
        Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (getPropertyDocumentEdition());
        pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyDocEdit->getPtySelection());
    }

    pPtySelection->selectAll();
}
//-----------------------------------------------------------------------------
void DocumentBase::deselectAllProperties()
{
    Ptr<PropertySelection> pPtySelection;
    Ptr<Property>           pPtyEdit = getPtyForEdition();

    if (pPtyEdit!=null)
    {
        pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyEdit->getPtySelection());
    }
    else
    {
        Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (getPropertyDocumentEdition());
        pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyDocEdit->getPtySelection());
    }

    pPtySelection->deselectAll();
}
//-----------------------------------------------------------------------------
void DocumentBase::selectProperties(const Core::List<int32>& ids)
{
    LOG_FCT_NAME

    Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (getPropertyDocumentEdition());
    Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyDocEdit->getPtySelection());

    Core::List<Ptr<Property> > pties;
    getProperty(ids, pties);

    pPtySelection->select(pties);
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >& DocumentBase::getSelectedProperties() const
{
    LOG_FCT_NAME

    Ptr<PropertySelection> pPty = LM_DEBUG_PTR_CAST<PropertySelection>  (getPropertyDocumentEdition()->getPtySelection());
    return pPty->getSelectedPties();
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentBase::getProperty(int32 id) const
{
    LOG_FCT_NAME

    Ptr<Property> ret;

    _impl->propertyRoot->findPtyWithId(ret, id);

    return ret;
}
//-----------------------------------------------------------------------------
void DocumentBase::getProperty(const Core::List<int32>& ids, Core::List<Ptr<Property> >& pties)
{
    LOG_FCT_NAME

    pties.clear();

    for (int32 ii=0; ii<ids.size(); ++ii)
    {
        pties.push_back(getProperty(ids[ii]));
    }
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentBase::getDefaultProperty() const
{
    LOG_FCT_NAME

    return null;
}
//-----------------------------------------------------------------------------
void DocumentBase::setGenerationProperty(Ptr<Property> pProperty)
{
    LOG_FCT_NAME

    //_impl->setGenerationProperty(pProperty);
    connect(pProperty.get(), SIGNAL(generate(const Ptr<ImportInfo> &)), this, SLOT(generateInfo(const Ptr<ImportInfo> &)));
}
//-----------------------------------------------------------------------------
int32 DocumentBase::getPropertyIdFromUniverseNode(const Ptr<Universe::Node>& pNode)
{
    LOG_FCT_NAME

    Ptr<PropertyNode> pPtyContent = LM_DEBUG_PTR_CAST<PropertyNode> (getPropertyDocumentContent());

    return ptyTravelgetPropertyIdFromUniverseNode(pPtyContent, pNode);
}
//-----------------------------------------------------------------------------
QList<QAction*>& DocumentBase::getActionsCreateProperty()
{
    LOG_FCT_NAME

    return _impl->actionsCreateProperty;
}
//-----------------------------------------------------------------------------
void DocumentBase::setGuiDescription(const Ptr<GuiDescription>& pGuiDesc)
{
    LOG_FCT_NAME

    _impl->pGuiDesc = pGuiDesc;
}
//-----------------------------------------------------------------------------
void DocumentBase::activeProperty(int32 id)
{
    LOG_FCT_NAME

    selectProperty(id);
}
//-----------------------------------------------------------------------------
void DocumentBase::generateInfo(const Ptr<ImportInfo> & pImportInfo)
{
    LOG_FCT_NAME

    emit generate(pImportInfo);
}
//-----------------------------------------------------------------------------
void DocumentBase::contentIsModified()
{
    LOG_FCT_NAME

    setModified(true);
}
//-----------------------------------------------------------------------------
void DocumentBase::editIsModified()
{
    LOG_FCT_NAME
}
//-----------------------------------------------------------------------------
bool DocumentBase::ptiesSelectedBeforeUndo()
{
    const Core::List<Ptr<Property> >&   pties = getSelectedProperties();

    bool ret = true;

    for (int32 ii=0; ii<pties.size(); ++ii)
    {
        ret = pties[ii]->beforeUndo();
        if (ret==false)
            break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
bool DocumentBase::ptiesSelectedBeforeRedo()
{
    const Core::List<Ptr<Property> >&   pties = getSelectedProperties();

    bool ret = true;

    for (int32 ii=0; ii<pties.size(); ++ii)
    {
        ret = pties[ii]->beforeRedo();
        if (ret==false)
            break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
void DocumentBase::undo()
{
    LOG_FCT_NAME

    if (ptiesSelectedBeforeUndo() == true)
    {
        if (getStateRecorder()->getNbUndoState() == 0)
        {
            QMessageBox::information(NULL, "", "no undo");
        }
        else
        {
            getStateRecorder()->previousStateChangeSet(*this);
            emit documentContentUpdated();
        }
    }
}
//-----------------------------------------------------------------------------
void DocumentBase::redo()
{
    LOG_FCT_NAME

    if (ptiesSelectedBeforeRedo() == true)
    {
        if (getStateRecorder()->getNbRedoState() == 0)
        {
            QMessageBox::information(NULL, "", "no redo");
        }
        else
        {
            getStateRecorder()->nextStateChangeSet(*this);
            emit documentContentUpdated();
        }
    }
}
//-----------------------------------------------------------------------------
void DocumentBase::save()
{
    LOG_FCT_NAME
}
//-----------------------------------------------------------------------------
void DocumentBase::saveAs()
{
    LOG_FCT_NAME
}
//-----------------------------------------------------------------------------
void DocumentBase::currentSelectionChanged()
{
    LOG_FCT_NAME

    //_impl->currentSelectionChanged();
    Ptr<PropertySelection> pPty = LM_DEBUG_PTR_CAST<PropertySelection>  (getPropertyDocumentEdition()->getPtySelection());
    emit currentSelectionChanged(pPty->getSelectedPties());
}
//-----------------------------------------------------------------------------
void DocumentBase::runStopScript()
{
    LOG_FCT_NAME
}
//-----------------------------------------------------------------------------
bool DocumentBase::isEditing()
{
    LOG_FCT_NAME

    return _impl->isEditing;
}
//-----------------------------------------------------------------------------
void DocumentBase::setEditing(bool edit)
{
    LOG_FCT_NAME

    _impl->isEditing = edit;
    emit editingChange (_impl->isEditing);
}
//-----------------------------------------------------------------------------
Ptr<IDocumentWidget> DocumentBase::createDocumentWidget(QWidget* parent) const
{
    return null;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::World>& DocumentBase::getWorldInfoContent() const
{
    return _impl->pWorldInfo;
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<PtyNodeCamera> >& DocumentBase::getAllCameraContent() const
{
    return _impl->idContent.ptiesCameraContent;
}
//-----------------------------------------------------------------------------
Core::List<Ptr<Property> > DocumentBase::getAllGizmoContent() const
{
    Core::List<Ptr<Property> > ptiesGizmo;

    for (int32 ii=0; ii<_impl->idContent.idPtiesGizmo.size(); ++ii)
    {
        Ptr<Property> pPty = getProperty(_impl->idContent.idPtiesGizmo[ii]);

        if (pPty!=null) ptiesGizmo.push_back(pPty);
    }

    return ptiesGizmo;
}
//-----------------------------------------------------------------------------
Ptr<PtyNodeCamera>& DocumentBase::getCameraContent(int32 ptyID) const
{
    if (_impl->idContent.ptiesCameraContent.size() == 0) return pPtyCameraBidon;

    int32 ii=0;
    for (; ii<_impl->idContent.idPtiesCameraContent.size(); ii++)
    {
        if (_impl->idContent.idPtiesCameraContent[ii] == ptyID)
        {
            break;
        }
    }

    if (ii>=_impl->idContent.idPtiesCameraContent.size())
        return pPtyCameraBidon;

    LM_ASSERT(_impl->idContent.ptiesCameraContent[ii]->getId() == ptyID);
    return _impl->idContent.ptiesCameraContent[ii];
}
//-----------------------------------------------------------------------------
const Ptr<Property>& DocumentBase::getPropertyDocumentContentRoot() const
{
    return getPropertyDocumentContentStart();
}
//-----------------------------------------------------------------------------
void DocumentBase::createProperty(Ptr<Property> newPty, Ptr<Property> ptyParent)
{
    getStateRecorder()->startRecording();
        Ptr<PropertySelection> pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>  (getPropertyDocumentEdition()->getPtySelection());
        Ptr<IState> pUndoStatePtyS (new StatePropertyChanged (*pPtyS));
        getStateRecorder()->getCurrentChangeSet()->recordUndoState(pUndoStatePtyS);
        pPtyS->deselectAll();
        addChildRecording(getStateRecorder(), newPty, ptyParent);
        pPtyS->select(newPty);
        Ptr<IState> pRedoStatePtyS (new StatePropertyChanged (*pPtyS));
        getStateRecorder()->getCurrentChangeSet()->recordRedoState(pRedoStatePtyS);
    getStateRecorder()->stopRecording();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyNodeEmitter()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::NodeEmitter> pNodeEmitter = getWorldDocument()->createBillboardEmitter();
    pNodeEmitter->beginMatrixUpdate();
    pNodeEmitter->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    pNodeEmitter->endMatrixUpdate();

    Ptr<Property> pPtyNodeEmitter(new PtyNodeEmitter(getWorldDocument(), pNodeEmitter));

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentRoot();
    if (pParent->isLeaf() == true)  pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyNodeEmitter, pParent);
    addContent(pPtyNodeEmitter);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyNodeTrail()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::NodeTrail> pNode = getWorldDocument()->createTrail();
    pNode->beginMatrixUpdate();
    pNode->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    pNode->endMatrixUpdate();

    Ptr<Property> pPtyNodeTrail(new PtyNodeTrail(getWorldInfoContent(), getWorldDocument(), pNode));

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentRoot();
    if (pParent->isLeaf() == true)  pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyNodeTrail, pParent);
    addContent(pPtyNodeTrail);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyNodeSound()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::NodeSoundSource> pNode = getWorldDocument()->createSoundSource();
        pNode->beginMatrixUpdate();
        pNode->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
        pNode->endMatrixUpdate();


    Ptr<Property> pPty  (new PtyNodeSound(getWorldInfoContent(), getWorldDocument(), pNode));

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentRoot();
    if (pParent->isLeaf() == true)  pParent = getPropertyDocumentContentRoot();

    createProperty(pPty, pParent);
    addContent(pPty);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyNodePointLight()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::NodePointLight> pNode = getWorldDocument()->createPointLight();
    pNode->beginMatrixUpdate();
    pNode->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    pNode->endMatrixUpdate();

    Ptr<Property> pPtyNodePL(new PtyNodePointLight(getWorldInfoContent(), getWorldDocument(), pNode));

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentRoot();
    if (pParent->isLeaf() == true)  pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyNodePL, pParent);
    addContent(pPtyNodePL);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyStorm()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::NodeStorm> pStorm = getWorldDocument()->createStorm();
    pStorm->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));

    Ptr<Property> pPtyNodeStorm(new PtyNodeStorm(getWorldDocument(), pStorm));

    Ptr<Property> pParent;
    if(selectedPties.size() > 0) pParent = selectedPties[0];
    else                         pParent = getPropertyDocumentContentRoot();
    if(pParent->isLeaf())        pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyNodeStorm, pParent);
    addContent(pPtyNodeStorm);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyNodeGroup()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::NodeGroup> pNodeG = getWorldDocument()->createGroup();
    pNodeG->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));

    Ptr<Property> pPtyNodeGroup(new PtyNode(getWorldDocument(), pNodeG, L"Group"));

    Ptr<Property> pParent;
    if(selectedPties.size() > 0) pParent = selectedPties[0];
    else                         pParent = getPropertyDocumentContentRoot();
    if(pParent->isLeaf())        pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyNodeGroup, pParent);
    addContent(pPtyNodeGroup);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyDecal()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::NodeDecal> pNodeDecal = getWorldDocument()->createDecal();
    pNodeDecal->beginMatrixUpdate();
    pNodeDecal->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    pNodeDecal->endMatrixUpdate();

    Ptr<Property> pPtyNodeDecal(new PtyNodeDecal(getWorldDocument(), pNodeDecal));

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentRoot();
    if (pParent->isLeaf() == true)  pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyNodeDecal, pParent);
    addContent(pPtyNodeDecal);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyTerrain()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();
    
    Ptr<Universe::NodeTerrain> pNodeTerrain = getWorldDocument()->createTerrain();
    Ptr<Property>  pPtyTerrain (new PtyTerrain(getWorldDocument(), pNodeTerrain, getDocumentRenderer(), L"Terrain"));

    setGenerationProperty(pPtyTerrain);

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentRoot();
    if (pParent->isLeaf() == true)  pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyTerrain, pParent);
    addContent(pPtyTerrain);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyWaterPatch()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();
    
    Ptr<Universe::NodeWaterPatch> pNodeW = getWorldDocument()->createWaterPatch(200, 200);
    Ptr<Property>  pPtyWater (new PtyWaterPatch(getWorldDocument(), pNodeW));

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentRoot();
    if (pParent->isLeaf() == true)  pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyWater, pParent);
    addContent(pPtyWater);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyTerrain(const Core::String & fileName)
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::NodeTerrain> pNodeTerrain = getWorldDocument()->createTerrain(fileName);

    Ptr<Property>  pPtyTerrain (new PtyTerrain(getWorldDocument(), pNodeTerrain, getDocumentRenderer(), fileName));

    setGenerationProperty(pPtyTerrain);

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentRoot();
    if (pParent->isLeaf() == true)  pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyTerrain, pParent);
    addContent(pPtyTerrain);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentBase::createPropertyNodeInstance(const Core::String & fileName, Ptr<PtyNodeInstance>& pPtyNodeI)
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::Node> pNodeInstance = getWorldDocument()->createInstance(fileName);

    Ptr<Property>  pPtyNI (new PtyNodeInstance(getWorldDocument(), pNodeInstance, fileName));

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentRoot();
    if (pParent->isLeaf() == true)  pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyNI, pParent);
    addContent(pPtyNI);

    emit documentContentUpdated();
    contentIsModified();

    pPtyNodeI = LM_DEBUG_PTR_CAST<PtyNodeInstance>(pPtyNI);
}
//-----------------------------------------------------------------------------
void DocumentBase::createGizmo(const Ptr<Property>& pPty)
{
    Ptr<Universe::Node> pNode = getWorldDocument()->createNode();
    pNode->addTag(TAG_GIZMO);

    Ptr<Property>  pPtyNG (new PtyNodeGizmo(getWorldInfoContent(), getWorldDocument(), pNode, L"Gizmo"));

    Ptr<Property> pParent;
    if (pPty !=null)                pParent = pPty;
    else                            pParent = getPropertyDocumentContentRoot();
    if (pParent->isLeaf() == true)  pParent = getPropertyDocumentContentRoot();

    createProperty(pPtyNG, pParent);
    addContent(pPtyNG);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
bool DocumentBase::canClose(bool userInteraction) const
{
    if (isClosable() == false)
    {
        return false;
    }
    else
    {
        if (isModified() == true)
        {
            if (userInteraction==false)
            {
                return false;
            }

            int ret = QMessageBox::warning(NULL, "Close Document Without saving ?","Close Document Without saving ?",
                QMessageBox::Ok | QMessageBox::No, QMessageBox::No);

            if (ret==QMessageBox::No) return false;
            else return true;
        }
    }

    return true;
}
//-----------------------------------------------------------------------------
bool DocumentBase::dropInViewportIsPossible() const
{
    if (_impl->formatDropable.size()>0) return true;
    return false;
}
//-----------------------------------------------------------------------------
bool DocumentBase::dropThisFormatIsAuthorized(const QMimeData* mimeData, QString& format) const
{
    bool isMimeDataAuthorized = false;

    int iMimeData = 0;
    while(iMimeData < _impl->formatDropable.size() && isMimeDataAuthorized == false)
    {
        if(mimeData->hasFormat(_impl->formatDropable[iMimeData]))
        {
            isMimeDataAuthorized = true;
            format = _impl->formatDropable[iMimeData];
        }

        ++iMimeData;
    }

    return isMimeDataAuthorized;
}
//-----------------------------------------------------------------------------
void DocumentBase::dropThisFile(const QMimeData* mimeData, QString& format)
{
    QString text = mimeData->data(format);
    Core::String fileName(text.toStdString().c_str());


    if(format == "asset/terrain")
    {
        createPropertyTerrain(fileName);
    }
    else if(format == "asset/model" || format == "asset/particles" || format == "asset/group")
    {
        Ptr<PtyNodeInstance> pPNI;
        createPropertyNodeInstance(fileName, pPNI);
    }
}
//-----------------------------------------------------------------------------
Ptr<Property>   DocumentBase::getActivePlayerPosition()
{
    if (_impl->idContent.idPtiesPlayersPos.size() == 0)
        return null;

    int32 id = _impl->idContent.idPtiesPlayersPos[0];
    return getProperty(id);
}
//-----------------------------------------------------------------------------
void DocumentBase::addContent(const Ptr<Property>& pPty)
{
    switch(pPty->getCalssName())
    {
        case PTYNODE_PLAYER_POSITION:   _impl->idContent.idPtiesPlayersPos.push_back(pPty->getId()); break;
        case PTYNODE_CAMERA_CLASS:      
            _impl->idContent.idPtiesCameraContent.push_back(pPty->getId()); 
            _impl->idContent.ptiesCameraContent.push_back(LM_DEBUG_PTR_CAST<PtyNodeCamera>(pPty));
            break;
        case PTYNODE_GIZMO:
             _impl->idContent.idPtiesGizmo.push_back(pPty->getId()); 
            break;
        case PTY_GROUP_MEMOCAM:         _impl->idContent.idPtyGroupMemoCamera = pPty->getId(); break;
    }

    if (pPty->isRepresented() == true)
    {
        _impl->idContent.idPtiesWithRepresentation.push_back(pPty->getId());
    }

    for (int32 idChild=0; idChild<pPty->getNbChildren(); ++idChild)
    {
        addContent(pPty->getChild(idChild));
    }
}
//-----------------------------------------------------------------------------
void DocumentBase::removeContent(const Core::List<Ptr<Property> >& pties)
{
    for (int32 ii=0; ii<pties.size(); ii++)
    {
        removeContent(*pties[ii]);
    }
}
//-----------------------------------------------------------------------------
void DocumentBase::removeContent(const Property& pty)
{
    for (int32 idChild=0; idChild<pty.getNbChildren(); ++idChild)
    {
        removeContent(*(pty.getChild(idChild)));
    }

    Core::List<int32>::iterator it;

    switch(pty.getCalssName())
    {
    case PTYNODE_PLAYER_POSITION: it = std::find(_impl->idContent.idPtiesPlayersPos.begin(), _impl->idContent.idPtiesPlayersPos.end(), pty.getId()); _impl->idContent.idPtiesPlayersPos.erase(it); break;
    case PTYNODE_CAMERA_CLASS: 
    {
        it = std::find(_impl->idContent.idPtiesCameraContent.begin(), _impl->idContent.idPtiesCameraContent.end(), pty.getId());
        _impl->idContent.idPtiesCameraContent.erase(it);

        int32 ii=0;
        for (; ii<_impl->idContent.ptiesCameraContent.size(); ++ii)
        {
            if (_impl->idContent.ptiesCameraContent[ii]->getId() == pty.getId())
            {
                break;
            }
        }

        LM_ASSERT(ii<_impl->idContent.ptiesCameraContent.size());
        _impl->idContent.ptiesCameraContent.erase(_impl->idContent.ptiesCameraContent.begin() + ii);
        break;
    }
    case PTYNODE_GIZMO: 
        it = std::find(_impl->idContent.idPtiesGizmo.begin(), _impl->idContent.idPtiesGizmo.end(), pty.getId()); _impl->idContent.idPtiesGizmo.erase(it); break;
        break;
    }

    if (pty.isRepresented() == true)
    {
        it = std::find(_impl->idContent.idPtiesWithRepresentation.begin(), _impl->idContent.idPtiesWithRepresentation.end(), pty.getId()); _impl->idContent.idPtiesWithRepresentation.erase(it);
    }
}
//-----------------------------------------------------------------------------
void DocumentBase::travelInitializeIdContent(const Ptr<Property>& pPty)
{
    addContent(pPty);

    for (int32 ii=0; ii<pPty->getNbChildren(); ++ii)
    {
        travelInitializeIdContent(pPty->getChild(ii));
    }
}

void DocumentBase::initializeIdContent()
{
    _impl->idContent.clear();
    Ptr<Property>   pRoot = getPropertyDocumentContent();
    //travelInitializeIdContent(pRoot);
    addContent(pRoot);
}
//-----------------------------------------------------------------------------
QStringList& DocumentBase::getFormatDropable()
{
    return _impl->formatDropable;
}
//-----------------------------------------------------------------------------
void DocumentBase::exportCurrentPtyAsGroup()
{
    const Core::List<Ptr<Property> >&   pties = getSelectedProperties();

    if (pties.size() != 1 || pties[0]->isDeletable() == false)  return;

    if (pties[0]->hasFlag(FLAG_PTY_NODE) == false)              return;


    QtToolbox::VFSDialog dlg (*getDocumentRenderer()->getVFS(), QtToolbox::VFSD_SAVE, DOCUMENT_GROUP_EXT, NULL);

    if(dlg.exec())
    {
        Core::String fileName = dlg.getSelectedFile();

        Ptr<PtyNode> pPtyNode = LM_DEBUG_PTR_CAST<PtyNode>  (pties[0]);
        Ptr<Universe::NodeGroup>    pNodeG;

        if (pPtyNode->getUniverseNode()->getNodeType() == Universe::NODE_GROUP)
        {
            pNodeG = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(pPtyNode->getUniverseNode());
        }
        else
        {
            pNodeG = getWorldDocument()->createGroup();
            pNodeG->addChild(pPtyNode->getUniverseNode(), true);
            
        }

        
        
        Ptr<PtyTransform>           pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(pPtyNode->getPtyTransform()->clone());

        LM_DEBUG_PTR_CAST<PtyTransform>(pPtyNode->getPtyTransform())->reset();

        

        saveGroup(getDocumentRenderer()->getVFS(), getWorldDocument(), pNodeG, fileName);

        if (pPtyNode->getUniverseNode()->getNodeType() != Universe::NODE_GROUP)
        {
            pNodeG->removeChild(pPtyNode->getUniverseNode(), true);
            pNodeG->kill();
            pNodeG = null;
        }

        int ret = QMessageBox::information(NULL, "Question :", "Convert selection to group Instance ?",
                    QMessageBox::Ok | QMessageBox::No, QMessageBox::No);

        if (ret==QMessageBox::No) {pPtyNode->getPtyTransform()->copy(pPtyT); return;}
            
        getStateRecorder()->startRecording();
            deleteSelectedProperties();
            Ptr<PtyNodeInstance> pPtyNI;
            createPropertyNodeInstance(fileName, pPtyNI);
            pPtyNI->getPtyTransform()->copy(pPtyT);
        getStateRecorder()->stopRecording();

        Ptr<Universe::Node> test2 = pPtyNI->getUniverseNode();
    }
}
//-----------------------------------------------------------------------------
void DocumentBase::escape()
{
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera> DocumentBase::getCameraEdition() const
{
    return null;
}
//-----------------------------------------------------------------------------
Ptr<PtyNodeCamera>  DocumentBase::createPropertyMemoCamera(const EMemoCameraID& id)
{
    if (_impl->idContent.idPtyGroupMemoCamera == -1)
    {
        Ptr<Universe::Node> pNode = getWorldDocument()->createNode();
        pNode->addTag(TAG_GROUP_MEMO_CAM);

        pNode->beginMatrixUpdate();
            pNode->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
        pNode->endMatrixUpdate();

        Ptr<Property> pPtyNodeMC(new PtyNodeGroupMemoCam(getWorldDocument(), pNode));
        getPropertyDocumentContentStart()->addChild(pPtyNodeMC);
        addContent(pPtyNodeMC);
    }

    Ptr<PtyNodeGroupMemoCam> pPtyMC = LM_DEBUG_PTR_CAST<PtyNodeGroupMemoCam>(getProperty(_impl->idContent.idPtyGroupMemoCamera));

    Ptr<Universe::NodeCamera> pNodeC = getWorldDocument()->createPerspectiveCamera();
    String tag = EPI::toString(id);
    pNodeC->addTag(tag);
    pNodeC->beginMatrixUpdate();
    pNodeC->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    pNodeC->endMatrixUpdate();


    Ptr<PtyNodeCamera> pPtyNodeC(new PtyNodeCamera(getWorldInfoContent(), getWorldDocument(), pNodeC));
    pPtyNodeC->setName(L"MemoCam_"+Core::toString(id+1));
    pPtyMC->addChild(pPtyNodeC);

    addContent(pPtyNodeC);

    emit documentContentUpdated();
    contentIsModified();

    return pPtyNodeC;
}
//-----------------------------------------------------------------------------
void DocumentBase::setMemoCamera(const Ptr<Universe::NodeCamera>& pCam, const EMemoCameraID& id)
{
    Core::List<int32>& idsCamContent = _impl->idContent.idPtiesCameraContent;

    Ptr<PtyNodeCamera> pPtyMemoCam = null;

    for (int32 ii=0; ii<idsCamContent.size(); ++ii)
    {
        Ptr<PtyNodeCamera> pPtyCam = LM_DEBUG_PTR_CAST<PtyNodeCamera>(getProperty(idsCamContent[ii]));

        if (pPtyCam->getUniverseNode()->hasTag(EPI::toString(id)) == true)
        {
            pPtyMemoCam = pPtyCam;
            break;
        }
    }

    if (pPtyMemoCam == null)
    {
        pPtyMemoCam = createPropertyMemoCamera(id);
    }

    Ptr<PtyTransformNode> pPtyT = LM_DEBUG_PTR_CAST<PtyTransformNode>(pPtyMemoCam->getPtyTransform());
    pPtyT->setWorldPosition(pCam->getWorldPosition());
    pPtyT->setWorldOrientation(pCam->getLocalToWorldMatrix().getRotation());
    pPtyT->updateData();
}
//-----------------------------------------------------------------------------
void DocumentBase::getMemoCamera(Ptr<Universe::NodeCamera> pCam, const EMemoCameraID& id)
{
    Core::List<int32>& idsCamContent = _impl->idContent.idPtiesCameraContent;

    Ptr<PtyNodeCamera> pPtyMemoCam = null;

    for (int32 ii=0; ii<idsCamContent.size(); ++ii)
    {
        Ptr<PtyNodeCamera> pPtyCam = LM_DEBUG_PTR_CAST<PtyNodeCamera>(getProperty(idsCamContent[ii]));

        if (pPtyCam->getUniverseNode()->hasTag(EPI::toString(id)) == true)
        {
            pPtyMemoCam = pPtyCam;
            break;
        }
    }

    if (pPtyMemoCam!=null)
    {
        Ptr<PtyTransformNode> pPtyT = LM_DEBUG_PTR_CAST<PtyTransformNode>(pPtyMemoCam->getPtyTransform());

        pCam->setWorldPosition(pPtyT->getWorldPosition());
        pCam->setLocalOrientation(Core::Quaternionf(pPtyT->getWorldAxis()));
    }
}
//-----------------------------------------------------------------------------
void DocumentBase::convertInstance(Core::List<Ptr<Property> > pties)
{
    int ret = QMessageBox::information(NULL, "Question :", "Convert selection ?",
                    QMessageBox::Ok | QMessageBox::No, QMessageBox::No);

    if (ret==QMessageBox::No) {return;}


    Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (getPropertyDocumentEdition());
    Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyDocEdit->getPtySelection());

    pPtySelection->deselectAll();
    pties = removePropertyChildren  (pties);


    getStateRecorder()->startRecording();
    for (int32 ii=0; ii<pties.size(); ++ii)
    {
        if (pties[ii]->getCalssName() == PTYNODE_INSTANCE)
        {
            Core::List<int32 >      generationPtyNode;

            Ptr<PtyNodeInstance> pPtyNI = LM_DEBUG_PTR_CAST<PtyNodeInstance> (pties[ii]);
            convertInstanceProperty(getStateRecorder(), pPtyNI, getWorldInfoContent(), getWorldDocument(), getDocumentRenderer(), *(pties[ii]->getParent()), generationPtyNode);

            for(int32 iPty = 0; iPty < generationPtyNode.size(); ++iPty)
                setGenerationProperty(getProperty(generationPtyNode[iPty]));
        }
    }
    getStateRecorder()->stopRecording();

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
Ptr<IWorldScriptable>  DocumentBase::getWorldScriptable() const
{
    return _impl->pWorldA;
}
//-----------------------------------------------------------------------------
void DocumentBase::restart()
{
}
//-----------------------------------------------------------------------------
void DocumentBase::stop()
{
}
//-----------------------------------------------------------------------------
void DocumentBase::activeDesactiveIHM()
{
    _impl->showIHM = !_impl->showIHM;

    for (int32 ii=0; ii<_impl->idContent.idPtiesWithRepresentation.size(); ++ii)
    {
        Ptr<Property> pPty = getProperty(_impl->idContent.idPtiesWithRepresentation[ii]);
        if (pPty!=null)
        {
            pPty->setRepresentationVisible(_impl->showIHM);
        }
    }
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeListener> DocumentBase::getDocumentSoundListener()
{
    return getWorldDocument()->getNodeListener();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_EPI QIcon documentTypeToIcon(DocumentType type)
{
    QIcon ret;

    switch(type)
    {
    case DOC_MATERIAL:      ret = QIcon(":/icons/model.png"); break;
    case DOC_MODEL:         ret = QIcon(":/icons/model.png"); break;
    case DOC_TEXTURE:       ret = QIcon(":/icons/texture.png"); break;
    case DOC_TERRAIN:       ret = QIcon(":/icons/terrain.png"); break;
    case DOC_WORLD:         ret = QIcon(":/icons/world.png"); break;
    case DOC_SCRIPT:        ret = QIcon(":/icons/script.png"); break;
    case DOC_GROUP:         ret = QIcon(":/icons/group.png"); break;
    case DOC_SPLINE:        ret = QIcon(":/icons/spline.png"); break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}  // namespace EPI

