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
#include <Core/IO/BufferStream.h>
#include <Workflow/PicBankGeneration.h>

void testPicBanks()
{
    UTEST_PACKAGE("Picture banks");

    Ptr<Assets::PictureBank> pPBK;


    UTEST_START_TEST(L"Generation");

    pPBK = Workflow::generatePictureBank(L"Data/TUData/Pictures/Bank00", 128, Assets::TEX_FORMAT_ARGB8);

    UTEST_EQU(pPBK->getTextureList().size(), 3);
    UTEST_EQU(pPBK->getPicture(L"cube00.png").textureIndex, 0);
    UTEST_EQU(pPBK->getPicture(L"cube01.png").textureIndex, 0);

    UTEST_CATCH_CORE_EXCEPTION(pPBK->getPicture(L"cube06.png").textureIndex)

    UTEST_END_TEST;


    UTEST_START_TEST(L"Streaming");

    Ptr<Assets::PictureBank> pPBK2(new Assets::PictureBank());
    Core::BufferStream buffer;
    pPBK->write(buffer);
    pPBK2->read(buffer);

    UTEST_EQU(pPBK2->getTextureList().size(), 3);
    UTEST_EQU(pPBK2->getPicture(L"cube00.png").textureIndex, 0);
    UTEST_EQU(pPBK2->getPicture(L"cube01.png").textureIndex, 0);

    UTEST_CATCH_CORE_EXCEPTION(pPBK2->getPicture(L"cube06.png").textureIndex)

    UTEST_END_TEST;
}
