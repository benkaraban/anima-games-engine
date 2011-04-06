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

#include "VFSMountPoint.h"

#include <Core/Strings/Tools.h>

namespace Core
{
VFSMountPoint::VFSMountPoint()
{}

VFSMountPoint::~VFSMountPoint()
{}

bool VFSMountPoint::mount(const String & key, const Ptr<VirtualFileSystem> & pVFS)
{
    bool returnCode = false;

    if(key == L"/")
    {
        _pRootVFS = pVFS;
        returnCode = true;
    }
    else if(isPathValid(key) && !isVFSMounted(key))
    {
        _mountedVFS[key] = pVFS;
        returnCode = true;
    }
    return returnCode;
}

void VFSMountPoint::listDirs(const String & path, List<String> & content)
{
    String subPath = path;
    String vfsKey = extractVFSKey(subPath);

    if(isPathValid(vfsKey) && isVFSMounted(vfsKey))
    {
        Ptr<VirtualFileSystem> vfs = _mountedVFS[vfsKey];
        vfs->listDirs(subPath, content);
    }
    else
    {
        if(_pRootVFS != null)
            _pRootVFS->listDirs(path, content);

        if(path == L"/")
        {
            List<String> keys;
            internalGetMountedPointKeys(keys, false);
            String target;
            for(List<String>::const_iterator iKey = keys.begin(); iKey != keys.end(); ++iKey)
            {
                trimLeft(*iKey, target, L"/");
                content.push_back(target);
            }
        }
    }
}

void VFSMountPoint::listFiles(const String & path, List<String> & content)
{
    String subPath = path;
    String vfsKey = extractVFSKey(subPath);

    if(isPathValid(vfsKey) && isVFSMounted(vfsKey))
    {
        Ptr<VirtualFileSystem> vfs = _mountedVFS[vfsKey];
        vfs->listFiles(subPath, content);
    }
    else if(_pRootVFS != null)
    {
        _pRootVFS->listFiles(path, content);
    }
}

bool VFSMountPoint::fileExists(const String & fileName)
{
    bool returnCode = false;
    String subPath = fileName;
    String vfsKey = extractVFSKey(subPath);

    if(isPathValid(vfsKey) && isVFSMounted(vfsKey))
    {
        Ptr<VirtualFileSystem> vfs = _mountedVFS[vfsKey];
        returnCode = vfs->fileExists(subPath);
    }
    else if(_pRootVFS != null)
    {
        returnCode = _pRootVFS->fileExists(fileName);
    }
    return returnCode;
}

bool VFSMountPoint::dirExists(const String & dirName)
{
    bool returnCode = false;
    String subPath = dirName;
    String vfsKey = extractVFSKey(subPath);

    if(isPathValid(vfsKey) && isVFSMounted(vfsKey))
    {
        Ptr<VirtualFileSystem> vfs = _mountedVFS[vfsKey];
        returnCode = vfs->dirExists(subPath);
    }
    else if(_pRootVFS != null)
    {
        returnCode = _pRootVFS->dirExists(dirName);
    }
    return returnCode;
}

void VFSMountPoint::createDir(const String & dirName)
{
    String subPath = dirName;
    String vfsKey = extractVFSKey(subPath);

    if(isPathValid(vfsKey) && isVFSMounted(vfsKey))
    {
        Ptr<VirtualFileSystem> vfs = _mountedVFS[vfsKey];
        vfs->createDir(subPath);
    }
    else if(_pRootVFS != null)
    {
        _pRootVFS->createDir(dirName);
    }
}

bool VFSMountPoint::moveFile(const String & from, const String & to)
{
    bool returnCode = false;
    String subPathFrom  = from;
    String subPathTo    = to;
    String vfsKeyFrom   = extractVFSKey(subPathFrom);
    String vfsKeyTo     = extractVFSKey(subPathTo);

    if(isPathValid(vfsKeyFrom) && isVFSMounted(vfsKeyFrom) 
    && vfsKeyFrom == vfsKeyTo)
    {
        Ptr<VirtualFileSystem> vfs = _mountedVFS[vfsKeyFrom];
        returnCode = vfs->moveFile(subPathFrom, subPathTo);
    }
    else if(_pRootVFS != null)
    {
        _pRootVFS->moveFile(from, to);
    }

    return returnCode;
}

bool VFSMountPoint::moveDir(const String & from, const String & to)
{
    bool returnCode = false;
    String subPathFrom  = from;
    String subPathTo    = to;
    String vfsKeyFrom   = extractVFSKey(subPathFrom);
    String vfsKeyTo     = extractVFSKey(subPathTo);

    if(isPathValid(vfsKeyFrom) && isVFSMounted(vfsKeyFrom) 
    && vfsKeyFrom == vfsKeyTo)
    {
        Ptr<VirtualFileSystem> vfs = _mountedVFS[vfsKeyFrom];
        returnCode = vfs->moveDir(subPathFrom, subPathTo);
    }
    else if(_pRootVFS != null)
    {
        _pRootVFS->moveDir(from, to);
    }

    return returnCode;
}

TimeValue VFSMountPoint::getLastModificationDate(const String & fileName)
{
    TimeValue result;
    String subPath = fileName;
    String vfsKey = extractVFSKey(subPath);

    if(isPathValid(vfsKey) && isVFSMounted(vfsKey))
    {
        Ptr<VirtualFileSystem> vfs = _mountedVFS[vfsKey];
        result = vfs->getLastModificationDate(subPath);
    }
    else if(_pRootVFS != null)
    {
        result = _pRootVFS->getLastModificationDate(fileName);
    }
    else
    {
        String errorMessage = fileName;
        errorMessage << L" is not a validPath or the corresponding VFS is not mounted.";
        throw Core::Exception(errorMessage);
    }

    return result;
}

Ptr<InputStream> VFSMountPoint::readFile(const String & fileName)
{
    Ptr<InputStream> input(NULL);

    String subPath = fileName;
    String vfsKey = extractVFSKey(subPath);

    if(isPathValid(vfsKey) && isVFSMounted(vfsKey))
    {
        Ptr<VirtualFileSystem> vfs = _mountedVFS[vfsKey];
        if(vfs->fileExists(subPath))
            input = vfs->readFile(subPath);
    }
    else if(_pRootVFS != null)
    {
        input = _pRootVFS->readFile(fileName);
    }

    return input;
}

Ptr<OutputStream> VFSMountPoint::writeFile( const String & fileName,
                                            FileOutputStream::EFileAccess fileAccess)
{
    Ptr<OutputStream> output(NULL);
    String subPath = fileName;
    String vfsKey = extractVFSKey(subPath);

    if(isPathValid(vfsKey) && isVFSMounted(vfsKey))
    {
        Ptr<VirtualFileSystem> vfs = _mountedVFS[vfsKey];
        output = vfs->writeFile(subPath, fileAccess);
    }
    else if(_pRootVFS != null)
    {
        output = _pRootVFS->writeFile(fileName, fileAccess);
    }

    return output;
}

bool VFSMountPoint::isPathValid(const String & path)
{
    //La longueur doit être au minimum de deux puisque le chemin
    //doit comporter au moins un slash et un caractère
    if(path.length()<2)
        return false;

    //Un chemin d'un VFS doit être de la forme "/xxxxxx"
    if(path.find(L"/", 0) != 0)
        return false;

    int32 slashFound = path.find(L"/", 1);

    return (slashFound<0);
}

bool VFSMountPoint::isVFSMounted(const String & path)
{
    return  (_mountedVFS.find(path) != _mountedVFS.end());
}

String VFSMountPoint::toString() const
{
    String result(L"MountPoint {\n");

    for(Map<String, Ptr<VirtualFileSystem> >::const_iterator iMount = _mountedVFS.begin();
        iMount != _mountedVFS.end();
        ++iMount)
        result << iMount->first << L" => " << iMount->second->toString() << L"\n";

    result << L"}";

    return result;
}

void VFSMountPoint::getMountedPointKeys(List<String> & mountedPointKeys)
{
    internalGetMountedPointKeys(mountedPointKeys, true);
}

void VFSMountPoint::internalGetMountedPointKeys(List<String> & mountedPointKeys, bool includeRoot)
{
    mountedPointKeys.clear();
    for(Map<String, Ptr<VirtualFileSystem> >::const_iterator iMount = _mountedVFS.begin();
        iMount != _mountedVFS.end();
        ++iMount)
        mountedPointKeys.push_back(iMount->first);

    if(_pRootVFS != null && includeRoot)
        mountedPointKeys.push_back(L"/");
}

Ptr<VirtualFileSystem> VFSMountPoint::getMountedPoint(const String & vfsKey)
{
    Ptr<VirtualFileSystem> pVFS;
    if(isPathValid(vfsKey) && isVFSMounted(vfsKey))
        pVFS = _mountedVFS[vfsKey];
    else if(vfsKey == L"/")
        pVFS = _pRootVFS;
    return pVFS;
}

String VFSMountPoint::extractVFSKey(String & path)
{
    String mountPoint;

    if(path.find(L"/", 0) != 0)
        return mountPoint;

    int32 slashFound = path.find(L"/", 1);
    if(slashFound<0)
    {
        mountPoint = path;
        path.clear();
    }
    else
    {
        mountPoint = path.substr(0, slashFound);
        path = path.substr(slashFound);
    }
    return mountPoint;
}

}
