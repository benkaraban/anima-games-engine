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
#include <Renderer/PerfAnalyzer.h>
#include <Core/Math/Color.h>

namespace Renderer
{
PerfAnalyzer::PerfAnalyzer(const Ptr<Renderer::IRenderer> & pRenderer)
: _pRenderer(pRenderer)
, _pHUD(pRenderer->createHUD())
{
    _barScale = 100 * 1000.0f;
    _barHeight = 10.0f;
    _barPadding = 2.0f;

    Ptr<Renderer::IRectangle> pBar(_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR));
    pBar->setPosition(Core::Vector2f(0.0f, 0.0f));
    pBar->setSize(Core::Vector2f(1920.0f, 100.0f));
    pBar->setPriority(-1);
    pBar->setColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.5f));

    _pFontFPS = _pRenderer->createFont(L"Consolas", 8);
    _pFontText = _pRenderer->createFont(L"Consolas", 6);
    _pTextFPS = _pHUD->createText(_pFontFPS, L"");
    _pTextFPS->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    _pTextFPS->setPosition(Core::Vector2f(5.0f, 5.0f));

    _pTextRenderStats = _pHUD->createText(_pFontFPS, L"");
    _pTextRenderStats->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    _pTextRenderStats->setPosition(Core::Vector2f(100.0f, 5.0f));

    _colors[L"update"] = Core::Vector4f(0.7f, 0.5f, 0.5f, 1.0f);

    _colors[L"render"] = Core::Vector4f(0.60f, 0.63f, 0.83f, 1.0f);

    Core::List<Core::Vector4f> & crender = _subColors[L"render"];
    crender.push_back(Core::Vector4f(0.50f, 0.53f, 0.70f, 1.00f));
    crender.push_back(Core::Vector4f(0.37f, 0.39f, 0.52f, 1.00f));

    _sections[L"SHADOW"] = L"render";
    _sections[L"prep"] = L"render";
    _sections[L"init"] = L"render";
    _sections[L"light"] = L"render";
    _sections[L"shadow"] = L"render";
    _sections[L"stdmat"] = L"render";
    _sections[L"bill"] = L"render";
    _sections[L"glow"] = L"render";
    _sections[L"glow"] = L"render";

    _colors[L"flip"] = Core::Vector4f(0.7f, 0.5f, 0.5f, 1.0f);
}

void PerfAnalyzer::update()
{
    const Core::PerfLogger & logger = Core::PerfLogger::sPerfLogger;
    double elapsed = _sw.elapsed().ftotalSeconds();

    if(_bars.empty() || elapsed > 1.0)
        update(logger.getHistory());

    _fps.nextFrame();
    _pTextFPS->setText(_fps.getText() + L"\n" + Core::toStringByteSize(_pRenderer->getVRAMUsage()));
    _pTextRenderStats->setText(_pRenderer->getRenderStats());
}

void PerfAnalyzer::update(const Core::List<Core::PerfLogger::Work> & hist)
{
    for(int32 ii=0; ii < _bars.size(); ii++)
        _bars[ii]->kill();

    for(int32 ii=0; ii < _texts.size(); ii++)
        _texts[ii]->kill();
    
    _iSubColor.clear();
    _sw.reset();
    _bars.clear();
    _texts.clear();

    float hp = _barHeight + _barPadding;
    float x0 = 0.0f;
    float y0 = 60.0f;

    for(int32 ii=0; ii < hist.size(); ii++)
    {
        const Core::PerfLogger::Work & work = hist[ii];
        Ptr<Renderer::IRectangle> pBar(_pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR));

        float x1 = x0 + float(_barScale * work.startTime);
        float x2 = x0 + float(_barScale * work.endTime);

        float y1 = y0 + hp * float(work.iTrack);
        float y2 = y1 + _barHeight;

        pBar->setPosition(Core::Vector2f(x1, y1));
        pBar->setSize(Core::Vector2f(x2 - x1, y2 - y1));
        pBar->setPriority(work.iTrack + 1);

        if(work.iTrack == 0)
        {
            pBar->setColor(_colors[work.name]);
        }
        else
        {
            Core::List<Core::Vector4f> & subColors = _subColors[_sections[work.name]];
            int32 & iSubColor = _iSubColor[_sections[work.name]][work.iTrack];

            if(!subColors.empty())
                pBar->setColor(subColors[(iSubColor ++) % subColors.size()]);
        }

        if((x2 - x1) > 15.0f)
        {
            String text;
            text << Core::toString(float(work.endTime - work.startTime) * 1000.0f, 1);

            String textLabel(work.name);
            textLabel.resize(std::min(textLabel.size(), int32((x2 - x1) / 5.0f)));

            Ptr<Renderer::IText> pTextLabel(_pHUD->createText(_pFontText, textLabel));
            pTextLabel->setPosition(Core::Vector2f(x1 + 1.0f, y1 + 1.0f));
            pTextLabel->setPriority(pBar->getPriority() + 1);
            pTextLabel->setColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.5f));

            Ptr<Renderer::IText> pTextTime(_pHUD->createText(_pFontText, text));
            pTextTime->setPosition(Core::Vector2f(0.5f * (x1 + x2) - 0.5f * (5.0f * (text.size() - 0.5f)), y1 + 1.0f));
            pTextTime->setPriority(pBar->getPriority() + 2);
            pTextTime->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.75f));

            _texts.push_back(pTextLabel);
            _texts.push_back(pTextTime);
        }

        _bars.push_back(pBar);
    }
}

void PerfAnalyzer::display(const Ptr<Renderer::IRenderView> & pView)
{
    _pRenderer->renderHUD(pView, _pHUD);
}

}

