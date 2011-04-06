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
#ifndef ABSTRACT_IBILLBOARD_H_
#define ABSTRACT_IBILLBOARD_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Math/Vector.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/GeometryComputations.h>
#include <Renderer/ITexture.h>

namespace Renderer
{

LM_ENUM_4(EBillboardMode, BILLBOARD_LERP, BILLBOARD_ADD, BILLBOARD_ADD_LERP, BILLBOARD_REFRAC);

LM_ENUM_5(EBillboardAlign, 
          BILLBOARD_ALIGN_FACE_CAM, 
          BILLBOARD_ALIGN_HORIZONTAL_ON_VEC, 
          BILLBOARD_ALIGN_VERTICAL_ON_VEC,
          BILLBOARD_ALIGN_HORIZONTAL_ON_DIR, 
          BILLBOARD_ALIGN_VERTICAL_ON_DIR);

/**
 * Cette structure permet de manipuler rapidement un Billboard,
 * quel que soit sa représentation interne. Cette représentation est
 * utilisée pour communiquer avec l'extérieur. La représentation
 * interne dépend de l'implémentation.
 *
 * Il s'agit d'une bête structure parce que justement, cette structure
 * ne doit pas dépendre de l'implémentation du Renderer.
 */
struct LM_API_RDR Billboard
{
    /**
     * La position du billboard en 3D. La position correspond au centre
     * du rectangle.
     */
    Core::Vector3f position;

    /**
     * Couleur du billboard.
     */
    Core::Vector4f  color;

    /**
     * Largeur du billboard, du point de vue de la caméra. La direction
     * de la largeur est celle de l'axe des X du point de vue de la caméra.
     */
    float          width;

    /**
     * Hauteur du billboard, du point de vue de la caméra. La direction
     * de la hauteur est celle de l'axe des Y du point de vue de la caméra.
     */
    float          height;

    /**
     * Cet angle contrôle la rotation sur l'axe Z, du point de vue de la caméra.
     * Visuellement, cela correspond à une rotation 2D. Modifier cette rotation
     * permet de créer une illusion de variétés à un groupe de billboards
     * autrement identiques.
     */
    union
    {
        float   angle;
        byte    dir[4];
        uint32  dirInt;
    };

    /**
     * Le temps dans l'animation (cad. la coordonnée W dans la texture 3D)
     */
    float          time;
};

const int32 MAX_BILLBOARD_COUNT = 10000;

/**
 * Cette classe permet de gérer un groupe de billboards. Les billboards sont des
 * rectangles qui font toujours face à la caméra. Ce sont donc des sprites positionnés
 * en 3D. Les billboards sont utilisés pour implémenter de multiples effets 3D commes
 * des lens flares, des halos de lumières, des explosions ou des particules.
 *
 * Les billboards d'un même groupe partagent la même instance de matérial et donc
 * la même texture. En revanche chacun d'entre eux peut avoir une couleur spécifique.
 */
class LM_API_RDR IBillboardGroup
{
public:
    virtual ~IBillboardGroup() {};

    /**
     * Cette méthode, avec la méthode endUpdate, permet de modifier
     * le contenu du groupe. Lorsque l'on désire modifier le groupe, que ce soit
     * pour ajouter de nouveaux éléments, en retirer ou les mettre à jour, on
     * doit appeler la méthode beginUpdate, modifier la liste dont la référence
     * est retournée puis appeler endUpdate.
     *
     * A la fin de la mise à jour, le groupe sera optimisé pour être rendu efficacement,
     * en utilisant le nouveau contenu de la liste.
     *
     * La référence retournée par beginUpdate n'est valide que jusqu'au prochain
     * appel à endUpdate, elle ne doit en aucun cas être utilisée après cet appel.
     *
     * Chaque update ayant un coût fixe, il est plus efficace d'effectuer toutes les
     * modifications de la liste avec une seule paire de begin/end.
     */
    virtual Core::List<Billboard> & beginUpdate() = 0;
    virtual void endUpdate() = 0;

    virtual void setMaxSize(int32 maxSize) = 0;
    virtual int32 getMaxSize() const = 0;

    virtual void setPartScale(float s) = 0;
    virtual float getPartScale() const = 0;

    virtual Ptr<ITextureMap> getTexture() const  = 0;
    virtual void setTexture(const Ptr<ITextureMap> & pTexture) = 0;

    virtual const Core::Matrix4f & getWorldMatrix() const = 0;
    virtual void setWorldMatrix(const Core::Matrix4f & matrix) = 0;

    virtual void setMode(EBillboardMode mode) = 0;
    virtual EBillboardMode getMode() const = 0;

    virtual void setAlign(EBillboardAlign align) = 0;
    virtual EBillboardAlign getAlign() const = 0;

    virtual void setAlignVector(const Core::Vector3f & vec) = 0;
    virtual const Core::Vector3f & getAlignVector() const = 0;

    /**
     * Précise que la texture utilisée est un tableau de sprites contenant
     * uCount sprites en largeurs * vCount sprites en hauteur.
     */
    virtual void setSpriteLayout(int32 uCount, int32 vCount) = 0;
    virtual int32 getUSpriteLayout() const = 0;
    virtual int32 getVSpriteLayout() const = 0;

    virtual void setSpriteSmooth(bool s) = 0;
    virtual bool isSpriteSmooth() const = 0;

    virtual void setColorFactor(float f) = 0;
    virtual float getColorFactor() const = 0;

    virtual void setGlowFactor(float f) = 0;
    virtual float getGlowFactor() const = 0;

    virtual void setRefractionScale(float r) = 0;
    virtual float getRefractionScale() const = 0;

    virtual void setZBias(float z) = 0;
    virtual float getZBias() const = 0;

    virtual void setFogFactor(float f) = 0;
    virtual float getFogFactor() const = 0;

    /**
     * Pour qu'un billboard soit animé il faut lui assigner une texture animée
     * ou lui fournir une texture contenant un tableau de sprites (cf. setSpriteLayout).
     */
    virtual bool isAnimated() const = 0;
    virtual bool hasSpriteLayout() const = 0;

    virtual const Core::AABoxf & getBoundingBox() const = 0;

    virtual void setVisible(bool flag) = 0;
    virtual bool isVisible() const = 0;

    virtual void kill() = 0;
    virtual bool isAlive() const = 0;
};
}

#endif /*ABSTRACT_IBILLBOARD_H_*/
