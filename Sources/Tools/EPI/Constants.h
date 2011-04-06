/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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
#ifndef EPI_CONSTANTS_H_
#define EPI_CONSTANTS_H_

#include <QStringList>
#include <QList>
#include <QVariant>

#include <Assets/Texture.h>
#include <Workflow/TexCompression.h>

#include <Renderer/Material.h>

#define StrToQStr(string)  (QString(Core::String8(string).c_str()))
#define QStrToStr(string)  (String(Core::String8(string.toStdString().c_str())))

inline QList<QVariant> ListStrToQListQVariant(const Core::List<String>& strings)
{
    QList<QVariant>  paths;

    for (int32 ii=0; ii<strings.size(); ++ii)
    {
        QString str (String8(strings[ii]).c_str());
        paths.push_back(QVariant(str));
    }

    return paths;
}
inline Core::List<String> ListStrToQListQVariant(const QList<QVariant>& strings)
{
    Core::List<String>  paths;

    for (int32 ii=0; ii<strings.size(); ++ii)
    {
        String str (Core::String8(strings[ii].toString().toStdString().c_str()));
        paths.push_back(str);
    }

    return paths;
}

inline QStringList ListStrToQStringList(const Core::List<String>& strings)
{
    QStringList  paths;

    for (int32 ii=0; ii<strings.size(); ++ii)
    {
        QString str (String8(strings[ii]).c_str());
        paths.push_back(str);
    }

    return paths;
}

inline Core::List<String> QStringListToListStr(const QStringList& strings)
{
    Core::List<String>  paths;

    for (int32 ii=0; ii<strings.size(); ++ii)
    {
        String str (Core::String8(strings[ii].toStdString().c_str()));
        paths.push_back(str);
    }

    return paths;
}


