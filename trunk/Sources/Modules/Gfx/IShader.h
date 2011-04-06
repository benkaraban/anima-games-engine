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
#ifndef ISHADER_H_
#define ISHADER_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Map.h>
#include <Core/IO/Streams.h>
#include <Gfx/GfxType.h>
#include <Gfx/ITexture.h>

//TODO voir pourquoi il est encore necessaire de retirer la definition de far et near
#ifdef far
#undef far
#endif

#ifdef near
#undef near
#endif

#include <Core/Math/Matrix.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
struct LM_API_GFX ShaderMacro : public Core::Object
{
    String name;
    String value;

    ShaderMacro(const String & name = L"", const String & value = L"") :
        name(name), value(value)
    {};
};
//-----------------------------------------------------------------------------
struct LM_API_GFX ShaderConstantDesc : public Core::Object, public Core::Serializable
{
    String              name;
    EShaderConstantType type;
    int32               bytesPerElement;  //(suivant le langage cela diffère du type de l'element)
    int32               nbElements;
    int32               registerStart;
    int32               registerCount;
    bool                isRowMajor;

    ShaderConstantDesc();

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;
};
//-----------------------------------------------------------------------------
const int32 UNDEFINED_SHADER_CONST = -1;

class LM_API_GFX ShaderConstantTable : public Core::Serializable
{
public:
    virtual ~ShaderConstantTable();

    virtual int32 getConstantCount() const = 0;
    virtual const ShaderConstantDesc & getConstantDesc(int32 index) const = 0;
    virtual const ShaderConstantDesc & getConstantDesc(const String & name) const = 0;

    virtual int32 getConstantIndex(const String & name) const = 0;
    virtual int32 getConstantIndexIfExists(const String & name) const = 0;

    virtual int32 getSamplerId(int32 index) const = 0;

    virtual void setBurstMode(bool bm) = 0;
    virtual bool isBurstMode() const = 0;
    virtual void flushBurst() = 0;

    virtual void setConstant(int32 index, bool value) = 0;
    virtual void setConstant(int32 index, int32 value) = 0;
    virtual void setConstant(int32 index, float value) = 0;
    virtual void setConstant(int32 index, const Core::Vector2f & value) = 0;
    virtual void setConstant(int32 index, const Core::Vector3f & value) = 0;
    virtual void setConstant(int32 index, const Core::Vector4f & value) = 0;
    virtual void setConstant(int32 index, const Core::Matrix4f & value) = 0;
    virtual void setConstantMat3x4(int32 index, const Core::Matrix4f & value) = 0;
    virtual void setConstantMat3x4Array(int32 index, const Core::Matrix4f * pValues, int32 count) = 0;

    virtual void setSampler1D(int32 index, const IShaderResourceViewPtr & pRes) = 0;
    virtual void setSampler2D(int32 index, const IShaderResourceViewPtr & pRes) = 0;
    virtual void setSampler3D(int32 index, const IShaderResourceViewPtr & pRes) = 0;
    virtual void setSamplerCube(int32 index, const IShaderResourceViewPtr & pRes) = 0;

    virtual void setConstantSafe(int32 index, bool value);
    virtual void setConstantSafe(int32 index, int32 value);
    virtual void setConstantSafe(int32 index, float value);
    virtual void setConstantSafe(int32 index, const Core::Vector2f & value);
    virtual void setConstantSafe(int32 index, const Core::Vector3f & value);
    virtual void setConstantSafe(int32 index, const Core::Vector4f & value);
    virtual void setConstantSafe(int32 index, const Core::Matrix4f & value);
    virtual void setConstantMat3x4Safe(int32 index, const Core::Matrix4f & value);
    virtual void setConstantMat3x4ArraySafe(int32 index, const Core::Matrix4f * pValues, int32 count);

    virtual void setSampler1DSafe(int32 index, const IShaderResourceViewPtr & pRes);
    virtual void setSampler2DSafe(int32 index, const IShaderResourceViewPtr & pRes);
    virtual void setSampler3DSafe(int32 index, const IShaderResourceViewPtr & pRes);
    virtual void setSamplerCubeSafe(int32 index, const IShaderResourceViewPtr & pRes);
};
typedef Ptr<ShaderConstantTable > ShaderConstantTablePtr;

//-----------------------------------------------------------------------------
class LM_API_GFX IShaderIncluder
{
public:
    virtual ~IShaderIncluder() {};

    virtual bool getIncludeData(const String & fileName, Core::List<byte> & data) = 0;
};

//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
class LM_API_GFX IPixelShaderBinObject : public Core::Serializable
{
public:
    virtual ~IPixelShaderBinObject();
    virtual ShaderConstantTablePtr getConstantTable() const = 0;
};
typedef Ptr<IPixelShaderBinObject> IPixelShaderBinObjectPtr;

//-----------------------------------------------------------------------------
class LM_API_GFX IVertexShaderBinObject : public Core::Serializable
{
public:
    virtual ~IVertexShaderBinObject();
    virtual ShaderConstantTablePtr getConstantTable() const = 0;
};
typedef Ptr<IVertexShaderBinObject> IVertexShaderBinObjectPtr;

//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
class LM_API_GFX IPixelShader
{
public:
    virtual ~IPixelShader();
    virtual ShaderConstantTablePtr getConstantTable() const = 0;
};
typedef Ptr<IPixelShader > IPixelShaderPtr;

//-----------------------------------------------------------------------------
class LM_API_GFX IVertexShader
{
public:
    virtual ~IVertexShader();
    virtual ShaderConstantTablePtr getConstantTable() const = 0;
};
typedef Ptr<IVertexShader > IVertexShaderPtr;
//-----------------------------------------------------------------------------
} //namespace Gfx

#endif /* ISHADER_H_ */
