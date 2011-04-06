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
#include <Core/VFS/ArchiveZip.h>
#include <Core/List.h>

namespace Core
{
struct ZipCentralDirEnd
{
    static const int32 MAGIC = 0x06054b50;
    static const int32 SIZE  = 22;

    void read(InputStream & input)
    {
        uint32 magic = input.readUInt32();
        if(magic != MAGIC)
            throw Exception(L"MAGIC");

        input.read(diskNumber);
        input.read(diskStart);
        input.read(diskEntries);
        input.read(totalEntries);
        input.read(centralDirectorySize);
        input.read(centralDirectoryOffset);
        
        uint16 commentLength = input.readUInt16();
        comment.resize(commentLength);
        input.readData((void*)comment.data(), commentLength);
    }

    uint16  diskNumber;
    uint16  diskStart;
    uint16  diskEntries;
    uint16  totalEntries;
    uint32  centralDirectorySize;
    uint32  centralDirectoryOffset;
    String8 comment;
};

struct ZipLocalFileHeader
{
    static const int32 MAGIC = 0x04034b50;

    void read(InputStream & input)
    {
        uint32 magic = input.readUInt32();
        if(magic != MAGIC)
            throw Exception(L"MAGIC");

        input.read(versionNeeded);
        input.read(generalPurposeFlag);
        input.read(compressionMethod);
        input.read(lastModifTime);
        input.read(lastModifDate);
        input.read(crc32);
        input.read(compressedSize);
        input.read(uncompressedSize);
        input.read(fileNameLength);
        input.read(extraFieldLength);
        
        fileName.resize(fileNameLength);
        extraField.resize(extraFieldLength);

        input.readData((void*)fileName.data(), fileNameLength);
        input.readData((void*)extraField.data(), extraFieldLength);
    }

    uint16  versionNeeded;
    uint16  generalPurposeFlag;
    uint16  compressionMethod;
    uint16  lastModifTime;
    uint16  lastModifDate;
    uint32  crc32;
    uint32  compressedSize;
    uint32  uncompressedSize;
    uint16  fileNameLength;
    uint16  extraFieldLength;
    String8 fileName;
    String8 extraField;
    // filename
    // extra field
};

struct ZipCentralDirHeader
{
    static const int32 MAGIC = 0x02014b50;

    void read(InputStream & input)
    {
        uint32 magic = input.readUInt32();
        if(magic != MAGIC)
            throw Exception(L"MAGIC");

        input.read(versionMadeBy);
        input.read(versionToExtract);
        input.read(generalPurposeFlags);
        input.read(compressionMethod);
        input.read(lastModifTime);
        input.read(lastModifDate);
        input.read(crc32);
        input.read(compressedSize);
        input.read(uncompressedSize);
        input.read(fileNameLength);
        input.read(extraFieldLength);
        input.read(commentLength);
        input.read(diskStart);
        input.read(internalFileAttrib);
        input.read(externalFileAttrib);
        input.read(localFileHeaderOffset);
        
        fileName.resize(fileNameLength);
        extraField.resize(extraFieldLength);
        comment.resize(commentLength);

        input.readData((void*)fileName.data(), fileNameLength);
        input.readData((void*)extraField.data(), extraFieldLength);
        input.readData((void*)comment.data(), commentLength);
    }

    uint16  versionMadeBy;
    uint16  versionToExtract;
    uint16  generalPurposeFlags;
    uint16  compressionMethod;
    uint16  lastModifTime;
    uint16  lastModifDate;
    uint32  crc32;
    uint32  compressedSize;
    uint32  uncompressedSize;
    uint16  fileNameLength;
    uint16  extraFieldLength;
    uint16  commentLength;
    uint16  diskStart;
    uint16  internalFileAttrib;
    uint32  externalFileAttrib;
    uint32  localFileHeaderOffset;
    String8 fileName;
    String8 extraField;
    String8 comment;
    // filename
    // extra field
    // file comment
};

void extractFilesFromZip(SeekableInputStream & arch, Core::Map<String, ArchiveEntry> & entries)
{
    int64 archLength = arch.getLength();
    uint32 magic;
    int64 centralDirEndOffset = archLength - ZipCentralDirEnd::SIZE;
    
    arch.setPosition(centralDirEndOffset, SEEK_ORIGIN_SET);
    arch.read(magic);

    while(magic != ZipCentralDirEnd::MAGIC && centralDirEndOffset > 0)
    {
        centralDirEndOffset -= 1;
        arch.setPosition(centralDirEndOffset, SEEK_ORIGIN_SET);
        arch.read(magic);
    }

    ZipCentralDirEnd centralDirEnd;
    arch.setPosition(centralDirEndOffset, SEEK_ORIGIN_SET);
    centralDirEnd.read(arch);

    arch.setPosition(centralDirEnd.centralDirectoryOffset, SEEK_ORIGIN_SET);

    Core::List<ZipCentralDirHeader> centralDirHeaders;
    centralDirHeaders.resize(centralDirEnd.totalEntries);

    for(int32 iFile=0; iFile < centralDirHeaders.size(); iFile++)
        centralDirHeaders[iFile].read(arch);

    for(int32 iFile=0; iFile < centralDirHeaders.size(); iFile++)
    {
        ArchiveEntry entry;
        ZipLocalFileHeader localHeader;

        arch.setPosition(centralDirHeaders[iFile].localFileHeaderOffset, SEEK_ORIGIN_SET);
        localHeader.read(arch);

        int32 year  = 1980 + (localHeader.lastModifDate >> 9);
        int32 month = (localHeader.lastModifDate >> 5) & 0x000F;
        int32 day   = localHeader.lastModifDate & 0x001F;

        int32 hour  = localHeader.lastModifTime >> 11;
        int32 min   = (localHeader.lastModifTime >> 5) & 0x003F;
        int32 sec   = 2 * (localHeader.lastModifTime & 0x001F);

        entry.offset = arch.getPosition();
        entry.size = localHeader.compressedSize;
        entry.lastModification = TimeValue(Date(year, EMonthName(month - 1), day), TimeDuration(hour, min, sec, 0));

        entries[String(localHeader.fileName)] = entry;
    }
}

}