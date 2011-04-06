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
#ifndef ASSETS_MATERIAL_H_
#define ASSETS_MATERIAL_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/IO/Streams.h>
#include <Core/Math/Vector.h>
#include <Core/Math/Matrix.h>

namespace Assets
{
/**
 * Définit le type d'un paramètre de material.
 */
LM_ENUM_2(EMaterialType,
          MAT_STANDARD,
          MAT_CUSTOM);

LM_ENUM_2(EOperationType,
          OP_ADD,
          OP_MUL);

LM_ENUM_4(ECustomMode,
          CM_ADD,
          CM_LERP,
          CM_ADD_LERP,
          CM_REFRAC);

LM_POD_STREAM(EMaterialType);
LM_POD_STREAM(EOperationType);
LM_POD_STREAM(ECustomMode);

const int32 MAT_FLAG_TRANSLUCID_TEXTURE          = 0x00000001;
const int32 MAT_FLAG_RESERVED1                   = 0x00000002;
const int32 MAT_FLAG_CLAMPED_TEXTURE_BORDER      = 0x00000004;
const int32 MAT_FLAG_DYNAMIC_SHADOW_CASTER       = 0x00000008;
const int32 MAT_FLAG_DYNAMIC_SHADOW_RECEIVER     = 0x00000010;


/**
 * Cette classe représente une matière.
 * La matière est définie par un type de matière (peau, plastique, bois, métal...) et
 * des paramètres pour cette matière (couleur, texture, intensité des réflexions).
 */
class LM_API_ASS Material : public Core::Object, public Core::Serializable
{
public:
    Material();
    Material(const Material & mat);
    Material & operator = (const Material & mat);

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    bool operator == (const Material & mat) const;
    bool operator != (const Material & mat) const { return !(*this == mat); };

    EMaterialType      type;
    int32              flags;

    // Standard material ------------------------------------------------------

    String             diffuseTexName;
    String             specularTexName;
    String             normalTexName;

    Core::Vector4f     emissive;
    Core::Vector4f     ambient;
    Core::Vector4f     diffuse;

    Core::Vector4f     specular;
    float              shininess;

    Core::Vector4f     glowColor;
    Core::Vector3f     glowExtent;

    Core::Vector4f     reflexions;

    Core::Vector4f     refracColor;
    float              refracScale;
    float              refracIndex;

    // Custom material --------------------------------------------------------

    ECustomMode         customMode;
    String              texture0Name;
    String              texture1Name;
    String              texture2Name;
    Core::Vector4f      customColor;
    float               colorFactor;
    float               glowFactor;
    float               zBias;
    EOperationType      opColor01;
    EOperationType      opColor12;
    Core::Vector2f      duv0;
    Core::Vector2f      duv1;
    Core::Vector2f      duv2;
};
}

#endif /*ASSETS_MATERIAL_H_*/
