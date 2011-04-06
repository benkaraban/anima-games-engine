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
#include "Session.h"

Session::Session()
:   _idSession(-1),
    _opened(false),
    _messageProcessed(0),
    _totalSizeReceived(0)
{
}

void Session::close()
{
    _opened = false;
}

void Session::open()
{
    if(!isOpened())
    {
        _opened = true;
    }
    else
        throw Core::Exception(L"Attempting to open an opened session.");
}

void Session::processMessage(const BenchNetworkMessage & message, BenchNetworkMessage & answer, bool & broadcast)
{
    _messageProcessed++;

    _totalSizeReceived += message._networkMessage.length();

    if(message._networkMessageType == MESSAGE_UNICAST)
    {
        answer._networkMessageType = MESSAGE_UNICAST;
        answer._networkMessage = 
L"Do you worry that you`re not liked \
How long till you break \
You`re happy cause you smile \
But how much can you fake \
An ordinary boy, an ordinary name \
But ordinary`s just not good enough today \
 \
Ow ah oooh, ow ah oooh, ow ah oooh (2x) \
 \
Alone, alone, alone, alone, alone, alone, alone, alone... I`m thinking \
 \
Why-e-i-e-i-e yeah, Superman`s dead \
Why-e-i-e-i-e yeah, is it in my head \
Why-e-i-e-i-e yeah, we`ll just laugh instead, \
Why, worry `bout the weather, whether not you should hate \
 \
Are you worried about your faith \
Kneel down and obey \
You`reh appy you`re in love \
You need someone to hate \
An ordinary girl, an ordinary waist \
But ordinary's just not good enough today \
 \
Alone, alone, alone, alone, alone, alone, alone, alone... I`m thinking \
 \
Doesn`t anybody ever know \
Doesn`t anybody ever know \
Doesn`t anybody ever know \
That the world`s a subway, subway \
Doesn`t anybody ever know \
Doesn`t anybody ever know \
Doesn`t anybody ever know \
That the world`s a subway, subway \
That the world`s a subway, subway";
        broadcast = false;
    }
    else if(message._networkMessageType == MESSAGE_BROADCAST)
    {
        answer._networkMessageType = MESSAGE_UNICAST;
        answer._networkMessage = 
L"Do you worry that you`re not liked \
How long till you break \
You`re happy cause you smile \
But how much can you fake \
An ordinary boy, an ordinary name \
But ordinary`s just not good enough today \
 \
Ow ah oooh, ow ah oooh, ow ah oooh (2x) \
 \
Alone, alone, alone, alone, alone, alone, alone, alone... I`m thinking \
 \
Why-e-i-e-i-e yeah, Superman`s dead \
Why-e-i-e-i-e yeah, is it in my head \
Why-e-i-e-i-e yeah, we`ll just laugh instead, \
Why, worry `bout the weather, whether not you should hate \
 \
Are you worried about your faith \
Kneel down and obey \
You`reh appy you`re in love \
You need someone to hate \
An ordinary girl, an ordinary waist \
But ordinary's just not good enough today \
 \
Alone, alone, alone, alone, alone, alone, alone, alone... I`m thinking \
 \
Doesn`t anybody ever know \
Doesn`t anybody ever know \
Doesn`t anybody ever know \
That the world`s a subway, subway \
Doesn`t anybody ever know \
Doesn`t anybody ever know \
Doesn`t anybody ever know \
That the world`s a subway, subway \
That the world`s a subway, subway";

        broadcast = true;
    }
    else
    {
        throw Core::Exception(L"Stop test!");
    }
}