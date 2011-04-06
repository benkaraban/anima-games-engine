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
#ifndef CORE_STREAMS_H_
#define CORE_STREAMS_H_

#include <Core/Standard.h>

namespace Core
{
class Serializable;

/**
 * \defgroup IO Package IO
 * Ce package regroupe les classes dédiées aux entrées/sorties.
 * 
 * \section Streams Streams
 * 
 * Tout le système des entrées/sorties repose sur le principe de flux. Les classes
 * Core::InputStream et Core::OutputStream offrent respectivement des interfaces 
 * communes pour les flux de lecture et d'écriture, qu'il s'agisse de fichiers,
 * de zones mémoire, de socket ou d'autre chose.
 * 
 * Ces interfaces communes permettent de découpler le code du medium utilisé
 * et garantissent une plus grande souplesse des composants qui les utilisent.
 * De fait, il faut passer par ces interfaces abstraites et éviter autant que
 * possible de lier "en dur" certaines parties du code à un type de stream 
 * particulier. 
 * 
 * \section Filtres Filtres
 * 
 * Outre les streams qui représentent un flux de lecture ou d'écriture, il
 * existe des streams de type filtre qui modifient les données qu'elles recoivent
 * avant de les transmettre à une autre stream qui peut à son tour les
 * transformer ou les traiter. On peut ainsi imaginer des streams qui assurent
 * le cryptage ou la compression des données.
 * 
 * \ingroup Core
 */

/**
 * Cette exception est levée en cas d'erreur sur une opération d'entrée/sortie.
 * \ingroup IO
 */
class LM_API_COR IOException : public Exception 
{
public:
    /**
     * Construit l'exception à partir d'un message d'erreur.
     */
    IOException(const String & message) : Exception(message) {};
};

//-----------------------------------------------------------------------------
/**
 * Cette classe abstraite définit l'interface commune à tous les flux d'entrées.
 * 
 * Elle permet de lire tous les types de base, ainsi que des blocs de données
 * de taille arbitraire.
 * 
 * Pour lire un bloc de données, les méthodes read() et readUpTo() sont adaptées mais
 * de manières différentes. On utilise read() quand on connaît à l'avance la taille
 * des données qu'on veut lire car la méthode lève une Core::IOException si
 * le flux ne contient plus assez de données pour cette lecture. C'est la méthode
 * à utiliser quand on décode un flux et qu'on s'attend à obtenir un format particulier.
 * 
 * A l'inverse, la méthode readUpTo() permet de lire des blocs de données sans
 * trop se soucier de la manière de les interpréter. En particulier, cette méthode
 * permet d'itérer sur une stream dont on ne peut pas connaître la taille à l'avance.
 * 
 * Dans tous les cas, la méthode eof() permet de savoir si l'on a atteint la fin de
 * la stream et dans les deux cas, c'est une erreur de lire dans une stream qui est
 * en eof. 
 * 
 * L'exemple ci-dessous illustre les deux manières de lire une stream : 
 * 
 * \code
 * // Lit le contenu d'une liste d'entiers dans une stream.
 * // La stream doit contenir le nombre d'élément puis le contenu
 * // de la liste. 
 * void readList(InputStream & input, List<int32> & list)
 * {
 *     int32 size = input.getInt32();
 *     list.resize(size);
 * 
 *     foreach(iCur, list)
 *     {
 *         *iCur = input.getInt32();
 *     }
 * }
 * 
 * void copyStream(InputStream & input, OutputStream & output)
 * {
 *     static const int32 BUFFER_SIZE = 100;
 * 
 *     byte  buffer[BUFFER_SIZE];
 * 
 *     int32 readCount = input.readUpTo(buffer, BUFFER_SIZE);
 *     output.write(buffer, readCount);
 * 
 *     while(!input.eof())
 *     {
 *         readCount = input.readUpTo(buffer, BUFFER_SIZE);
 *         output.write(buffer, readCount);
 *     }
 * }
 * \endcode
 * 
 * Les méthodes readInt8(), readUInt8(), etc... suivent le même principe que la méthode
 * read() : si on lit un type précis, c'est qu'on s'attend à recevoir une taille précise
 * et c'est donc une erreur d'obtenir moins de données que ce qu'on attendait. Toutes
 * ces méthodes sont implémentées dans la classe InputStream et utilisent la lecture
 * de données par la méthode read(), les classes filles n'ont donc pas à toutes les
 * réimplémenter, il leur suffit de définir la méthode read().
 * 
 * \warning Les méthodes readIntXXX() dépendent de l'endianess de la machine.  
 * 
 * \ingroup IO 
 */
class LM_API_COR InputStream
{
public:
    /**
     * Ce destructeur virtuel est défini pour garantir la destruction correcte
     * des classes filles.
     */
    virtual ~InputStream() {};
    
