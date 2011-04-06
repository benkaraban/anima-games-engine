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
#include <UTests.h>

#include <Core/Standard.h>
#include <Core/VFS/VFSArchiveMapping.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/VFS/VFSMountPoint.h>
#include <Core/VFS/VFSXMLTranslator.h>
#include <Core/IO/BufferStream.h>
#include <Core/IO/Tools.h>
#include <Core/XML/XMLDocument.h>

void testZip()
{
    UTEST_START_TEST(L"Zip archive data");

    Core::VFSArchiveMapping vfsZip;
    vfsZip.linkArchive(L"./Data/TUData/VFS/vfs-zips.zip");

    Ptr<Core::InputStream> pInput1 = vfsZip.readFile(L"file1.txt");
    Ptr<Core::InputStream> pInput2 = vfsZip.readFile(L"file2.txt");
    Ptr<Core::InputStream> pInput3 = vfsZip.readFile(L"file3.txt");
    Ptr<Core::InputStream> pInput4 = vfsZip.readFile(L"file4.txt");

    UTEST_NOT_EQU(pInput1, null);
    UTEST_NOT_EQU(pInput2, null);
    UTEST_NOT_EQU(pInput3, null);
    UTEST_EQU(pInput4, null);

    Core::List<byte> data1;
    Core::List<byte> data2;
    Core::List<byte> data3;

    Core::readAllStream(data1, *pInput1);
    Core::readAllStream(data2, *pInput2);
    Core::readAllStream(data3, *pInput3);

    UTEST_EQU(data1.size(), 10);
    UTEST_EQU(data2.size(), 20);
    UTEST_EQU(data3.size(), 5);

    for(int32 ii=0; ii < data1.size(); ii++)
        UTEST_EQU(data1[ii], 'a');

    for(int32 ii=0; ii < data2.size(); ii++)
        UTEST_EQU(data2[ii], 'b');

    for(int32 ii=0; ii < data3.size(); ii++)
        UTEST_EQU(data3[ii], 'c');

    UTEST_END_TEST;


    UTEST_START_TEST(L"Zip archive directories");

    Core::VFSArchiveMapping vfsZip;
    vfsZip.linkArchive(L"./Data/TUData/VFS/vfs-zips2.zip");

    Core::List<String> content;
    vfsZip.listDirs(L"/dir/", content);
    UTEST_EQU(content.size(), 2);

    vfsZip.listFiles(L"/dir/", content);
    UTEST_EQU(content.size(), 1);
    
    vfsZip.listFiles(L"/dir/subdir", content);
    UTEST_EQU(content.size(), 1);

    vfsZip.listFiles(L"/dir/emptysubdir", content);
    UTEST_EQU(content.size(), 0);

    UTEST_END_TEST;
}

