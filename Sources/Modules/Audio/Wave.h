/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef AUDIO_WAV_H_
#define AUDIO_WAV_H_

#include <Core/IO/Streams.h>
#include <Core/Buffer.h>

#include <Audio/Playable.h>

namespace Audio
{

class LM_API_AUD WaveHeader : public Core::Object, public Core::Serializable
{
    friend class Wave;
/** Format de l'entête
    FileTypeBlocID  (4 octets) : Constante «RIFF»  (0x52,0x49,0x46,0x46)
    FileSize        (4 octets) : Taille du fichier moins 8 octets
    FileFormatID    (4 octets) : Format = «WAVE»  (0x57,0x41,0x56,0x45)

[Bloc décrivant le format audio]
    FormatBlocID    (4 octets) : Identifiant «fmt »  (0x66,0x6D, 0x74,0x20)
    BlocSize        (4 octets) : Nombre d'octets du bloc - 8  (0x10)

    AudioFormat     (2 octets) : Format du stockage dans le fichier (1: PCM, ...)
    NbrCanaux       (2 octets) : Nombre de canaux (de 1 à 6, cf. ci-dessous)
    Frequence       (4 octets) : Fréquence d'échantillonnage (en Hertz) [Valeurs standardisées : 11025, 22050, 44100 et éventuellement 48000 et 96000]
    BytePerSec      (4 octets) : Nombre d'octets à lire par seconde (i.e., Frequence * BytePerBloc).
    BytePerBloc     (2 octets) : Nombre d'octets par bloc d'échantillonnage (i.e., tous canaux confondus : NbrCanaux * BitsPerSample/8).
    BitsPerSample   (2 octets) : Nombre de bits utilisées pour le codage de chaque échantillon (8, 16, 24)

    //Optional
    ExtraFormatBytes ...

[Bloc des données]
    DataBlocID      (4 octets) : Constante «data»  (0x64,0x61,0x74,0x61)
    DataSize        (4 octets) : Nombre d'octets des données (i.e. "Data[]", i.e. taille_du_fichier - taille_de_l'entête  (qui fait 44 octets normalement).
   */
public:
    WaveHeader();
    virtual ~WaveHeader();

    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

private:
    int8    riffTag[4];//"RIFF"
    uint32  fileSize;
    int8    waveTag[4];//"WAVE"
    int8    formatTag[4];//"fmt "
    int32   blocSize;
    uint16  audioFormat;
    uint16  nbChannels;
    uint32  frequency;
    uint32  bytesPerSec;
    uint16  bytesPerBloc;
    uint16  bitsPerSample;
    int8    dataTag[4];//"data"
    uint32  dataSize;
};

class LM_API_AUD Wave : public Core::Object, public Audio::Playable
{
public:
    Wave();
    virtual ~Wave();

    //Serializable
    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    //Playable
    inline virtual EPlayableType getType() const { return Audio::WAV_TYPE; }
    virtual uint32 getFrequency() const;
    virtual uint32 getNbChannels() const;
    virtual uint32 getBufferSizeFor250ms() const;

    virtual void rewind();
    virtual int32 getData(Core::Buffer & buffer);

    //Specific
    uint16 getBitsPerSample() const;

private:
    WaveHeader      _waveHeader;
    Core::Buffer    _pcmData;

    int32           _offset;
};

}//namespace Audio

#endif/*AUDIO_WAV_H_*/