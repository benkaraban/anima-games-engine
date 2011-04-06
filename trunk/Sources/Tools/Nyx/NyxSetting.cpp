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
#include "NyxSetting.h"

#include <QSettings>
#include <Renderer/IRenderer.h>

#include <EPI/Constants.h>

namespace Nyx
{


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GeneralSetting::GeneralSetting(const String& initFileName):
    ISetting(initFileName)
{
    initToDefaultValue();
}
//-----------------------------------------------------------------------------
void GeneralSetting::initToDefaultValue()
{
    showConsol = false;
    playSound = true;
    showWorkingDirectoryDialog = true;
    sleepBetweenTwoUpdateWorld = true;
    workingDirectoryPaths.clear();
    workingDirectoryIndex = -1;

    paths.initToDefaultValue();
}
//-----------------------------------------------------------------------------
void GeneralSetting::writeSetting() const
{
    QSettings settings(String8(fileName).c_str(), QSettings::IniFormat);

    QList<QVariant>  workPath        = ListStrToQListQVariant(workingDirectoryPaths);
    QList<QVariant>  qMemoPathShared = ListStrToQListQVariant(paths.memoPathShared);
    QList<QVariant>  qMemoPathVfs    = ListStrToQListQVariant(paths.memoPathVfs);

     settings.beginGroup("GeneralSetting");
        settings.setValue("showConsol", showConsol);
        settings.setValue("playSound", playSound);
        settings.setValue("showWorkingDirectoryDialog", showWorkingDirectoryDialog);
        settings.setValue("sleepBetweenTwoUpdateWorld", sleepBetweenTwoUpdateWorld);
        settings.setValue("workingDirectoryIndex", workingDirectoryIndex);
        settings.setValue("workingDirectoryPaths", workPath);
        settings.setValue("defaultWorld", StrToQStr(paths.defaultWorld));
        settings.setValue("defaultTagGameplay", StrToQStr(paths.defaultTagGameplay));
        settings.setValue("memoPathShared", qMemoPathShared);
        settings.setValue("memoPathVfs", qMemoPathVfs);
     settings.endGroup();
}

//-----------------------------------------------------------------------------
void GeneralSetting::readSetting()
{
    QSettings settings(String8(fileName).c_str(), QSettings::IniFormat);

    GeneralSetting default (L"");

    QList<QVariant>  workPath;
    QList<QVariant>  qMemoPathShared;
    QList<QVariant>  qMemoPathVfs;

    settings.beginGroup("GeneralSetting");
        showConsol                  = settings.value("showConsol", default.showConsol).toBool();
        playSound                   = settings.value("playSound", default.playSound).toBool();
        showWorkingDirectoryDialog  = settings.value("showWorkingDirectoryDialog", default.showWorkingDirectoryDialog).toBool();
        sleepBetweenTwoUpdateWorld  = settings.value("sleepBetweenTwoUpdateWorld", default.sleepBetweenTwoUpdateWorld).toBool();
        workingDirectoryIndex       = settings.value("workingDirectoryIndex", default.workingDirectoryIndex).toInt();
        workPath                    = settings.value("workingDirectoryPaths", workPath).toList();
        paths.defaultWorld          = QStrToStr(settings.value("defaultWorld", StrToQStr(default.paths.defaultWorld)).toString());
        paths.defaultTagGameplay    = QStrToStr(settings.value("defaultTagGameplay", StrToQStr(default.paths.defaultTagGameplay)).toString());
        qMemoPathShared             = settings.value("memoPathShared", qMemoPathShared).toList();
        qMemoPathVfs                = settings.value("memoPathVfs", qMemoPathVfs).toList();
    settings.endGroup();
    
    workingDirectoryPaths = ListStrToQListQVariant(workPath);
    paths.memoPathShared  = ListStrToQListQVariant(qMemoPathShared);
    paths.memoPathVfs     = ListStrToQListQVariant(qMemoPathVfs);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
WidgetSetting::WidgetSetting(const String& initFileName):
    ISetting(initFileName)
{
    initToDefaultValue();
}
//-----------------------------------------------------------------------------
void WidgetSetting::initToDefaultValue()
{
}
//-----------------------------------------------------------------------------
void WidgetSetting::writeSetting() const
{
    QSettings settings(String8(fileName).c_str(), QSettings::IniFormat);

    settings.beginGroup("WidgetSetting");
        settings.setValue("geometryMainWindow", geometryMainWindow);
        settings.setValue("stateMainWindow",    stateMainWindow);
        settings.setValue("propertiesPanel",    int(propertiesPanel));
        settings.setValue("propertiesExplorer", int(propertiesExplorer));
        settings.setValue("vfsExplorer",        int(vfsExplorer));
        settings.setValue("widgetInfo",         int(widgetInfo));
        settings.setValue("toolsPanel",         int(toolsPanel));
        settings.setValue("outputWidget",       int(outputWidget));
    settings.endGroup();
}
//-----------------------------------------------------------------------------
void WidgetSetting::readSetting()
{
    QList<QVariant>  workPath;

    QSettings settings(String8(fileName).c_str(), QSettings::IniFormat);

    settings.beginGroup("WidgetSetting");
        geometryMainWindow = settings.value("geometryMainWindow").toByteArray();
        stateMainWindow    = settings.value("stateMainWindow").toByteArray();
        propertiesPanel    = (Qt::DockWidgetArea)settings.value("propertiesPanel", 0).toInt();
        propertiesExplorer = (Qt::DockWidgetArea)settings.value("propertiesExplorer", 0).toInt();
        vfsExplorer        = (Qt::DockWidgetArea)settings.value("vfsExplorer").toInt();
        widgetInfo         = (Qt::DockWidgetArea)settings.value("widgetInfo").toInt();
        toolsPanel         = (Qt::DockWidgetArea)settings.value("toolsPanel").toInt();
        outputWidget       = (Qt::DockWidgetArea)settings.value("outputWidget").toInt();
    settings.endGroup(); 
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RendererSetting::RendererSetting(const String& initFileName, Renderer::EGlobalLevel level) :
    ISetting(initFileName)
{
    initToLevelValue(level);
}
//-----------------------------------------------------------------------------
void RendererSetting::initToDefaultValue()
{
    initToLevelValue(Renderer::GLOBAL_VERY_HIGH);
}
//-----------------------------------------------------------------------------
void RendererSetting::initToLevelValue(Renderer::EGlobalLevel level)
{
    Renderer::RendererSettings rdrSet (level);
    
    shaderLevel   = rdrSet.shaderLevel;
    shadowLevel   = rdrSet.shadowLevel;
    textureLevel  = rdrSet.textureLevel;
    reflecLevel   = rdrSet.reflecLevel;
    refracLevel   = rdrSet.refracLevel;
    filterLevel   = rdrSet.filterLevel;
    msaaLevel     = rdrSet.msaaLevel;
}
//-----------------------------------------------------------------------------
void RendererSetting::writeSetting() const
{
    QSettings settings(String8(fileName).c_str(), QSettings::IniFormat);

    settings.beginGroup("RendererSetting");
        settings.setValue("shaderLevel",        int(shaderLevel));
        settings.setValue("shadowLevel",        int(shadowLevel));
        settings.setValue("textureLevel",       int(textureLevel));
        settings.setValue("reflecLevel",        int(reflecLevel));
        settings.setValue("refracLevel",        int(refracLevel));
        settings.setValue("filterLevel",        int(filterLevel));
        settings.setValue("msaaLevel",          int(msaaLevel));
    settings.endGroup();
}
//-----------------------------------------------------------------------------
void RendererSetting::readSetting()
{
    QSettings settings(String8(fileName).c_str(), QSettings::IniFormat);

    RendererSetting default (L"");

    settings.beginGroup("RendererSetting");
        shaderLevel      = (Renderer::EShaderLevel) settings.value("shaderLevel", default.shaderLevel).toInt();
        shadowLevel      = (Renderer::EShadowLevel) settings.value("shadowLevel", default.shadowLevel).toInt();
        textureLevel     = (Renderer::ETextureLevel)settings.value("textureLevel", default.textureLevel).toInt();
        reflecLevel      = (Renderer::EReflecLevel) settings.value("reflecLevel", default.reflecLevel).toInt();
        refracLevel      = (Renderer::ERefracLevel) settings.value("refracLevel", default.refracLevel).toInt();
        filterLevel      = (Renderer::EFilterLevel) settings.value("filterLevel", default.filterLevel).toInt();
        msaaLevel        = (Renderer::EMSAALevel)   settings.value("msaaLevel", default.msaaLevel).toInt();
    settings.endGroup(); 
}
//-----------------------------------------------------------------------------
void RendererSetting::apply(const Ptr<EPI::DocumentRenderer>& pDocRenderer)
{

    Renderer::RendererSettings rdrSet = pDocRenderer->getRenderer()->getRenderSettings();

        rdrSet.shaderLevel  = shaderLevel;
        rdrSet.shadowLevel  = shadowLevel;
        rdrSet.textureLevel = textureLevel;
        rdrSet.reflecLevel  = reflecLevel;
        rdrSet.refracLevel  = refracLevel;
        rdrSet.filterLevel  = filterLevel;
        rdrSet.msaaLevel    = msaaLevel;

    pDocRenderer->getRenderer()->setRenderSettings(rdrSet);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Ptr<ISetting> createSetting(ENyxSettingType type, const String& fileName)
{
    Ptr<ISetting> pRet;

    switch(type)
    {
        case GENERAL_SETTING:   pRet = Ptr<ISetting>(new GeneralSetting(fileName)); break;
        case WIDGET_SETTING:    pRet = Ptr<ISetting>(new WidgetSetting(fileName));  break;
        case RENDERER_SETTING:  pRet = Ptr<ISetting>(new RendererSetting(fileName));  break;
    }

    return pRet;
}
//-----------------------------------------------------------------------------
NyxSetting::NyxSetting(const String fileName):
    _fileName(fileName)
{
    for (int32 ii=0; ii<ENyxSettingType_COUNT; ++ii)
    {
        _settings[ii] = createSetting((ENyxSettingType)ii, fileName);
    }

    readSetting();
}
//-----------------------------------------------------------------------------
NyxSetting::~NyxSetting()
{
}
//-----------------------------------------------------------------------------
void NyxSetting::initToDefaultValue()
{
    for (int32 ii=0; ii<ENyxSettingType_COUNT; ++ii)
    {
        _settings[ii]->initToDefaultValue();
    }
}
//-----------------------------------------------------------------------------
void NyxSetting::readSetting()
{
    for (int32 ii=0; ii<ENyxSettingType_COUNT; ++ii)
    {
        _settings[ii]->readSetting();
    }
}
//-----------------------------------------------------------------------------
void NyxSetting::writeSetting()
{
    for (int32 ii=0; ii<ENyxSettingType_COUNT; ++ii)
    {
        _settings[ii]->writeSetting();
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace Nyx
