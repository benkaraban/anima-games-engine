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
#ifndef DOCUMENT_PARTICLES_H_
#define DOCUMENT_PARTICLES_H_

#include <EPI/Document/DocumentBase.moc.h>

namespace EPI
{
class PtyDocNodeEmitters;

class LM_API_EPI DocumentParticles : public DocumentBase
{
    Q_OBJECT
public:
    DocumentParticles(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr);
    DocumentParticles(GuiDocument& guiGoc, const Ptr<DocumentRenderer>& pDocRdr, const Core::String & fileName);

    virtual ~DocumentParticles();

    virtual Ptr<Property> getDefaultProperty() const;

    virtual Ptr<Universe::World> getWorldDeco();

public Q_SLOTS:
    void createPropertyNodeEmitter();

    virtual void save();
    virtual void saveAs();

private:
    void saveFile(const Core::String & fileName);
    void loadFile(const Core::String & fileName);
    

private:
    Ptr<PtyDocNodeEmitters> _pPtyDocNodeEmitters;
    
    Ptr<Universe::World>    _pWorldDeco;
    Core::String            _worldDecoPath;
};

}  // namespace EPI

#endif /*DOCUMENT_PARTICLES_H_*/