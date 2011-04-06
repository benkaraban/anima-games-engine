/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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
#include "GameSettings.h"

#include <Core/System/System.h>
#include <Window/InputManager.h>

#include <GameTools.h>

namespace ChronoRage
{

GameSettings::GameSettings(int32 width, int32 height, Renderer::EMSAALevel msaa)
:   _language(Core::ENGLISH),
    _playerName(L"ANIMA GAMES"),
    _currentLevel(0),
    _videoSettings(Renderer::GLOBAL_VERY_HIGH),
    _specialEffects(true),
    _musicVolume(1.0f),
    _soundVolume(1.0f),
    _selectedControls(SC_MOUSE_KEYBOARD),
    _selectedJoystick(0)
{
    _language = Core::System::getSystemSupportedLanguage();

    _videoSettings.fullscreen       = true;
    _videoSettings.vsync            = false;
    _videoSettings.width            = width;
    _videoSettings.height           = height;
    _videoSettings.msaaLevel        = msaa;
    _videoSettings.reflecLevel      = Renderer::REFLEC_NONE;
    _videoSettings.shadowLevel      = Renderer::SHADOW_NONE;
    //_videoSettings.shaderLevel      = Renderer::SHADER_NONE;

    setDefaultKeyboardMovementConfig();
    setDefaultKeyboardShootingConfig();

    setDefaultMouseSettings();
}

GameSettings::~GameSettings()
{}

void GameSettings::setPlayerName(const String & playerName)
{
    if(playerName.length() <= MAX_PLAYER_NAME_LENGTH)
        _playerName = playerName;
}

void GameSettings::setCurrentLevel(int32 currentLevel)
{
    if(GameTools::isLevelValid(currentLevel))
        _currentLevel = currentLevel;
}

void GameSettings::setResolution(int32 width, int32 height)
{
    _videoSettings.width        = width;
    _videoSettings.height       = height;
}

void GameSettings::enableSpecialEffects(bool specialEffects)
{
    if(specialEffects)
    {
        _videoSettings.refracLevel = Renderer::REFRAC_MEDIUM;
        _videoSettings.postFXOn = true;
        _videoSettings.glowOn = true;
    }
    else
    {
        _videoSettings.refracLevel = Renderer::REFRAC_NONE;
        _videoSettings.postFXOn = false;
        _videoSettings.glowOn = false;
    }

    _specialEffects = specialEffects;
}

void GameSettings::setSelectedControls(ESelectedControls selectedControls)
{
    if(isValid(selectedControls))
    {
        _selectedControls = selectedControls;
    }
    else
    {
        _selectedControls = SC_MOUSE_KEYBOARD;
    }
}

void GameSettings::getMovementKeyboardControls( int32 & up,
                                                int32 & down,
                                                int32 & left,
                                                int32 & right) const
{
    up      = _keyboardUp;
    down    = _keyboardDown;
    left    = _keyboardLeft;
    right   = _keyboardRight;
}

void GameSettings::getShootingKeyboardControls( int32 & up,
                                                int32 & down,
                                                int32 & left,
                                                int32 & right,
                                                int32 & timeWarp,
                                                int32 & fire) const
{
    up      = _keyboardShootUp;
    down    = _keyboardShootDown;
    left    = _keyboardShootLeft;
    right   = _keyboardShootRight;
    timeWarp= _keyboardShootTimeWarp;
    fire    = _keyboardFire;
}

void GameSettings::setKeyboardValue(EControlDirection cd, int32 value)
{
    int32 oldValue = 0;
    switch(cd)
    {
    case CD_MOVE_UP:
        oldValue = _keyboardUp;
        _keyboardUp = value;
        break;
    case CD_MOVE_DOWN:
        oldValue = _keyboardDown;
        _keyboardDown = value;
        break;
    case CD_MOVE_LEFT:
        oldValue = _keyboardLeft;
        _keyboardLeft = value;
        break;
    case CD_MOVE_RIGHT:
        oldValue = _keyboardRight;
        _keyboardRight = value;
        break;
    case CD_SHOOT_UP:
        oldValue = _keyboardShootUp;
        _keyboardShootUp = value;
        break;
    case CD_SHOOT_DOWN:
        oldValue = _keyboardShootDown;
        _keyboardShootDown = value;
        break;
    case CD_SHOOT_LEFT:
        oldValue = _keyboardShootLeft;
        _keyboardShootLeft = value;
        break;
    case CD_SHOOT_RIGHT:
        oldValue = _keyboardShootRight;
        _keyboardShootRight = value;
        break;
    case CD_FIRE:
        oldValue = _keyboardFire;
        _keyboardFire = value;
        break;
    case CD_TIME_WARP:
        oldValue = _keyboardShootTimeWarp;
        _keyboardShootTimeWarp = value;
        break;
    }
    switchKeyboardControls(cd, value, oldValue);
}

void GameSettings::switchKeyboardControls(EControlDirection cd, int32 value, int32 oldValue)
{
    if(cd != CD_MOVE_UP)
    {
        if(_keyboardUp == value)
            _keyboardUp = oldValue;
    }
    if(cd != CD_MOVE_DOWN)
    {
        if(_keyboardDown == value)
            _keyboardDown = oldValue;
    }
    if(cd != CD_MOVE_LEFT)
    {
        if(_keyboardLeft == value)
            _keyboardLeft = oldValue;
    }
    if(cd != CD_MOVE_RIGHT)
    {
        if(_keyboardRight == value)
            _keyboardRight = oldValue;
    }
    if(cd != CD_SHOOT_UP)
    {
        if(_keyboardShootUp == value)
            _keyboardShootUp = oldValue;
    }
    if(cd != CD_SHOOT_DOWN)
    {
        if(_keyboardShootDown == value)
            _keyboardShootDown = oldValue;
    }
    if(cd != CD_SHOOT_LEFT)
    {
        if(_keyboardShootLeft == value)
            _keyboardShootLeft = oldValue;
    }
    if(cd != CD_SHOOT_RIGHT)
    {
        if(_keyboardShootRight == value)
            _keyboardShootRight = oldValue;
    }
    if(cd != CD_TIME_WARP)
    {
        if(_keyboardShootTimeWarp == value)
            _keyboardShootTimeWarp = oldValue;
    }
}

void GameSettings::getShootingMouseControls(EMouseButton & shoot, EMouseButton & timeWarp) const
{
    shoot = _mouseShootButton;
    timeWarp = _mouseTimeWarpButton;
}

void GameSettings::setMouseValue(EMouseControl mouseControl, EMouseButton mouseButton)
{
    switch(mouseControl)
    {
    case MC_SHOOT:
        if(_mouseTimeWarpButton == mouseButton)
        {
            _mouseTimeWarpButton = _mouseShootButton;
        }
        _mouseShootButton = mouseButton;
        break;
    case MC_TIME_WARP:
        if(_mouseShootButton == mouseButton)
        {
            _mouseShootButton   = _mouseTimeWarpButton;
        }
        _mouseTimeWarpButton = mouseButton;
        break;
    }
}

JoySettings GameSettings::getJoySettings(const String & joyName) const
{
    Core::Map<String, JoySettings>::const_iterator iSettings = _joySettings.find(joyName);

    if(iSettings == _joySettings.end())
    {
        return JoySettings();
    }
    else
    {
        return iSettings->second;
    }
}

void GameSettings::importXML(const Ptr<Core::XMLNode> & pXMLNode)
{
    _joySettings.clear();

    int32 language = 0;
    pXMLNode->getValue(L"game-settings.language", language);
    if(Core::isValid(Core::ELanguage(language)))
        _language = Core::ELanguage(language);
    else
        _language = Core::ENGLISH;

    _playerName = L"ANIMA GAMES";
    pXMLNode->getValue(L"game-settings.player-name", _playerName);
    _currentLevel = 0;
    pXMLNode->getValue(L"game-settings.current-level", _currentLevel);

    //VidÈo
    int32 antiAlias = 0;
    pXMLNode->getValue(L"game-settings.video.fullscreen", _videoSettings.fullscreen);
    pXMLNode->getValue(L"game-settings.video.v-sync", _videoSettings.vsync);
    pXMLNode->getValue(L"game-settings.video.special-effects", _specialEffects);
    pXMLNode->getValue(L"game-settings.video.anti-alias", antiAlias);
    pXMLNode->getValue(L"game-settings.video.resolution.width", _videoSettings.width);
    pXMLNode->getValue(L"game-settings.video.resolution.height", _videoSettings.height);

    enableSpecialEffects(_specialEffects);

    Renderer::EMSAALevel msaaLevel = Renderer::MSAA_NONE;
    switch(antiAlias)
    {
    case 0:
        msaaLevel = Renderer::MSAA_NONE;
        break;
    case 2:
        msaaLevel = Renderer::MSAA_2X;
        break;
    case 4:
        msaaLevel = Renderer::MSAA_4X;
        break;
    case 8:
        msaaLevel = Renderer::MSAA_8X;
        break;
    }
    _videoSettings.msaaLevel = msaaLevel;

    //Audio
    pXMLNode->getValue(L"game-settings.audio.music-volume", _musicVolume);
    pXMLNode->getValue(L"game-settings.audio.sound-volume", _soundVolume);

    //Controls
    int32 selectedControls = 0;
    pXMLNode->getValue(L"game-settings.controls.selected-controls", selectedControls);
    pXMLNode->getValue(L"game-settings.controls.selected-joystick", _selectedJoystick);
    setSelectedControls(ESelectedControls(selectedControls));

    //Clavier
    int32 scanCode = -1;
    int32 vkCode;
    pXMLNode->getValue(L"game-settings.controls.keyboard.up", scanCode);
    vkCode = Core::System::scanCodeToVK(scanCode);
    _keyboardUp = ((vkCode == -1) ? _keyboardUp : vkCode);
    pXMLNode->getValue(L"game-settings.controls.keyboard.down", scanCode);
    vkCode = Core::System::scanCodeToVK(scanCode);
    _keyboardDown = ((vkCode == -1) ? _keyboardDown : vkCode);
    pXMLNode->getValue(L"game-settings.controls.keyboard.left", scanCode);
    vkCode = Core::System::scanCodeToVK(scanCode);
    _keyboardLeft = ((vkCode == -1) ? _keyboardLeft : vkCode);
    pXMLNode->getValue(L"game-settings.controls.keyboard.right", scanCode);
    vkCode = Core::System::scanCodeToVK(scanCode);
    _keyboardRight = ((vkCode == -1) ? _keyboardRight : vkCode);
    pXMLNode->getValue(L"game-settings.controls.keyboard.shoot-up", scanCode);
    vkCode = Core::System::scanCodeToVK(scanCode);
    _keyboardShootUp = ((vkCode == -1) ? _keyboardShootUp : vkCode);
    pXMLNode->getValue(L"game-settings.controls.keyboard.shoot-down", scanCode);
    vkCode = Core::System::scanCodeToVK(scanCode);
    _keyboardShootDown = ((vkCode == -1) ? _keyboardShootDown : vkCode);
    pXMLNode->getValue(L"game-settings.controls.keyboard.shoot-left", scanCode);
    vkCode = Core::System::scanCodeToVK(scanCode);
    _keyboardShootLeft = ((vkCode == -1) ? _keyboardShootLeft : vkCode);
    pXMLNode->getValue(L"game-settings.controls.keyboard.shoot-right", scanCode);
    vkCode = Core::System::scanCodeToVK(scanCode);
    _keyboardShootRight = ((vkCode == -1) ? _keyboardShootRight : vkCode);
    pXMLNode->getValue(L"game-settings.controls.keyboard.time-warp", scanCode);
    vkCode = Core::System::scanCodeToVK(scanCode);
    _keyboardShootTimeWarp = ((vkCode == -1) ? _keyboardShootTimeWarp : vkCode);
    pXMLNode->getValue(L"game-settings.controls.keyboard.fire", scanCode);
    vkCode = Core::System::scanCodeToVK(scanCode);
    _keyboardFire = ((vkCode == -1) ? _keyboardFire : vkCode);
    pXMLNode->getValue(L"game-settings.controls.keyboard.autofire", _keyboardAutoFire);
    
    //Souris
    int32 mouseButton = -1;
    pXMLNode->getValue(L"game-settings.controls.mouse.shoot",       mouseButton);
    _mouseShootButton = isValid(EMouseButton(mouseButton)) ? EMouseButton(mouseButton) : _mouseShootButton;
    pXMLNode->getValue(L"game-settings.controls.mouse.time-warp",   mouseButton);
    _mouseTimeWarpButton = isValid(EMouseButton(mouseButton)) ? EMouseButton(mouseButton) : _mouseTimeWarpButton;
    pXMLNode->getValue(L"game-settings.controls.mouse.autofire", _mouseAutoFire);

    //Joysticks
    Core::List<Ptr<Core::XMLNode> > joysNodes;
    pXMLNode->getAllNodes(L"game-settings.controls.joysticks", joysNodes);

    for(int32 ii=0; ii < joysNodes.size(); ii++)
    {
        String name;
        JoySettings settings;

        joysNodes[ii]->getValue(L"joy-name", name);
        joysNodes[ii]->getValue(L"time-warp", settings.timeWarpButton);
        joysNodes[ii]->getValue(L"fire", settings.fireButton);
        joysNodes[ii]->getValue(L"autofire", settings.autofire);
        joysNodes[ii]->getValue(L"inverse-sticks", settings.inverseSticks);

        _joySettings[name] = settings;
    }
}

void GameSettings::exportXML(const Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"game-settings.language", int32(_language));
    pXMLNode->setValue(L"game-settings.player-name", _playerName);
    pXMLNode->setValue(L"game-settings.current-level", _currentLevel);

