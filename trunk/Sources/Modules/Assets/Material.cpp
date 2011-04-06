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

#include <Core/IO/Tools.h>
#include <Core/Math/MathIO.h>
#include "Material.h"

namespace Assets
{
//-----------------------------------------------------------------------------
static const int32 MAGIC_MATERIAL = LM_FOURCC('L', 'M', 'A', 'T');
static const int32 VERSION = 2;
//-----------------------------------------------------------------------------
Material::Material() :
    type(MAT_STANDARD),
    flags(0),
    diffuseTexName(),
    specularTexName(),
    normalTexName(),
    emissive(0.0, 0.0, 0.0, 0.0),
    ambient(1.0, 1.0, 1.0, 1.0),
    diffuse(1.0, 1.0, 1.0, 1.0),
    specular(1.0, 1.0, 1.0, 1.0),
    shininess(64.0),
    glowColor(0.0, 0.0, 0.0, 0.0),
    glowExtent(0.0, 0.0, 0.0),
    reflexions(0.0, 0.0, 0.0, 0.0),
    refracColor(0.0, 0.0, 0.0, 0.0),
    refracScale(1.0f),
    refracIndex(0.7f),
    customMode(CM_ADD),
    customColor(1.0f, 1.0f, 1.0f, 1.0f),
    colorFactor(1.0f),
    glowFactor(0.0f),
    zBias(0.0f),
    opColor01(OP_MUL),
    opColor12(OP_MUL),
    duv0(0.0f, 0.0f),
    duv1(0.0f, 0.0f),
    duv2(0.0f, 0.0f)
{}
//-----------------------------------------------------------------------------
Material::Material(const Material & mat)
{
    *this = mat;
}
//-----------------------------------------------------------------------------
Material & Material::operator = (const Material & mat)
{
    if(&mat != this)
    {
        type = mat.type;
        flags = mat.flags;
        diffuseTexName = mat.diffuseTexName;
        specularTexName = mat.specularTexName;
        normalTexName = mat.normalTexName;
        emissive = mat.emissive;
        ambient = mat.ambient;
        diffuse = mat.diffuse;
        specular = mat.specular;
        shininess = mat.shininess;
        glowColor = mat.glowColor;
        glowColor = mat.glowColor;
        reflexions = mat.reflexions;
        refracColor = mat.refracColor;
        refracScale = mat.refracScale;
        refracIndex = mat.refracIndex;


        customMode = mat.customMode;
        texture0Name = mat.texture0Name;
        texture1Name = mat.texture1Name;
        texture2Name = mat.texture2Name;
        customColor = mat.customColor;
        colorFactor = mat.colorFactor;
        glowFactor = mat.glowFactor;
        zBias = mat.zBias;
        opColor01 = mat.opColor01;
        opColor12 = mat.opColor12;
        duv0 = mat.duv0;
        duv1 = mat.duv1;
        duv2 = mat.duv2;
    }

    return *this;
}
//-----------------------------------------------------------------------------
const int32 RESERVED_COUNT = 54;
//-----------------------------------------------------------------------------
void Material::read(Core::InputStream & input)
{
    // Valeurs par d�faut
    *this = Material();

    int32 magic = 0;
    int32 version = 0;

    input.read(magic);
    input.read(version);

    if(magic != MAGIC_MATERIAL)
        throw Core::IOException(L"Invalid material");

    if(version != VERSION)
        throw Core::IOException(L"Invalid material version : " + Core::toString(VERSION) + L" expected, " + Core::toString(version) + L" found");

    Assets::read(input, type);
    input.read(flags);

    if(type == MAT_STANDARD)
    {
        input.read(diffuseTexName);
        input.read(specularTexName);
        input.read(normalTexName);
        Core::read(input, emissive);
        Core::read(input, ambient);
        Core::read(input, diffuse);
        Core::read(input, specular);
        Core::read(input, shininess);
        Core::read(input, glowColor);
        Core::read(input, glowExtent);
        Core::read(input, reflexions);
        Core::read(input, refracColor);
        Core::read(input, refracScale);
        Core::read(input, refracIndex);

        float reserved[RESERVED_COUNT];
        input.readData(reserved, sizeof(float) * RESERVED_COUNT);
    }
    else if(type == MAT_CUSTOM)
    {
        Assets::read(input, customMode);
        input.read(texture0Name);
        input.read(texture1Name);
        input.read(texture2Name);
        Core::read(input, customColor);
        input.read(colorFactor);
        input.read(glowFactor);
        Core::read(input, refracScale);
        Assets::read(input, opColor01);
        Assets::read(input, opColor12);
        Core::read(input, duv0);
        Core::read(input, duv1);
        Core::read(input, duv2);
        Core::read(input, zBias);

        float reserved[RESERVED_COUNT];
        input.readData(reserved, sizeof(float) * RESERVED_COUNT);
    }
}
//-----------------------------------------------------------------------------
void Material::write(Core::OutputStream & output) const
{
    output.write(MAGIC_MATERIAL);
    output.write(VERSION);
    Assets::write(output, type);
    output.write(flags);

    if(type == MAT_STANDARD)
    {
        output.write(diffuseTexName);
        output.write(specularTexName);
        output.write(normalTexName);
        Core::write(output, emissive);
        Core::write(output, ambient);
        Core::write(output, diffuse);
        Core::write(output, specular);
        Core::write(output, shininess);
        Core::write(output, glowColor);
        Core::write(output, glowExtent);
        Core::write(output, reflexions);
        Core::write(output, refracColor);
        Core::write(output, refracScale);
        Core::write(output, refracIndex);

        float reserved[RESERVED_COUNT];
        for(int32 ii=0; ii < RESERVED_COUNT; ii++)
            reserved[ii] = 0.0;
        output.writeData(reserved, sizeof(float) * RESERVED_COUNT);
    }
    else if(type == MAT_CUSTOM)
    {
        Assets::write(output, customMode);
        output.write(texture0Name);
        output.write(texture1Name);
        output.write(texture2Name);
        Core::write(output, customColor);
        output.write(colorFactor);
        output.write(glowFactor);
        Core::write(output, refracScale);
        Assets::write(output, opColor01);
        Assets::write(output, opColor12);
        Core::write(output, duv0);
        Core::write(output, duv1);
        Core::write(output, duv2);
        Core::write(output, zBias);

        float reserved[RESERVED_COUNT];
        for(int32 ii=0; ii < RESERVED_COUNT; ii++)
            reserved[ii] = 0.0;
        output.writeData(reserved, sizeof(float) * RESERVED_COUNT);
    }
}
//-----------------------------------------------------------------------------
bool Material::operator == (const Material & mat) const
{
    return type == mat.type &&
           flags == mat.flags &&

           ((type == MAT_STANDARD &&
               diffuseTexName == mat.diffuseTexName &&
               specularTexName == mat.specularTexName &&
               normalTexName == mat.normalTexName &&
               emissive == mat.emissive &&
               ambient == mat.ambient &&
               diffuse == mat.diffuse &&
               specular == mat.specular &&
               shininess == mat.shininess &&
               glowColor == mat.glowColor &&
               glowExtent == mat.glowExtent &&
               reflexions == mat.reflexions &&
               refracColor == mat.refracColor &&
               refracScale == mat.refracScale &&
               refracIndex == mat.refracIndex) 
               ||
           (type == MAT_CUSTOM &&
               customMode == mat.customMode &&
               texture0Name == mat.texture0Name &&
               texture1Name == mat.texture1Name &&
               texture2Name == mat.texture2Name &&
               colorFactor == mat.colorFactor &&
               glowFactor == mat.glowFactor &&
               zBias == mat.zBias &&
               opColor01 == mat.opColor01 &&
               opColor12 == mat.opColor12 &&
               duv0 == mat.duv0 &&
               duv1 == mat.duv1 &&
               duv2 == mat.duv2));
}
//-----------------------------------------------------------------------------
}
