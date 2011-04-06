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

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

namespace Core
{
/**
 * Octet. Pour les calculs numériques, ce type est traité comme un entier
 * non signé sur 8 bits. En pratique le type "byte" est souvent utilisé
 * dans des tableau pour exprimer le fait qu'on considère qu'il s'agit
 * d'un bloc de données brutes, qu'il faut interprêter. C'est généralement
 * le type à utiliser pour faire de l'arithmétique de pointeurs puisqu'il
 * permet de calculer les adresses à l'octet près.
 * \see BYTE_MIN BYTE_MAX
 * \ingroup Standard
 */
typedef unsigned char byte;

/**
 * Entier signé sur 8 bits. Ce type ne doit être utilisé
 * que lorsque la taille du type a vraiment une importance, par exemple
 * pour les entrées/sorties. En règle générale, c'est plutôt le type int32
 * qui doit être utilisé.
 * \see INT8_MIN INT8_MAX
 * \ingroup Standard
 */
typedef signed char int8;

/**
 * Entier non signé sur 8 bits. Ce type ne doit être utilisé
 * que lorsque la taille du type a vraiment une importance, par exemple
 * pour les entrées/sorties. En règle générale, c'est plutôt le type int32
 * qui doit être utilisé.
 * \see UINT8_MIN UINT8_MAX
 * \ingroup Standard
 */
typedef unsigned char uint8;

/**
 * Entier signé sur 16 bits. Ce type ne doit être utilisé
 * que lorsque la taille du type a vraiment une importance, par exemple
 * pour les entrées/sorties. En règle générale, c'est plutôt le type int32
 * qui doit être utilisé.
 * \see INT16_MIN INT16_MAX
 * \ingroup Standard
 */
typedef signed short int16;

/**
 * Entier non signé sur 16 bits. Ce type ne doit être utilisé
 * que lorsque la taille du type a vraiment une importance, par exemple
 * pour les entrées/sorties. En règle générale, c'est plutôt le type int32
 * qui doit être utilisé.
 * \see UINT16_MIN UINT16_MAX
 * \ingroup Standard
 */
typedef unsigned short uint16;

/**
 * Entier signé sur 32 bits. C'est le type standard qui doit être
 * utilisé par défaut, quand il n'y a pas de raison particulière d'utiliser
 * un autre type.
 * \see INT32_MIN INT32_MAX
 * \ingroup Standard
 */
typedef signed int int32;

/**
 * Entier non signé sur 32 bits. Ce type ne doit être utilisé
 * que lorsque la taille du type a vraiment une importance, par exemple
 * pour les entrées/sorties. En règle générale, c'est plutôt le type int32
 * qui doit être utilisé.
 * \see UINT32_MIN UINT32_MAX
 * \ingroup Standard
 */
typedef unsigned int uint32;

/**
 * Entier signé sur 64 bits. Ce type ne doit être utilisé
 * que lorsque la taille du type a vraiment une importance, par exemple
 * pour les entrées/sorties. En règle générale, c'est plutôt le type int32
 * qui doit être utilisé.
 * \see INT64_MIN INT64_MAX
 * \ingroup Standard
 */
typedef signed long long int64;

/**
 * Entier non signé sur 64 bits. Ce type ne doit être utilisé
 * que lorsque la taille du type a vraiment une importance, par exemple
 * pour les entrées/sorties. En règle générale, c'est plutôt le type int32
 * qui doit être utilisé.
 * \see UINT64_MIN UINT64_MAX
 * \ingroup Standard
 */
typedef unsigned long long uint64;

const byte BYTE_MIN = 0;
const byte BYTE_MAX = 0xff;
const int8 INT8_MIN = (-128);
const int8 INT8_MAX = 127;
const uint8 UINT8_MIN = 0;
const uint8 UINT8_MAX = 0xff;
const int16 INT16_MIN = (-32768);
const int16 INT16_MAX = 32767;
const uint16 UINT16_MIN = 0;
const uint16 UINT16_MAX = 0xffff;
const int32 INT32_MIN = (-2147483647 - 1);
const int32 INT32_MAX = 2147483647;
const uint32 UINT32_MIN = 0;
const uint32 UINT32_MAX = 0xffffffff;
const int64 INT64_MIN = (-9223372036854775807LL - 1);
const int64 INT64_MAX = 9223372036854775807LL;
const uint64 UINT64_MIN = 0;
const uint64 UINT64_MAX = 0xffffffffffffffffuLL;
};

#endif

