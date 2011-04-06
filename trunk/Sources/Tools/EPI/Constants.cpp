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
#include "Constants.h"

namespace EPI
{

Core::String textureFormatToString(Assets::ETextureFormat format)
{
    Core::String formatName = L"ARGB8";
    // Format
    switch(format)
    {
    case Assets::TEX_FORMAT_ARGB8:
        formatName = L"ARGB8";
        break;
    //case Assets::TEX_FORMAT_ARGB16:
    //    format = L"ARGB16";
    //    break;
    //case Assets::TEX_FORMAT_ARGB16F:
    //    format = L"ARGB16F";
    //    break;
    //case Assets::TEX_FORMAT_ARGB32:
    //    format = L"ARGB32";
    //    break;
    case Assets::TEX_FORMAT_DXTC1:
        formatName = L"DXTC1";
        break;
    case Assets::TEX_FORMAT_DXTC3:
        formatName = L"DXTC3";
        break;
    case Assets::TEX_FORMAT_DXTC5:
        formatName = L"DXTC5";
        break;
    }

    return formatName;
}

Core::String mipmapFilterToString(Workflow::EMipmapFilter mipmapFilter)
{
    Core::String mipmapFilterName = L"BOX";

    //Mipmap Filter
    switch(mipmapFilter)
    {
    case Workflow::MIPMAP_FILTER_BOX:
        mipmapFilterName = L"BOX";
        break;
    case Workflow::MIPMAP_FILTER_TRIANGLE:
        mipmapFilterName = L"TRIANGLE";
        break;
    case Workflow::MIPMAP_FILTER_KAISER:
        mipmapFilterName = L"KAISER";
        break;
    }

    return mipmapFilterName;
}

Core::String textureTypeToString(Workflow::ETextureType textureType)
{
    Core::String textureTypeName = L"COLORMAP";

    //Texture type
    switch(textureType)
    {
    case Workflow::TEXTURE_COLORMAP:
        textureTypeName = L"COLORMAP";
        break;
    case Workflow::TEXTURE_HEIGHTMAP:
        textureTypeName = L"HEIGHTMAP";
        break;
    case Workflow::TEXTURE_LIGHTMAP:
        textureTypeName = L"LIGHTMAP";
        break;
    case Workflow::TEXTURE_NORMALMAP:
        textureTypeName = L"NORMALMAP";
        break;
    }

    return textureTypeName;
}

Assets::ETextureFormat stringToTextureFormat(const Core::String & formatString)
{
    Assets::ETextureFormat format = Assets::TEX_FORMAT_ARGB8;

    if(formatString == L"ARGB8")
        format = Assets::TEX_FORMAT_ARGB8;
    //else if(formatString == L"ARGB16")
    //    format = Assets::TEX_FORMAT_ARGB16;
    //else if(formatString == L"ARGB16F")
    //    format = Assets::TEX_FORMAT_ARGB16F;
    //else if(formatString == L"ARGB32")
    //    format = Assets::TEX_FORMAT_ARGB32;
    else if(formatString == L"DXTC1")
        format = Assets::TEX_FORMAT_DXTC1;
    else if(formatString == L"DXTC3")
        format = Assets::TEX_FORMAT_DXTC3;
    else if(formatString == L"DXTC5")
        format = Assets::TEX_FORMAT_DXTC5;

    return format;
}

Workflow::EMipmapFilter stringToMipmapFilter(const Core::String & mipmapFilterString)
{
    Workflow::EMipmapFilter mipmapFilter = Workflow::MIPMAP_FILTER_BOX;
    
    if(mipmapFilterString == L"BOX")
        mipmapFilter = Workflow::MIPMAP_FILTER_BOX;
    else if(mipmapFilterString == L"TRIANGLE")
        mipmapFilter = Workflow::MIPMAP_FILTER_TRIANGLE;
    else if(mipmapFilterString == L"KAISER")
        mipmapFilter = Workflow::MIPMAP_FILTER_KAISER;

    return mipmapFilter;
}

Workflow::ETextureType stringToTextureType(const Core::String & textureTypeString)
{
    Workflow::ETextureType textureType = Workflow::TEXTURE_COLORMAP;

    if(textureTypeString == L"COLORMAP")
        textureType = Workflow::TEXTURE_COLORMAP;
    else if(textureTypeString == L"HEIGHTMAP")
        textureType = Workflow::TEXTURE_HEIGHTMAP;
    else if(textureTypeString == L"LIGHTMAP")
        textureType = Workflow::TEXTURE_LIGHTMAP;
    else if(textureTypeString == L"NORMALMAP")
        textureType = Workflow::TEXTURE_NORMALMAP;
    
    return textureType;
}


void EpiLib::initialize()
{
    RdrConst::initialize();
    AssetsConst::initialize();
    MiscConst::initialize();
}






#define LM_KILL_CONST(type, var) (*(const_cast<type*>(&var)))


bool MiscConst::initialized = false;
String MiscConst::pathWorldDefault = L"";
String MiscConst::defaultTagGameplay = L"";
float MiscConst::sSoundGlobalGain = 1.f;

Core::List<String> MiscConst::memoPathShared;
Core::List<String> MiscConst::memoPathVfs;
const Core::List<String> MiscConst::sharedFolderShortcuts;
const Core::List<String> MiscConst::vfsFolderShortcuts;

void MiscConst::initialize()
{
    initialized = true;

    pathWorldDefault = L"";
    defaultTagGameplay = L"";
    memoPathShared.clear();
    memoPathVfs.clear();

    LM_KILL_CONST(Core::List<String>, sharedFolderShortcuts).push_back(IMPORT_FROM_SHARED_FOLDER + L"/" + L"HOO/Levels");
    LM_KILL_CONST(Core::List<String>, sharedFolderShortcuts).push_back(IMPORT_FROM_SHARED_FOLDER + L"/" + L"HOO/Spells");

    sSoundGlobalGain = 1.f;
}

void MiscConst::addPathToMemoPath(Core::List<String>& memoPath, const String& path)
{
    Core::List<String>::iterator it = std::find(memoPath.begin(), memoPath.end(), path);

    if (it!=memoPath.end())
    {
        memoPath.erase(it);
    }

    memoPath.push_back(path);
}

bool AssetsConst::initialized = false;
const Assets::Material AssetsConst::sMatPlayer1;
const Assets::Material AssetsConst::sMatPlayer2;

void AssetsConst::initialize()
{
    initialized = true;

    LM_KILL_CONST(Assets::Material, sMatPlayer1).diffuse.r = 0.08f;
    LM_KILL_CONST(Assets::Material, sMatPlayer1).diffuse.g = 0.08f;
    LM_KILL_CONST(Assets::Material, sMatPlayer1).diffuse.b = 1.f;
  
    LM_KILL_CONST(Assets::Material, sMatPlayer2).diffuse.r = 1.f;
    LM_KILL_CONST(Assets::Material, sMatPlayer2).diffuse.g = 0.08f;
    LM_KILL_CONST(Assets::Material, sMatPlayer2).diffuse.b = 0.08f;
}
bool RdrConst::initialized = false;
const Renderer::Material RdrConst::sMatAxisX;
const Renderer::Material RdrConst::sMatAxisY;
const Renderer::Material RdrConst::sMatAxisZ;

void RdrConst::initialize()
{
    initialized = true;

    Renderer::Material matAxis;
        matAxis.diffuse.r               = 1.0f;
        matAxis.diffuse.g               = 0.f;
        matAxis.diffuse.b               = 0.f;
        matAxis.diffuse.a               = 1.f;
        matAxis.ambient                 = Core::Vector4f(0.01f);
        matAxis.ambient.a               = 1.f;
        matAxis.dynamicShadowCaster     = false;
        matAxis.dynamicShadowReceiver   = false;
        matAxis.emissive                = Core::Vector4f(0.f);
        matAxis.emissive.a              = 1.f;
        matAxis.shininess               = 12.f;
        matAxis.specular                = Core::Vector4f(0.05f);
        matAxis.specular.a              = 1.0f;
        matAxis.glowColor               = Core::Vector4f(0.f);
        matAxis.glowColor.a             = 1.f;
        matAxis.glowExtent              = Core::Vector3f(0.f);
        matAxis.reflexions              = Core::Vector4f(0.f);
        matAxis.reflexions.a            = 0.f;
        matAxis.staticLighting          = false;
        matAxis.transDiffuseTex         = false;
        matAxis.zBias                   = 0.1f;

  //Material Axis
        LM_KILL_CONST(Renderer::Material, sMatAxisX) = matAxis;
        LM_KILL_CONST(Renderer::Material, sMatAxisY) = matAxis;
        LM_KILL_CONST(Renderer::Material, sMatAxisZ) = matAxis;
        LM_KILL_CONST(Renderer::Material, sMatAxisX).diffuse.r = 1.f;
        LM_KILL_CONST(Renderer::Material, sMatAxisX).diffuse.g = 0.08f;
        LM_KILL_CONST(Renderer::Material, sMatAxisX).diffuse.b = 0.08f;
        LM_KILL_CONST(Renderer::Material, sMatAxisY).diffuse.r = 0.08f;
        LM_KILL_CONST(Renderer::Material, sMatAxisY).diffuse.g = 1.f;
        LM_KILL_CONST(Renderer::Material, sMatAxisY).diffuse.b = 0.08f;
        LM_KILL_CONST(Renderer::Material, sMatAxisZ).diffuse.r = 0.08f;
        LM_KILL_CONST(Renderer::Material, sMatAxisZ).diffuse.g = 0.08f;
        LM_KILL_CONST(Renderer::Material, sMatAxisZ).diffuse.b = 1.f;
}

}//namespace EPI