inline String getPath(const String& str)
{
    String ret;


    return ret;
}
namespace EPI
{

const int32 MAX_UNDO_REDO = 20;


LM_ENUM_10(EAssetType,  TEXTURE_ASSET, 
                        MODEL_ASSET, 
                        TERRAIN_ASSET, 
                        PARTICLES_ASSET, 
                        WORLD_ASSET, 
                        TERRAIN_LAYERS,
                        SCRIPT_ASSET,
                        GROUP_ASSET,
                        SPLINE_ASSET,
                        PIC_BANK_ASSET)

//Extension de fichier autorisÈ pour les images
const Core::String AUTHORISED_IMAGE_EXT = L"*.png *.jpeg *.jpg *.tga *.tif *.bmp *.psd";
const Core::String AUTHORISED_MODEL_EXT = L"*.dae *.x *.obj";
const Core::String AUTHORISED_TERRAIN_EXT = L"*.exr *.png *.jpeg *.jpg *.tga *.tif *.bmp *.psd";
const Core::String AUTHORISED_SOUND_EXT = L"*.ogg  *.wav";

//Extention Document
const Core::String DOCUMENT_WORLD_EXT = L"world";
const Core::String DOCUMENT_GROUP_EXT = L"group";
const Core::String DOCUMENT_SPLINE_EXT = L"spline";
const Core::String DOCUMENT_PARTS_EXT = L"parts";
const Core::String DOCUMENT_MODEL_EXT = L"mdl";
const Core::String DOCUMENT_LUA_SCRIPT_EXT = L"lua";


// Chemin du dossier partagÈ pour les imports
const Core::String IMPORT_FROM_SHARED_FOLDER = L"Z:/";



const String    VIEW_TYPE_NAME_TOP      = L"Top";
const String    VIEW_TYPE_NAME_BOTTOM   = L"Bottom";
const String    VIEW_TYPE_NAME_LEFT     = L"Left";
const String    VIEW_TYPE_NAME_RIGHT    = L"Right";
const String    VIEW_TYPE_NAME_FRONT    = L"Front";
const String    VIEW_TYPE_NAME_BACK     = L"Back";
const String    VIEW_TYPE_NAME_USER     = L"User";

const String    POLYGON_MODE_NAME_SOLID             = L"Solid";
const String    POLYGON_MODE_NAME_SOLID_WIREFRAME   = L"S/W";
const String    POLYGON_MODE_NAME_POLY_WIREFRAME    = L"Wireframe";

const String    RENDER_MODE_NAME_RGB        = L"RGB";
const String    RENDER_MODE_NAME_HQ_PREVIEW = L"HQ Preview";
const String    RENDER_MODE_NAME_SHADOWMAP  = L"Shadow Map";
const String    RENDER_MODE_NAME_UV         = L"UV";
const String    RENDER_MODE_NAME_NORMAL     = L"Normal";
const String    RENDER_MODE_NAME_T          = L"T";
const String    RENDER_MODE_NAME_B          = L"B";
const String    RENDER_MODE_NAME_N          = L"N";
const String    RENDER_MODE_NAME_MIP        = L"MipMap";


const String    ASPECT_RATIO_NAME_FREE      = L"Aspect Free";
const String    ASPECT_RATIO_NAME_5_4       = L"Aspect 5/4";
const String    ASPECT_RATIO_NAME_4_3       = L"Aspect 4/3";
const String    ASPECT_RATIO_NAME_3_2       = L"Aspect 3/2";
const String    ASPECT_RATIO_NAME_16_10     = L"Aspect 16/10";
const String    ASPECT_RATIO_NAME_16_9      = L"Aspect 16/9";



// Import de texture
Core::String textureFormatToString(Assets::ETextureFormat format);
Core::String mipmapFilterToString(Workflow::EMipmapFilter mipmapFilter);
Core::String textureTypeToString(Workflow::ETextureType textureType);

Assets::ETextureFormat  stringToTextureFormat(const Core::String & formatString);
Workflow::EMipmapFilter stringToMipmapFilter(const Core::String & mipmapFilterString);
Workflow::ETextureType  stringToTextureType(const Core::String & textureTypeString);


//Tag specifique au node de la position des joueurs
const Core::String TAG_PLAYERS_POSITION = L"PlayersPosition";
const float PLAYER_DISTANCE = 24.f;
const Core::String TAG_GROUP_MEMO_CAM = L"GroupMemoCam";

const Core::String TAG_PLAYER         = L"player";
const Core::String TAG_OPPONENT       = L"opponent";

const Core::String TAG_PLAYERS_TARGET = L"target";
const Core::String TAG_PLAYERS_CASTER = L"caster";

const Core::String TAG_CAM_SCRIPT = L"CAMERA_SCRIPT";

const Core::String TAG_NODE_GAMEPLAY = L"gameplay";
const String TAG_GIZMO = L"TagNodeGizmo";

class LM_API_EPI EpiLib
{
public:
    void initialize();
};

class LM_API_EPI MiscConst
{
public:
    static void initialize();

    static String pathWorldDefault;
    static String defaultTagGameplay;
    static Core::List<String> memoPathShared;
    static Core::List<String> memoPathVfs;
    static const Core::List<String> sharedFolderShortcuts;
    static const Core::List<String> vfsFolderShortcuts;

    static void addPathToMemoPath(Core::List<String>& memoPath, const String& path);
    static float sSoundGlobalGain;

private:
    static bool initialized;
};

class LM_API_EPI AssetsConst
{
public:
    static void initialize();

    static const Assets::Material sMatPlayer1;
    static const Assets::Material sMatPlayer2;

private:
    static bool initialized;
};

class LM_API_EPI RdrConst
{
public:
    static void initialize();

    static const Renderer::Material sMatAxisX;
    static const Renderer::Material sMatAxisY;
    static const Renderer::Material sMatAxisZ;

private:
    static bool initialized;
};

}//namespace EPI

#endif /*EPI_CONSTANTS_H_*/