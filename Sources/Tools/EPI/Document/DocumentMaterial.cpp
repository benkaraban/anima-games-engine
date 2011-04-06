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
#include <EPI/Document/DocumentMaterial.h>

#include <QMessageBox>

#include <EPI/Document/Properties/PtyDocMaterial.h>
#include <EPI/Document/Properties/PtySun.moc.h>
#include <EPI/Document/Tools.h>

#include <Packing/TexCompressionOnTheFly.h>

#include <EPI/Document/GuiDescription.h>
#include <EPI/DocumentRenderer.h>

namespace EPI
{

//-----------------------------------------------------------------------------
DocumentMaterial::DocumentMaterial(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr)
:   DocumentBase(guiGoc, DOC_MATERIAL, pDocRdr)
{
    Ptr<GuiDescription> pGuiDesc(new GuiDescription());

    Core::List<EViewportCameraNavigation> viewCameraNavigation;
        viewCameraNavigation.push_back(VIEWPORT_CAMERA_ORBITAL);
        viewCameraNavigation.push_back(VIEWPORT_CAMERA_QUAKE);
    pGuiDesc->setViewportCameraNavigations(viewCameraNavigation);

    setGuiDescription(pGuiDesc);

    Ptr<PropertyNode> pPtyContent = LM_DEBUG_PTR_CAST<PropertyNode> (getPropertyDocumentContent());
        pPtyContent->addChild(Ptr<Property>(new PtyDocMaterial(getWorld(), L"Material")));
        pPtyContent->addChild(Ptr<Property>(new PtySun(guiGoc, getWorld(), L"Sun")));
        
}
//-----------------------------------------------------------------------------
DocumentMaterial::~DocumentMaterial()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> DocumentMaterial::getDefaultProperty() const
{
    return getPropertyDocumentContent()->getChild(0);
}
//-----------------------------------------------------------------------------
Ptr<Universe::World> DocumentMaterial::getWorldDeco()
{
    Ptr<PtyDocMaterial> pPtyDocM = LM_DEBUG_PTR_CAST<PtyDocMaterial>(getPropertyDocumentContent()->getChild(0));

    if (_worldDecoPath != pPtyDocM->getWorldDecoPath())
    {
        _worldDecoPath = pPtyDocM->getWorldDecoPath();
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

            copyEnvironnementParam(*getWorld(), *_pWorldDeco);
            configWorldDeco(getWorld(), _pWorldDeco);

            //on met a jour les property concerner par le copyEnvironnementParam
            getPropertyDocumentContent()->getChild(1)->updateProperty();
        }
    }   

    return _pWorldDeco;
}
//-----------------------------------------------------------------------------
Core::Orientationf DocumentMaterial::getDefaultCameraOrientation() const
{
    return Core::Orientationf (Core::deg2rad(90),0,0);
}
//-----------------------------------------------------------------------------
}  // namespace EPI
