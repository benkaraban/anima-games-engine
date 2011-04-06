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
#ifdef _WIN32
#include <winsafe.h>
#endif

#include <stdio.h>
#include <stdlib.h>

/**
 * Ce fichier contient les définitions des fonctions appelées lors du chargement et du déchargement
 * de la bibliothèque par une application.
 * Sous Windows DllMain est aussi appelée si un thread est créé (ou se termine) à l'interieur du processus
 * ayant chargée la bibliothèque.  
 */

#ifdef _WIN32
extern "C" BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH :	
			//chargement de la DLL
			if(!lpvReserved)
			{
				//chargement explicit | Run-time dynamic linking | Dynamic load linking
			}
			else
			{
				//chargement implicit | Static load linking | Load-time dynamic linking
			}
			break;
		case DLL_THREAD_ATTACH :	
			//le processus ayant chargé la DLL vient de créer un thread
			//DllMain est appelée dans le contexte de ce nouveau thread
			break;
		case DLL_PROCESS_DETACH :
			//déchargement de la DLL
			//par la fin du processus, par l'appel de la fonction FreeLibrary, ou suite à une erreur de chargement
			if(!lpvReserved)
			{
				// Le chargement à échoué ou il y a eu appel de la fonction FreeLibrary
			}
			else
			{
				// le processus est terminé
			}
			break;
		case DLL_THREAD_DETACH :
			//le processus ayant chargé la DLL vient de terminer un thread
			//DllMain est appelée dans le contexte de ce thread
			break;
	}
	return TRUE;
}
#else
//chargement de la bibliothèque
static void _init()
{
}
//déchargement de la bibliothèque
static void _fini()
{
}
#endif