    /**
     * Récupère \a size octets de la stream et les copie dans buffer.
     * Attention : le buffer doit avoir une taille suffisante. La
     * stream est supposée contenir au moins \a size octets. Si ce n'est pas
     * le cas la méthode lève une exception de type IOException.
     */
    virtual void readData(void * buffer, int32 size) = 0;
    
    /**
     * Récupère jusqu'à \a size octets de la stream et les copie dans le buffer.
     * Attention : le buffer doit avoir une taille suffisante. 
     * 
     * Le fonction retourne le nombre
     * d'octets effectivement lus. Ce nombre peut être inférieur au nombre 
     * d'octets demandés s'il s'agit des derniers octets de la stream. Dans ce
     * cas le flag EOF du fichier est mis à vrai. Appeler cette méthode alors
     * que ce flag a déjà été positionné lève une exception de type Core::IOException.
     */
    virtual int32 readUpTo(void * buffer, int32 size) = 0;
    
    /*
     * Les méthodes suivantes ne sont pas abstraites car par défaut elles sont
     * définies à partir de la méthode read précédente, qui prend un pointeur
     * en argument. Elles sont néanmoins virtuelles, ce qui permet à une
     * classe de les redéfinir. 
     */
    virtual bool readBool();
    virtual byte readByte();
    virtual int8 readInt8();
    virtual uint8 readUInt8();
    virtual int16 readInt16();
    virtual uint16 readUInt16();
    virtual int32 readInt32();
    virtual uint32 readUInt32();
    virtual int64 readInt64();
    virtual uint64 readUInt64();
    virtual String readString();
    virtual String readStringSafe(int32 maxSize);

    /*
     * Cette syntaxe est parfois moins pratique que la précédente, à base
     * de fonctions qui retournent des valeurs, mais elle permet d'être 
     * symètrique avec la syntaxe des 'write' des OutputStream, ce qui 
     * permet de simplifier l'écriture des méthodes de lecture et 
     * d'écriture à de simples copier/coller/remplacer.
     */
    virtual void read(bool & value);
    virtual void read(int8 & value);
    virtual void read(uint8 & value);
    virtual void read(int16 & value);
    virtual void read(uint16 & value);
    virtual void read(int32 & value);
    virtual void read(uint32 & value);
    virtual void read(int64 & value);
    virtual void read(uint64 & value);
    virtual void read(float & value);
    virtual void read(double & value);
    virtual void read(String & value);
    virtual void readSafe(String & value, int32 maxSize);
    virtual void read(Serializable & value);
    
    /**
     * Retourne true si toutes les données de la stream ont été lues.
     */
    virtual bool eof() const = 0;
    
    /**
     * Ferme la stream. Le résultat de l'opération dépend de la nature exacte de la stream
     * mais d'une manière générale toutes les ressources associées à la lecture de la
     * stream doivent être libérées.
     */
    virtual void close() = 0;
};
//-----------------------------------------------------------------------------
/**
 * Cette classe abstraite définit l'interface commune à tous les flux de sortie.

 * Elle permet de lire tous les types de base, ainsi que des blocs de données
 * de taille arbitraire.
 * 
 * Les méthodes readInt8(), readUInt8(), etc... sont implémentées dans la classe 
 * OutputStream et utilisent la lecture
 * de données par la méthode read(), les classes filles n'ont donc pas à toutes les
 * réimplémenter, il leur suffit de définir la méthode write().
 * 
 * \warning Les méthodes readIntXXX() dépendent de l'endianess de la machine.  
 *
 * \ingroup IO 
 */
class LM_API_COR OutputStream
{
public:
    /**
     * Ce destructeur virtuel est défini pour garantir la destruction correcte
     * des classes filles.
     */
    virtual ~OutputStream() {};
    
