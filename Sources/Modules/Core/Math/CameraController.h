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
#ifndef CORE_CAMERACONTROLLER_H_
#define CORE_CAMERACONTROLLER_H_

#include <Core/Standard.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Vector.h>

namespace Core
{
/**
 * Contrôleur de déplacement d'une caméra. Pour utiliser toutes les possibilités de
 * déplacement il faut connaître la classe exacte.
 */
class LM_API_COR ICameraController
{
public:
    virtual ~ICameraController() {};

    /// positif = droite, négatif = gauche
    virtual void turnLeftRight(float delta) = 0;

    /// positif = haut, négatif = bas
    virtual void turnUpDown(float delta) = 0;

    virtual Core::Matrix4f getWorldMatrix() const = 0;
};

/**
 * Contrôle une caméra en la faisant tourner autour d'un point d'observation.
 * L'objectif de la caméra est toujours orienté vers ce point.
 */
class LM_API_COR CameraOrbital : public Object, public ICameraController
{
public:
    CameraOrbital();
    CameraOrbital(const Core::Vector3f & observedPosition, float distance);

    /**
     * Modifie le point autour duquel pivote la caméra.
     */
    virtual void setObservedPoint(const Core::Vector3f & position);

    /**
     * Tourne autour du pont observé.
     */
    virtual void turnLeftRight(float delta);
    virtual void turnUpDown(float delta);

    /**
     * S'approche (delta positif) ou s'écarte (delta négatif) du point observé.
     */
    virtual void zoomInOut(float delta);

    /**
     * Déplace la point observé d'après les coordonnées X/Y du point de vue courant.
     * Correspond au "pan" dans les softs 3D (typiquement : bouton molette + déplacement souris)
     */
    virtual void relativeMove(float dx, float dy);

    virtual Core::Matrix4f getWorldMatrix() const;

    virtual void setDistance(float d);

protected:
    virtual void updateMatrix();

    Core::Matrix4f         _matOrient;
    Core::Matrix4f         _matTrans;
    Core::Matrix4f         _matrix;
    float                  _distance;
};

/**
 * Contrôle une caméra à la manière d'un Quake-like.
 */
class LM_API_COR CameraQuake : public Object, public ICameraController
{
public:
    CameraQuake();
    CameraQuake(const Core::Vector3f & position);

    // Orientation -------------------------------------------

    virtual void turnLeftRight(float delta);
    virtual void turnUpDown(float delta);

    // Déplacements ------------------------------------------

    virtual void forward(float delta);
    virtual void backward(float delta);

    virtual void strafeRight(float delta);
    virtual void strafeLeft(float delta);

    // Comme avec un jetpack
    virtual void moveUp(float delta);
    virtual void moveDown(float delta);

    virtual void setPosition(const Vector3f & position);
    virtual Core::Matrix4f getWorldMatrix() const;

    const Core::Vector3f & getPosition() const { return _position; };
    Core::Vector3f getDirection() const { return _matrix.getZVector(); };

protected:
    virtual void updateMatrix();

    Core::Matrix4f         _matrix;
    Core::Vector3f         _position;
    float                  _yaw;
    float                  _pitch;
};

}

#endif
