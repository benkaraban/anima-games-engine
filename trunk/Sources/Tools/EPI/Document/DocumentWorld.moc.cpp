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
#include <EPI/Document/DocumentWorld.moc.h>

#include <QAction>
#include <QMessageBox>

#include <QtToolbox/VFSDialog.moc.h>

#include <EPI/Document/StateRecorder.h>

#include <EPI/Document/Properties/PtyDocWorld.h>
#include <EPI/Document/Properties/PtyNodeInstance.h>
#include <EPI/Document/Properties/PtyNodeCamera.h>

#include <EPI/Document/Properties/PtyDocInformation.moc.h>
#include <EPI/ImportInfo/ImportWorldInfo.h>

#include <EPI/Document/Properties/PtyNodeEmitter/PtyNodeEmitter.moc.h>
#include <EPI/Document/Properties/PtyWaterPatch.h>
#include <EPI/Document/Properties/PtyTerrain.moc.h>
#include <EPI/Document/Properties/PtyNodeDecal.moc.h>
#include <EPI/Document/Properties/PtyNodeStorm.moc.h>
#include <EPI/Document/Properties/PtyNodePlayerPosition.moc.h>
#include <EPI/Document/LuaOutput.moc.h>
#include <EPI/Document/Tools.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DocumentWorld::DocumentWorld(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr)
:   DocumentBase(guiGoc, DOC_WORLD, pDocRdr, true)
{
    Ptr<Property> pPtyDocWorld (new PtyDocWorld(guiGoc, getWorld(), getDocumentRenderer(), L"World-Editing"));
    getPropertyDocumentContent()->addChild(pPtyDocWorld);

    initialize();
}
//-----------------------------------------------------------------------------
DocumentWorld::DocumentWorld(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr, const Core::String & fileName)
:   DocumentBase(guiGoc, DOC_WORLD, pDocRdr, true)
{
    loadFile(fileName);

    Ptr<PtyDocWorld> pPtyDocWorld (new PtyDocWorld(guiGoc, getWorld(), getDocumentRenderer(), L"World-Editing"));
    getPropertyDocumentContent()->addChild(pPtyDocWorld);

    Core::List<Ptr<Universe::Node> > nodes;
    getWorld()->getAllNodesRoots(nodes);
    universeNodesToProperties(getWorldInfoContent(), getWorld(), getDocumentRenderer(), nodes, *(getPropertyDocumentContent()->getChild(0)), _generationPtyNode);

    const Core::List<int32> & generationNodes = getGenerationProperty();

    for(int32 iPty = 0; iPty < generationNodes.size(); ++iPty)
        setGenerationProperty(getProperty(generationNodes[iPty]));

    initialize();
}
//-----------------------------------------------------------------------------
DocumentWorld::~DocumentWorld()
{

}
//-----------------------------------------------------------------------------
void DocumentWorld::initialize()
{
    //actions
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
    getActionsCreateProperty().push_back(new QAction("Camera", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyCamera()));
    getActionsCreateProperty().push_back(new QAction("Group", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyNodeGroup()));
    getActionsCreateProperty().push_back(new QAction("Players Position", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyNodePlayerPosition()));
    getActionsCreateProperty().push_back(new QAction("Sound", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyNodeSound()));

    /*
    getActionsCreateProperty().push_back(new QAction("Test", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyTest()));*/


    QStringList& dropable = getFormatDropable();
    dropable << "asset/terrain" << "asset/model" << "asset/particles" << "asset/group";

    initializeIdContent();

    restart();
}
//-----------------------------------------------------------------------------
void DocumentWorld::createPropertyCamera()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::NodeCamera> pNodeC = getWorld()->createPerspectiveCamera();
    pNodeC->beginMatrixUpdate();
    pNodeC->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 0.0f));
    pNodeC->endMatrixUpdate();



    Ptr<Property> pPtyNodeC(new PtyNodeCamera(getWorldInfoContent(), getWorld(), pNodeC));

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentStart();
   
    createProperty(pPtyNodeC, pParent);

    addContent(pPtyNodeC);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentWorld::createPropertyNodePlayerPosition()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();

    Ptr<Universe::Node> pNode = getWorld()->createNode();
    pNode->addTag(TAG_PLAYERS_POSITION);
    Ptr<Property> pPtyNodePPos(new PtyNodePlayerPosition(getWorldInfoContent(), getWorld(), pNode));

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentStart();

    createProperty(pPtyNodePPos, pParent);

    addContent(pPtyNodePPos);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& DocumentWorld::getPropertyDocumentContentStart() const
{
    return getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
Ptr<Property>& DocumentWorld::getPropertyDocumentContentStart()
{
    return getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
void DocumentWorld::save()
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
void DocumentWorld::saveAs()
{
    QtToolbox::VFSDialog dlg (*getDocumentRenderer()->getVFS(), QtToolbox::VFSD_SAVE, DOCUMENT_WORLD_EXT, NULL);
    if(dlg.exec())
    {
        Core::String fileName = dlg.getSelectedFile();
        saveFile(fileName);
        Ptr<PyDocumentInformation> pPinfo = LM_DEBUG_PTR_CAST<PyDocumentInformation>(getPyDocumentInformation());
        pPinfo->setTitle(fileName);
    }
}
//-----------------------------------------------------------------------------
void DocumentWorld::saveFile(const Core::String & fileName)
{
    Core::XMLDocument xmlDocument;

    getWorldDocument()->exportWorldXML(xmlDocument);

    Ptr<Core::OutputStream> pOutput = getDocumentRenderer()->getVFS()->writeFile(fileName);
    xmlDocument.saveDocument(*pOutput);
    pOutput->close();

    Ptr<ImportWorldInfo> pParticlesInfo(new ImportWorldInfo());
    pParticlesInfo->worldFileName = fileName;

    Ptr<Core::XMLDocument> pXMLWorldInfo(new Core::XMLDocument());
    pParticlesInfo->exportXML(pXMLWorldInfo);

    Core::String infoFile;
    infoFile << fileName << L".xml";
    Ptr<Core::OutputStream> pWorldInfoOutput = getDocumentRenderer()->getVFS()->writeFile(infoFile);
    pXMLWorldInfo->saveDocument(*pWorldInfoOutput);
    pWorldInfoOutput->close();

    setModified(false);
}
//-----------------------------------------------------------------------------
void DocumentWorld::loadFile(const Core::String & fileName)
{
    Core::XMLDocument xmlDocument;
    Ptr<Core::InputStream> pInput = getDocumentRenderer()->getVFS()->readFile(fileName);

    try
    {
        xmlDocument.loadDocument(*pInput);

        getWorld()->importWorldXML(xmlDocument);
    }
    catch(Core::Exception & e)
    {
        String message;
        message << L"An exception has been caught while extracting data from the XML file :\n" << e.getMessage();
        QMessageBox::critical( NULL, "Nyx", Core::String8(message).c_str());
    }

}
//-----------------------------------------------------------------------------
void DocumentWorld::runStopScript()
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
void DocumentWorld::errorInScript()
{
    runStopScript();
}
//-----------------------------------------------------------------------------
void DocumentWorld::escape()
{
    if (getWorldScriptable()->isRunning() == true)
    {
        runStopScript();
    }
}
//-----------------------------------------------------------------------------
void DocumentWorld::restart()
{
    getPropertyDocumentContentRoot()->restart();
}
//-----------------------------------------------------------------------------
void DocumentWorld::stop()
{
    getPropertyDocumentContentRoot()->stop();
}
//-----------------------------------------------------------------------------
const Ptr<Universe::World>& DocumentWorld::getWorld() const
{
    Ptr<IWorldScriptable>   pWorldA = getWorldScriptable();
    return pWorldA->getWorldForAnimation();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}  // namespace EPI