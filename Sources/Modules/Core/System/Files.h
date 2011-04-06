/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef CORE_FILES_H_
#define CORE_FILES_H_

#include <Core/List.h>
#include <Core/DateAndTime.h>
#include <Core/System/System.h>

namespace Core
{
namespace System
{
LM_API_COR String getExecutableName();
LM_API_COR String getNormalizedPath(const String & path);
LM_API_COR String getPath(const String & fullName);
LM_API_COR String getFileName(const String & fullName);
LM_API_COR String getFileExt(const String & fullName);
/** Retourne le nom du fichier sans l'extension */
LM_API_COR String getFileBaseName(const String & fullName);

LM_API_COR bool moveFile(const String & from, const String & to);
LM_API_COR bool moveDir(const String & from, const String & to);
LM_API_COR void copyFile(const String & source, const String & target);

LM_API_COR bool fileExists(const String & file);
LM_API_COR bool dirExists(const String & dir);

LM_API_COR void deleteFile(const String & file);
LM_API_COR void deleteDirectory(const String & dir);
LM_API_COR void deleteDirContent(const String & dir);

LM_API_COR String getParent(const String & dir);
LM_API_COR void createDirectory(const String & dir, bool errorIfExists = false);
LM_API_COR void getDirContent(const String & dir, List<String> & files, List<String> & subDirs);

LM_API_COR void getRecursiveDirContent(const String & dir, List<String> & files);

LM_API_COR String getEnvVar(const String & vars);
LM_API_COR void getEnv(List<String> & vars, List<String> & values);

LM_API_COR String getCurrentDir();
LM_API_COR void setCurrentDir(const String & dir);

// Répertoire dédié aux applications pour cet utilisateur, pour cette machine uniquement (pas de config "roaming")
LM_API_COR String getUserLocalAppDir();

LM_API_COR String getTempDir();

LM_API_COR TimeValue getLastModificationDate(const String & fileName);

} // System
} // Core

#endif /*CORE_FILES_H_*/
