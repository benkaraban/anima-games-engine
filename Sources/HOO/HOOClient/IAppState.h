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
#ifndef HOOCLIENT_IAPPSTATE_H_
#define HOOCLIENT_IAPPSTATE_H_

#include <Core/Dictionary.h>
#include <Core/VFS/VirtualFileSystem.h>

#include <Audio/IDevice.h>
#include <Audio/IDriver.h>

#include <Gfx/IDriver.h>
#include <Gfx/IDevice.h>

#include <Renderer/IRenderer.h>

#include <GUI/Manager.h>

#include <Universe/World.h>

#include <Window/Window.h>
#include <Window/WinHandler.h>
#include <Window/InputManager.h>

#include <GameEngine/GameLibrary.h>

#include <HOOUserProtocol/UserAccountInfos.h>

#include <HOOClient/GUI/WidgetFactory.h>
#include <HOOClient/Network/Connexion.h>
#include <HOOClient/InGame/ClientGame.h>
#include <HOOClient/HOOConfig.h>
#include <HOOClient/OutGameMusic.h>

LM_ENUM_2(EAppState, APPSTATE_OUTGAME, APPSTATE_INGAME);

struct AppContext
{
    Ptr<Window::Window>             pWindow;
    Ptr<Window::WinHandler>         pWinHandler;
    Ptr<Window::InputManager>       pInput;
    
    Ptr<Core::VirtualFileSystem>    pVFS;
    HOOConfig                       config;

    String                          serverAddress;

#ifdef LM_DEVMODE
    bool                            debugAutoConnect;
    bool                            debugAutoMatch;
    bool                            debugQuickLoad;
    bool                            debugFullscreen;
#endif

    Core::ELanguage                 language;
    Ptr<Core::Dictionary>           pDico;
    
    Ptr<Gfx::IDriver>               pGfxDriver;
    Ptr<Renderer::IRenderer>        pRenderer;
    
    Ptr<Audio::IDriver>             pAudioDriver;
    Ptr<Audio::IDevice>             pAudioDevice;

    Ptr<GUI::Manager>               pGUI;
    Ptr<WidgetFactory>              pWidgets;

    Ptr<Universe::RessourcePool>    pMasterResPool;
    Ptr<Universe::World>            pWorldGUI;
    Ptr<OutGameMusic>               pOutGameMusic;
    Ptr<Universe::NodeCamera>       pCameraGUI;
    Ptr<Universe::NodeSkeleton>     pAvatarGUI;

    Ptr<GameEngine::GameLibrary>    pGameLib;
    Ptr<Connexion>                  pConnexion;
    bool                            mustExit;
    EAppState                       nextState;

    String                              login;
    HOOUserProtocol::UserAccountInfos   account;

    ClientGame                      game;
    int32                           playerId;
};

class IAppState : public Core::Object
{
public:
    virtual ~IAppState() {};

    virtual EAppState getState() const = 0;

    virtual void update(double elapsed) = 0;
    virtual void render() = 0;

    virtual void startState() {}
    virtual void endState() {}
};

#endif
