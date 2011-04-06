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
#include <Core/System/Files.h>
#include <Core/Containers.h>
#include <Core/DateAndTime.h>
#include <Core/IO/FileStream.h>

using Core::SystemException;
using namespace Core::System;

static void testSystemExtract()
{
    String fullName1(L"C:\\this\\is\\the\\\\path\\filename.ext");
    String fullName2(L"C:\\this\\is\\the\\\\path\\filename");
    String fullName3(L"C:\\filename");
    String fullName4(L"filename.0001.extension");

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Extract path/filename/ext");
    UTEST_EQU(getPath(fullName1),            L"C:/this/is/the/path/");
    UTEST_EQU(getFileName(fullName1),        L"filename.ext");
    UTEST_EQU(getFileBaseName(fullName1),    L"filename");
    UTEST_EQU(getFileExt(fullName1),         L"ext");
    UTEST_EQU(getParent(getPath(fullName1)), L"C:/this/is/the/");

    UTEST_EQU(getPath(fullName2),            L"C:/this/is/the/path/");
    UTEST_EQU(getFileName(fullName2),        L"filename");
    UTEST_EQU(getFileBaseName(fullName2),    L"filename");
    UTEST_EQU(getFileExt(fullName2),         L"");
    UTEST_EQU(getParent(getPath(fullName2)), L"C:/this/is/the/");

    UTEST_EQU(getPath(fullName3),            L"C:/");
    UTEST_EQU(getFileName(fullName3),        L"filename");
    UTEST_EQU(getFileBaseName(fullName3),    L"filename");
    UTEST_EQU(getFileExt(fullName3),         L"");
    UTEST_EQU(getParent(getPath(fullName3)), L"C:/");

    UTEST_EQU(getPath(fullName4),            L"");
    UTEST_EQU(getFileName(fullName4),        L"filename.0001.extension");
    UTEST_EQU(getFileBaseName(fullName4),    L"filename.0001");
    UTEST_EQU(getFileExt(fullName4),         L"extension");
    UTEST_EQU(getParent(getPath(fullName4)), L"");

    UTEST_END_TEST;
}

static void testSystemEnvVar()
{
   //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Environment variables");

    Core::List<String>  vars;
    Core::List<String>  values;
    getEnv(vars, values);
    UTEST_EQU(vars.size(), values.size());
#if defined _MSC_VER
    String result(getEnvVar(L"PROCESSOR_ARCHITECTURE"));
    String expected(L"x86");
#else
    String result(getEnvVar(L"SHELL"));
    String expected(L"/bin/bash");
#endif
    UTEST_EQU(int32(result.size()), int32(expected.size()));
    UTEST_EQU(result, expected);
    UTEST_CATCH_EXCEPTION(SystemException, getEnvVar(L"DOES_NOT_EXIST"));
    UTEST_END_TEST;
}

static void testSystemWorkDir()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Working directory");
    String workDir = LM_WORKING_DIRECTORY;
    String wantedDir = workDir + L"/Data/TUData";
    UTEST_CATCH_EXCEPTION(SystemException, setCurrentDir(L"dtc"));
    setCurrentDir(wantedDir);
    UTEST_EQU(getNormalizedPath(getCurrentDir()), getNormalizedPath(wantedDir));
    UTEST_END_TEST;
}

static void testSystemFileExist()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Testing file existence");
    UTEST_EQU(dirExists(L"System/dir1"), true);
    UTEST_EQU(dirExists(L"System/dir2"), true);
    UTEST_EQU(dirExists(L"System/not_here"), false);
    UTEST_EQU(dirExists(L"System/file1.h"), false);

    UTEST_EQU(fileExists(L"System/file1.h"), true);
    UTEST_EQU(fileExists(L"System/not_here"), false);
    UTEST_EQU(fileExists(L"System/dir1"), false);
    UTEST_END_TEST;
}

