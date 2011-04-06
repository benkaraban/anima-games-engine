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
#include <Core/Strings/Tools.h>
#include <Core/IO/Tools.h>
#include <Assets/Model.h>
#include <Workflow/XFile.h>
#include <Workflow/ModelImporterOBJ.h>

namespace Workflow
{
//-----------------------------------------------------------------------------
LM_ENUM_6(EOBJCommand, 
          OBJ_COM_V,
          OBJ_COM_VT,
          OBJ_COM_VN,
          OBJ_COM_F,
          OBJ_COM_G,
          OBJ_COM_SKIP);
//-----------------------------------------------------------------------------
static const int32 OBJ_MAX_INDICES = 3;
//-----------------------------------------------------------------------------
struct OBJCommand
{
    EOBJCommand    type;

    String         name;
    Core::Vector3f vec;

    int32       vi[OBJ_MAX_INDICES];
    int32       ni[OBJ_MAX_INDICES];
    int32       ti[OBJ_MAX_INDICES];

    bool        no[OBJ_MAX_INDICES];
    bool        to[OBJ_MAX_INDICES];
};
//-----------------------------------------------------------------------------
void parseLine(const String & l, OBJCommand & command, int32 lineNumber)
{
    String line;
    Core::trimAll(l, line, L" \r\t");

    Core::List<String> tokens;
    Core::tokenize(line, tokens, L" ");
    
    if(tokens.empty())
    {
        // Ligne vide
        command.type = OBJ_COM_SKIP;
    }
    else if(tokens[0] == L"v")
    {
        command.type = OBJ_COM_V;
        if(tokens.size() != 4)
            throw Core::Exception(L"OBJ import syntax error at line " + Core::toString(lineNumber));
        command.vec.x = float(Core::toFloat(tokens[1]));
        command.vec.y = float(Core::toFloat(tokens[2]));
        command.vec.z = float(Core::toFloat(tokens[3]));
    }
    else if(tokens[0] == L"vt")
    {
        command.type = OBJ_COM_VT;
        if(tokens.size() != 3 && tokens.size() != 4)
            throw Core::Exception(L"OBJ import syntax error at line " + Core::toString(lineNumber));
        command.vec.x = float(Core::toFloat(tokens[1]));
        command.vec.y = float(Core::toFloat(tokens[2]));
    }
    else if(tokens[0] == L"vn")
    {
        command.type = OBJ_COM_VN;
        if(tokens.size() != 4)
            throw Core::Exception(L"OBJ import syntax error at line " + Core::toString(lineNumber));
        command.vec.x = float(Core::toFloat(tokens[1]));
        command.vec.y = float(Core::toFloat(tokens[2]));
        command.vec.z = float(Core::toFloat(tokens[3]));
    }
    else if(tokens[0] == L"f")
    {
        command.type = OBJ_COM_F;
        if(tokens.size() != 4)
            throw Core::Exception(L"OBJ import error (non triangle polygon) at line " + Core::toString(lineNumber));

        for(int32 ii=0; ii < OBJ_MAX_INDICES; ii++)
        {
            Core::List<String> subTokens;
            Core::tokenize(tokens[ii + 1], subTokens, L"/");

            command.vi[ii] = Core::toInteger(subTokens[0]);

            if(subTokens.size() >= 2 && !subTokens[1].empty())
            {
                command.ti[ii] = Core::toInteger(subTokens[1]);
                command.to[ii] = false;
            }
            else
            {
                command.to[ii] = false;
            }

            if(subTokens.size() >= 3 && !subTokens[2].empty())
            {
                command.ni[ii] = Core::toInteger(subTokens[2]);
                command.no[ii] = true;
            }
            else
            {
                command.no[ii] = false;
            }
        }
    }
    else if(tokens[0] == L"g")
    {
        command.type = OBJ_COM_G;

        if(tokens.size() != 2)
            throw Core::Exception(L"OBJ import syntax error at line " + Core::toString(lineNumber));

        command.name = tokens[1];
    }
    else
    {
        // Commentaires, commandes ignor�es : s, usemtl
        command.type = OBJ_COM_SKIP;
    }
}
//-----------------------------------------------------------------------------
bool ModelImporterOBJ::supportFormat(const String & fileExtension) const
{
    return fileExtension == L"obj";
}
//-----------------------------------------------------------------------------
void addOBJModel(const Ptr<Assets::Model> & pModel, const Assets::VerticeSet & vset, const String & name)
{
    Assets::ModelMesh mesh(0, name);
    mesh.getLODByID(0) = vset;
    int32 idMesh = pModel->addMesh(name, mesh);
    pModel->addMeshNode(name, Assets::ROOT_NODE_ID, idMesh);
}
//-----------------------------------------------------------------------------
Ptr<Assets::Model> ModelImporterOBJ::importModel(Core::InputStream & input)
{
    Ptr<Assets::Model> pModel(new Assets::Model());

    Core::List<Core::Vector3f> vpos;
    Core::List<Core::Vector3f> norm;
    Core::List<Core::Vector2f> texc;

    Assets::VerticeSet vset;
    Core::List<Assets::Vertex>  & vert = vset.getVertices();
    Core::List<uint16> & ind = vset.getIndices();

    String fileContent;
    Core::readAllStream(fileContent, input);

    Core::List<String> lines;
    Core::tokenize(fileContent, lines, L"\n");

    bool isFirstGroup = true;
    String curGroupName;

    for(int32 iLine=0; iLine < lines.size(); iLine++)
    {
        OBJCommand command;
        parseLine(lines[iLine], command, iLine + 1);

        switch(command.type)
        {
        case OBJ_COM_V:
            vpos.push_back(command.vec);
            break;
        case OBJ_COM_VT:
            texc.push_back(Core::Vector2f(command.vec.x, command.vec.y));
            break;
        case OBJ_COM_VN:
            norm.push_back(command.vec);
            break;
        case OBJ_COM_F:
            
            for(int32 ii=0; ii < OBJ_MAX_INDICES; ii++)
            {
                Assets::Vertex v;
                int32 vi = command.vi[ii] - 1;
                int32 ni = command.ni[ii] - 1;
                int32 ti = command.ti[ii] - 1;

                v.position  = (vi >= 0) ? vpos[vi] : vpos[vpos.size() + vi];

                if(command.no[ii])
                    v.normale   = (ni >= 0) ? norm[ni] : norm[norm.size() + ni];
                else
                    v.normale   = Core::Vector3f(0.0f, 0.0f, 1.0f);

                if(command.to[ii])
                    v.texCoords = (ti >= 0) ? texc[ti] : texc[texc.size() + ti];
                else
                    v.texCoords = Core::Vector2f::ZERO;

                for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
                {
                    v.boneId[iBone] = 0;
                    v.boneWeight[iBone] = 0.0f;
                }

                ind.push_back(vert.size());
                vert.push_back(v);
            }

            break;

        case OBJ_COM_G:
            
            if(!isFirstGroup)
                addOBJModel(pModel, vset, curGroupName);

            curGroupName = command.name;
            isFirstGroup = false;
            break;

        case OBJ_COM_SKIP:
            break;
        }
    }

    addOBJModel(pModel, vset, curGroupName);
    pModel->cleanup();

    return pModel;
}
//-----------------------------------------------------------------------------
}
