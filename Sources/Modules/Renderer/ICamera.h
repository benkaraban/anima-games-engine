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
#ifndef ABSTRACT_ICAMERA_H_
#define ABSTRACT_ICAMERA_H_

#include <Core/Standard.h>
#include <Core/Math/Matrix.h>
#include <Renderer/IViewVolume.h>
#include <Renderer/Settings.h>

namespace Renderer
{
LM_ENUM_2(ECameraType,
          CAMERA_PERSPECTIVE,
          CAMERA_ORTHOGONAL);
/**
 * La classe mère de toutes les caméras.
 */
class LM_API_RDR ICamera
{
public:
    virtual ~ICamera() {};

    virtual ECameraType getCameraType() const = 0;

    /**
     * Modifie la matrice de transformation de la caméra. Cette matrice
     * va affecter aussi bien la position que l'orientation de la caméra.
     * La caméra ne dispose volontairement pas de méthodes particulières pour la faire
     * regarder un point précis ou autres fonctionnalités du même genre. L'idée
     * est que ces fonctionnalités ne concernent pas que les caméras et doivent
     * être implémentées dans un module spécifique, qui puisse être facilement
     * réutilisé.
     */
    virtual void setWorldMatrix(const Core::Matrix4f & matrix) = 0;
    virtual void getWorldMatrix(Core::Matrix4f & matrix) const = 0;

    /**
     * Retourne la "view matrix", c'est à dire la matrice qui transforme un point
     * pour qu'il se retrouve dans le repère local de la caméra.
     */
    virtual void getViewMatrix(Core::Matrix4f & matrix) const = 0;

    /**
     * Retourne la matrice de projection, c'est à dire la matrice qui projette
     * un point 3D.
     */
    virtual void getProjectionMatrix(Core::Matrix4f & matrix) const = 0;

    virtual void getProjectionMatrixSplit(Core::Matrix4f & matrix, float near, float far) const = 0;

    virtual void setPostFX(const PostFX & fx) = 0;
    virtual const PostFX & getPostFX() const = 0;

    /**
     * Modifie le Field Of View de la caméra.
     */
    virtual void setFOV(float angle) = 0;
    virtual float getFOV() const = 0;

     /**
     * Modifie la distance du "near plane". Les objets situés devant ce plan
     * ne sont pas affichés.
     */
    virtual void setNearPlane(float near) = 0;
    virtual float getNearPlane() const = 0;

    /**
     * Modifie la distance du "far plane". Les objets situés au dela de ce plan
     * ne sont pas affichés.
     */
    virtual void setFarPlane(float far) = 0;
    virtual float getFarPlane() const = 0;

    /**
     * Modifie le ratio entre la largeur et la hauteur de l'image obtenue.
     * Par exemple : 4/3 ou 16/9. Généralement c'est le ratio de la fenêtre
     * utilisée pour l'affichage.
     */
    virtual void setAspectRatio(float ratio) = 0;
    virtual float getAspectRatio() const = 0;

    /**
    */
    virtual Ptr<IViewVolume> createViewVolume() const = 0;
};

/**
 * Cette classe représente une caméra qui projette en perspective.
 * C'est la caméra utilisée en temps normal.
 */
class LM_API_RDR IPerspectiveCamera : public ICamera
{
public:
    virtual ECameraType getCameraType() const { return CAMERA_PERSPECTIVE;}
};

/**
 * Cette classe représente une caméra orthographique.
 * Les projections obtenues avec cette caméra ne tiennent pas compte des distances :
 * un objet a la même taille quelle que soit sa position, et les angles sont conservés.
 * Si cette projection n'est pas terrible pour l'immersion, elle s'avère très pratique
 * pour les outils du genre éditeur de niveau car elle permet de mieux appréhender
 * des situations dans leur ensemble.
 *
 * Cette caméra permet également de travailler en deux dimensions. Par exemple
 * avec une caméra ortho initialisée avec setOrthoView(0.0, 400.0, 0.0, 300.0) et
 * des near/far à -1.0/1.0, on peut considérer que le point (0.0, 0.0) est en bas à
 * gauche de la fenêtre et le point (400.0, 300.0) est en haut à droite.
 */
class LM_API_RDR IOrthoCamera : public ICamera
{
public:
    virtual ECameraType getCameraType() const { return CAMERA_ORTHOGONAL;}
    virtual void setOrthoView(float left, float right, float down, float top) = 0;
    virtual float getWorkdistance() = 0;
    virtual void setWorkdistance(float distance) = 0;
    virtual float getLeftPlane() const= 0;
    virtual float getRightPlane() const= 0;
    virtual float getTopPlane()const = 0;
    virtual float getDownPlane() const= 0;
};

}
#endif /*ABSTRACT_ICAMERA_H_*/
