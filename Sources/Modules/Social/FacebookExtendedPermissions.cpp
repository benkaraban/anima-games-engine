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
#include "FacebookExtendedPermissions.h"
#include "Social/SocialException.h"

namespace Social
{
//-----------------------------------------------------------------------------
String8 getExtendedPermissionString(EFBExtendedPermission p)
{
    String8 ret;

    switch(p)
    {
    case EP_PUBLISH_STREAM:                 ret="publish_stream"; break;
	case EP_CREATE_EVENT:                   ret="create_event"; break;
	case EP_RSVP_EVENT:                     ret="rsvp_event"; break;
	case EP_SMS:                            ret="sms"; break;
	case EP_OFFLINE_ACCESS:                 ret="offline_access"; break;
	case EP_MANAGE_PAGES:                   ret="manage_pages"; break;
	case EP_EMAIL:                          ret="email"; break;
	case EP_READ_INSIGHTS:                  ret="read_insights"; break;
	case EP_READ_STREAM:                    ret="read_stream"; break;
	case EP_READ_MAILBOX:                   ret="read_mailbox"; break;
	case EP_ADS_MANAGEMENT:                 ret="ads_management"; break;
	case EP_XMPP_LOGIN:                     ret="xmpp_login"; break;
	case EP_USER_ABOUT_ME:                  ret="user_about_me"; break;
	case EP_USER_ACTIVITIES:                ret="user_activities"; break;
	case EP_USER_BIRTHDAY:                  ret="user_birthday"; break;
	case EP_USER_EDUCATION_HISTORY:         ret="user_education_history"; break;
	case EP_USER_EVENTS:                    ret="user_events"; break;
	case EP_USER_GROUPS:                    ret="user_groups"; break;
	case EP_USER_HOMETOWN:                  ret="user_hometown"; break;
	case EP_USER_INTERESTS:                 ret="user_interests"; break;
	case EP_USER_LIKES:                     ret="user_likes"; break;
	case EP_USER_LOCATION:                  ret="user_location"; break;
	case EP_USER_NOTES:                     ret="user_notes"; break;
	case EP_USER_ONLINE_PRESCENCE:          ret="user_online_presence"; break;
	case EP_USER_PHOTO_VIDEO_TAGS:          ret="user_photo_video_tags"; break;
	case EP_USER_PHOTOS:                    ret="user_photos"; break;
	case EP_USER_RELATIONSHIPS:             ret="user_relationships"; break;
	case EP_USER_RELIGION_POLITICS:         ret="user_religion_politics"; break;
	case EP_USER_STATUS:                    ret="user_status"; break;
	case EP_USER_VIDEOS:                    ret="user_videos"; break;
	case EP_USER_WEBSITE:                   ret="user_website"; break;
	case EP_USER_WORK_HISTORY:              ret="user_work_history"; break;
	case EP_READ_FRIENDLISTS:               ret="read_friendlists"; break;
	case EP_READ_REQUESTS:                  ret="read_requests"; break;
	case EP_FRIENDS_ABOUT_ME:               ret="friends_about_me"; break;
	case EP_FRIENDS_ACTIVITIES:             ret="friends_activities"; break;
	case EP_FRIENDS_BIRTHDAY:               ret="friends_birthday"; break;
	case EP_FRIENDS_EDUCATION_HISTORY:      ret="friends_education_history"; break;
	case EP_FRIENDS_EVENTS:                 ret="friends_events"; break;
	case EP_FRIENDS_GROUPS:                 ret="friends_groups"; break;
	case EP_FRIENDS_HOMETOWN:               ret="friends_hometown"; break;
	case EP_FRIENDS_INTERESTS:              ret="friends_interests"; break;
	case EP_FRIENDS_LIKES:                  ret="friends_likes"; break;
	case EP_FRIENDS_LOCATION:               ret="friends_location"; break;
	case EP_FRIENDS_NOTES:                  ret="friends_notes"; break;
	case EP_FRIENDS_ONLINE_PRESCENCE:       ret="friends_online_presence"; break;
	case EP_FRIENDS_PHOTO_VIDEO_TAGS:       ret="friends_photo_video_tags"; break;
	case EP_FRIENDS_PHOTOS:                 ret="friends_photos"; break;
	case EP_FRIENDS_RELATIONSHIPS:          ret="friends_relationships"; break;
	case EP_FRIENDS_RELIGION_POLITICS:      ret="friends_religion_politics"; break;
	case EP_FRIENDS_STATUS:                 ret="friends_status"; break;
	case EP_FRIENDS_VIDEOS:                 ret="friends_videos"; break;
	case EP_FRIENDS_WEBSITE:                ret="friends_website"; break;
    case EP_FRIENDS_WORK_HISTORY:           ret="user_work_history"; break;
    default:
        String message(L"getExtendedPermissionString : Invalid Argument");
        throw SocialException(message);
    }

    return ret;
}
//-----------------------------------------------------------------------------
FacebookExtendedPermission::FacebookExtendedPermission()
{
}
//-----------------------------------------------------------------------------
void FacebookExtendedPermission::addPermission(EFBExtendedPermission permission)
{
	_flags.set(permission, true);
}
//-----------------------------------------------------------------------------
bool FacebookExtendedPermission::permissionsRequested() const
{
    return (0 != _flags.count());
} 
//-----------------------------------------------------------------------------
String8 FacebookExtendedPermission::getPermissionsString() const
{
    String8 ret;

    for(int32 ii = 0; ii<EFBExtendedPermission_COUNT; ++ii)
    {
        if(_flags[ii])
        {
            if(ret!="")
                ret	+= ",";

            ret += getExtendedPermissionString((EFBExtendedPermission)ii);
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
void FacebookExtendedPermission::clear()
{
    _flags.reset();
}
//-----------------------------------------------------------------------------
} // namespace Social

