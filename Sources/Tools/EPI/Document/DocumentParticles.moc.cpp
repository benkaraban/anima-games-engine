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
#include <EPI/Document/DocumentParticles.moc.h>

#include <QAction>
#include <QMessageBox>

#include <QtToolbox/VFSDialog.moc.h>

#include <EPI/Document/StateRecorder.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyDocNodeEmitters.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyNodeEmitter.moc.h>
#include <EPI/Document/Properties/PtyDocInformation.moc.h>
#include <EPI/Document/Tools.h>

#include <EPI/ImportInfo/ImportParticlesInfo.h>

#include <EPI/Document/GuiDescription.h>
#include <EPI/DocumentRenderer.h>
namespace EPI
{
//-----------------------------------------------------------------------------
DocumentParticles::DocumentParticles(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr)
:   DocumentBase(guiGoc, DOC_PARTICLES, pDocRdr),
    _pPtyDocNodeEmitters(null)
{
    Ptr<GuiDescription> pGuiDesc(new GuiDescription());

    Core::List<EViewportCameraNavigation> viewCameraNavigation;
    viewCameraNavigation.push_back(VIEWPORT_CAMERA_ORBITAL);
    viewCameraNavigation.push_back(VIEWPORT_CAMERA_QUAKE);
    pGuiDesc->setViewportCameraNavigations(viewCameraNavigation);

    setGuiDescription(pGuiDesc);

    _pPtyDocNodeEmitters = Ptr<PtyDocNodeEmitters>(new PtyDocNodeEmitters(getWorld(), false));
    getPropertyDocumentContent()->addChild(_pPtyDocNodeEmitters);

    //actions
    getActionsCreateProperty().push_back(new QAction("Node emitter", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyNodeEmitter()));

}
//-----------------------------------------------------------------------------
DocumentParticles::DocumentParticles(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr, const Core::String & fileName)
:   DocumentBase(guiGoc, DOC_PARTICLES, pDocRdr),
    _pPtyDocNodeEmitters(null)
{
    Ptr<GuiDescription> pGuiDesc(new GuiDescription());

    Core::List<EViewportCameraNavigation> viewCameraNavigation;
    viewCameraNavigation.push_back(VIEWPORT_CAMERA_ORBITAL);
    viewCameraNavigation.push_back(VIEWPORT_CAMERA_QUAKE);
    pGuiDesc->setViewportCameraNavigations(viewCameraNavigation);

    setGuiDescription(pGuiDesc);

    _pPtyDocNodeEmitters = Ptr<PtyDocNodeEmitters>(new PtyDocNodeEmitters(getWorld(), true));
    getPropertyDocumentContent()->addChild(_pPtyDocNodeEmitters);

    loadFile(fileName);

    //actions
    getActionsCreateProperty().push_back(new QAction("Node emitter", this));
    connect(getActionsCreateProperty().back(), SIGNAL(triggered()), this, SLOT(createPropertyNodeEmitter()));
}
//-----------------------------------------------------------------------------
DocumentParticles::~DocumentParticles()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentParticles::getDefaultProperty() const
{
    if(getPropertyDocumentContent()->getChild(0)->getNbChildren() > 0)
        return getPropertyDocumentContent()->getChild(0)->getChild(0);
    else
        return getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
Ptr<Universe::World> DocumentParticles::getWorldDeco()
{
    Ptr<PtyDocNodeEmitters> pPtyDocNodeEmitters = LM_DEBUG_PTR_CAST<PtyDocNodeEmitters>(getPropertyDocumentContent()->getChild(0));

    if (_worldDecoPath != pPtyDocNodeEmitters->getWorldDecoPath())
    {
        _worldDecoPath = pPtyDocNodeEmitters->getWorldDecoPath();
        _pWorldDeco = null;

        if (_worldDecoPath != L"")
        {
            _pWorldDeco = Ptr<Universe::World>( new Universe::World(getDocumentRenderer()->getRenderer(),  getDocumentRenderer()->getVFS(), null, getDocumentRenderer()->getAudioDevice()));
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

            copyEnvironnementParam(*getWorld(), *_pWorldDeco);
            configWorldDeco(getWorld(), _pWorldDeco);
        }
    }   

    return _pWorldDeco;
}
//-----------------------------------------------------------------------------
void DocumentParticles::createPropertyNodeEmitter()
{
    const Core::List<Ptr<Property> >& selectedPties = getSelectedProperties();
    
    Ptr<Property> pPtyNodeEmitter = _pPtyDocNodeEmitters->createNewNodeEmitter();

    Ptr<Property> pParent;
    if (selectedPties.size() > 0)   pParent = selectedPties[0];
    else                            pParent = getPropertyDocumentContentStart();
    addChildRecording(getStateRecorder(), pPtyNodeEmitter, pParent);

    emit documentContentUpdated();
    contentIsModified();
}
//-----------------------------------------------------------------------------
void DocumentParticles::save()
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
void DocumentParticles::saveAs()
{
    QtToolbox::VFSDialog dlg (*getDocumentRenderer()->getVFS(), QtToolbox::VFSD_SAVE, DOCUMENT_PARTS_EXT, NULL);
    if(dlg.exec())
    {
        Core::String fileName = dlg.getSelectedFile();
        saveFile(fileName);
        Ptr<PyDocumentInformation> pPinfo = LM_DEBUG_PTR_CAST<PyDocumentInformation>(getPyDocumentInformation());
        pPinfo->setTitle(fileName);
    }
}
//-----------------------------------------------------------------------------
void DocumentParticles::saveFile(const Core::String & fileName)
{
    Core::XMLDocument xmlDocument;

    for(int32 iChild = 0; iChild < _pPtyDocNodeEmitters->getNbChildren(); ++iChild)
    {
        Ptr<PtyNode> pProperty = LM_DEBUG_PTR_CAST<PtyNode>(_pPtyDocNodeEmitters->getChild(iChild));

        if (pProperty!=null)
        {
            Ptr<Universe::NodeEmitter> pNodeEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(pProperty->getUniverseNode());
            if(pNodeEmitter != null)
            {
                Ptr<Core::XMLNode> pXMLNode(new Core::XMLNode(L"node"));
                getWorld()->exportXML(pXMLNode, pNodeEmitter);
                xmlDocument.addNode(L"root", pXMLNode);
            }
        }
    }
    
    Ptr<Core::OutputStream> pOutput = getDocumentRenderer()->getVFS()->writeFile(fileName);
    xmlDocument.saveDocument(*pOutput);
    pOutput->close();

    Ptr<ImportParticlesInfo> pParticlesInfo(new ImportParticlesInfo());
    pParticlesInfo->particlesFileName = fileName;

    Ptr<Core::XMLDocument> pXMLParticlesInfo(new Core::XMLDocument());
    pParticlesInfo->exportXML(pXMLParticlesInfo);

    Core::String infoFile;
    infoFile << fileName << L".xml";
    Ptr<Core::OutputStream> pParticlesInfoOutput = getDocumentRenderer()->getVFS()->writeFile(infoFile);
    pXMLParticlesInfo->saveDocument(*pParticlesInfoOutput);
    pParticlesInfoOutput->close();

    setModified(false);
}
//-----------------------------------------------------------------------------
void DocumentParticles::loadFile(const Core::String & fileName)
{
    Core::XMLDocument xmlDocument;

    Ptr<Core::InputStream> pInput = getDocumentRenderer()->getVFS()->readFile(fileName);

    Ptr<Property> pPtyNodeEmitter = null;

    try
    {
        xmlDocument.loadDocument(*pInput);

        Core::List<Ptr<Core::XMLNode> > nodes;
        xmlDocument.getNodes(L"root", L"node", nodes);
        
        for(int32 iNode=0; iNode < nodes.size(); ++iNode)
        {
            Ptr<Universe::Node> pNode = getWorld()->importXML(nodes[iNode]);
            Ptr<Universe::NodeEmitter> pEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(pNode);

            pPtyNodeEmitter = _pPtyDocNodeEmitters->addNodeEmitter(pEmitter);

            getPropertyDocumentContent()->getChild(0)->addChild(pPtyNodeEmitter);
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
}  // namespace EPI