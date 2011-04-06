#ifndef EXT_WINSAFE_H_
#define EXT_WINSAFE_H_

/**
 * Ce fichier header permet d'inclure le header windows.h sans tout en limitant
 * la pollution de macros inutiles qui peuvent entrer en conflit avec le reste.
 */
#define _WINSOCKAPI_

#include <Guiddef.h>
#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <shlobj.h>
#include <Winsock2.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#ifdef INCLUDE_WIN_BULLSHIT
#include <wbemidl.h>
#endif

#include <dxgi.h>

#include <winclean.h>

#endif /*EXT_WINSAFE_H_*/
