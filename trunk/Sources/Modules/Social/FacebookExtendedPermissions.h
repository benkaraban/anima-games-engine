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
#ifndef SOCIAL_FACEBOOK_EXTENDEDPERMISSIONS_H_
#define SOCIAL_FACEBOOK_EXTENDEDPERMISSIONS_H_

#include <Core/Standard.h>
#include <bitset>

namespace Social
{
//-----------------------------------------------------------------------------
LM_ENUM_54 (EFBExtendedPermission,
	EP_PUBLISH_STREAM,
	EP_CREATE_EVENT,
	EP_RSVP_EVENT,
	EP_SMS,
	EP_OFFLINE_ACCESS,
	EP_MANAGE_PAGES,
	EP_EMAIL,
	EP_READ_INSIGHTS,
	EP_READ_STREAM,
	EP_READ_MAILBOX,
	EP_ADS_MANAGEMENT,
	EP_XMPP_LOGIN,
	EP_USER_ABOUT_ME,
	EP_USER_ACTIVITIES,
	EP_USER_BIRTHDAY,
	EP_USER_EDUCATION_HISTORY,
	EP_USER_EVENTS,
	EP_USER_GROUPS,
	EP_USER_HOMETOWN,
	EP_USER_INTERESTS,
	EP_USER_LIKES,
	EP_USER_LOCATION,
	EP_USER_NOTES,
	EP_USER_ONLINE_PRESCENCE,
	EP_USER_PHOTO_VIDEO_TAGS,
	EP_USER_PHOTOS,
	EP_USER_RELATIONSHIPS,
	EP_USER_RELIGION_POLITICS,
	EP_USER_STATUS,
	EP_USER_VIDEOS,
	EP_USER_WEBSITE,
	EP_USER_WORK_HISTORY,
	EP_READ_FRIENDLISTS,
	EP_READ_REQUESTS,
	EP_FRIENDS_ABOUT_ME,
	EP_FRIENDS_ACTIVITIES,
	EP_FRIENDS_BIRTHDAY,
	EP_FRIENDS_EDUCATION_HISTORY,
	EP_FRIENDS_EVENTS,
	EP_FRIENDS_GROUPS,
	EP_FRIENDS_HOMETOWN,
	EP_FRIENDS_INTERESTS,
	EP_FRIENDS_LIKES,
	EP_FRIENDS_LOCATION,
	EP_FRIENDS_NOTES,
	EP_FRIENDS_ONLINE_PRESCENCE,
	EP_FRIENDS_PHOTO_VIDEO_TAGS,
	EP_FRIENDS_PHOTOS,
	EP_FRIENDS_RELATIONSHIPS,
	EP_FRIENDS_RELIGION_POLITICS,
	EP_FRIENDS_STATUS,
	EP_FRIENDS_VIDEOS,
	EP_FRIENDS_WEBSITE,
	EP_FRIENDS_WORK_HISTORY);
//-----------------------------------------------------------------------------
class LM_API_SCL FacebookExtendedPermission
{
public:
    FacebookExtendedPermission();

	void addPermission(EFBExtendedPermission permission);
	bool permissionsRequested() const;
	String8 getPermissionsString() const;
	void clear();

private:
    std::bitset<EFBExtendedPermission_COUNT> _flags;
};
//-----------------------------------------------------------------------------
} // namespace Social

#endif //SOCIAL_FACEBOOK_EXTENDEDPERMISSIONS_H_