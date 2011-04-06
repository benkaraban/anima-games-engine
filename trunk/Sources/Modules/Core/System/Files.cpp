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

#ifdef _WIN32
#include <winsafe.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

#include <Core/Strings/Tools.h>
#include "Files.h"

namespace Core
{
namespace System
{
void deleteEmptyDirectory(const String & dir);
void createFinalDirectory(const String & dir);

//-----------------------------------------------------------------------------
String getExecutableName()
{
#ifdef _WIN32
    const int32 MAX_SIZE = 512;
    wchar_t name[MAX_SIZE];
    GetModuleFileNameW(NULL, name, MAX_SIZE);
    return String(name);
#else
    char exeName[PATH_MAX];
    int32 length = readlink("/proc/self/exe", exeName, PATH_MAX - 1);
    if (length <= 0)
    {
        // I guess we're not running on the right version of unix
        throw SystemException(L"System error in getExecutableName : unable to determine executable name.");
    }
    exeName[length] = '\0';

    return String(String8(exeName));

#endif
}
//-----------------------------------------------------------------------------
String getNormalizedPath(const String & path)
{
    String slashPath(path);
    replaceChar(slashPath, L'\\', L'/');

    String result;
    trim(slashPath, result, L"/");
    return result;
}
//-----------------------------------------------------------------------------
String getFileName(const String & fullName)
{
    String norm(getNormalizedPath(fullName));
    int32 iSlash = norm.find_last_of(L"/");
    if(iSlash == String::npos)
        return fullName;
    else
        return norm.slice(iSlash + 1);
}
//-----------------------------------------------------------------------------
String getPath(const String & fullName)
{
    String norm(getNormalizedPath(fullName));
    int32 iSlash = norm.find_last_of(L"/");
    if(iSlash == String::npos)
        return L"";
    else
        return norm.slice(0, iSlash);
}
//-----------------------------------------------------------------------------
String getFileExt(const String & fullName)
{
    String norm(getNormalizedPath(fullName));
    int32 iPoint = norm.find_last_of(L".");
    if(iPoint == String::npos)
        return L"";
    else
        return norm.slice(iPoint + 1);
}
//-----------------------------------------------------------------------------
String getFileBaseName(const String & fullName)
{
    String fileName = getFileName(fullName);
    int32 iPoint = fileName.find_last_of(L".");
    if(iPoint == String::npos)
        return fileName;
    else
        return fileName.slice(0, iPoint - 1);
}
//-----------------------------------------------------------------------------
void deleteDirectory(const String & dir)
{
    deleteDirContent(dir);
    deleteEmptyDirectory(dir);
}
//-----------------------------------------------------------------------------
void deleteDirContent(const String & dir)
{
    List<String>  files;
    List<String>  subDirs;

    getDirContent(dir, files, subDirs);
    for(List<String>::iterator iFile = files.begin(); iFile != files.end(); ++iFile)
        deleteFile(dir + L"/" + *iFile);
    for(List<String>::iterator iDir = subDirs.begin(); iDir != subDirs.end(); ++iDir)
        deleteDirectory(dir + L"/" + *iDir);
}
//-----------------------------------------------------------------------------
void createDirectory(const String & dir, bool errorIfExists)
{
    String normDir = getNormalizedPath(dir);
    String parentDir = getParent(normDir);
    if(!dirExists(parentDir))
    {
        if(parentDir.length() == normDir.length())
            throw SystemException(L"System error in createDirectory : unable to find root dir.");
        else
            createDirectory(parentDir, errorIfExists);
    }

    if(errorIfExists || !dirExists(normDir))
        createFinalDirectory(normDir);
}
//-----------------------------------------------------------------------------
String getParent(const String & dir)
{
    String normDir(getNormalizedPath(dir));
    int32 iSlash = normDir.find_last_of(L"/");
    if(iSlash == dir.length() - 1)
    {
        if(dir.length() == 3 && dir[1] == L':')
            iSlash = dir.length();
        else
            iSlash = normDir.find_last_of(L"/", iSlash - 1);
    }
    if(iSlash == String::npos)
        iSlash = normDir.length();

    return normDir.slice(0, iSlash);
}
//-----------------------------------------------------------------------------
void internalGetRecursiveDirContent(const String & dir, List<String> & files, const String & append)
{
    List<String> dirFiles;
    List<String> subDirs;
    getDirContent(dir, dirFiles, subDirs);

    for(int32 ii=0; ii < dirFiles.size(); ii++)
        files.push_back(append + dirFiles[ii]);

    for(int32 ii=0; ii < subDirs.size(); ii++)
        internalGetRecursiveDirContent(getNormalizedPath(dir + L"/" + subDirs[ii]), files, getNormalizedPath(subDirs[ii] + L"/"));
}
//-----------------------------------------------------------------------------
void getRecursiveDirContent(const String & dir, List<String> & files)
{
    files.clear();
    internalGetRecursiveDirContent(dir, files, L"");
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
#ifdef _WIN32
//-----------------------------------------------------------------------------
static void handleWinError(const String & name, const String & file)
{
    String message(L"System error in ");
    message << name << L" call (file '" << file << L"'). Windows Error : " << Core::toStringHex(GetLastError(), 8);
    throw SystemException(message);
}
//-----------------------------------------------------------------------------
void deleteEmptyDirectory(const String & dir)
{
    if(!RemoveDirectoryW(dir.c_str()))
        handleWinError(L"deleteDirectory", dir);
}
//-----------------------------------------------------------------------------
bool moveFile(const String & from, const String & to)
{
    bool returnCode = false;

    if(fileExists(from) && !fileExists(to))
        returnCode = (MoveFileW(from.c_str(), to.c_str()) != 0);

    return returnCode;
}
//-----------------------------------------------------------------------------
bool moveDir(const String & from, const String & to)
{
    bool returnCode = false;

    if(dirExists(from) && !dirExists(to))
        returnCode = (MoveFileW(from.c_str(), to.c_str()) != 0);

    return returnCode;
}
//-----------------------------------------------------------------------------
void copyFile(const String & source, const String & target)
{
    if(!CopyFileW(source.c_str(), target.c_str(), TRUE))
        handleWinError(L"deleteFile", source + L" -> " + target);
}
//-----------------------------------------------------------------------------
void deleteFile(const String & file)
{
    if(!DeleteFileW(file.c_str()))
        handleWinError(L"deleteFile", file);
}
//-----------------------------------------------------------------------------
void createFinalDirectory(const String & dir)
{
    if(!CreateDirectoryW(getNormalizedPath(dir).c_str(), NULL))
        handleWinError(L"createDirectory", dir);
}
//-----------------------------------------------------------------------------
void getDirContent(const String & dir, List<String> & files, List<String> & subDirs)
{
    String scanDir(dir);
    scanDir << L"/*";

    files.clear();
    subDirs.clear();
    WIN32_FIND_DATAW findData;

    HANDLE handle = FindFirstFileW(getNormalizedPath(scanDir).c_str(), &findData);
    if(handle == INVALID_HANDLE_VALUE)
    {
        handleWinError(L"getDirContent", dir);
    }
    else
    {
        do
        {
            if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                String dirName(findData.cFileName);
                if(!dirName.startsWith(L"."))
                    subDirs.push_back(dirName);
            }
            else
            {
                files.push_back(String(findData.cFileName));
            }
        }
        while (FindNextFileW(handle, &findData));
        FindClose(handle);
    }
}
//-----------------------------------------------------------------------------
bool fileExists(const String & file)
{
    int32 attrib = GetFileAttributesW(getNormalizedPath(file).c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
}
//-----------------------------------------------------------------------------
bool dirExists(const String & dir)
{
    int32 attrib = GetFileAttributesW(getNormalizedPath(dir).c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
}
//-----------------------------------------------------------------------------
String getEnvVar(const String & var)
{
    String result;
    int32 length = GetEnvironmentVariableW(var.c_str(), NULL, 0);
    
    if(length == 0)
        throw SystemException(L"Environment variable not defined " + var);

    result.reserve(length);
    result.resize(length - 1);
    GetEnvironmentVariableW(var.c_str(), &result[0], length);

    return result;
}
//-----------------------------------------------------------------------------
void getEnv(List<String> & vars, List<String> & values)
{
    vars.clear();
    wchar_t * pVars = GetEnvironmentStringsW();

    bool finished = false;
    int32 iStart = 0;
    int32 iEnd = 0;

    while(!finished)
    {
        while(pVars[iEnd] != '\0')
            iEnd ++;

        if(iEnd > iStart+1)
        {
            String varLine(&pVars[iStart]);
            int32 iEqual = varLine.find_first_of(L"=");
            vars.push_back(varLine.slice(0, iEqual - 1));
            values.push_back(varLine.slice(iEqual + 1));
        }
        else
        {
            finished = true;
        }

        iEnd ++;
        iStart = iEnd;
    }
    FreeEnvironmentStringsW(pVars);
}
//-----------------------------------------------------------------------------
String getCurrentDir()
{
    int32 length = GetCurrentDirectoryW(0, NULL);
    String result;
    result.reserve(length);
    result.resize(length - 1);
    GetCurrentDirectoryW(length, &result[0]);
    return getNormalizedPath(result);
}
//-----------------------------------------------------------------------------
void setCurrentDir(const String & dir)
{
    if(!SetCurrentDirectoryW(getNormalizedPath(dir).c_str()))
        handleWinError(L"setCurDir", dir);
}
//-----------------------------------------------------------------------------
String getUserLocalAppDir()
{
    wchar_t buffer[MAX_PATH + 1];

    if(SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, buffer)))
        return getNormalizedPath(String(buffer));

    return L"";
}
//-----------------------------------------------------------------------------
String getTempDir()
{
    int32 length = GetTempPathW(0, NULL);
    String result;
    result.reserve(length);
    result.resize(length - 1);
    GetTempPathW(length, &result[0]);
    return getNormalizedPath(result);
}
//-----------------------------------------------------------------------------
TimeValue getLastModificationDate(const String & fileName)
{
    bool error = true;
    String normFileName =  getNormalizedPath(fileName);

    HANDLE hFile = ::CreateFileW(   normFileName.c_str(),
                                    0,
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_EXISTING,
                                    0,
                                    NULL);

    FILETIME ftCreate, ftAccess, ftWrite;

    if( hFile != INVALID_HANDLE_VALUE)
    {
        if(::GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite) != 0)//if GetFileTime == 0 => failed
        {
            error = false;
        }
        ::CloseHandle(hFile);
    }

