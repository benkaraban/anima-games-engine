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
#ifndef CHRONORAGE_GAMESETTINGS_H_
#define CHRONORAGE_GAMESETTINGS_H_

#include <Core/Standard.h>
#include <Core/Dictionary.h>
#include <Core/XML/XMLNode.h>
#include <Renderer/Settings.h>

namespace Window
{
    class InputManager;
}

namespace ChronoRage
{

LM_ENUM_3(ESelectedControls,    SC_MOUSE_KEYBOARD,
                                SC_KEYBOARD_ONLY,
                                SC_JOYSTICK);

LM_ENUM_10(EControlDirection,   CD_MOVE_UP,
                                CD_MOVE_DOWN,
                                CD_MOVE_LEFT,
                                CD_MOVE_RIGHT,
                                CD_SHOOT_UP,
                                CD_SHOOT_DOWN,
                                CD_SHOOT_LEFT,
                                CD_SHOOT_RIGHT,
                                CD_TIME_WARP,
                                CD_FIRE);

LM_ENUM_3(EMouseButton,         MOUSE_LEFT_B,
                                MOUSE_RIGHT_B,
                                MOUSE_MIDDLE_B);

LM_ENUM_2(EMouseControl,        MC_SHOOT,
                                MC_TIME_WARP);

LM_ENUM_2(EJoystickControl,     JC_TIME_WARP,
                                JC_FIRE);

const int32 MAX_PLAYER_NAME_LENGTH = 11;

struct LM_API_CHR JoySettings : public Core::Object
{
    JoySettings() : timeWarpButton(5), fireButton(4), autofire(true), inverseSticks(false) {}

    int32       timeWarpButton;
    int32       fireButton;
    bool        autofire;
    bool        inverseSticks;
};

class LM_API_CHR GameSettings : public Core::Object
{
public:
    GameSettings(int32 width, int32 height, Renderer::EMSAALevel msaa);
    virtual ~GameSettings();

    Core::ELanguage getLanguage() const { return _language; }
    void setLanguage(Core::ELanguage language){ _language = language; }

    const String & getPlayerName() { return _playerName;}
    void setPlayerName(const String & playerName);

    int32 getCurrentLevel() { return _currentLevel;}
    void setCurrentLevel(int32 currentLevel);

    int32 getScreenWidth() const { return _videoSettings.width; }
    int32 getScreenHeight() const { return _videoSettings.height; }
    void setResolution(int32 width, int32 height);

    bool fullscreen() const { return _videoSettings.fullscreen; }
    void setFullscreen(bool fullscreen){ _videoSettings.fullscreen = fullscreen; }

    void setVSync(bool vsync){ _videoSettings.vsync = vsync; }

    bool specialEffectsEnabled() const { return _specialEffects; }
    void enableSpecialEffects(bool specialEffects);

    void setAntiAlias(Renderer::EMSAALevel msaaLevel){ _videoSettings.msaaLevel = msaaLevel; }

    const Renderer::RendererSettings & getRendererSettings() const { return _videoSettings; }

    float getMusicVolume() const { return _musicVolume; }
    void setMusicVolume(float musicVolume) { _musicVolume = musicVolume; }
    float getSoundVolume() const { return _soundVolume; }
    void setSoundVolume(float soundVolume) { _soundVolume = soundVolume; }

    ESelectedControls getSelectedControls() const { return _selectedControls;}
    void setSelectedControls(ESelectedControls selectedControls);

    int32 getSelectedJoystick() const { return _selectedJoystick; }
    void setSelectedJoystick(int32 index) { _selectedJoystick = index; }

    void getMovementKeyboardControls(   int32 & up,
                                        int32 & down,
                                        int32 & left,
                                        int32 & right) const;
    void getShootingKeyboardControls(   int32 & up,
                                        int32 & down,
                                        int32 & left,
                                        int32 & right,
                                        int32 & timeWarp,
                                        int32 & fire) const;

    void setKeyboardValue(EControlDirection cd, int32 value);

    bool getKeyboardAutoFire() const { return _keyboardAutoFire; }
    void setKeyboardAutoFire(bool autoFire) { _keyboardAutoFire = autoFire; }

    void setDefaultKeyboardMovementConfig();
    void setDefaultKeyboardShootingConfig();
    
    void getShootingMouseControls(EMouseButton & shoot, EMouseButton & timeWarp) const;
    void setMouseValue(EMouseControl mouseControl, EMouseButton mouseButton);

    bool getMouseAutoFire() const { return _mouseAutoFire; }
    void setMouseAutoFire(bool autoFire) { _mouseAutoFire = autoFire; }

    JoySettings getJoySettings(const String & joyName) const;
    void setJoySettings(const String & joyName, const JoySettings & settings) { _joySettings[joyName] = settings; }

    void setDefaultMouseSettings();

    void importXML(const Ptr<Core::XMLNode> & pXMLNode);
    void exportXML(const Ptr<Core::XMLNode> & pXMLNode) const;

    void cleanupSettings(const Window::InputManager & input);

private:
    void switchKeyboardControls(EControlDirection cd, int32 value, int32 oldValue);
    
private:
    Core::ELanguage             _language;
    String                      _playerName;
    int32                       _currentLevel;

    Renderer::RendererSettings  _videoSettings;
    bool                        _specialEffects;
    float                       _musicVolume;
    float                       _soundVolume;

    ESelectedControls           _selectedControls;
    int32                       _selectedJoystick;

    int32                       _keyboardUp;
    int32                       _keyboardDown;
    int32                       _keyboardLeft;
    int32                       _keyboardRight;

    EMouseButton                _mouseShootButton;
    EMouseButton                _mouseTimeWarpButton;

    bool                        _mouseAutoFire;

    int32                       _keyboardShootUp;
    int32                       _keyboardShootDown;
    int32                       _keyboardShootLeft;
    int32                       _keyboardShootRight;
    int32                       _keyboardFire;
    int32                       _keyboardShootTimeWarp;
    bool                        _keyboardAutoFire;

    Core::Map<String, JoySettings>    _joySettings;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GAMESETTINGS_H_*/