    /**
     * Ecrit \a size octets dans la stream. Les données à écrire sont lues à
     * partir de l'adresse \a buffer.
     */ 
    virtual void writeData(const void * buffer, int32 size) = 0;
    
    /*
     * Les méthodes suivantes ne sont pas abstraites car par défaut elles sont
     * définies à partir de la méthode write précédente, qui prend un pointeur
     * en argument. Elles sont néanmoins virtuelles, ce qui permet à une
     * classe de les redéfinir. 
     */
    
    /**
     * Ecrit une valeur dans la stream. Cette méthode lève une Core::IOException
     * si la stream est fermée ou si une erreur survient lors de l'écriture
     * dans la stream.
     */
    virtual void write(bool value);
    virtual void write(int8 value);
    virtual void write(uint8 value);
    virtual void write(int16 value);
    virtual void write(uint16 value);
    virtual void write(int32 value);
    virtual void write(uint32 value);
    virtual void write(int64 value);
    virtual void write(uint64 value);
    virtual void write(float value);
    virtual void write(double value);
    virtual void write(const String & value);
    virtual void write(const Serializable & value);

    void writeUInt8(int32 value) { write(uint8(value)); }
    void writeInt8(int32 value) { write(int8(value)); }
    void writeUInt16(int32 value) { write(uint16(value)); }
    void writeInt16(int32 value) { write(int16(value)); }
    
    /**
     * D'une manière générale, cette méthode permet de s'assurer que toutes
     * les données envoyées sont effectivement écrites et ne sont pas conservées
     * dans un tampon par exemple. L'effet exact dépend de la classe fille.
     */
    virtual void flush() = 0;
    
    /**
     * Ferme la stream. Le résultat de l'opération dépend de la nature exacte de la stream
     * mais d'une manière générale toutes les ressources associées à l'écriture de la
     * stream doivent être libérées.
     */
    virtual void close() = 0;
};

//-----------------------------------------------------------------------------
/**
 * Cette interface est commune à tous les objets sérialisables.
 * Un objet sérialisable est un objet qui peut être lu ou écrit dans
 * une stream, indépendamment du type exact de stream.
 * \ingroup IO
 */
class LM_API_COR Serializable
{
public:
    virtual ~Serializable(){}
    /**
     * Initialise l'objet à partir d'une stream.
     */
    virtual void read(InputStream & inputStream) = 0;
    
    /**
     * Sérialise l'objet dans une stream.
     */
    virtual void write(OutputStream & outputStream) const = 0;
};

//-----------------------------------------------------------------------------
enum ESeekOrigin {
    SEEK_ORIGIN_SET,     // <=> début du flux
    SEEK_ORIGIN_CUR,     // <=> à partir de la position courante
    SEEK_ORIGIN_END      // <=> fin du flux
};

LM_API_COR bool isValid(ESeekOrigin value);

/**
 * Cette classe abstraite définit une interface pour les streams qui peuvent
 * être parcourues de manière aléatoire. Pour être parcourue de cette manière,
 * la stream doit avoir une longueur fixe.
 * 
 * \ingroup IO
 */
class LM_API_COR Seekable
{
public:   
    /**
     * Retourne la position courante dans le flux. La position est exprimée
     * en octets à partir du début du flux. La première position est la 
     * position zéro.
     */    
    virtual int64 getPosition() const = 0;
    
    /**
     * Retourne la longueur totale du flux, en octets.
     */    
    virtual int64 getLength() const = 0;
    
