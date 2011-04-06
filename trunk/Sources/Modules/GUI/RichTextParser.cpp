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
#include <Core/Strings/Tools.h>
#include <GUI/RichTextParser.h>

namespace GUI
{
//-------------------------------------------------------------------------
LM_ENUM_3(ERTParsingState, RTPARSE_TEXT, RTPARSE_START_TAG, RTPARSE_TAG);
//-------------------------------------------------------------------------
void interpretToken(RichTextToken & token)
{
    Core::List<String> tokens;
    Core::tokenize(token.text, tokens, L" \n\r\t");

    if(tokens.empty() || tokens[0].empty())
        throw Core::Exception(L"empty token");

    token.tag = tokens[0];

    if(tokens[0][0] == L'/')
    {
        token.type = RT_TAG_STOP;
        token.tag = token.text.slice(1);
    }
    else if(tokens[0] == L"b")
        token.type = RT_TAG_B;
    else if(tokens[0] == L"i")
        token.type = RT_TAG_I;
    else if(tokens[0] == L"n")
        token.type = RT_TAG_N;
    else if(tokens[0] == L"img")
    {
        token.type = RT_TAG_IMG;

        if(tokens.size() != 2)
            throw Core::Exception(L"incorrect img token");
        else 
            token.text = tokens[1];
    }
    else if(tokens[0] == L"color")
    {
        token.type = RT_TAG_COLOR;

        if(tokens.size() != 5 && tokens.size() != 2)
            throw Core::Exception(L"incorrect color token");
        else if(tokens.size() == 2)
        {
            if(tokens[1].size() != 8 && tokens[1].size() != 6)
                throw Core::Exception(L"incorrect color hex token");

            token.color.r = Core::toIntegerFromHex(tokens[1].slice(0, 1)) / 255.0f;
            token.color.g = Core::toIntegerFromHex(tokens[1].slice(2, 3)) / 255.0f;
            token.color.b = Core::toIntegerFromHex(tokens[1].slice(4, 5)) / 255.0f;

            if(tokens[1].size() == 8)
                token.color.a = Core::toIntegerFromHex(tokens[1].slice(6, 7)) / 255.0f;
            else
                token.color.a = 1.0f;
        }
        else
        {
            token.color.r = float(Core::toFloat(tokens[1]));
            token.color.g = float(Core::toFloat(tokens[2]));
            token.color.b = float(Core::toFloat(tokens[3]));
            token.color.a = float(Core::toFloat(tokens[4]));
        }
    }
    else if(tokens[0] == L"size")
    {
        token.type = RT_TAG_SIZE;

        if(tokens.size() != 2)
            throw Core::Exception(L"incorrect size token");
        else 
            token.value = Core::toInteger(tokens[1]);
    }
    else if(tokens[0] == L"rsize")
    {
        token.type = RT_TAG_RSIZE;

        if(tokens.size() != 2)
            throw Core::Exception(L"incorrect rsize token");
        else 
            token.value = Core::toInteger(tokens[1]);
    }
    else if(tokens[0] == L"font")
    {
        token.type = RT_TAG_FONT;

        if(tokens.size() != 2)
            throw Core::Exception(L"incorrect font token");
        else 
            token.text = tokens[1];
    }
    else if(tokens[0] == L"style")
    {
        token.type = RT_TAG_STYLE;

        if(tokens.size() != 2)
            throw Core::Exception(L"incorrect style token");
        else 
            token.text = tokens[1];
    }
    else if(tokens[0] == L"align")
    {
        token.type = RT_TAG_ALIGN;

        if(tokens.size() != 2)
            throw Core::Exception(L"incorrect align token");
        else if(tokens[1] == L"left")
            token.hLayout = HL_LEFT;
        else if(tokens[1] == L"right")
            token.hLayout = HL_RIGHT;
        else if(tokens[1] == L"center")
            token.hLayout = HL_CENTER;
        else
            throw Core::Exception(L"incorrect align token");
    }
    else if(tokens[0] == L"valign")
    {
        token.type = RT_TAG_ALIGN;

        if(tokens.size() != 2)
            throw Core::Exception(L"incorrect valign token");
        else if(tokens[1] == L"up")
            token.vLayout = VL_UP;
        else if(tokens[1] == L"down")
            token.vLayout = VL_DOWN;
        else if(tokens[1] == L"center")
            token.vLayout = VL_CENTER;
        else
            throw Core::Exception(L"incorrect align token");
    }
}
//-------------------------------------------------------------------------
void parseRichText(Core::List<RichTextToken> & tokens, const String & source)
{
    ERTParsingState state;
    RichTextToken   curToken;

    tokens.clear();
    state = RTPARSE_TEXT;
    int32 ii = 0;

    while(ii < source.size())
    {
        wchar_t c = source[ii];

        switch(state)
        {
        case RTPARSE_TEXT:
        {
            if(c == L'{')
                state = RTPARSE_START_TAG;
            else
                curToken.text.push_back(c);
            break;
        } // RTPARSE_TEXT

        case RTPARSE_START_TAG:
        {
            if(c == L'{')
            {
                state = RTPARSE_TEXT;
                curToken.text.push_back(c);
            }
            else
            {
                if(curToken.text.size() > 0)
                {
                    curToken.type = RT_TEXT;
                    tokens.push_back(curToken);
                }

                curToken.text.clear();
                curToken.text.push_back(c);
                state = RTPARSE_TAG;
            }
            break;
        } // RTPARSE_START_TAG

        case RTPARSE_TAG:
        {
            if(c == L'}')
            {
                interpretToken(curToken);
                tokens.push_back(curToken);

                curToken.text.clear();
                state = RTPARSE_TEXT;
            }
            else
            {
                curToken.text.push_back(c);
            }
            break;
        } // RTPARSE_TAG
        }

        ii++;
    }

    if(state != RTPARSE_TEXT)
        throw Core::Exception(L"Unexpected end of rich text string");

    if(curToken.text.size() > 0)
    {
        curToken.type = RT_TEXT;
        tokens.push_back(curToken);
    }
}
//-------------------------------------------------------------------------
}