static void testSystemDirCreate()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Directory creation");

    createDirectory(L"Tmp/test1");
    UTEST_EQU(dirExists(L"Tmp/test1"), true);
    createDirectory(L"Tmp/test2/subtest2/subsubtest2");
    UTEST_EQU(dirExists(L"Tmp/test2/"), true);
    UTEST_EQU(dirExists(L"Tmp/test2/subtest2/"), true);
    UTEST_EQU(dirExists(L"Tmp/test2/subtest2/subsubtest2"), true);
    createDirectory(L"Tmp/test2/subtest2/subsubtest3");
    UTEST_EQU(dirExists(L"Tmp/test2/subtest2/subsubtest3"), true);
    createDirectory(L"Tmp/test2/subtest2/subsubtest4");
    UTEST_EQU(dirExists(L"Tmp/test2/subtest2/subsubtest4"), true);
    createDirectory(L"Tmp/test2/subtest3");
    UTEST_EQU(dirExists(L"Tmp/test2/subtest3"), true);
    createDirectory(L"Tmp/test2/subtest4");
    UTEST_EQU(dirExists(L"Tmp/test2/subtest4"), true);

    Core::List<String>  expectedFiles;
    Core::List<String>  expectedDirs;
    expectedDirs.push_back(L"subsubtest2");
    expectedDirs.push_back(L"subsubtest3");
    expectedDirs.push_back(L"subsubtest4");

    Core::List<String> files;
    Core::List<String> dirs;

    getDirContent(L"./Tmp/test2/subtest2", files, dirs);
    UTEST_EQU(files.size(), 0);
    UTEST_EQU(dirs.size(), 3);
    UTEST_EQU(sameContent(files, expectedFiles), true);
    UTEST_EQU(sameContent(dirs, expectedDirs), true);

    UTEST_END_TEST;
}

static void testSystemFileCopyDelete()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"File copy");
    copyFile(L"System/file1.h", L"Tmp/file.h");
    UTEST_EQU(fileExists(L"Tmp/file.h"), true);
    UTEST_CATCH_EXCEPTION(SystemException, copyFile(L"System/no_here", L"Tmp/file.h"));
    UTEST_CATCH_EXCEPTION(SystemException, copyFile(L"System/no_here", L"not_here/file.h"));
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"File delete");
    deleteFile(L"Tmp/file.h");
    UTEST_EQU(fileExists(L"Tmp/file.h"), false);
    UTEST_CATCH_EXCEPTION(SystemException, deleteFile(L"Tmp/file.h"));
    UTEST_END_TEST;
}

static void testSystemDirDelete()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Directory delete");

    deleteDirectory(L"Tmp/test2/subtest2");

    UTEST_EQU(dirExists(L"Tmp/test2/"), true);
    UTEST_EQU(dirExists(L"Tmp/test2/subtest2/"), false);
    UTEST_CATCH_EXCEPTION(SystemException, deleteDirectory(L"Tmp/test2/subtest2"));

    deleteDirContent(L"Tmp/test2");

    UTEST_EQU(dirExists(L"Tmp/test2/"), true);
    UTEST_EQU(dirExists(L"Tmp/test2/subtest2"), false);
    UTEST_EQU(dirExists(L"Tmp/test2/subtest3"), false);
    UTEST_EQU(dirExists(L"Tmp/test2/subtest4"), false);

    deleteDirectory(L"Tmp/test2");
    UTEST_EQU(dirExists(L"Tmp/test2/"), false);

    deleteDirectory(L"Tmp/test1");
    UTEST_EQU(dirExists(L"Tmp/test1/"), false);

    UTEST_END_TEST;
}

static void testSystemFileListing()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"File listing");

    Core::List<String> files;
    Core::System::getRecursiveDirContent(L"System", files);

    UTEST_EQU(files.size(), 2);

    for(int32 ii=0; ii <files.size(); ii++)
    {
        Core::FileInputStream input(L"System/" + files[ii]);
        UTEST_EQU(input.isOpened(), true);
    }

    UTEST_END_TEST;
}

static void testSystemFileDate()
{
    UTEST_START_TEST(L"Files Date");
    String workDir = LM_WORKING_DIRECTORY;
    String wantedDir = workDir + L"/Data/TUData";
    setCurrentDir(wantedDir);
    UTEST_EQU(fileExists(L"System/file1.xyz"), false);
    UTEST_CATCH_EXCEPTION(SystemException, Core::TimeValue lastModificationDate = getLastModificationDate(L"System/file1.xyz"));

    UTEST_EQU(fileExists(L"System/file1.h"), true);
    Core::TimeValue lastModificationDate = getLastModificationDate(L"System/file1.h");

    UTEST_END_TEST;
}

void testSystem()
{
    UTEST_PACKAGE("System");

    String previousDir(getCurrentDir());

    testSystemExtract();
    testSystemEnvVar();
    testSystemWorkDir();
    testSystemFileExist();
    testSystemDirCreate();
    testSystemFileCopyDelete();
    testSystemDirDelete();
    testSystemFileListing();
    testSystemFileDate();
 
    setCurrentDir(previousDir);

}