    //VidÈo
    int32 antiAlias = 0;
    
    switch(_videoSettings.msaaLevel)
    {
    case Renderer::MSAA_NONE:
        antiAlias = 0;
        break;
    case Renderer::MSAA_2X:
        antiAlias = 2;
        break;
    case Renderer::MSAA_4X:
        antiAlias = 4;
        break;
    case Renderer::MSAA_8X:
        antiAlias = 8;
        break;
    }

    pXMLNode->setValue(L"game-settings.video.fullscreen", _videoSettings.fullscreen);
    pXMLNode->setValue(L"game-settings.video.v-sync", _videoSettings.vsync);
    pXMLNode->setValue(L"game-settings.video.special-effects", _specialEffects);
    pXMLNode->setValue(L"game-settings.video.anti-alias", antiAlias);
    pXMLNode->setValue(L"game-settings.video.resolution.width", _videoSettings.width);
    pXMLNode->setValue(L"game-settings.video.resolution.height", _videoSettings.height);

    //Audio
    pXMLNode->setValue(L"game-settings.audio.music-volume", _musicVolume);
    pXMLNode->setValue(L"game-settings.audio.sound-volume", _soundVolume);

    //Controls
    pXMLNode->setValue(L"game-settings.controls.selected-controls", int32(_selectedControls));
    pXMLNode->setValue(L"game-settings.controls.selected-joystick", int32(_selectedJoystick));

