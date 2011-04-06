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
#include "IShader.h"

namespace Gfx
{
//---------------------------------------------------------------------
ShaderConstantDesc::ShaderConstantDesc() :
name(Core::String::EMPTY),
    type(SHADER_PARAM_UNKNOWN),
    bytesPerElement(0),
    nbElements(0),
    registerStart(0),
    registerCount(0),
    isRowMajor(false)
{
}
//-----------------------------------------------------------------------------
void ShaderConstantDesc::read(Core::InputStream & inputStream)
{
    inputStream.read(name);
    inputStream.read(bytesPerElement);
    inputStream.read(nbElements);
    inputStream.read(registerStart);
    inputStream.read(registerCount);
    type = (EShaderConstantType)inputStream.readInt32();
    inputStream.read(isRowMajor);
}
//-----------------------------------------------------------------------------
void ShaderConstantDesc::write(Core::OutputStream & outputStream) const
{
    outputStream.write(name);
    outputStream.write(bytesPerElement);
    outputStream.write(nbElements);
    outputStream.write(registerStart);
    outputStream.write(registerCount);
    outputStream.write(int32(type));
    outputStream.write(isRowMajor);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
ShaderConstantTable::~ShaderConstantTable()
{
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setConstantSafe(int32 index, bool value)
{
    if(index != UNDEFINED_SHADER_CONST)
        setConstant(index, value);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setConstantSafe(int32 index, int32 value)
{
    if(index != UNDEFINED_SHADER_CONST)
        setConstant(index, value);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setConstantSafe(int32 index, float value)
{
    if(index != UNDEFINED_SHADER_CONST)
        setConstant(index, value);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setConstantSafe(int32 index, const Core::Vector3f & value)
{
    if(index != UNDEFINED_SHADER_CONST)
        setConstant(index, value);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setConstantSafe(int32 index, const Core::Vector4f & value)
{
    if(index != UNDEFINED_SHADER_CONST)
        setConstant(index, value);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setConstantSafe(int32 index, const Core::Vector2f & value)
{
    if(index != UNDEFINED_SHADER_CONST)
        setConstant(index, value);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setConstantSafe(int32 index, const Core::Matrix4f & value)
{
    if(index != UNDEFINED_SHADER_CONST)
        setConstant(index, value);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setConstantMat3x4Safe(int32 index, const Core::Matrix4f & value)
{
    if(index != UNDEFINED_SHADER_CONST)
        setConstantMat3x4(index, value);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setConstantMat3x4ArraySafe(int32 index, const Core::Matrix4f * pValues, int32 count)
{
    if(index != UNDEFINED_SHADER_CONST)
        setConstantMat3x4Array(index, pValues, count);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setSampler1DSafe(int32 index, const IShaderResourceViewPtr & pRes)
{
    if(index != UNDEFINED_SHADER_CONST)
        setSampler1D(index, pRes);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setSampler2DSafe(int32 index, const IShaderResourceViewPtr & pRes)
{
    if(index != UNDEFINED_SHADER_CONST)
        setSampler2D(index, pRes);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setSampler3DSafe(int32 index, const IShaderResourceViewPtr & pRes)
{
    if(index != UNDEFINED_SHADER_CONST)
        setSampler3D(index, pRes);
}
//-----------------------------------------------------------------------------
void ShaderConstantTable::setSamplerCubeSafe(int32 index, const IShaderResourceViewPtr & pRes)
{
    if(index != UNDEFINED_SHADER_CONST)
        setSamplerCube(index, pRes);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
IVertexShaderBinObject::~IVertexShaderBinObject()
{
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
IPixelShaderBinObject::~IPixelShaderBinObject()
{
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
IVertexShader::~IVertexShader()
{
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
IPixelShader::~IPixelShader()
{
}
//---------------------------------------------------------------------
} //namespace Gfx

/*
namespace Core
{
String toString(const Gfx::ShaderConstantBufferDesc& scd)
{
	String str;
	str += L"type : ";
	str += toString(scd._type);
	str += L" ";
	str += L" ";
	str += L" ";
	str += L"bytesPerElement : ";
	str += toString(scd._bytesPerElement);
	str += L" ";
	str += L"nbElements : ";
	str += toString(scd._nbElements);
	str += L" ";

	return str;
}
} // namespace Core
*/
