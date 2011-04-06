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
#include "VFSXMLTranslator.h"
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/VFS/VFSMountPoint.h>
#include <Core/VFS/VFSArchiveMapping.h>

namespace Core
{

Ptr<XMLNode> VFSToXMLNode(const Ptr<VirtualFileSystem> pVFS)
{
    Ptr<XMLNode> pXMLNode(new XMLNode(L"vfs"));
    EVFSType vfsType = pVFS->getType();

    if(vfsType == VFS_DIRECT_MAPPING)
    {
        Ptr<VFSDirectMapping> pDirectMapping = LM_DEBUG_PTR_CAST<VFSDirectMapping>(pVFS);
        pXMLNode->setAttribute(L"type", toString(vfsType));
        pXMLNode->setValue(L"directory-mapping", pDirectMapping->getMappedDirectory());
    }
    else if(vfsType == VFS_MOUNT_POINT)
    {
        Ptr<VFSMountPoint> pMountPoint = LM_DEBUG_PTR_CAST<VFSMountPoint>(pVFS);
        pXMLNode->setAttribute(L"type", toString(pMountPoint->getType()));
        pXMLNode->setValue(L"children", L"");
        List<String> mountedPointKeys;
        pMountPoint->getMountedPointKeys(mountedPointKeys);
        for(List<String>::const_iterator iMountedPointKey = mountedPointKeys.begin(); iMountedPointKey != mountedPointKeys.end(); ++iMountedPointKey)
        {
            Ptr<XMLNode> pXMLChild = VFSToXMLNode(pMountPoint->getMountedPoint(*iMountedPointKey));
            pXMLChild->setAttribute(L"key", *iMountedPointKey);
            pXMLNode->addNode(L"children", pXMLChild);
        }
    }
    else if(vfsType == VFS_ARCHIVE_MAPPING)
    {
        Ptr<VFSArchiveMapping> pArchive = LM_DEBUG_PTR_CAST<VFSArchiveMapping>(pVFS);
        pXMLNode->setAttribute(L"type", toString(pArchive->getType()));
        pXMLNode->setValue(L"archive-file", pArchive->getArchiveName());
    }
    else
        throw Core::Exception(L"Impossible to translate VFS in XML : Unknown VFS type.");

    return pXMLNode;
}

Ptr<VirtualFileSystem> VFSFromXMLNode(const Ptr<XMLNode> pXMLNode)
{
    bool success = true;
    Ptr<VirtualFileSystem> pVFS;
    String type;
    success &= pXMLNode->getAttribute(L"", L"type", type);
    String reason;
    if(type == toString(Core::VFS_DIRECT_MAPPING))
    {
        Ptr<VFSDirectMapping> pDirectMapping(new VFSDirectMapping());
        String directory;
        success &= pXMLNode->getValue(L"directory-mapping", directory);
        if(!success)
            reason = L"Unable to get the 'directory-mapping' value.";
        if(success)
            success &= pDirectMapping->linkDirectory(directory);
        if(!success)
            reason = L"Unable to link to the directory : " + directory;
        pVFS = pDirectMapping;
    }
    else if(type == toString(Core::VFS_MOUNT_POINT))
    {
        Ptr<VFSMountPoint> pMountPoint(new VFSMountPoint());

        Core::List<Ptr<Core::XMLNode> > XMLChildren;
        success &= pXMLNode->getNodes(L"children", L"vfs", XMLChildren);

        for(Core::List<Ptr<Core::XMLNode> >::const_iterator iXMLChild = XMLChildren.begin(); iXMLChild != XMLChildren.end(); ++iXMLChild)
        {
            Ptr<VirtualFileSystem> pVFSChild = VFSFromXMLNode(*iXMLChild);
            String key;
            success &= (*iXMLChild)->getAttribute(L"key", key);
            pMountPoint->mount(key, pVFSChild);
        }
        pVFS = pMountPoint;
    }
    else if(type == toString(Core::VFS_ARCHIVE_MAPPING))
    {
        Ptr<VFSArchiveMapping> pArchive(new VFSArchiveMapping());
        String archive;
        success &= pXMLNode->getValue(L"archive-file", archive);
        if(!success)
            reason = L"Unable to get the 'archive-file' value.";
        if(success)
            success &= pArchive->linkArchive(archive);
        if(!success)
            reason = L"Unable to link to the archive : " + archive;

        pVFS = pArchive;
    }
    else
    {
        String message = L"An error occured while trying to recreate the VFS.\n";
        message << L"Unknown type : "<<type<<L".";
        throw Core::Exception(message);
    }

    if(!success)
    {
        String message = L"An error occured while trying to recreate the VFS.";
        if(reason != String::EMPTY)
        {
            message << L"\n" <<reason;
        }
        throw Core::Exception(message);
    }
    return pVFS;
}

}//namespace Core
