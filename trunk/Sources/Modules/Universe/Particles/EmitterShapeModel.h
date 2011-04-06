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
#ifndef UNIVERSE_EMITTERSHAPEMODEL_H_
#define UNIVERSE_EMITTERSHAPEMODEL_H_

#include <Core/Standard.h>
#include <Universe/RessourcePool.h>
#include <Universe/Particles/IEmitterShape.h>

namespace Universe
{
/**
 * Emetteur de particules dans une sphère.
 */
class LM_API_UNI EmitterShapeModel : public Core::Object, public IEmitterShape
{
public:
    /**
     * Construit un émetteur sur une sphere de centre \a position et de rayon
     * \a radius. Si \a border est à true alors les particules sont émises sur le
     * bord de la sphère. Dans le cas contraire elles sont émises à l'intérieur
     * de la sphère.
     */
    EmitterShapeModel(const Ptr<RessourcePool> & pResPool);
    virtual void createParticle(Particle & part);

    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime);

    void setModel(const String & model, const String & mesh);
    const String & getModel() const { return _modelName; };
    const String & getMesh() const { return _meshName; };

    inline virtual EShape getType() const { return Universe::SHAPE_MODEL; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    struct TriangleInfos
    {
        Core::Vector3f  o;
        Core::Vector3f  n;
        Core::Vector3f  u;
        Core::Vector3f  v;
        double          psum;
    };

    Ptr<RessourcePool>  _pResPool;
    String              _modelName;
    String              _meshName;
    Ptr<Assets::Model>  _pModel;
    Ptr<Assets::ModelMesh> _pMesh;
    Core::List<TriangleInfos> _tris;
    double                    _psum;
};
}

#endif /*PARTICLE_EMITTERSHAPESPHERE_H_*/