    //Clavier
    int32 scanCode = -1;
    scanCode = MapVirtualKeyW(_keyboardUp, 0);
    pXMLNode->setValue(L"game-settings.controls.keyboard.up", scanCode);
    scanCode = MapVirtualKeyW(_keyboardDown, 0);
    pXMLNode->setValue(L"game-settings.controls.keyboard.down", scanCode);
    scanCode = MapVirtualKeyW(_keyboardLeft, 0);
    pXMLNode->setValue(L"game-settings.controls.keyboard.left", scanCode);
    scanCode = MapVirtualKeyW(_keyboardRight, 0);
    pXMLNode->setValue(L"game-settings.controls.keyboard.right", scanCode);
    scanCode = MapVirtualKeyW(_keyboardShootUp, 0);
    pXMLNode->setValue(L"game-settings.controls.keyboard.shoot-up", scanCode);
    scanCode = MapVirtualKeyW(_keyboardShootDown, 0);
    pXMLNode->setValue(L"game-settings.controls.keyboard.shoot-down", scanCode);
    scanCode = MapVirtualKeyW(_keyboardShootLeft, 0);
    pXMLNode->setValue(L"game-settings.controls.keyboard.shoot-left", scanCode);
    scanCode = MapVirtualKeyW(_keyboardShootRight, 0);
    pXMLNode->setValue(L"game-settings.controls.keyboard.shoot-right", scanCode);
    scanCode = MapVirtualKeyW(_keyboardShootTimeWarp, 0);
    pXMLNode->setValue(L"game-settings.controls.keyboard.time-warp", scanCode);
    scanCode = MapVirtualKeyW(_keyboardFire, 0);
    pXMLNode->setValue(L"game-settings.controls.keyboard.fire", scanCode);
    pXMLNode->setValue(L"game-settings.controls.keyboard.autofire", _keyboardAutoFire);

