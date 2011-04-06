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
#ifndef XFILE_H_
#define XFILE_H_

#include <Core/Standard.h>
#include <Core/IO/Streams.h>
#include <Core/List.h>
#include <Core/Map.h>
#include <Core/Math/Vector.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Quaternion.h>

namespace Workflow
{

struct XTransKey : public Core::Object
{
    int32 keyFrame;
    Core::Vector3f position;
    XTransKey() : keyFrame(0), position(Core::ZERO) {};
};

struct XOrientKey : public Core::Object
{
    int32 keyFrame;
    Core::Quaternionf orient;
    XOrientKey() : keyFrame(0), orient(Core::IDENTITY) {};
};

struct XScaleKey : public Core::Object
{
    int32 keyFrame;
    Core::Vector3f scale;
    XScaleKey() : keyFrame(0), scale(Core::ONE) {};
};

struct XMatKey : public Core::Object
{
    int32 keyFrame;
    Core::Matrix4f matrix;
    XMatKey() : keyFrame(0), matrix(Core::IDENTITY) {};
};

struct XAnimation : public Core::Object
{
    String  name;
    String  frameName;
    Core::List<XTransKey> transKeys;
    Core::List<XOrientKey> orientKeys;
    Core::List<XScaleKey> scaleKeys;
    Core::List<XMatKey> matKeys;
    XAnimation(const String & name) : name(name) {};
};

struct XAnimationSet : public Core::Object
{
    String name;
    Core::List<Ptr<XAnimation> > anims;
    float                        ticksPerSecond;
    XAnimationSet(const String & name) : name(name) {};
};

struct XBone : public Core::Object
{
    String frameName;
    Core::List<int32>   indices;
    Core::List<float>   weight;
    Core::Matrix4f      offsetMatrix;
    XBone() : offsetMatrix(Core::IDENTITY) {};
};

struct XFace : public Core::Object
{
    int32 i1;
    int32 i2;
    int32 i3;
    XFace() : i1(0), i2(0), i3(0) {};
};

struct XTexCoord : public Core::Object
{
    float u;
    float v;
    XTexCoord() : u(0.0), v(0.0) {};
};

struct XNormales : public Core::Object
{
    Core::List<XFace>          faces;
    Core::List<Core::Vector3f> normales;
};

struct XTexCoords : public Core::Object
{
    Core::List<XTexCoord>  uv;
};

struct XMesh : public Core::Object
{
    String name;
    int32                      id;
    Core::List<XFace>          faces;
    Core::List<Core::Vector3f> positions;
    XNormales                  normales;
    XTexCoords                 texCoords;
    Core::List<Ptr<XBone> >    bones;
    XMesh(const String & name) : name(name), id(0) {};
};

struct XFrame : public Core::Object
{
    String name;
    int32  id;
    Core::Matrix4f            matrix;
    Core::List<Ptr<XFrame> >  frames;
    Core::List<Ptr<XMesh> >   meshes;
    XFrame(const String & name) : name(name), id(0), matrix(Core::IDENTITY) {};
};
struct XModel : public Core::Object
{
    Core::Map<String, Ptr<XMesh> >  meshesLib;
    Core::List<Ptr<XMesh> >         meshes;
    Core::List<Ptr<XFrame> >        rootFrames;
    Core::List<Ptr<XAnimationSet> > animSets;
};

LM_ENUM_24(EXFileToken,
           XTOKEN_EOF,
           XTOKEN_OPENED_BRACE,
           XTOKEN_CLOSED_BRACE,
           XTOKEN_TEMPLATE,
           XTOKEN_FRAME,
           XTOKEN_FRAME_MATRIX,
           XTOKEN_MESH,
           XTOKEN_MESH_TEX_COORDS,
           XTOKEN_MESH_NORMALES,
           XTOKEN_MESH_MAT_LIST,
           XTOKEN_MESH_FACE_WRAPS,
           XTOKEN_MESH_VERT_COLORS,
           XTOKEN_VERTEX_DUPLICATION,
           XTOKEN_MATERIAL,
           XTOKEN_TEXTURE_FILENAME,
           XTOKEN_SKIN_HEADER,
           XTOKEN_SKIN_WEIGHTS,
           XTOKEN_ANIMATION,
           XTOKEN_ANIMATION_TICKS_PER_SECOND,
           XTOKEN_ANIMATION_SET,
           XTOKEN_ANIMATION_KEY,
           XTOKEN_ANIMATION_OPTIONS,
           XTOKEN_IGNORE,
           XTOKEN_UNKOWN);

struct XToken : public Core::Object
{
    EXFileToken  type;
    String       name;
};

class XFileParser : public Core::Object
{
public:
    XFileParser(Core::InputStream & rInput, bool float64)
        : _rInput(rInput), _float64(float64)
    {};

