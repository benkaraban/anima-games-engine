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
#include "AudioGlobals.h"

namespace Audio
{

LPOVCLEAR           ov_clear = NULL;
LPOVREAD            ov_read = NULL;
LPOVPCMTOTAL        ov_pcm_total = NULL;
LPOVINFO            ov_info = NULL;
LPOVCOMMENT         ov_comment = NULL;
LPOVOPENCALLBACKS   ov_open_callbacks = NULL;
LPOVTIMESEEK        ov_time_seek = NULL;

bool vorbisFileLoaded = false;
// Ogg Voribis DLL Handle
HINSTANCE vorbisFileDLL = NULL;

void initVorbisFileDll()
{
    if (vorbisFileLoaded)
		return;

	// Try and load Vorbis DLLs (VorbisFile.dll will load ogg.dll and vorbis.dll)
	vorbisFileDLL = LoadLibrary("vorbisfile.dll");
	if (vorbisFileDLL)
	{
		ov_clear = (LPOVCLEAR)GetProcAddress(vorbisFileDLL, "ov_clear");
		ov_read = (LPOVREAD)GetProcAddress(vorbisFileDLL, "ov_read");
		ov_pcm_total = (LPOVPCMTOTAL)GetProcAddress(vorbisFileDLL, "ov_pcm_total");
		ov_info = (LPOVINFO)GetProcAddress(vorbisFileDLL, "ov_info");
		ov_comment = (LPOVCOMMENT)GetProcAddress(vorbisFileDLL, "ov_comment");
		ov_open_callbacks = (LPOVOPENCALLBACKS)GetProcAddress(vorbisFileDLL, "ov_open_callbacks");
        ov_time_seek = (LPOVTIMESEEK)GetProcAddress(vorbisFileDLL, "ov_time_seek");

		if (ov_clear && ov_read && ov_pcm_total && ov_info && ov_comment && ov_open_callbacks && ov_time_seek)
		{
			vorbisFileLoaded = true;
		}
	}
}

void shutdownVorbisFileDll()
{
    if (vorbisFileLoaded)
	{
		FreeLibrary(vorbisFileDLL);
		vorbisFileDLL = NULL;
	}
	vorbisFileLoaded = false;
}

}//namespace Audio