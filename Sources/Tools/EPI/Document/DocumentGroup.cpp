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
#include <EPI/Document/DocumentGroup.h>

#include <QMessageBox>
#include <QAction>

#include <QtToolbox/VFSDialog.moc.h>

#include <EPI/Document/Tools.h>

#include <EPI/Document/Properties/PtyDocEdition.moc.h>
#include <EPI/Document/PropertySelection.moc.h>
#include <EPI/Document/Properties/PtyDocGroup.h>
#include <EPI/Document/Properties/PtySun.moc.h>
#include <EPI/Document/Properties/PtySky.h>
#include <EPI/Document/Properties/PtyFog.moc.h>
#include <EPI/Document/Properties/PtyWorld.h>
#include <EPI/Document/Properties/PtyDocInformation.moc.h>
#include <Packing/TexCompressionOnTheFly.h>

#include <EPI/ImportInfo/ImportGroupInfo.h>

#include <EPI/Document/GuiDescription.h>
#include <EPI/DocumentRenderer.h>

#include <EPI/Document/Tools.h>
#include <EPI/Document/LuaOutput.moc.h>
namespace EPI
{
//-----------------------------------------------------------------------------
DocumentGroup::DocumentGroup(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr)
:   DocumentBase(guiGoc, DOC_GROUP, pDocRdr, true)
{
    Ptr<PropertyNode> pPtyContent = LM_DEBUG_PTR_CAST<PropertyNode> (getPropertyDocumentContent());
        pPtyContent->addChild(Ptr<Property>(new PtyDocGroup(getWorldDocument()->createGroup(), L"Group")));
        
    Ptr<Property> pPtySun   (new PtySun(guiGoc, getWorldDocument()));
    Ptr<Property> pPtySky   (new PtySky(getWorldDocument()));
    Ptr<Property> pPtyFog   (new PtyFog(getWorldDocument()));
    Ptr<Property> pPtyWorld (new PtyWorld(getWorldDocument()));

    pPtyContent->addChild(pPtySun);
    pPtyContent->addChild(pPtySky);
    pPtyContent->addChild(pPtyFog);
    pPtyContent->addChild(pPtyWorld);

    initialize();
}
//-----------------------------------------------------------------------------
DocumentGroup::DocumentGroup(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr, const Core::String & fileName)
:   DocumentBase(guiGoc, DOC_GROUP, pDocRdr, true)
{
    Ptr<PropertyNode> pPtyContent = LM_DEBUG_PTR_CAST<PropertyNode> (getPropertyDocumentContent());
        pPtyContent->addChild(Ptr<Property>(new PtyDocGroup(getWorldDocument()->createGroup(), L"Group")));
        pPtyContent->addChild(Ptr<Property>(new PtySun(guiGoc, getWorldDocument(), L"Sun")));

    loadFile(fileName);

    Core::List<int32 >      generationPtyNode;

    Ptr<PtyDocGroup> pPtyDocG = LM_DEBUG_PTR_CAST<PtyDocGroup>(getPropertyDocumentContent()->getChild(0));
    Core::List<Ptr<Universe::Node> > nodes = pPtyDocG->getNodeGroup()->getChildren();
       // nodes.push_back(pPtyDocG->getNodeGroup());

    universeNodesToProperties(getWorldInfoContent(), getWorldDocument(), getDocumentRenderer(), nodes, *(getPropertyDocumentContent()->getChild(0)), generationPtyNode);

    for(int32 iPty = 0; iPty < generationPtyNode.size(); ++iPty)
        setGenerationProperty(getProperty(generationPtyNode[iPty]));

    initialize();
}
//-----------------------------------------------------------------------------
DocumentGroup::~DocumentGroup()
{
}
//-----------------------------------------------------------------------------
Core::Orientationf DocumentGroup::getDefaultCameraOrientation() const
{
    return Core::Orientationf (Core::deg2rad(-10), Core::deg2rad(180), Core::deg2rad(0));
}
//-----------------------------------------------------------------------------
void DocumentGroup::initialize()
{
    getActionsCreateProperty().push_back(new QAction("WaterPatch", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyWaterPatch()));
    getActionsCreateProperty().push_back(new QAction("Terrain", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyTerrain()));
    getActionsCreateProperty().push_back(new QAction("NodeEmitter", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyNodeEmitter()));
    getActionsCreateProperty().push_back(new QAction("Trail", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyNodeTrail()));
    getActionsCreateProperty().push_back(new QAction("Point Light", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyNodePointLight()));
    getActionsCreateProperty().push_back(new QAction("Storm", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyStorm()));
    getActionsCreateProperty().push_back(new QAction("Decal", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyDecal()));

    QStringList& dropable = getFormatDropable();
    dropable << "asset/terrain" << "asset/model" << "asset/particles" << "asset/group";

    Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (getPropertyDocumentEdition());
    Ptr<PropertySelectionDefault> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelectionDefault> (pPtyDocEdit->getPtySelection());
    //pPtySelection->setRootPtySelectable(getPropertyDocumentContentRoot().get());
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentGroup::getDefaultProperty() const
{
    return getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
Ptr<Universe::World> DocumentGroup::getWorldDeco()
{
    Ptr<PtyDocGroup> pPtyDocG = LM_DEBUG_PTR_CAST<PtyDocGroup>(getPropertyDocumentContent()->getChild(0));

    if (_worldDecoPath != pPtyDocG->getWorldDecoPath())
    {
        _worldDecoPath = pPtyDocG->getWorldDecoPath();
        _pWorldDeco = null;

        if (_worldDecoPath != L"")
        {
            _pWorldDeco = Ptr<Universe::World>( new Universe::World(getDocumentRenderer()->getRenderer(),  getDocumentRenderer()->getVFS(), null, getDocumentRenderer()->getAudioDevice()));
                Ptr<Universe::IOnTheFlyTextureGenerator> pTexGen(new Packing::OnTheFlyTextureGenerator());
                _pWorldDeco->getRessourcesPool()->setOnTheFlyTextureGenerator(pTexGen);
                _pWorldDeco->getNodeListener()->setGain(0.f);
            try
            {
                Ptr<Core::InputStream> pInput = getDocumentRenderer()->getVFS()->readFile(_worldDecoPath);
                Core::XMLDocument xmlDocument;
                xmlDocument.loadDocument(*pInput);
                _pWorldDeco->importWorldXML(xmlDocument);
            }
            catch(Core::Exception & e)
            {
                String message;
                message << L"An exception has been caught while extracting data from the XML file :\n" << e.getMessage();
                QMessageBox::critical( NULL, "Nyx", Core::String8(message).c_str());
            }

            copyEnvironnementParam(*getWorldDocument(), *_pWorldDeco);
            configWorldDeco(getWorldDocument(), _pWorldDeco);

            //on met a jour les property concerner par le copyEnvironnementParam
            getPropertyDocumentContent()->getChild(1)->updateProperty();
        }
    }   

    return _pWorldDeco;
}
//-----------------------------------------------------------------------------
void DocumentGroup::save()
{
    if (getTitle().startsWith(L"/") == true)
    {
        saveFile(getTitle());
    }
    else
    {
        saveAs();
    }
}
//-----------------------------------------------------------------------------
void DocumentGroup::saveAs()
{
    QtToolbox::VFSDialog dlg (*getDocumentRenderer()->getVFS(), QtToolbox::VFSD_SAVE, DOCUMENT_GROUP_EXT, NULL);
    if(dlg.exec())
    {
        Core::String fileName = dlg.getSelectedFile();
        saveFile(fileName);
        Ptr<PyDocumentInformation> pPinfo = LM_DEBUG_PTR_CAST<PyDocumentInformation>(getPyDocumentInformation());
        pPinfo->setTitle(fileName);
    }
}
//-----------------------------------------------------------------------------
void DocumentGroup::saveFile(const Core::String & fileName)
{
    Ptr<PtyDocGroup> pPtyDocG = LM_DEBUG_PTR_CAST<PtyDocGroup>(getPropertyDocumentContent()->getChild(0));

    saveGroup(getDocumentRenderer()->getVFS(), getWorldDocument(), pPtyDocG->getNodeGroup(), fileName);

    setModified(false);
}
//-----------------------------------------------------------------------------
void DocumentGroup::loadFile(const Core::String & fileName)
{
    Core::XMLDocument xmlDocument;

    Ptr<Core::InputStream> pInput = getDocumentRenderer()->getVFS()->readFile(fileName);

    try
    {
        xmlDocument.loadDocument(*pInput);

        Core::List<Ptr<Core::XMLNode> > nodes;
        xmlDocument.getNodes(L"root", L"node", nodes);

        //Ptr<Universe::Node> nodeG = getWorld()->importXML(nodes[0]);
        Ptr<Universe::Node> nodeG = getWorldDocument()->importXML(nodes[0]);

        if (nodeG->getNodeType() != Universe::NODE_GROUP)
        {
            throw Core::Exception(L"Error when loading group Document");
        }
        else
        {
            Ptr<PtyDocGroup> pPtyDocG = LM_DEBUG_PTR_CAST<PtyDocGroup>(getPropertyDocumentContent()->getChild(0));
            pPtyDocG->setNodeGroup(LM_DEBUG_PTR_CAST<Universe::NodeGroup>(nodeG));
        }
    }
    catch(Core::Exception & e)
    {
        String message;
        message << L"An exception has been caught while extracting data from the XML file :\n" << e.getMessage();
        QMessageBox::critical( NULL, "Nyx", Core::String8(message).c_str());

    }
}
//-----------------------------------------------------------------------------
const Ptr<Property>& DocumentGroup::getPropertyDocumentContentRoot() const
{
    return getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
void DocumentGroup::runStopScript()
{
    Ptr<IWorldScriptable>   pWorldA = getWorldScriptable();
    
    if (isEditing() == true)
    {
        setEditing(false);
        connect(pWorldA->getLuaOutput().get(), SIGNAL(newErrorMsg(const String & )), this, SLOT(errorInScript()));
        pWorldA->startWorldScripts();
        emit runStopScriptChanged();
    }
    else
    {
        setEditing(true);
        pWorldA->stopScript();
        disconnect(pWorldA->getLuaOutput().get(), SIGNAL(newErrorMsg(const String & )), this, SLOT(errorInScript()));
        emit runStopScriptChanged();
    }
}
//-----------------------------------------------------------------------------
void DocumentGroup::escape()
{
    if (getWorldScriptable()->isRunning() == true)
    {
        runStopScript();
    }
}
//-----------------------------------------------------------------------------
void DocumentGroup::restart()
{
    getPropertyDocumentContentRoot()->restart();
}
//-----------------------------------------------------------------------------
void DocumentGroup::stop()
{
    getPropertyDocumentContentRoot()->stop();
}
//-----------------------------------------------------------------------------
const Ptr<Universe::World>& DocumentGroup::getWorld() const
{
    Ptr<IWorldScriptable>   pWorldA = getWorldScriptable();
    return pWorldA->getWorldForAnimation();
}
//-----------------------------------------------------------------------------
void DocumentGroup::addChildrenToCurrentProperty(Core::List<Ptr<Property> > pties)
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

    Ptr<PropertyNode> pPtyContent = LM_DEBUG_PTR_CAST<PropertyNode> (getPropertyDocumentContent());

    if (pParent->getId() == pPtyContent->getId())
    {
        Ptr<PtyDocGroup> pPtyDocG = LM_DEBUG_PTR_CAST<PtyDocGroup>(getPropertyDocumentContent()->getChild(0));
        pParent = pPtyDocG.get();
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
//-----------------------------------------------------------------------------
}  // namespace EPI
