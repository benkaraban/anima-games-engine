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
#ifndef NYX_SETTING_H_
#define NYX_SETTING_H_

#include <QByteArray>
#include <QWidget>

#include <Core/Standard.h>
#include <Core/List.h>

#include <Nyx/Constants.h>

#include <Renderer/Settings.h>
#include <EPI/DocumentRenderer.h>

class QSettings;

namespace Nyx
{
//-----------------------------------------------------------------------------
LM_ENUM_3 (ENyxSettingType,
           GENERAL_SETTING,
           WIDGET_SETTING,
           RENDERER_SETTING)
//-----------------------------------------------------------------------------
struct ISetting
{
    ISetting(const String& initFileName) {fileName = initFileName;}
    virtual ENyxSettingType getSettnigType() = 0;
    virtual void writeSetting() const = 0;
    virtual void readSetting() = 0;
    virtual void initToDefaultValue() = 0;

    String fileName;
};
//-----------------------------------------------------------------------------
struct PathSetting
{
    String defaultWorld;
    String defaultTagGameplay;
    Core::List<String>  memoPathShared;
    Core::List<String>  memoPathVfs;

    void initToDefaultValue() {defaultWorld=L""; defaultTagGameplay=DEFAULT_TAG_GAMEPLAY; memoPathShared.clear(); memoPathVfs.clear();}
};

struct GeneralSetting : public ISetting
{
    bool                        showConsol;
    bool                        showWorkingDirectoryDialog;
    Core::List<Core::String>    workingDirectoryPaths;
    int32                       workingDirectoryIndex;
    bool                        sleepBetweenTwoUpdateWorld;
    PathSetting                 paths;
    bool                        playSound;

    GeneralSetting(const String& initFileName);
    virtual ENyxSettingType getSettnigType() {return GENERAL_SETTING;}

    virtual void initToDefaultValue();
    virtual void writeSetting() const;
    virtual void readSetting();
};
//-----------------------------------------------------------------------------
struct WidgetSetting : public ISetting
{
    QByteArray      geometryMainWindow;
    QByteArray      stateMainWindow;
    Qt::DockWidgetArea  propertiesPanel;
    Qt::DockWidgetArea  propertiesExplorer;
    Qt::DockWidgetArea  vfsExplorer;
    Qt::DockWidgetArea  widgetInfo;
    Qt::DockWidgetArea  toolsPanel;
    Qt::DockWidgetArea  outputWidget;
    

    WidgetSetting(const String& initFileName);
    virtual ENyxSettingType getSettnigType() {return WIDGET_SETTING;}

    virtual void initToDefaultValue();
    virtual void writeSetting() const;
    virtual void readSetting();
};
//-----------------------------------------------------------------------------
struct RendererSetting : public ISetting
{
    Renderer::EShaderLevel    shaderLevel;
    Renderer::EShadowLevel    shadowLevel;
    Renderer::ETextureLevel   textureLevel;
    Renderer::EReflecLevel    reflecLevel;
    Renderer::ERefracLevel    refracLevel;
    Renderer::EFilterLevel    filterLevel;
    Renderer::EMSAALevel      msaaLevel;

    RendererSetting(const String& initFileName, Renderer::EGlobalLevel level = Renderer::GLOBAL_VERY_HIGH);

    virtual ENyxSettingType getSettnigType() {return RENDERER_SETTING;}

    void apply(const Ptr<EPI::DocumentRenderer>& pDocRenderer);
    virtual void initToDefaultValue();
    void initToLevelValue(Renderer::EGlobalLevel level);
    virtual void writeSetting() const;
    virtual void readSetting();
};
//-----------------------------------------------------------------------------
class NyxSetting : public Core::Object
{
public:
    NyxSetting(const String fileName);
    virtual ~NyxSetting();

    void readSetting();
    void writeSetting();
    Ptr<ISetting>   getSetting(ENyxSettingType type) {return _settings[type];}
private:
    void initToDefaultValue();

private:
    Ptr<ISetting>    _settings [ENyxSettingType_COUNT];
    String           _fileName;
};
//-----------------------------------------------------------------------------
} //Nyx
#endif NYX_SETTING_H_