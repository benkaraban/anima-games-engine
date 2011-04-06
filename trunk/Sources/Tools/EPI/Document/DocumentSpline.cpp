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
#include <EPI/Document/DocumentSpline.h>

#include <QMessageBox>
#include <QtToolbox/VFSDialog.moc.h>

#include <EPI/Document/Properties/PtyDocSpline.h>
#include <EPI/Document/Properties/PtyTrajectory/PtyTrajectory.moc.h>
#include <EPI/Document/Properties/PtyDocInformation.moc.h>
#include <EPI/Document/Tools.h>
#include <Packing/TexCompressionOnTheFly.h>

#include <EPI/Document/GuiDescription.h>
#include <EPI/DocumentRenderer.h>

#include <EPI/ImportInfo/ImportSplineInfo.h>

#include <Core/Math/Splines/Trajectory.h>

namespace EPI
{

//-----------------------------------------------------------------------------
DocumentSpline::DocumentSpline(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr)
:   DocumentBase(guiGoc, DOC_SPLINE, pDocRdr)
{
    Ptr<PropertyNode> pPtyContent = LM_DEBUG_PTR_CAST<PropertyNode> (getPropertyDocumentContent());
        pPtyContent->addChild(Ptr<Property>(new PtyDocSpline(getGuiDocument(), getWorldInfoContent(), getWorld(), getStateRecorder(), L"Spline")));

    initialize();
}
//-----------------------------------------------------------------------------
DocumentSpline::DocumentSpline(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr, const Core::String & fileName)
:   DocumentBase(guiGoc, DOC_SPLINE, pDocRdr)
{
    Ptr<PropertyNode> pPtyContent = LM_DEBUG_PTR_CAST<PropertyNode> (getPropertyDocumentContent());
        pPtyContent->addChild(Ptr<Property>(new PtyDocSpline(getGuiDocument(), getWorldInfoContent(), getWorld(), getStateRecorder(), L"Spline")));

    loadFile(fileName);

    initialize();
}
//-----------------------------------------------------------------------------
DocumentSpline::~DocumentSpline()
{
}
//-----------------------------------------------------------------------------
void DocumentSpline::initialize()
{
    Ptr<GuiDescription> pGuiDesc(new GuiDescription());

    Core::List<EViewportCameraNavigation> viewCameraNavigation;
        viewCameraNavigation.push_back(VIEWPORT_CAMERA_QUAKE);
        viewCameraNavigation.push_back(VIEWPORT_CAMERA_ORBITAL);
    pGuiDesc->setViewportCameraNavigations(viewCameraNavigation);

    Core::List<EViewportPickingType> viewPickingTypes;
        viewPickingTypes.push_back(VIEWPORT_PICKING_PTY);
    pGuiDesc->setViewportPickingTypes(viewPickingTypes);

    setGuiDescription(pGuiDesc);

    Ptr<PtyDocSpline> pPtyDocS = LM_DEBUG_PTR_CAST<PtyDocSpline>(getPropertyDocumentContent()->getChild(0));
    pPtyDocS->setWorldDecoPath(MiscConst::pathWorldDefault);
  //  pPtyDocS->updateData();
}
//-----------------------------------------------------------------------------
Ptr<Universe::World> DocumentSpline::getWorldDeco()
{
    Ptr<PtyDocSpline> pPtyDocS = LM_DEBUG_PTR_CAST<PtyDocSpline>(getPropertyDocumentContent()->getChild(0));
    Ptr<PtyTrajectory> pPtySpline = LM_DEBUG_PTR_CAST<PtyTrajectory>(pPtyDocS->getPtySpline());

    if (_worldDecoPath != pPtyDocS->getWorldDecoPath())
    {
        _worldDecoPath = pPtyDocS->getWorldDecoPath();
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

                Ptr<Universe::Node> pNodePlayersPosition = _pWorldDeco->getFirstNodeWithTag(TAG_PLAYERS_POSITION);
                pPtySpline->setNodePivotTrajectory(pNodePlayersPosition);
            }
            catch(Core::Exception & e)
            {
                String message;
                message << L"An exception has been caught while extracting data from the XML file :\n" << e.getMessage();
                QMessageBox::critical( NULL, "Nyx", Core::String8(message).c_str());
            }

            copyEnvironnementParam(*getWorld(), *_pWorldDeco);
            configWorldDeco(getWorld(), _pWorldDeco);
            //on met a jour les property concerner par le copyEnvironnementParam
           // getPropertyDocumentContent()->getChild(1)->updateProperty();
        }
        else
        {
            pPtySpline->setNodePivotTrajectory(null);
        }
    }   

    return _pWorldDeco;
}
//-----------------------------------------------------------------------------
void DocumentSpline::save()
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
void DocumentSpline::saveAs()
{
    QtToolbox::VFSDialog dlg (*getDocumentRenderer()->getVFS(), QtToolbox::VFSD_SAVE, DOCUMENT_SPLINE_EXT, NULL);
    if(dlg.exec())
    {
        Core::String fileName = dlg.getSelectedFile();
        saveFile(fileName);
        Ptr<PyDocumentInformation> pPinfo = LM_DEBUG_PTR_CAST<PyDocumentInformation>(getPyDocumentInformation());
        pPinfo->setTitle(fileName);
    }
}
//-----------------------------------------------------------------------------
void DocumentSpline::saveFile(const Core::String & fileName)
{
    Ptr<PtyDocSpline> pPtyDocS = LM_DEBUG_PTR_CAST<PtyDocSpline>(getPropertyDocumentContent()->getChild(0));
    const Core::Trajectory& trajec = LM_DEBUG_PTR_CAST<PtyTrajectory>(pPtyDocS->getPtySpline())->getTrajectory();

    Core::XMLDocument xmlDocument;
    Ptr<Core::XMLNode> pXMLNode(new Core::XMLNode(L"node"));

    trajec.exportXML(pXMLNode);
    xmlDocument.addNode(L"root", pXMLNode);


    Ptr<Core::OutputStream> pOutput = getDocumentRenderer()->getVFS()->writeFile(fileName);
    xmlDocument.saveDocument(*pOutput);
    pOutput->close();

    Ptr<ImportSplineInfo> pInfo(new ImportSplineInfo());
    pInfo->fileName = fileName;

    Ptr<Core::XMLDocument> pXMLInfo(new Core::XMLDocument());
    pInfo->exportXML(pXMLInfo);

    Core::String infoFile;
    infoFile << fileName << L".xml";
    Ptr<Core::OutputStream> pInfoOutput = getDocumentRenderer()->getVFS()->writeFile(infoFile);
    pXMLInfo->saveDocument(*pInfoOutput);
    pInfoOutput->close();


    //saveGroup(getDocumentRenderer()->getVFS(), getWorld(), pPtyDocG->getNodeGroup(), fileName);

    setModified(false);
}
//-----------------------------------------------------------------------------
void DocumentSpline::loadFile(const Core::String & fileName)
{
    Core::XMLDocument xmlDocument;

    Ptr<Core::InputStream> pInput = getDocumentRenderer()->getVFS()->readFile(fileName);

    try
    {
        xmlDocument.loadDocument(*pInput);

        Core::List<Ptr<Core::XMLNode> > nodes;
        xmlDocument.getNodes(L"root", L"node", nodes);

        Core::Trajectory trajec;
        trajec.importXML(nodes[0]);

        Ptr<PtyDocSpline> pPtyDocS = LM_DEBUG_PTR_CAST<PtyDocSpline>(getPropertyDocumentContent()->getChild(0));
        LM_DEBUG_PTR_CAST<PtyTrajectory>(pPtyDocS->getPtySpline())->setTrajectory(trajec);
    }
    catch(Core::Exception & e)
    {
        String message;
        message << L"An exception has been caught while extracting data from the XML file :\n" << e.getMessage();
        QMessageBox::critical( NULL, "Nyx", Core::String8(message).c_str());

    }
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentSpline::getPtyForEdition() const
{
    Ptr<PtyDocSpline> pPtyDocS  = LM_DEBUG_PTR_CAST<PtyDocSpline>(getPropertyDocumentContent()->getChild(0));
    Ptr<PtyTrajectory> pPtyTraj = LM_DEBUG_PTR_CAST<PtyTrajectory>(pPtyDocS->getPtySpline());

    if (pPtyTraj->isInEditMode() == true)
    {
        return getDefaultProperty();
    }

    return null;
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentSpline::getDefaultProperty() const
{
    Ptr<PtyDocSpline> pPtyDocS = LM_DEBUG_PTR_CAST<PtyDocSpline>(getPropertyDocumentContent()->getChild(0));

    return pPtyDocS->getPtySpline();
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera> DocumentSpline::getCameraEdition() const
{
    Ptr<PtyDocSpline> pPtyDocS  = LM_DEBUG_PTR_CAST<PtyDocSpline>(getPropertyDocumentContent()->getChild(0));
    Ptr<PtyTrajectory> pPtyTraj = LM_DEBUG_PTR_CAST<PtyTrajectory>(pPtyDocS->getPtySpline());

    return pPtyTraj->getCamera();
}
//-----------------------------------------------------------------------------
void DocumentSpline::runStopScript()
{
    Ptr<PtyDocSpline> pPtyDocS  = LM_DEBUG_PTR_CAST<PtyDocSpline>(getPropertyDocumentContent()->getChild(0));
    Ptr<PtyTrajectory> pPtyTraj = LM_DEBUG_PTR_CAST<PtyTrajectory>(pPtyDocS->getPtySpline());


    pPtyTraj->runStopTask();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}  // namespace EPI
