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

#include "VFSDirectMapping.h"

#include <Core/System/Files.h>
#include <Core/IO/FileStream.h>

namespace Core
{

VFSDirectMapping::VFSDirectMapping()
{}
VFSDirectMapping::~VFSDirectMapping()
{}

bool VFSDirectMapping::linkDirectory(const String & name)
{
    bool dirExists = false;

    const String temp = L"$TEMP";
    const String user = L"$USER";
    Core::String dirName = name;

    if(dirName.startsWith(temp))
    {
        String tempDir = System::getTempDir();
        dirName.replace(0, temp.length(), tempDir);

        if(!System::dirExists(dirName))
            System::createDirectory(dirName);
    }
    else if(dirName.startsWith(user))
    {
        String userDir = System::getUserLocalAppDir();
        dirName.replace(0, user.length(), userDir);

        if(!System::dirExists(dirName))
            System::createDirectory(dirName);
    }

    if((dirExists = System::dirExists(dirName)))
        _directoryMapping = dirName;

    return dirExists;
}

void VFSDirectMapping::linkDirectoryThrow(const String & name)
{
    if(!linkDirectory(name))
        throw Core::Exception(L"VFSDirectMapping::linkDirectory error on '" + name + L"'");
}

void VFSDirectMapping::listDirs(const String & path, List<String> & content)
{
    String fullPath = createFullPath(path);
    if(System::dirExists(fullPath))
    {
        List<String> files;
        System::getDirContent(fullPath, files, content);
    }
}

void VFSDirectMapping::listFiles(const String & path, List<String> & content)
{
    String fullPath = createFullPath(path);
    if(System::dirExists(fullPath))
    {
        List<String> subDirs;
        System::getDirContent(fullPath, content, subDirs);
    }
}

bool VFSDirectMapping::fileExists(const String & fileName)
{
    String fullPath = createFullPath(fileName);
    return System::fileExists(fullPath);
}

bool VFSDirectMapping::dirExists(const String & dirName)
{
    String fullPath = createFullPath(dirName);
    return System::dirExists(fullPath);
}

void VFSDirectMapping::createDir(const String & dirName)
{
    String fullPath = createFullPath(dirName);
    if(!Core::System::dirExists(fullPath))
    {
        Core::System::createDirectory(fullPath);
    }
}

bool VFSDirectMapping::moveFile(const String & from, const String & to)
{
    String fullPathFrom = createFullPath(from);
    String fullPathTo   = createFullPath(to);

    return Core::System::moveFile(fullPathFrom, fullPathTo);
}

bool VFSDirectMapping::moveDir(const String & from, const String & to)
{
    String fullPathFrom = createFullPath(from);
    String fullPathTo   = createFullPath(to);

    return Core::System::moveDir(fullPathFrom, fullPathTo);
}

TimeValue VFSDirectMapping::getLastModificationDate(const String & fileName)
{
    String fullPath = createFullPath(fileName);
    return System::getLastModificationDate(fullPath);
}

Ptr<InputStream> VFSDirectMapping::readFile(const String & fileName)
{
    if(!fileExists(fileName))
        return Ptr<InputStream>(NULL);

    String fullPath(createFullPath(fileName));

    Ptr<InputStream> inputStream(new FileInputStream(fullPath));

    return inputStream;
}

Ptr<OutputStream> VFSDirectMapping::writeFile(  const String & fileName,
                                                FileOutputStream::EFileAccess fileAccess)
{
    String fullPath(createFullPath(fileName));
    if(!Core::System::dirExists(Core::System::getPath(fullPath)))
    {
        Core::System::createDirectory(Core::System::getPath(fullPath));
    }

    Ptr<OutputStream> outputStream(new FileOutputStream(fullPath, fileAccess));

    return outputStream;
}

String VFSDirectMapping::createFullPath(const String & path)
{
    String fullPath = _directoryMapping;
    fullPath<<path;

    return fullPath;
}

String VFSDirectMapping::toString() const
{
    return L"direct mapping to " + _directoryMapping;
}

}//namespace Core