void testVFS()
{
    bool ok = false;
    String value;
    int32 intValue;

    Core::List<String> content;
    UTEST_PACKAGE("Virtual File System");

    UTEST_START_TEST(L"Direct Mapping");
    Core::VFSDirectMapping vfsDirectMapping;

    UTEST_EQU(vfsDirectMapping.linkDirectory(L"/Should/Not/Work"), false);
    UTEST_EQU(vfsDirectMapping.linkDirectory(L"./Data/TUData"), true);
    content.clear();
    vfsDirectMapping.listDirs(L"/System", content);
    //Le répertoire "./Data/TUData/System" contient 3 répertoires
    UTEST_EQU(content.size(), 2);
    content.clear();
    vfsDirectMapping.listFiles(L"/System", content);
    //Le répertoire "./Data/TUData/System" contient un fichier
    UTEST_EQU(content.size(), 1);
    UTEST_EQU(vfsDirectMapping.fileExists(L"/System/file1.h"), true);
    UTEST_EQU(vfsDirectMapping.fileExists(L"/Should/Not/Work.h"), false);
    UTEST_EQU(vfsDirectMapping.fileExists(L"/System/ShouldNotWork.h"), false);

    UTEST_CATCH_EXCEPTION(Core::SystemException, Core::TimeValue lastModificationDate = vfsDirectMapping.getLastModificationDate(L"/System/file1.xyz"));
    bool exceptionCatched = false;
    try
    {
        Core::TimeValue lastModificationDate = vfsDirectMapping.getLastModificationDate(L"/Should/Not/Work.h");
    }catch(Core::Exception &)
    {
        exceptionCatched = true;
    }
    UTEST_EQU(exceptionCatched, true);

    Core::TimeValue lastModificationDate = vfsDirectMapping.getLastModificationDate(L"/System/file1.h");

    Core::String8 xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\
        <ConfigGfx Detail=\"High\" Stuff=\"17\">\
            <Resolution depth=\"32\">\
                <width>1680</width>\
                <height>1050</height>\
            </Resolution>\
        </ConfigGfx>";

    Core::BufferStream bufferStream;
    bufferStream.writeData(xmlString.c_str(), xmlString.length());
    Core::XMLDocument xmlDocument(L".");
    xmlDocument.loadDocument(bufferStream);
    UTEST_EQU(xmlDocument.saveDocument(L"Data/TUData/Tmp/testVFS.xml"), true);

    Ptr<Core::InputStream> input = vfsDirectMapping.readFile(L"/Tmp/testVFS.xml");
    UTEST_EQU( input.isNull(), false);

    Core::XMLDocument xmlDocument2(L".");
    UTEST_EQU(xmlDocument2.loadDocument(*input), true);

    //On vérifie que les valeurs chargées sont exactes
    ok = xmlDocument2.getAttribute(L"ConfigGfx",L"Detail", value);
    UTEST_EQU((ok && value == L"High"), true);

    ok = xmlDocument2.getAttribute(L"ConfigGfx",L"Stuff", intValue);
    UTEST_EQU((ok && intValue == 17), true);

    ok = xmlDocument2.getValue(L"ConfigGfx.Resolution.width", intValue);
    UTEST_EQU((ok && intValue == 1680), true);

    ok = xmlDocument2.getValue(L"ConfigGfx.Resolution.height", intValue);
    UTEST_EQU((ok && intValue == 1050), true);

    ok = xmlDocument2.getAttribute(L"ConfigGfx.Resolution",L"depth", intValue);
    UTEST_EQU((ok && intValue == 32), true);

    Ptr<Core::OutputStream> output = vfsDirectMapping.writeFile(L"/Tmp/testVFS.txt");
    Core::String8 dummyString = "This is only a test! I repeat, this is only a test!";
    output->writeData(dummyString.c_str(), dummyString.length());
    output->flush();

    Ptr<Core::InputStream> input2 = vfsDirectMapping.readFile(L"/Tmp/testVFS.txt");

    UTEST_EQU( input2.isNull(), false);
    char data[512];
    int32 nbRead = input2->readUpTo(data, 512);
    if(nbRead>=0 && nbRead<512)
        data[nbRead] = '\0';

    Core::String8 dummyString2 = data;

    UTEST_EQU(dummyString, dummyString2);

    UTEST_END_TEST;

    UTEST_START_TEST(L"Mount Point");

    Core::VFSMountPoint vfsMountPoint;
    Ptr<Core::VFSDirectMapping> vfsDirectMapping2(new Core::VFSDirectMapping());
    UTEST_EQU(vfsDirectMapping2->linkDirectory(L"./Data/TUData/Tmp"), true);
    Ptr<Core::VFSDirectMapping> vfsDirectMapping3(new Core::VFSDirectMapping());
    UTEST_EQU(vfsDirectMapping3->linkDirectory(L"./Data/TUData/System"), true);

    UTEST_EQU(vfsMountPoint.mount(L"TempDir",   vfsDirectMapping2), false);
    UTEST_EQU(vfsMountPoint.mount(L"//TempDir", vfsDirectMapping2), false);
    UTEST_EQU(vfsMountPoint.mount(L"/Temp/Dir", vfsDirectMapping2), false);
    UTEST_EQU(vfsMountPoint.mount(L"/TempDir/", vfsDirectMapping2), false);
    UTEST_EQU(vfsMountPoint.mount(L"T/empDir",  vfsDirectMapping2), false);
    UTEST_EQU(vfsMountPoint.mount(L"/TempDir",  vfsDirectMapping2), true);
    UTEST_EQU(vfsMountPoint.mount(L"/TempDir",  vfsDirectMapping2), false);

    Ptr<Core::VFSMountPoint> vfsMountPoint2(new Core::VFSMountPoint());
    UTEST_EQU(vfsMountPoint2->mount(L"/System", vfsDirectMapping3), true);

    UTEST_EQU(vfsMountPoint.mount(L"/MP2", vfsMountPoint2), true);

    UTEST_EQU(vfsMountPoint.fileExists(L"/MP2/System/file1.h"), true);
    UTEST_EQU(vfsMountPoint.fileExists(L"/MP2/System/file2.h"), false);
    UTEST_EQU(vfsMountPoint.fileExists(L"/Should/Not/Work.h"), false);
    content.clear();
    vfsMountPoint.listDirs(L"/MP2/System/", content);
    UTEST_EQU(content.size(), 2);

    UTEST_CATCH_EXCEPTION(Core::SystemException, Core::TimeValue lastModificationDate = vfsMountPoint.getLastModificationDate(L"/MP2/System/file1.xyz"));
    bool exceptionCatched = false;
    try
    {
        Core::TimeValue lastModificationDate = vfsMountPoint.getLastModificationDate(L"/Should/Not/Work.h");
    }catch(Core::Exception &)
    {
        exceptionCatched = true;
    }
    UTEST_EQU(exceptionCatched, true);

    Core::TimeValue lastModificationDate = vfsMountPoint.getLastModificationDate(L"/MP2/System/file1.h");

    Ptr<Core::InputStream> input = vfsMountPoint.readFile(L"/TempDir/testVFS.xml");
    UTEST_EQU( input.isNull(), false);

    Core::XMLDocument xmlDocument3(L".");
    UTEST_EQU(xmlDocument3.loadDocument(*input), true);

    //On vérifie que les valeurs chargées sont exactes
    ok = xmlDocument3.getAttribute(L"ConfigGfx",L"Detail", value);
    UTEST_EQU((ok && value == L"High"), true);
    ok = xmlDocument3.getAttribute(L"ConfigGfx",L"Stuff", intValue);
    UTEST_EQU((ok && intValue == 17), true);
    ok = xmlDocument3.getValue(L"ConfigGfx.Resolution.width", intValue);
    UTEST_EQU((ok && intValue == 1680), true);
    ok = xmlDocument3.getValue(L"ConfigGfx.Resolution.height", intValue);
    UTEST_EQU((ok && intValue == 1050), true);
    ok = xmlDocument3.getAttribute(L"ConfigGfx.Resolution",L"depth", intValue);
    UTEST_EQU((ok && intValue == 32), true);

    Ptr<Core::OutputStream> output = vfsMountPoint.writeFile(L"/TempDir/testVFS2.txt");
    Core::String8 dummyString = "This is only a test! I repeat, once again, this is only a test!";
    output->writeData(dummyString.c_str(), dummyString.length());
    output->flush();

    Ptr<Core::InputStream> input2 = vfsMountPoint.readFile(L"/TempDir/testVFS2.txt");

    UTEST_EQU( input2.isNull(), false);
    char data[512];
    int32 nbRead = input2->readUpTo(data, 512);
    if(nbRead>=0 && nbRead<512)
        data[nbRead] = '\0';

    Core::String8 dummyString2 = data;

    UTEST_EQU(dummyString, dummyString2);

    UTEST_END_TEST;



    UTEST_START_TEST(L"Mount Point with root read");

    Core::VFSMountPoint vfsMountPoint;
    Ptr<Core::VFSDirectMapping> vfsDirectMapping2(new Core::VFSDirectMapping());
    Ptr<Core::VFSDirectMapping> vfsDirectMapping3(new Core::VFSDirectMapping());

    UTEST_EQU(vfsDirectMapping2->linkDirectory(L"./Data/TUData/"), true);
    UTEST_EQU(vfsDirectMapping3->linkDirectory(L"./Data/TUData/System"), true);

    UTEST_EQU(vfsMountPoint.mount(L"/MountPoint",  vfsDirectMapping2), true);
    UTEST_EQU(vfsMountPoint.mount(L"/",  vfsDirectMapping3), true);

    UTEST_EQU(vfsMountPoint.fileExists(L"/file1.h"), true);
    UTEST_EQU(vfsMountPoint.fileExists(L"/file2.h"), false);
    UTEST_EQU(vfsMountPoint.fileExists(L"/file3.h"), false);
    UTEST_EQU(vfsMountPoint.fileExists(L"/dir1/file1.h"), false);
    UTEST_EQU(vfsMountPoint.fileExists(L"/dir1/file2.h"), true);
    UTEST_EQU(vfsMountPoint.fileExists(L"/dir2/file1.h"), false);

    UTEST_EQU(vfsMountPoint.dirExists(L"/dir1/"), true);
    UTEST_EQU(vfsMountPoint.dirExists(L"/dir2/"), true);
    UTEST_EQU(vfsMountPoint.dirExists(L"/dir3/"), false);

    UTEST_EQU(vfsMountPoint.fileExists(L"/MountPoint/System/file1.h"), true);
    UTEST_EQU(vfsMountPoint.fileExists(L"/MountPoint/System/file2.h"), false);
    UTEST_EQU(vfsMountPoint.fileExists(L"/MountPoint/System/file3.h"), false);

    UTEST_EQU(vfsMountPoint.dirExists(L"/MountPoint/"), true);
    UTEST_EQU(vfsMountPoint.dirExists(L"/MountPoint/System/"), true);
    UTEST_EQU(vfsMountPoint.dirExists(L"/MountPoint/System/dir1"), true);
    UTEST_EQU(vfsMountPoint.dirExists(L"/MountPoint/System/dir3"), false);

    content.clear();
    vfsMountPoint.listDirs(L"/MountPoint/System", content);
    UTEST_EQU(content.size(), 2);

    content.clear();
    vfsMountPoint.listFiles(L"/MountPoint/System", content);
    UTEST_EQU(content.size(), 1);

    content.clear();
    vfsMountPoint.listDirs(L"/", content);
    UTEST_EQU(content.size(), 3);

    content.clear();
    vfsMountPoint.listFiles(L"/", content);
    UTEST_EQU(content.size(), 1);

    UTEST_CATCH_EXCEPTION(Core::SystemException, Core::TimeValue lastModificationDate = vfsMountPoint.getLastModificationDate(L"/MP2/System/file1.xyz"));

    Core::TimeValue date1 = vfsMountPoint.getLastModificationDate(L"/file1.h");
    Core::TimeValue date2 = vfsMountPoint.getLastModificationDate(L"/MountPoint/System/file1.h");

    UTEST_EQU((date1 == date2), true);

    Ptr<Core::InputStream> pInput1 = vfsMountPoint.readFile(L"/file1.h");
    Ptr<Core::InputStream> pInput2 = vfsMountPoint.readFile(L"/MountPoint/System/file1.h");

    Core::List<byte> data1, data2;

    Core::readAllStream(data1, *pInput1);
    Core::readAllStream(data2, *pInput2);

    UTEST_EQU((data1 == data2), true);


    UTEST_END_TEST;


    UTEST_START_TEST(L"Mount Point with root write");

    Core::VFSMountPoint vfsMountPoint;
    Ptr<Core::VFSDirectMapping> vfsDirectMapping2(new Core::VFSDirectMapping());
    Ptr<Core::VFSDirectMapping> vfsDirectMapping3(new Core::VFSDirectMapping());

    UTEST_EQU(vfsDirectMapping2->linkDirectory(L"./Data/TUData/Tmp"), true);
    UTEST_EQU(vfsDirectMapping3->linkDirectory(L"./Data/TUData/Tmp"), true);

    UTEST_EQU(vfsMountPoint.mount(L"/MountPoint",  vfsDirectMapping2), true);
    UTEST_EQU(vfsMountPoint.mount(L"/",  vfsDirectMapping3), true);

    {
        Ptr<Core::OutputStream> pOutput1 = vfsMountPoint.writeFile(L"/tmp-file1.txt");
        Ptr<Core::OutputStream> pOutput2 = vfsMountPoint.writeFile(L"/MountPoint/tmp-file2.txt");
    }

    UTEST_EQU(vfsMountPoint.fileExists(L"/tmp-file2.txt"), true);
    UTEST_EQU(vfsMountPoint.fileExists(L"/MountPoint/tmp-file2.txt"), true);

    UTEST_END_TEST;

    testZip();

    UTEST_START_TEST(L"XML Translation");

    // root ---> VFSMP1 ---> VFSDM2
    //   `---> VFSDM1
    Ptr<Core::VFSMountPoint> root(new Core::VFSMountPoint());
    Ptr<Core::VFSMountPoint> VFSMP1(new Core::VFSMountPoint());
    Ptr<Core::VFSDirectMapping> VFSDM1(new Core::VFSDirectMapping());
    Ptr<Core::VFSDirectMapping> VFSDM2(new Core::VFSDirectMapping());
    Ptr<Core::VFSArchiveMapping> VFSARCH(new Core::VFSArchiveMapping());

    VFSDM1->linkDirectory(L"./Data/TUData/Tmp");
    VFSDM2->linkDirectory(L"./Data/TUData/System");
    VFSARCH->linkArchive(L"./Data/TUData/VFS/vfs-zips2.zip");
    VFSMP1->mount(L"/VFSDM2", VFSDM2);
    root->mount(L"/VFSMP1", VFSMP1);
    root->mount(L"/VFSDM1", VFSDM1);
    root->mount(L"/VFSARCH", VFSARCH);

    Ptr<Core::XMLNode> xmlNode = Core::VFSToXMLNode(root);
    Ptr<Core::XMLDocument> xmlDocument(new Core::XMLDocument());
    xmlDocument->addNode(xmlNode);

    UTEST_EQU(xmlDocument->saveDocument(L"./Data/TUData/Tmp/VFSTranslation.xml"), true);

    Ptr<Core::XMLDocument> xmlDocument2(new Core::XMLDocument());

    UTEST_EQU(xmlDocument2->loadDocument(L"./Data/TUData/Tmp/VFSTranslation.xml"), true);

    Ptr<Core::VirtualFileSystem> pVFS = Core::VFSFromXMLNode(xmlDocument2->firstChildNode(L"vfs"));

    UTEST_EQU(pVFS->fileExists(L"/VFSMP1/VFSDM2/file1.h"), true);
    UTEST_EQU(pVFS->fileExists(L"/VFSARCH/dir/file1.txt"), true);

    UTEST_END_TEST;
}