    if(error)
    {
        String errorMessage = L"An error has occured while trying to get file times on file : ";
        errorMessage << fileName;
        throw Core::SystemException(errorMessage);
    }

    return TimeValue(ftWrite);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
#else
static void handleLinuxError(const String & name, const String & file)
{
    char strerrbuf[512];
    char const * str = strerror_r(errno, strerrbuf, 512);
    String message(L"System error in ");
    message << name << L" call (file '" << file << L"'). Linux Error : ";

    if(str != NULL)
        message << Core::String(str);
    else
        message << L"Unknown error.";

    throw SystemException(message);
}
//-----------------------------------------------------------------------------
void deleteEmptyDirectory(const String & dir)
{
    if(dirExists(dir))
    {
        Core::String8 dir8(dir);

        if( remove( dir8.c_str() ) != 0 )
            handleLinuxError(L"deleteEmptyDirectory", dir);
    }
}
//-----------------------------------------------------------------------------
bool moveFile(const String & from, const String & to)
{
    if(fileExists(from))
    {
        Core::String8 from8(from);
        Core::String8 to8(to);

        if( rename( from8.c_str(), to8.c_str() ) != 0 )
        {
            handleLinuxError(L"moveFile", from + L" -> " + to);
        }
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
bool moveDir(const String & from, const String & to)
{
    if(dirExists(from))
    {
        Core::String8 from8(from);
        Core::String8 to8(to);

        if( rename( from8.c_str(), to8.c_str() ) != 0 )
        {
            handleLinuxError(L"moveDir", from + L" -> " + to);
        }
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
void copyFile(const String & source, const String & target)
{
    FILE * from = NULL;
    FILE * to = NULL;
    char ch;
    String8 source8(source);
    String8 target8(target);

    if(!fileExists(source))
    {
        String message;
        message << L"System error in copyFile " << source << L" -> " << target << L" : " << source << L" does not exists.";
        throw SystemException(message);
    }

    /* open source file */
    if( (from = fopen(source8.c_str(), "rb")) == NULL)
    {
        String message;
        message << L"System error in copyFile " << source << L" -> " << target << L" : Cannot open " << source;
        throw SystemException(message);
    }

    /* open destination file */
    if((to = fopen(target8.c_str(), "wb"))==NULL)
    {
        String message;
        message << L"System error in copyFile " << source << L" -> " << target << L" : Cannot open " << target;
        throw SystemException(message);
    }

    /* copy the file */
    while(!feof(from))
    {
        ch = fgetc(from);

        if(!feof(from))
            fputc(ch, to);
    }

    fclose(from);
    fclose(to);
}
//-----------------------------------------------------------------------------
void createFinalDirectory(const String & dir)
{
    String8 dir8(dir);
    if(mkdir(dir8.c_str(), 0755) != 0)
        handleLinuxError(L"createFinalDirectory", dir);
}
//-----------------------------------------------------------------------------
void deleteFile(const String & file)
{
    Core::String8 file8(file);

    if( remove( file8.c_str() ) != 0 )
        handleLinuxError(L"deleteFile", file);
}
//-----------------------------------------------------------------------------
void getDirContent(const String & dirName, List<String> & files, List<String> & subDirs)
{
    String8 dirName8(dirName);
    DIR * dir;
    struct dirent entry;
    struct dirent *result;

    if((dir  = opendir(dirName8.c_str())) == NULL)
    {
        handleLinuxError(L"getDirContent", dirName);
    }
    else
    {
        int32 returnCode = 0;
        for( returnCode = readdir_r(dir, &entry, &result);
             result != NULL && returnCode == 0;
             returnCode = readdir_r(dir, &entry, &result))
        {
            Core::String entryName(entry.d_name);
            Core::String fullPath;
            fullPath << dirName<<L"/"<<entryName;

            if(dirExists(fullPath) && entryName.length() > 0 && entryName[0] != L'.')
                subDirs.push_back(entryName);
            else if(fileExists(fullPath))
                files.push_back(entryName);
            //else
            //  problème de droits?
        }
    }

}
//-----------------------------------------------------------------------------
bool fileExists(const String & fileName)
{
    String8 fileName8(fileName);
    if (FILE * file = fopen(fileName8.c_str(), "r"))
    {
        fclose(file);

        struct stat fileStat;
        if(stat(fileName8.c_str(), &fileStat) != 0)
            handleLinuxError(L"fileExists", fileName);


        return (fileStat.st_mode & S_IFREG);
    }
    return false;
}
//-----------------------------------------------------------------------------
bool dirExists(const String & dirName)
{
    String8 dirName8(dirName);
    DIR * dir;

    if((dir  = opendir(dirName8.c_str())) != NULL)
    {
        closedir(dir);
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------
String getEnvVar(const String & var)
{
    String value;

    char * envVar = getenv(String8(var).c_str());

    if(envVar == NULL)
        throw SystemException(L"Environment variable not defined " + var);

    value = String(envVar);

    return value;
}
//-----------------------------------------------------------------------------
void getEnv(List<String> & vars, List<String> & values)
{
    if(environ != NULL)
    {
        int i = 0;
        while(environ[i] != NULL)
        {
            String envNameValue(environ[i]);
            String envName;
            String envValue;

            int32 iStart = 0;
            int32 iToken = envNameValue.find_first_of(L"=");

            if(iToken != String::npos && iToken > 0)
            {
                envName = envNameValue.slice(iStart, iToken - 1);
                envValue = envNameValue.slice(iStart +1);

                vars.push_back(envName);
                values.push_back(envValue);
            }
            ++i;
        }
    }
}
//-----------------------------------------------------------------------------
String getCurrentDir()
{
    char buff[PATH_MAX];
    char * path = getcwd(buff, PATH_MAX);
    if(path == NULL)
        handleLinuxError(L"getCurrentDir", L"current directory");

    return String(path);
}
//-----------------------------------------------------------------------------
void setCurrentDir(const String & dir)
{

    if(chdir(String8(dir).c_str()) != 0)
        handleLinuxError(L"setCurrentDir", dir);
}
//-----------------------------------------------------------------------------
String getTempDir()
{
    String tempDir;
    if( (tempDir = getEnvVar(L"TMPDIR")) == String::EMPTY)
        if( (tempDir = getEnvVar(L"TMP")) == String::EMPTY)
            if( (tempDir = getEnvVar(L"TEMP")) == String::EMPTY)
                tempDir = L"/tmp";

    return tempDir;
}
//-----------------------------------------------------------------------------
TimeValue getLastModificationDate(const String & fileName)
{
    struct stat fileStat;
    String8 fileName8(fileName);
    if(stat(fileName8.c_str(), &fileStat) != 0)
        handleLinuxError(L"getLastModificationDate", fileName);

    return TimeValue(fileStat.st_mtime);
}
//-----------------------------------------------------------------------------
String getUserLocalAppDir()
{
    return getEnvVar(L"HOME");
}
#endif
} // System
} // Core