    virtual void parseTokenHeader(XToken & token) = 0;
    virtual void parseTokenEnd() = 0;
    virtual void parseElementTokenEnd() = 0;
    virtual void readRef(String8 & ref) = 0;

    virtual String parseString() = 0;
    virtual int32 parseInt32() = 0;
    virtual void parseInt32(int32 & value) = 0;
    virtual void parseInt32Array(Core::List<int32> & value) = 0;

    virtual void parseFloat(float & value) = 0;
    virtual void parseFloatArray(Core::List<float> & value) = 0;

    virtual void parseVector(Core::Vector3f & value) = 0;
    virtual void parseVectorArray(Core::List<Core::Vector3f> & value) = 0;
    virtual void skipBlock() = 0;

    virtual void parseMatrix(Core::Matrix4f & matrix);

    virtual void parseXBone(XBone & bone);
    virtual void parseXSkinHeader();
    virtual void parseXOrientKey(XOrientKey & key);
    virtual void parseXTransKey(XTransKey & key);
    virtual void parseXScaleKey(XScaleKey & key);
    virtual void parseXMatKey(XMatKey & key);
    virtual void parseXFace(XFace & face);
    virtual void parseXTexCoords(XTexCoords & coords);
    virtual void parseXNormales(XNormales & xnorms);
    virtual void parseXMesh(XMesh & mesh);
    virtual void parseXFrame(XFrame & frame, XModel & model);
    virtual void parseXAnimationSet(XAnimationSet & anim, XModel & model);
    virtual void parseXAnimationKey(XAnimation & anim, XModel & model);
    virtual void parseXAnimation(XAnimation & anim, XModel & model);


    virtual void parseXModel(XModel & model);

protected:
    Core::InputStream & _rInput;
    bool                _float64;

    virtual void error(const String & message) const = 0;
    virtual void warning(const String & message) const = 0;
    virtual void warningUnknownToken(const XToken & token) const;
};

class XFileParserText : public XFileParser
{
public:
    XFileParserText(Core::InputStream & rInput, bool float64);

    virtual void parseTokenHeader(XToken & token);
    virtual void parseTokenEnd();
    virtual void parseElementTokenEnd();
    virtual void readRef(String8 & ref);

    virtual String parseString();
    virtual int32 parseInt32();
    virtual void parseInt32(int32 & value);
    virtual void parseInt32Array(Core::List<int32> & value);

    virtual void parseFloat(float & value);
    virtual void parseFloatArray(Core::List<float> & value);

    virtual void parseVector(Core::Vector3f & value);
    virtual void parseVectorArray(Core::List<Core::Vector3f> & value);
    virtual void skipBlock();

protected:
    void getNextToken(String8 & token);
    void readToken(String8 & token, char & separator);
    virtual void error(const String & message) const;
    virtual void warning(const String & message) const;

    char getChar();
    char getBasicChar();
    void unget(char c);

    int32   _line;
    char    _remainingChar;
    bool    _hasRemainingChar;
};
}
#endif /* XFILE_H_ */
