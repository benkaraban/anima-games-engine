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
#ifndef RENDERER_PERFANALYZER_H_
#define RENDERER_PERFANALYZER_H_

#include <Core/Debug/FPSCounter.h>
#include <Core/Debug/PerfLogger.h>
#include <Renderer/IRenderer.h>

namespace Renderer
{
class LM_API_RDR PerfAnalyzer : public Core::Object
{
public:
    PerfAnalyzer(const Ptr<Renderer::IRenderer> & pRenderer);

    void update();
    void display(const Ptr<Renderer::IRenderView> & pView);

    Core::Map<String, String>               _sections;
    Core::Map<String, Core::Vector4f>       _colors;
    Core::Map<String, Core::List<Core::Vector4f> >       _subColors;
    Core::Map<String, Core::Map<int32, int32> >          _iSubColor;

protected:
    void update(const Core::List<Core::PerfLogger::Work> & hist);
    Core::Vector4f getWorkColor() const;

    Core::StopWatch             _sw;
    Ptr<Renderer::IRenderer>    _pRenderer;
    Ptr<Renderer::IHUD>         _pHUD;
    Core::FPSCounter            _fps;
    Ptr<Renderer::IFont>        _pFontFPS;
    Ptr<Renderer::IFont>        _pFontText;
    Ptr<Renderer::IText>        _pTextFPS;
    Ptr<Renderer::IText>        _pTextRenderStats;

    float                       _barScale;
    float                       _barHeight;
    float                       _barPadding;

    Core::List<Ptr<Renderer::IRectangle> >  _bars;
    Core::List<Ptr<Renderer::IText> >       _texts;
};

}

#endif
