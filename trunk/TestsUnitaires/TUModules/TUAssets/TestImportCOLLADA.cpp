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
#include <Core/Logger.h>
#include <Core/IO/FileStream.h>
#include <Assets/SkeletonPose.h>
#include <Workflow/COLLADA/ModelImporterCOLLADA.h>

using namespace Core;
using namespace Workflow;

void testImportCOLLADA()
{
    UTEST_PACKAGE("COLLADA import");

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Static mesh");

    FileInputStream input(L"Data/TUData/Modeles/simplescene.DAE");

    UTEST_EQU(input.isOpened(), true);

    ModelImporterCOLLADA importer;
    Ptr<Assets::Model> pModel = importer.importModel(input);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Animated mesh");

    FileInputStream input(L"Data/TUData/Modeles/simpleanim.DAE");

    UTEST_EQU(input.isOpened(), true);

    ModelImporterCOLLADA importer;
    Ptr<Assets::Model> pModel = importer.importModel(input);

    UTEST_END_TEST;
}