    //Souris
    pXMLNode->setValue(L"game-settings.controls.mouse.shoot",       _mouseShootButton);
    pXMLNode->setValue(L"game-settings.controls.mouse.time-warp",   _mouseTimeWarpButton);
    pXMLNode->setValue(L"game-settings.controls.mouse.autofire",    _mouseAutoFire);

    //Joysticks
    for(Core::Map<String, JoySettings>::const_iterator iSettings=_joySettings.begin(); iSettings != _joySettings.end(); ++iSettings)
    {
        Ptr<Core::XMLNode> pNode(new Core::XMLNode(L"joy"));
        pNode->setValue(L"joy-name", iSettings->first);
        pNode->setValue(L"time-warp", iSettings->second.timeWarpButton);
        pNode->setValue(L"fire", iSettings->second.fireButton);
        pNode->setValue(L"autofire", iSettings->second.autofire);
        pNode->setValue(L"inverse-sticks", iSettings->second.inverseSticks);

        pXMLNode->addNode(L"game-settings.controls.joysticks", pNode);
    }
}

void GameSettings::cleanupSettings(const Window::InputManager & input)
{
    int32 joyCount = input.getJoysticks().size();

    if(joyCount == 0)
    {
        _selectedJoystick = 0;

        if(_selectedControls == SC_JOYSTICK)
            _selectedControls = SC_MOUSE_KEYBOARD;
    }
    else
    {
        if(_selectedJoystick >= joyCount)
            _selectedJoystick = 0;
    }
}

