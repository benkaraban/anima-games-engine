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
#ifndef CORE_TEXTSTREAM_H_
#define CORE_TEXTSTREAM_H_

#include <Core/Standard.h>
#include <Core/NonCopyable.h>
#include <Core/IO/Streams.h>

namespace Core
{
/**
 * Cette classe permet d'interpréter une stream comme du texte.
 * Elle sert de filtre avec une autre stream. 
 * 
 * La classe ignore les caractères LF ajoutés sous Windows, elle
 * interprète donc correctement les fichiers UNIX et les fichiers
 * Windows. 
 * \ingroup IO
 */
class LM_API_COR TextInputStream : public Object, public NonCopyable
{
public:
    /**
     * Construit la stream à partir d'une autre. Toutes les lectures
     * sont faites à partir de la stream passée en argument.
     */
    TextInputStream(const Ptr<InputStream> & pStream);
    
    /**
     * Détruit la stream et ferme la stream associée.
     */
    ~TextInputStream();

    /**
     * Retourne une ligne de texte.
     */
    void getLine(String & string);

    /**
     * Retourne une ligne de texte.
     */
    String getLine();
    
    /**
     * Retourne true si toutes les lignes de la stream ont été lues.
     */
    virtual bool eof() const;

    /**
     * Ferme la stream associée.
     */
    virtual void close();

private:
    TextInputStream();
    
    Ptr<InputStream>    _pStream;
};
//-----------------------------------------------------------------------------
/**
 * Cette classe permet d'écrire dans une stream interprétée comme du texte.
 * 
 * La classe peut générer du texte au format UNIX ou Windows (avec un LF supplémentaire
 * en fin de ligne). Les fins de lignes insérées explicitement via putLine()
 * ou endLine() sont dans le format appropriée. De plus, les chaînes passées à
 * la stream sont elles aussi analysées et ce sont les caractères appropriés
 * qui sont utilisés. 
 * \ingroup IO
 */
class LM_API_COR TextOutputStream : public Object, public NonCopyable
{
public:
    /**
     * Construit la stream à partir d'une autre. Toutes les écritures sont 
     * interprétées en texte et transmises à cette stream. Si windowMode,
     * ce sont des paires CR+LF qui sont utilisées pour les fins de lignes. Ce 
     * sont simplement des caractères CR (à la UNIX) dans le cas contraire.
     */
    TextOutputStream(const Ptr<OutputStream> & pStream, bool windowMode);
    ~TextOutputStream();

    /**
     * Ajoute \a string dans la stream.
     */
    void put(const String & string);
    
    /**
     * Ajoute \a string dans la stream, suivie d'un saut de ligne.
     * La nature exacte du saut de ligne dépend de la manière dont
     * est ouverte la stream.
     */
    void putLine(const String & string);
    
    /**
     * Ajoute un saut de ligne dans la stream.
     * La nature exacte du saut de ligne dépend de la manière dont
     * est ouverte la stream.
     */
    void endLine();
    
    /**
     * Fait un flush sur la stream associée.
     */
    virtual void flush();
    
    /**
     * Ferme la stream associée.
     */
    virtual void close();

    /**
     * Ajoute \a element à la stream, après l'avoir interprété comme du texte
     * avec la fonction toString. 
     */
    template <class TElement>
    TextOutputStream & operator << (const TElement & element);
    
private:
    TextOutputStream();
    
    Ptr<OutputStream>    _pStream;
    bool  _windowMode;
};
//-----------------------------------------------------------------------------
template <class TElement>
TextOutputStream & TextOutputStream::operator << (const TElement & element)
{
    put(Core::toString(element));
    return *this;
}
//-----------------------------------------------------------------------------
}

#endif /*CORE_TEXTSTREAM_H_*/
