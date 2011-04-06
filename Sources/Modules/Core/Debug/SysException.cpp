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
#include <Core/Debug/CallStack.h>
#include <Core/Debug/SysException.h>
#include <Core/Logger.h>

namespace Core
{
#ifdef _WIN32

int filterExceptions(unsigned int code, struct _EXCEPTION_POINTERS *ep)
{
    bool handleException = true;

    switch(code)
    {
    case EXCEPTION_ACCESS_VIOLATION:        ERR << L"EXCEPTION_ACCESS_VIOLATION\n"; break;
    case EXCEPTION_DATATYPE_MISALIGNMENT:   ERR << L"EXCEPTION_DATATYPE_MISALIGNMENT\n"; break;
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:   ERR << L"EXCEPTION_ARRAY_BOUNDS_EXCEEDED\n"; break;
    case EXCEPTION_FLT_DENORMAL_OPERAND:    ERR << L"EXCEPTION_FLT_DENORMAL_OPERAND\n"; break;
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:      ERR << L"EXCEPTION_FLT_DIVIDE_BY_ZERO\n"; break;
    case EXCEPTION_FLT_INEXACT_RESULT:      ERR << L"EXCEPTION_FLT_INEXACT_RESULT\n"; break;
    case EXCEPTION_FLT_INVALID_OPERATION:   ERR << L"EXCEPTION_FLT_INVALID_OPERATION\n"; break;
    case EXCEPTION_FLT_OVERFLOW:            ERR << L"EXCEPTION_FLT_OVERFLOW\n"; break;
    case EXCEPTION_FLT_STACK_CHECK:         ERR << L"EXCEPTION_FLT_STACK_CHECK\n"; break;
    case EXCEPTION_FLT_UNDERFLOW:           ERR << L"EXCEPTION_FLT_UNDERFLOW\n"; break;
    case EXCEPTION_INT_DIVIDE_BY_ZERO:      ERR << L"EXCEPTION_INT_DIVIDE_BY_ZERO\n"; break;
    case EXCEPTION_INT_OVERFLOW:            ERR << L"EXCEPTION_INT_OVERFLOW\n"; break;
    case EXCEPTION_PRIV_INSTRUCTION:        ERR << L"EXCEPTION_PRIV_INSTRUCTION\n"; break;
    case EXCEPTION_IN_PAGE_ERROR:           ERR << L"EXCEPTION_IN_PAGE_ERROR\n"; break;
    case EXCEPTION_ILLEGAL_INSTRUCTION:     ERR << L"EXCEPTION_ILLEGAL_INSTRUCTION\n"; break;
    case EXCEPTION_STACK_OVERFLOW:          ERR << L"EXCEPTION_STACK_OVERFLOW\n"; break;
    default:
        handleException = false;
        break;
    }

    if(handleException) 
    {
        Core::dumpSystemExceptionCallStack(ep->ContextRecord);
        return EXCEPTION_CONTINUE_SEARCH;
        // TODO : gérer le cas sans débuger
    }
    else 
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }
}

#else

#endif
}
