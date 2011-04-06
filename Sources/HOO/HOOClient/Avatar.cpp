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
#include <HOOClient/Avatar.h>
#include <Universe/World.h>

String getCharacterModel(int32 charId)
{
    switch(charId)
    {
    case CHARACTER_SILENE:
    case CHARACTER_HERO:
    case CHARACTER_PRIESTESS:
    default:
        //return L"/HOO/Characters/Default/DefaultChar.mdl";
        return L"/HOO/Characters/Silene/Silene.mdl";
        break;
    }
}

String getSetString(int32 setId)
{
    switch(setId)
    {
    case ES_BASE:       return L"BASE"; break;
    case ES_VENERABLE:  return L"VENERABLE"; break;
    case ES_BOLD:       return L"BOLD"; break;
    case ES_IMPETIOUS:  return L"IMPETIOUS"; break;
    default:            return L"???"; break;
    }
}

String getElementString(int32 elemId)
{
    switch(elemId)
    {
    case EE_SYMBOL:     return L"SYMBOL"; break;
    case EE_CLOTHES:    return L"CLOTHES"; break;
    case EE_HEADDRESS:  return L"HEADDRESS"; break;
    case EE_ORNAMENT:   return L"ORNAMENT"; break;
    case EE_AURA:       return L"AURA"; break;
    default:            return L"???"; break;
    }
}

String getEquipmentPrefix(int32 equipId)
{
    int32 setId = getSetFromEquipmentId(equipId);
    int32 elemId = getElementFromEquipmentId(equipId);

    return L"EQ_" + getSetString(setId) + L"_" + getElementString(elemId) + L"_";
}

void addAllMeshesWithPrefix(const Ptr<Universe::NodeSkeleton> & pSkeleton, Universe::World & world, const String & prefix)
{
    String model = pSkeleton->getModelName();
    Ptr<Assets::Model> pModel = world.getRessourcesPool()->getModel(model);

    for(int32 iNode=0; iNode < pModel->getNodeCount(); iNode++)
    {
        const Assets::ModelNode & node = pModel->getNode(iNode);

        if(node.getMeshId() != Assets::UNDEFINED_MESH_ID)
        {
            const Assets::ModelMesh & mesh = pModel->getMesh(node.getMeshId());

            if(mesh.getName().startsWith(prefix))
            {
                if(mesh.isSkin())
                {
                    pSkeleton->createSkin(model, mesh.getName());
                }
                else
                {
                    Ptr<Universe::NodeMesh> pNodeMesh = world.createMesh(model, mesh.getName());
                    pSkeleton->addModelNode(iNode, pNodeMesh);

                    //Ptr<Universe::Node> pAttachNode = pSkeleton->getChildWithTag(node.getName());
                    //pAttachNode->addChild(pNodeMesh);
                }
            }
        }
    }
}

void addEquipment(const Ptr<Universe::NodeSkeleton> & pSkeleton, Universe::World & world, int32 id)
{
    switch(id)
    {
    case EQUIP_SILENE_BASE_SYMBOL: // Il faut au moins un case
    default:
        addAllMeshesWithPrefix(pSkeleton, world, getEquipmentPrefix(id));        
        break;
    }
}

Ptr<Universe::NodeSkeleton> createAvatar(Universe::World & world, const AvatarDesc & desc)
{
    Ptr<Universe::NodeSkeleton> pAvatar = world.createSkeleton(getCharacterModel(desc.characterId));

    addAllMeshesWithPrefix(pAvatar, world, L"BODY_");

    for(int32 ii=0; ii < desc.equipment.size(); ii++)
        addEquipment(pAvatar, world, desc.equipment[ii]);

    return pAvatar;
}

Ptr<Universe::NodeSkeleton> createBaseAvatar(Universe::World & world, int32 charId)
{
    int32 charBase = charId * 100;

    AvatarDesc desc;
    desc.characterId = charId;
    desc.equipment.push_back(charBase + ES_BASE + EE_SYMBOL);
    desc.equipment.push_back(charBase + ES_BASE + EE_CLOTHES);
    desc.equipment.push_back(charBase + ES_BASE + EE_HEADDRESS);
    desc.equipment.push_back(charBase + ES_BASE + EE_ORNAMENT);
    desc.equipment.push_back(charBase + ES_BASE + EE_AURA);

    return createAvatar(world, desc);
}

