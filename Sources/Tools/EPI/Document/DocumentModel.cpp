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
#include "DocumentModel.h"

#include <QMessageBox>

#include <Universe/Node.h>

#include <EPI/ImportInfo/ImportModelInfo.h>

#include <EPI/Document/Properties/PtyDocModel.moc.h>
#include <EPI/Document/Properties/PtyDocInformation.moc.h>
#include <EPI/Document/Properties/PtyModel.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <EPI/Document/Tools.h>

#include <QtToolbox/VFSDialog.moc.h>

#include <EPI/Document/GuiDescription.h>
#include <EPI/DocumentRenderer.h>

namespace EPI
{
//-----------------------------------------------------------------------------
DocumentModel::DocumentModel(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr, const Ptr<ImportModelInfo>& pModelInfo) :
    DocumentBase(guiGoc, DOC_MODEL, pDocRdr)
{
    Ptr<GuiDescription> pGuiDesc(new GuiDescription());

    Core::List<EViewportCameraNavigation> viewCameraNavigation;
        viewCameraNavigation.push_back(VIEWPORT_CAMERA_ORBITAL);
        viewCameraNavigation.push_back(VIEWPORT_CAMERA_QUAKE);
    pGuiDesc->setViewportCameraNavigations(viewCameraNavigation);

    Core::List<EViewportPickingType> viewPickingTypes;
        viewPickingTypes.push_back(VIEWPORT_PICKING_PTY);
    pGuiDesc->setViewportPickingTypes(viewPickingTypes);

    setGuiDescription(pGuiDesc);

    LM_DEBUG_PTR_CAST<PyDocumentInformation> (getPyDocumentInformation())->setTitle(pModelInfo->modelName);

    Ptr<Property> pPtyDocModel (new PtyDocModel(guiGoc, getWorld(), getStateRecorder(), pModelInfo, L"Model-Editing"));
    getPropertyDocumentContent()->addChild(pPtyDocModel);

    setGenerationProperty(pPtyDocModel);
}
//-----------------------------------------------------------------------------
DocumentModel::~DocumentModel()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentModel::getDefaultProperty() const
{
    return LM_DEBUG_PTR_CAST<PtyDocModel>(getPropertyDocumentContent()->getChild(0))->getPtyModel();
}
//-----------------------------------------------------------------------------
Core::Orientationf DocumentModel::getDefaultCameraOrientation() const
{
    return Core::Orientationf (Core::deg2rad(-10), Core::deg2rad(180), Core::deg2rad(0));
}
//-----------------------------------------------------------------------------
Core::Vector3f DocumentModel::getDefaultCameraTargetPosition() const
{
    Ptr<PtyTransform> pPtyTrans;
    pPtyTrans = LM_DEBUG_PTR_CAST<PtyTransform>(LM_DEBUG_PTR_CAST<PtyModel>(getDefaultProperty())->getPtyTransform());
    return pPtyTrans->getCenterBoundingBoxInWorldAxis();
}
//-----------------------------------------------------------------------------
void DocumentModel::save()
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
void DocumentModel::saveAs()
{
    QtToolbox::VFSDialog dlg(*getDocumentRenderer()->getVFS(), QtToolbox::VFSD_SAVE, DOCUMENT_MODEL_EXT, NULL);
    if(dlg.exec())
    {
        Core::String fileName = dlg.getSelectedFile();
        saveFile(fileName);
        Ptr<PyDocumentInformation> pPinfo = LM_DEBUG_PTR_CAST<PyDocumentInformation>(getPyDocumentInformation());
        pPinfo->setTitle(fileName);
    }
}
//-----------------------------------------------------------------------------
void DocumentModel::saveFile(const Core::String & fileName)
{
    LM_DEBUG_PTR_CAST<PtyDocModel>(getPropertyDocumentContent()->getChild(0))->save(fileName);
    setModified(false);
}
//-----------------------------------------------------------------------------
Ptr<Universe::World> DocumentModel::getWorldDeco()
{
    Ptr<PtyDocModel> pPtyDocModel = LM_DEBUG_PTR_CAST<PtyDocModel>(getPropertyDocumentContent()->getChild(0));

    if (_worldDecoPath != pPtyDocModel->getWorldDecoPath())
    {
        _worldDecoPath = pPtyDocModel->getWorldDecoPath();
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
Ptr<Property> DocumentModel::getPtyForEdition() const
{
    return getDefaultProperty();
}
//-----------------------------------------------------------------------------
}  // namespace EPI