void GameSettings::setDefaultKeyboardMovementConfig()
{
    int32 vkCodeUp = 'W';
    int32 vkCodeDown = 'S';
    int32 vkCodeLeft = 'A';
    int32 vkCodeRight = 'D';

    int32 vkCode;

    int32 scanCodeUp = 17;
    int32 scanCodeDown = 31;
    int32 scanCodeLeft = 30;
    int32 scanCodeRight = 32;

     vkCode = Core::System::scanCodeToVK(scanCodeUp);
    _keyboardUp = ((vkCode == -1) ? vkCodeUp : vkCode);

    vkCode = Core::System::scanCodeToVK(scanCodeDown);
    _keyboardDown = ((vkCode == -1) ? vkCodeDown : vkCode);

    vkCode = Core::System::scanCodeToVK(scanCodeLeft);
    _keyboardLeft = ((vkCode == -1) ? vkCodeLeft : vkCode);

    vkCode = Core::System::scanCodeToVK(scanCodeRight);
    _keyboardRight = ((vkCode == -1) ? vkCodeRight : vkCode);
}

void GameSettings::setDefaultKeyboardShootingConfig()
{
    int32 vkCodeShootUp = VK_UP;
    int32 vkCodeShootDown = VK_DOWN;
    int32 vkCodeShootLeft = VK_LEFT;
    int32 vkCodeShootRight = VK_RIGHT;

    int32 vkCodeTimeWarp = VK_SPACE;

    int32 vkCodeFire = VK_LSHIFT;

    int32 vkCode;

    int32 scanCodeShootUp = 72;
    int32 scanCodeShootDown = 80;
    int32 scanCodeShootLeft = 75;
    int32 scanCodeShootRight = 77;

    int32 scanCodeTimeWarp = 57;

    int32 scanCodeFire = 42;

     vkCode = Core::System::scanCodeToVK(scanCodeShootUp);
    _keyboardShootUp = ((vkCode == -1) ? vkCodeShootUp : vkCode);

    vkCode = Core::System::scanCodeToVK(scanCodeShootDown);
    _keyboardShootDown = ((vkCode == -1) ? vkCodeShootDown : vkCode);

    vkCode = Core::System::scanCodeToVK(scanCodeShootLeft);
    _keyboardShootLeft = ((vkCode == -1) ? vkCodeShootLeft : vkCode);

    vkCode = Core::System::scanCodeToVK(scanCodeShootRight);
    _keyboardShootRight = ((vkCode == -1) ? vkCodeShootRight : vkCode);

    vkCode = Core::System::scanCodeToVK(scanCodeTimeWarp);
    _keyboardShootTimeWarp = ((vkCode == -1) ? vkCodeTimeWarp : vkCode);

    vkCode = Core::System::scanCodeToVK(scanCodeFire);
    _keyboardFire = ((vkCode == -1) ? vkCodeFire : vkCode);
}

void GameSettings::setDefaultMouseSettings()
{
    _mouseShootButton       = MOUSE_LEFT_B;
    _mouseTimeWarpButton    = MOUSE_RIGHT_B;

    _mouseAutoFire = false;
}

}//namespace ChronoRage