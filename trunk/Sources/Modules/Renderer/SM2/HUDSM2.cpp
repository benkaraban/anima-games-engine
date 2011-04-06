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
#include <Renderer/SM2/HUDSM2.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
Ptr<IText> HUDSM2::createText(const Ptr<IFont> & pFont, const String & text)
{
    Ptr<Text> pResult(new Text(pFont, text));
    _texts.push_back(pResult);
    return pResult;
}
//-----------------------------------------------------------------------------
Ptr<IRectangle> HUDSM2::createRectangle(const Renderer::PictureInfos &picture)
{
    Ptr<Rectangle> pResult(new Rectangle(picture));
    _rects.push_back(pResult);
    return pResult;
}
//-----------------------------------------------------------------------------
void HUDSM2::cleanUp()
{
#define CLEAN_LIST(list) \
    for(int32 ii=0; ii < list.size(); ) \
    { \
        if(list[ii]->isAlive()) \
            ii++; \
        else \
            list.qerase(ii); \
    }

    CLEAN_LIST(_texts);
    CLEAN_LIST(_rects);

#undef CLEAN_LIST
}
//-----------------------------------------------------------------------------
void HUDSM2::getTexts(Core::List<Text *> & texts)
{
    for(int32 ii=0; ii < _texts.size(); ii++)
        if(_texts[ii]->isVisible())
            texts.push_back(_texts[ii].get());
}
//-----------------------------------------------------------------------------
void HUDSM2::getRects(Core::List<Rectangle *> & rects)
{
    for(int32 ii=0; ii < _rects.size(); ii++)
        if(_rects[ii]->isVisible())
            rects.push_back(_rects[ii].get());
}
//-----------------------------------------------------------------------------
}