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
#include <Core/Logger.h>
#include "XFile.h"

namespace Workflow
{
//-----------------------------------------------------------------------------
#define LM_SYNTAX_ASSERT(cond) \
    if(!(cond)) { warning(L"Syntax error"); }

#define LM_ERROR_ASSERT(cond, message) \
    if(!(cond)) { error(message); }

#define LM_WARNING_ASSERT(cond, message) \
    if(!(cond)) { warning(message); }
//-----------------------------------------------------------------------------
XFileParserText::XFileParserText(Core::InputStream & rInput, bool float64)
    : XFileParser(rInput, float64),
      _line(1),
      _remainingChar('\0'),
      _hasRemainingChar(false)
{};
//-----------------------------------------------------------------------------
char XFileParserText::getBasicChar()
{
    if(_hasRemainingChar)
    {
        _hasRemainingChar = false;
        return _remainingChar;
    }
    else
    {
        char result = '\0';
        int32 size = _rInput.readUpTo(&result, sizeof(result));

        // Ignore les retour chariots (\r), ne conserve que les fins de ligne (\n)
        while(size == sizeof(result) && (result == '\r'))
            size = _rInput.readUpTo(&result, sizeof(result));

        if(result == '\n')
            _line ++;
        else if (result == '\t') // -> convertit les tab en espace
            result = ' ';
        return result;
    }
}
//-----------------------------------------------------------------------------
void XFileParserText::unget(char c)
{
    LM_SYNTAX_ASSERT(!_hasRemainingChar);
    _hasRemainingChar = true;
    _remainingChar = c;
}
//-----------------------------------------------------------------------------
char XFileParserText::getChar()
{
    char result;
    bool isInComment = false;

    do {
        result = getBasicChar();
        if(result == '\0')
        {
            isInComment = false;
        }
        if(result == '\n')
        {
            isInComment = false;
            result = ' ';
        }
        else if (result == '#')
        {
            isInComment = true;
        }
        else if (result == '/')
        {
            char nextChar = getBasicChar();
            if(nextChar == '/')
            {
                isInComment = true;
            }
            else
            {
                unget(nextChar);
            }
        }
    }
    while(isInComment);

    return result;
}
//-----------------------------------------------------------------------------
void XFileParserText::getNextToken(String8 & token)
{
    token.clear();
    char nextChar;
    do {
        nextChar = getChar();
    }
    while(nextChar == ' ');

    if(nextChar == '{' || nextChar == '}')
    {
        token.push_back(nextChar);
        return;
    }
    else
    {
        while(nextChar != ' ' && nextChar != '\0')
        {
            token.push_back(nextChar);
            nextChar = getChar();
        }
    }
}
//-----------------------------------------------------------------------------
void XFileParserText::readToken(String8 & token, char & separator)
{
    token.clear();
    char nextChar;
    do {
        nextChar = getChar();
    }
    while(nextChar == ' ');

    while(nextChar != ',' && nextChar != ';')
    {
        token.push_back(nextChar);
        nextChar = getChar();
    }
    separator = nextChar;
}
//-----------------------------------------------------------------------------
void XFileParserText::readRef(String8 & ref)
{
    ref.clear();
    char nextChar;
    do {
        nextChar = getChar();
    }
    while(nextChar == ' ');

    while(nextChar != ' ' && nextChar != '}')
    {
        ref.push_back(nextChar);
        nextChar = getChar();
    }

    while(nextChar != '}')
        nextChar = getChar();
}
//-----------------------------------------------------------------------------
void XFileParserText::skipBlock()
{
    int32 count = 1;
    do {
        char c = getChar();
        if(c == '{')
            count++;
        else if(c == '}')
            count--;
        else if(c == '\0')
            count = 0;
    }
    while(count > 0);
}
//-----------------------------------------------------------------------------
void XFileParserText::error(const String & message) const
{
    String msg(message);
    msg << L" in TEXT XFile at line " << Core::toString(_line);
    ERR << msg << L"\n";
    throw Core::Exception(msg);
}
//-----------------------------------------------------------------------------
void XFileParserText::warning(const String & message) const
{
    String msg(message);
    WAR << L" in TEXT XFile at line " << Core::toString(_line) << L" "
        << msg << L"\n";
}
//-----------------------------------------------------------------------------
void XFileParserText::parseTokenHeader(XToken & token)
{
    token.name.clear();

    String8 tokenType;
    getNextToken(tokenType);

    if(_rInput.eof())
    {
        token.type = XTOKEN_EOF;
    }
    else if (tokenType == "{")
    {
        token.type = XTOKEN_OPENED_BRACE;
    }
    else if (tokenType == "}")
    {
        token.type = XTOKEN_CLOSED_BRACE;
    }
    else
    {
        for(int32 ii=0; ii < int32(tokenType.size()); ii++)
            tokenType[ii] = tolower(tokenType[ii]);
        String8 name;
        getNextToken(name);

        if(name != "{")
        {
            token.name = String(name);
            String8 separator;
            getNextToken(separator);
            LM_SYNTAX_ASSERT(separator == "{");
        }

             if(tokenType == "mesh")                     token.type = XTOKEN_MESH;
        else if(tokenType == "frametransformmatrix")     token.type = XTOKEN_FRAME_MATRIX;
        else if(tokenType == "template")                 token.type = XTOKEN_TEMPLATE;
        else if(tokenType == "frame")                    token.type = XTOKEN_FRAME;
        else if(tokenType == "meshtexturecoords")        token.type = XTOKEN_MESH_TEX_COORDS;
        else if(tokenType == "meshnormals")              token.type = XTOKEN_MESH_NORMALES;
        else if(tokenType == "meshmateriallist")         token.type = XTOKEN_MESH_MAT_LIST;
        else if(tokenType == "meshvertexcolors")         token.type = XTOKEN_MESH_VERT_COLORS;
        else if(tokenType == "meshfacewraps")            token.type = XTOKEN_MESH_FACE_WRAPS;
        else if(tokenType == "vertexduplicationindices") token.type = XTOKEN_VERTEX_DUPLICATION;
        else if(tokenType == "material")                 token.type = XTOKEN_MATERIAL;
        else if(tokenType == "texturefilename")          token.type = XTOKEN_TEXTURE_FILENAME;
        else if(tokenType == "xskinmeshheader")          token.type = XTOKEN_SKIN_HEADER;
        else if(tokenType == "skinweights")              token.type = XTOKEN_SKIN_WEIGHTS;
        else if(tokenType == "animation")                token.type = XTOKEN_ANIMATION;
        else if(tokenType == "animationset")             token.type = XTOKEN_ANIMATION_SET;
        else if(tokenType == "animationkey")             token.type = XTOKEN_ANIMATION_KEY;
        else if(tokenType == "animationoptions")         token.type = XTOKEN_ANIMATION_OPTIONS;
        else if(tokenType == "animtickspersecond")       token.type = XTOKEN_ANIMATION_TICKS_PER_SECOND;
        else if(tokenType == "material")                 token.type = XTOKEN_MATERIAL;
        else if(tokenType == "decldata")                 token.type = XTOKEN_IGNORE; // Custom data
        else if(tokenType == "keyvaluepair")             token.type = XTOKEN_IGNORE; // Custom exporter MAX Panda
        else if(tokenType == "objectmatrixcomment")      token.type = XTOKEN_IGNORE; // Custom exporter MAX Panda
        else
        {
            warning(String(L"Unknown token type ") + String(tokenType));
            token.type = XTOKEN_UNKOWN;
        }
    }
}
//-----------------------------------------------------------------------------
void XFileParserText::parseTokenEnd()
{
    String8 token;
    char separator;
    readToken(token, separator);
    LM_SYNTAX_ASSERT(token.empty());
    LM_SYNTAX_ASSERT(separator == ';');
}
//-----------------------------------------------------------------------------
void XFileParserText::parseElementTokenEnd()
{
    String8 token;
    char separator;
    readToken(token, separator);
    LM_SYNTAX_ASSERT(token.empty());
    LM_SYNTAX_ASSERT(separator == ',' || separator == ';');
}
//-----------------------------------------------------------------------------
void XFileParserText::parseInt32(int32 & value)
{
    String8 token;
    char separator;
    readToken(token, separator);
    LM_SYNTAX_ASSERT(separator == ';');
    value = Core::toInteger(String(token));
}
//-----------------------------------------------------------------------------
int32 XFileParserText::parseInt32()
{
    String8 token;
    char separator;
    readToken(token, separator);
    LM_SYNTAX_ASSERT(separator == ';');
    return Core::toInteger(String(token));
}
//-----------------------------------------------------------------------------
void XFileParserText::parseInt32Array(Core::List<int32> & value)
{
    value.clear();
    char separator;
    do {
        String8 token;
        readToken(token, separator);
        value.push_back(Core::toInteger(String(token)));
    } while(separator == ',');

    LM_SYNTAX_ASSERT(separator == ';');
}
//-----------------------------------------------------------------------------
void XFileParserText::parseFloat(float & value)
{
    String8 token;
    char separator;
    readToken(token, separator);
    value = (float)Core::toFloat(String(token));
    LM_SYNTAX_ASSERT(separator == ';');
}
//-----------------------------------------------------------------------------
void XFileParserText::parseFloatArray(Core::List<float> & value)
{
    value.clear();
    char separator;
    do {
        String8 token;
        readToken(token, separator);
        value.push_back((float)Core::toFloat(String(token)));
    } while(separator == ',');

    LM_SYNTAX_ASSERT(separator == ';');
}
//-----------------------------------------------------------------------------
void XFileParserText::parseVector(Core::Vector3f & value)
{
    parseFloat(value._x);
    parseFloat(value._y);
    parseFloat(value._z);
}
//-----------------------------------------------------------------------------
void XFileParserText::parseVectorArray(Core::List<Core::Vector3f> & value)
{
    value.clear();
    char separator;
    do {
        Core::Vector3f vec;
        parseVector(vec);
        value.push_back(vec);
        separator = getChar();
    } while(separator == ',');

    LM_SYNTAX_ASSERT(separator == ';');
}
//-----------------------------------------------------------------------------
void XFileParser::parseXModel(XModel & model)
{
    float ticksPerSecond = 4600.0f; // Bizarre, mais cette valeur a l'air d'être la valeur par défaut de 3DSMAX
    model.rootFrames.clear();
    bool finished = false;

    while(!_rInput.eof() && !finished)
    {
        XToken token;
        parseTokenHeader(token);

        switch(token.type)
        {
        case XTOKEN_EOF:
        {
            finished = true;
            break;
        }
        case XTOKEN_MESH:
        {
            Ptr<XMesh> pMesh(new XMesh(token.name));
            parseXMesh(*pMesh);

            // Mesh inline racine, il faut lui créer une frame implicite
            Ptr<XFrame> pFrame(new XFrame((token.name == String::EMPTY) ? L"Root" : token.name));
            pFrame->meshes.push_back(pMesh);
            model.rootFrames.push_back(pFrame);
            model.meshes.push_back(pMesh);

            // Mesh nommé. Il est stocké dans la libliothèque pour être référencé plus tard.
            if(token.name != String::EMPTY)
                model.meshesLib[token.name] = pMesh;
            break;
        }
        case XTOKEN_FRAME:
        {
            Ptr<XFrame> pFrame(new XFrame(token.name));
            parseXFrame(*pFrame, model);
            model.rootFrames.push_back(pFrame);
            break;
        }
        case XTOKEN_ANIMATION_SET:
        {
            Ptr<XAnimationSet> pAnimSet(new XAnimationSet(token.name));
            pAnimSet->ticksPerSecond = ticksPerSecond;
            parseXAnimationSet(*pAnimSet, model);
            model.animSets.push_back(pAnimSet);
            break;
        }
        case XTOKEN_ANIMATION_TICKS_PER_SECOND:
        {
            parseFloat(ticksPerSecond);
            skipBlock();
            break;
        }
        case XTOKEN_TEMPLATE:
        case XTOKEN_MATERIAL:
        case XTOKEN_IGNORE:
        {
            skipBlock();
            break;
        }
        default:
        {
            warningUnknownToken(token);
            skipBlock();
            break;
        }
        }
    }
}
//-----------------------------------------------------------------------------
void XFileParser::parseXFace(XFace & face)
{
    int32              indiceCount;
    Core::List<int32>  indices;

    parseInt32(indiceCount);
    parseInt32Array(indices);

    LM_ERROR_ASSERT(indiceCount == 3, L"Indice count isn't 3");
    LM_WARNING_ASSERT(indices.size() == 3, L"Read and expected indice count don't match");

    face.i1 = indices[0];
    face.i2 = indices[1];
    face.i3 = indices[2];
}
//-----------------------------------------------------------------------------
void XFileParser::parseXTexCoords(XTexCoords & coords)
{
    int32 count = 0;
    parseInt32(count);
    coords.uv.resize(count);
    for(Core::List<XTexCoord>::iterator iCoords = coords.uv.begin(); iCoords != coords.uv.end(); ++iCoords)
    {
        parseFloat(iCoords->u);
        parseFloat(iCoords->v);
        parseElementTokenEnd();
    }
    skipBlock();
}
//-----------------------------------------------------------------------------
void XFileParser::parseXNormales(XNormales & xnorms)
{
    int32 normCount = 0;
    parseInt32(normCount);
    parseVectorArray(xnorms.normales);
    LM_WARNING_ASSERT(normCount == xnorms.normales.size(), L"Read and expected normales count don't match");

    int32 faceCount;
    parseInt32(faceCount);
    xnorms.faces.resize(faceCount);
    for(Core::List<XFace>::iterator iFace = xnorms.faces.begin(); iFace != xnorms.faces.end(); ++iFace)
    {
        parseXFace(*iFace);
        parseElementTokenEnd();
    }
    skipBlock();
}
//-----------------------------------------------------------------------------
void XFileParser::parseXMesh(XMesh & mesh)
{
    mesh.faces.clear();
    mesh.positions.clear();

    int32 vertexCount;
    parseInt32(vertexCount);
    mesh.positions.reserve(vertexCount);
    parseVectorArray(mesh.positions);

    int32 faceCount;
    parseInt32(faceCount);
    mesh.faces.resize(faceCount);
    for(Core::List<XFace>::iterator iFace = mesh.faces.begin(); iFace != mesh.faces.end(); ++iFace)
    {
        parseXFace(*iFace);
        parseElementTokenEnd();
    }

    bool meshFinished = false;

    while(!meshFinished)
    {
        XToken token;
        parseTokenHeader(token);

        switch(token.type)
        {
        case XTOKEN_CLOSED_BRACE:
            meshFinished = true;
            break;
        case XTOKEN_MESH_TEX_COORDS:
            parseXTexCoords(mesh.texCoords);
            break;
        case XTOKEN_MESH_NORMALES:
            parseXNormales(mesh.normales);
            break;
        case XTOKEN_MESH_MAT_LIST:
            skipBlock();
            break;
        case XTOKEN_SKIN_HEADER:
            parseXSkinHeader();
            break;
        case XTOKEN_SKIN_WEIGHTS:
        {
            Ptr<XBone> pBone(new XBone());
            parseXBone(*pBone);
            mesh.bones.push_back(pBone);
            break;
        }
        case XTOKEN_IGNORE:
        {
            skipBlock();
            break;
        }
        default:
            warningUnknownToken(token);
            skipBlock();
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void XFileParser::parseXFrame(XFrame & frame, XModel & model)
{
    frame.matrix.setIdentity();
    bool frameFinished = false;

    while(!frameFinished)
    {
        XToken token;
        parseTokenHeader(token);

        switch(token.type)
        {
        case XTOKEN_CLOSED_BRACE:
            frameFinished = true;
            break;
        case XTOKEN_OPENED_BRACE:
        {
            String8 meshName;
            readRef(meshName);
            Core::Map<String, Ptr<XMesh> >::const_iterator ipMesh = model.meshesLib.find(String(meshName));
            if(ipMesh == model.meshesLib.end())
                error(String(L"Unknown mesh : ") + String(meshName));
            frame.meshes.push_back(ipMesh->second);
            break;
        }
        case XTOKEN_MESH:
        {
            if(token.name == String::EMPTY)
                token.name = frame.name;
            Ptr<XMesh> pMesh(new XMesh(token.name));
            parseXMesh(*pMesh);
            frame.meshes.push_back(pMesh);
            model.meshes.push_back(pMesh);
            model.meshesLib[token.name] = pMesh;
            break;
        }
        case XTOKEN_FRAME:
        {
            Ptr<XFrame> pFrame(new XFrame(token.name));
            parseXFrame(*pFrame, model);
            frame.frames.push_back(pFrame);
            break;
        }
        case XTOKEN_FRAME_MATRIX:
        {
            parseMatrix(frame.matrix);
            skipBlock();
            break;
        }
        case XTOKEN_IGNORE:
        {
            skipBlock();
            break;
        }
        default:
            warningUnknownToken(token);
            skipBlock();
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void XFileParser::parseXAnimationSet(XAnimationSet & anim, XModel & model)
{
    bool animFinished = false;

    while(!animFinished)
    {
        XToken token;
        parseTokenHeader(token);

        switch(token.type)
        {
        case XTOKEN_CLOSED_BRACE:
        {
            animFinished = true;
            break;
        }
        case XTOKEN_ANIMATION:
        {
            Ptr<XAnimation> pAnim(new XAnimation(token.name));
            parseXAnimation(*pAnim, model);
            anim.anims.push_back(pAnim);
            break;
        }
        default:
            warningUnknownToken(token);
            skipBlock();
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void XFileParser::parseXAnimationKey(XAnimation & anim, XModel & model)
{
    int32 keyType;
    int32 keyFramesCount;

    parseInt32(keyType);
    parseInt32(keyFramesCount);

    switch(keyType)
    {
    case 0: // -> Quaternion
    {
        anim.orientKeys.clear();
        for(int32 ii=0; ii < keyFramesCount; ii++)
        {
            XOrientKey key;
            parseXOrientKey(key);
            parseElementTokenEnd();
            anim.orientKeys.push_back(key);
        }
        break;
    }
    case 1: // -> Scale
    {
        anim.scaleKeys.clear();
        for(int32 ii=0; ii < keyFramesCount; ii++)
        {
            XScaleKey key;
            parseXScaleKey(key);
            parseElementTokenEnd();
            anim.scaleKeys.push_back(key);
        }
        break;
    }
    case 2: // -> Translation
    {
        anim.transKeys.clear();
        for(int32 ii=0; ii < keyFramesCount; ii++)
        {
            XTransKey key;
            parseXTransKey(key);
            parseElementTokenEnd();
            anim.transKeys.push_back(key);
        }
        break;
    }
    case 3: // -> Matrix
    case 4:
    {
        anim.transKeys.clear();
        for(int32 ii=0; ii < keyFramesCount; ii++)
        {
            XMatKey key;
            parseXMatKey(key);
            parseElementTokenEnd();
            anim.matKeys.push_back(key);
        }
        break;
    }
    default:
        warning(String(L"Unknown animation key type ") + Core::toString(keyType));
        break;
    }
    skipBlock();
}
//-----------------------------------------------------------------------------
void XFileParser::parseXAnimation(XAnimation & anim, XModel & model)
{
    bool animFinished = false;

    while(!animFinished)
    {
        XToken token;
        parseTokenHeader(token);

        switch(token.type)
        {
        case XTOKEN_CLOSED_BRACE:
        {
            animFinished = true;
            break;
        }
        case XTOKEN_OPENED_BRACE:
        {
            String8 frameName;
            readRef(frameName);
            anim.frameName = String(frameName);
            break;
        }
        case XTOKEN_ANIMATION_KEY:
        {
            parseXAnimationKey(anim, model);
            break;
        }
        case XTOKEN_ANIMATION_OPTIONS:
        case XTOKEN_IGNORE:
        {
            skipBlock();
            break;
        }
        default:
            warningUnknownToken(token);
            skipBlock();
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void XFileParser::parseXOrientKey(XOrientKey & key)
{
    Core::List<float> values;

    parseInt32(key.keyFrame);
    parseInt32(); // Longueur quat -> info redondante
    parseFloatArray(values);

    LM_ERROR_ASSERT(values.size() == 4, L"Invalid quaternion key");
    key.orient._w    = values[0];
    key.orient._v._x = values[1];
    key.orient._v._y = values[2];
    key.orient._v._z = values[3];
    key.orient.inverseSelf();
    parseTokenEnd();
}
//-----------------------------------------------------------------------------
void XFileParser::parseXTransKey(XTransKey & key)
{
    Core::List<float> values;

    parseInt32(key.keyFrame);
    parseInt32(); // Longueur trans -> info redondante
    parseFloatArray(values);

    LM_ERROR_ASSERT(values.size() == 3, L"Invalid translation key");
    key.position._x = values[0];
    key.position._y = values[1];
    key.position._z = values[2];
    parseTokenEnd();
}
//-----------------------------------------------------------------------------
void XFileParser::parseXScaleKey(XScaleKey & key)
{
    Core::List<float> values;

    parseInt32(key.keyFrame);
    parseInt32(); // Longueur scale -> info redondante
    parseFloatArray(values);

    LM_ERROR_ASSERT(values.size() == 3, L"Invalid scale key");
    key.scale._x = values[0];
    key.scale._y = values[1];
    key.scale._z = values[2];
    parseTokenEnd();
}
//-----------------------------------------------------------------------------
void XFileParser::parseXMatKey(XMatKey & key)
{
    parseInt32(key.keyFrame);
    parseInt32(); // Longueur matrix -> info redondante
    parseMatrix(key.matrix);
    parseTokenEnd();
}
//-----------------------------------------------------------------------------
String XFileParserText::parseString()
{
    String8 rawValue;
    char separator;
    readToken(rawValue, separator);
    LM_SYNTAX_ASSERT(separator == ';');

    if(rawValue.size() >= 2 && rawValue[0] == '"' && rawValue[rawValue.size() - 1] == '"')
        return String(rawValue.substr(1, rawValue.size() - 2));
    else
        return String(rawValue);
}
//-----------------------------------------------------------------------------
void XFileParser::parseXBone(XBone & bone)
{
    bone.frameName = parseString();
    int32 indiceCount = parseInt32();
    parseInt32Array(bone.indices);
    parseFloatArray(bone.weight);
    parseMatrix(bone.offsetMatrix);

    LM_WARNING_ASSERT(indiceCount == bone.indices.size(), L"Bones indice count doesn't match expected value");
    LM_WARNING_ASSERT(indiceCount == bone.weight.size(), L"Bones weight count doesn't match expected value");
    skipBlock();
}
//-----------------------------------------------------------------------------
void XFileParser::parseMatrix(Core::Matrix4f & matrix)
{
    Core::List<float> values;
    parseFloatArray(values);
    LM_ERROR_ASSERT(values.size() == 16, L"Invalid matrix");
    for(int32 ii=0; ii < 16; ii++)
        matrix._val[ii] = values[ii];
    matrix.transposeSelf();
}
//-----------------------------------------------------------------------------
void XFileParser::parseXSkinHeader()
{
    int32 maxBonePerVertex = parseInt32();
    parseInt32(); // maxBonePerFace
    parseInt32(); // boneCount
    LM_ERROR_ASSERT(maxBonePerVertex <= 4, L"More than 4 bones per vertex");
    skipBlock();
}
//-----------------------------------------------------------------------------
void XFileParser::warningUnknownToken(const XToken & token) const
{
    String message(L"Unexpected ");
    message << Workflow::toString(token.type) << L" token named '" << token.name << L"'";
    warning(message);
}
//-----------------------------------------------------------------------------
}
