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
#ifndef ABSTRACT_ILIGHT_H_
#define ABSTRACT_ILIGHT_H_

#include <Core/Standard.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Vector.h>

namespace Renderer
{
/**
 * La classe mère de toutes les lumières. Cette classe devrait rarement être
 * manipulées directement par les utilisateurs mais elle définit l'interface commune
 * à toutes les lumières.
 *
 * A l'initialisation, une lumière a des couleurs diffuse et spéculaire de (1.0, 1.0, 1.0)
 * et une intensité de 1.0.
 */
class LM_API_RDR ILight
{
public:
    virtual ~ILight() {};

    /**
     * Indique que la lumière devrait générer des ombres. Cette indication
     * est une condition nécessaire mais pas suffisante à la génération d'ombres,
     * c'est l'implémentation qui décidera de l'effet exact de ce paramètre, en
     * fonction des niveaux de détails, des performances...
     */
    virtual void setShadowCasting(bool active) = 0;
    virtual void setAmbientColor(const Core::Vector4f & color) = 0;
    virtual void setDiffuseColor(const Core::Vector4f & color) = 0;
    virtual void setSpecularColor(const Core::Vector4f & color) = 0;

    virtual bool getShadowCasting() const = 0;
    virtual const Core::Vector4f & getAmbientColor() const = 0;
    virtual const Core::Vector4f & getDiffuseColor() const = 0;
    virtual const Core::Vector4f & getSpecularColor() const = 0;

    virtual void setVisible(bool flag) = 0;
    virtual bool isVisible() const = 0;

    virtual void kill() = 0;
    virtual bool isAlive() const = 0;
};

/**
 * Cette classe représente une lumière directionnelle. C'est une lumière
 * située à l'infini qui éclaire toute la scène. C'est le type de lumière le
 * moins coûteux en terme de performance.
 *
 * Par défaut une lumière directionnelle émet dans la direction (0.0, 0.0, 1.0).
 */
class LM_API_RDR IDirLight : public ILight
{
public:
    /**
     * Modifie la direction de la lumière. Le vecteur dir doit être normalisé.
     * L'orientation est donnée dans le repère du monde.
     */
    virtual void setDirection(const Core::Vector3f & dir) = 0;
};

/**
 * Cette lumière est un point qui émet dans toutes
 * les directions de manière uniforme. Elle n'émet que jusqu'à une certaine distance.
 *
 * Par défaut, une lumière positionnelle est positionnée en (0.0, 0.0, 0.0), elle
 * émet avec radius de 1.0.
 */
class LM_API_RDR IPointLight : public ILight
{
public:
    /**
     * La position de la lumière dans le repère du monde.
     */
    virtual void setPosition(const Core::Vector3f & position) = 0;
    virtual void setRadius(float radius) = 0;

    virtual const Core::Vector3f & getPosition() const = 0;
    virtual float getRadius() const = 0;
};

}

#endif /*ABSTRACT_ILIGHT_H_*/
