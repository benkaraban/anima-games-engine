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
#ifndef HOOUSERPROTOCOL_MAINTENANCEMESSAGE_H_
#define HOOUSERPROTOCOL_MAINTENANCEMESSAGE_H_

#include <HOOUserProtocol/IUserMessage.h>

namespace HOOUserProtocol
{

LM_ENUM_2(EMaintenanceType, MAINTENANCE_USER_ACCOUNT,
                            MAINTENANCE_SERVER);

class LM_API_HUP MaintenanceMessageAnswer : public Core::Object, public IUserAnswer
{
public:
    MaintenanceMessageAnswer()
    :   _maintenanceType(MAINTENANCE_SERVER),
        _minutesBeforeShutdown(0)
    {}

    MaintenanceMessageAnswer(EMaintenanceType maintenanceType, int32 minutesBeforeShutdown)
    :   _maintenanceType(maintenanceType),
        _minutesBeforeShutdown(minutesBeforeShutdown)
    {}

    virtual ~MaintenanceMessageAnswer(){}

    virtual EUserAnswerType getType() const { return USER_MAINTENANCE_MESSAGE_ANSWER; }

    int32 getMinutesBeforeShutdown() const { return _minutesBeforeShutdown; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    EMaintenanceType    _maintenanceType;
    int32               _minutesBeforeShutdown;
};

}//namespace HOOUserProtocol

#endif/*HOOUSERPROTOCOL_MAINTENANCEMESSAGE_H_*/