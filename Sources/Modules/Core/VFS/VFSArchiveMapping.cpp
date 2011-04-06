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
#include <Core/VFS/VFSArchiveMapping.h>
#include <Core/VFS/ArchiveZip.h>
#include <Core/Strings/Tools.h>
#include <Core/IO/MemoryStream.h>
#include <Core/Exception.h>

namespace Core
{

VFSArchiveMapping::VFSArchiveMapping()
{}

VFSArchiveMapping::~VFSArchiveMapping()
{}

bool VFSArchiveMapping::linkArchive(const String & fileName)
{
    _fileName = fileName;
    _entries.clear();

    _pArchive = Ptr<FileInputStream>(new FileInputStream());
    _pArchive->open(fileName);

    if(!_pArchive->isOpened())
        return false;

    extractFilesFromZip(*_pArchive, _entries);
    return true;
}

void VFSArchiveMapping::listDirs(const String & path, List<String> & content)
{
    content.clear();
    String normPath;
    Core::trimAll(path, normPath, L"/");
    normPath += L'/';

    for(Core::Map<String, ArchiveEntry>::const_iterator iEntry=_entries.begin(); iEntry != _entries.end(); ++iEntry)
    {
        const String & name = iEntry->first;

        if(name.startsWith(normPath))
        {
            int32 nextSlash = name.find_first_of(L"/", normPath.size());

            if(nextSlash == name.size() - 1)
                content.push_back(name.slice(normPath.size()));
        }
    }
}

void VFSArchiveMapping::listFiles(const String & path, List<String> & content)
{
    content.clear();
    String normPath;
    Core::trimAll(path, normPath, L"/");
    normPath += L'/';

    for(Core::Map<String, ArchiveEntry>::const_iterator iEntry=_entries.begin(); iEntry != _entries.end(); ++iEntry)
    {
        const String & name = iEntry->first;

        if(name.startsWith(normPath))
        {
            if(name.size() > normPath.size())
            {
                int32 nextSlash = name.find_first_of(L"/", normPath.size());

                if(nextSlash == String::npos)
                    content.push_back(name.slice(normPath.size()));
            }
        }
    }
}

bool VFSArchiveMapping::fileExists(const String & fileName)
{
    String normName;
    Core::trimAll(fileName, normName, L"/");
    return _entries.count(normName) > 0;
}

bool VFSArchiveMapping::dirExists(const String & dirName)
{
    String normName;
    Core::trimAll(dirName, normName, L"/");
    normName += L'/';
    return _entries.count(normName) > 0;
}

void VFSArchiveMapping::createDir(const String & dirName)
{
    throw Exception(L"operation not supported on archive VFS");
}

bool VFSArchiveMapping::moveFile(const String & from, const String & to)
{
    throw Exception(L"operation not supported on archive VFS");
    return false;
}

bool VFSArchiveMapping::moveDir(const String & from, const String & to)
{
    throw Exception(L"operation not supported on archive VFS");
    return false;
}

TimeValue VFSArchiveMapping::getLastModificationDate(const String & fileName)
{
    String normName;
    Core::trimAll(fileName, normName, L"/");
    return _entries[normName].lastModification;
}

Ptr<InputStream> VFSArchiveMapping::readFile(const String & name)
{
    Ptr<InputStream> pInput;
    String normName;
    Core::trimAll(name, normName, L"/");

    Core::Map<String, ArchiveEntry>::const_iterator iEntry = _entries.find(normName);

    if(iEntry != _entries.end())
    {
        byte * pData = new byte[size_t(iEntry->second.size)];
        _pArchive->setPosition(iEntry->second.offset, SEEK_ORIGIN_SET);
        _pArchive->readData(pData, uint32(iEntry->second.size));
        pInput = Ptr<InputStream>(new MemoryInputStream(pData, int32(iEntry->second.size), true));
    }
    
    return pInput;
}

Ptr<OutputStream> VFSArchiveMapping::writeFile(const String & fileName, FileOutputStream::EFileAccess fileAccess)
{
    throw Exception(L"operation not supported on archive VFS");
    return null;
}

String VFSArchiveMapping::toString() const
{
    return L"archive mapping to " + _fileName;
}

}//namespace Core