    /**
     * Modifie la position courante du flux. Toutes les lectures et écritures
     * ultèrieures se feront à partir de cette position. L'offset est ajouté à l'origine.
     * Si \a origin vaut SEEK_ORIGIN_SET alors \a offset représente la position en valeur
     * absolue, il faut donc qu'il soit positif ou nul. Si \a origin vaut SEEK_ORIGIN_END,
     * \a offset est ajouté à la position de fin de flux. Dans ce cas, offset devrait être 
     * négatif : avec une valeur de -1, on obtient le dernier octet de la stream. Enfin,
     * si \a origin vaut SEEK_ORIGIN_CURRENT, \a offset est ajouté à la position
     * courante, il peut être aussi bien négatif que positif.
     */
    virtual void setPosition(int64 offset, ESeekOrigin origin) = 0;
    
    /**
     * Positionne le curseur courant au début de la stream.
     * C'est équivalent à setPosition(0, SEEK_SET);
     */    
    virtual void rewind() = 0;
};

/**
 * Interface pour les flux de lecture dans lesquels on peut déplacer le curseur de lecture.
 * Les classes concrêtes qui supportent ces deux interfaces pourraient bien
 * en hériter directement mais l'existence d'une classe abstraite qui hérite des
 * deux permet de déclarer des pointeurs vers des classes abstraites qui supportent
 * les deux interfaces.
 * 
 * \ingroup IO
 */
class LM_API_COR SeekableInputStream : public InputStream, public Seekable
{};

/**
 * Interface pour les flux qui supportent à la fois les lecture et les écritures.
 * Les classes concrêtes qui supportent ces deux interfaces pourraient bien
 * en hériter directement mais l'existence d'une classe abstraite qui hérite des
 * deux permet de déclarer des pointeurs vers des classes abstraites qui supportent
 * les deux interfaces.
 * 
 * \ingroup IO
 */
class LM_API_COR InputOutputStream : public InputStream, public OutputStream
{};

#define LM_POD_STREAM(type) \
    inline void read(Core::InputStream & input, type & data) \
    { \
        input.readData(&data, sizeof(type)); \
    } \
    inline void write(Core::OutputStream & output, const type & data) \
    { \
        output.writeData(&data, sizeof(type)); \
    }

// Ces fonctions permettent d'utiliser la même syntaxe pour les types
// de base, les types sérialisés, et les fonctions rajoutées après coup,
// ce qui simplifie l'écriture des 
inline void read(InputStream & input, bool & value) { input.read(value); };
inline void read(InputStream & input, int8 & value) { input.read(value); };
inline void read(InputStream & input, uint8 & value) { input.read(value); };
inline void read(InputStream & input, int16 & value) { input.read(value); };
inline void read(InputStream & input, uint16 & value) { input.read(value); };
inline void read(InputStream & input, int32 & value) { input.read(value); };
inline void read(InputStream & input, uint32 & value) { input.read(value); };
inline void read(InputStream & input, int64 & value) { input.read(value); };
inline void read(InputStream & input, uint64 & value) { input.read(value); };
inline void read(InputStream & input, float & value) { input.read(value); };
inline void read(InputStream & input, double & value) { input.read(value); };
inline void read(InputStream & input, String & value) { input.read(value); };
inline void read(InputStream & input, Serializable & value) { input.read(value); };

inline void write(OutputStream & output, bool value) { output.write(value); };
inline void write(OutputStream & output, int8 value) { output.write(value); };
inline void write(OutputStream & output, uint8 value) { output.write(value); };
inline void write(OutputStream & output, int16 value) { output.write(value); };
inline void write(OutputStream & output, uint16 value) { output.write(value); };
inline void write(OutputStream & output, int32 value) { output.write(value); };
inline void write(OutputStream & output, uint32 value) { output.write(value); };
inline void write(OutputStream & output, int64 value) { output.write(value); };
inline void write(OutputStream & output, uint64 value) { output.write(value); };
inline void write(OutputStream & output, float value) { output.write(value); };
inline void write(OutputStream & output, double value) { output.write(value); };
inline void write(OutputStream & output, String value) { output.write(value); };
inline void write(OutputStream & output, const Serializable & value) { output.write(value); };
};

#endif /*CORE_STREAMS_H_*/
