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
#include "Mail.h"
#include <curl/curl.h>
#include <Web/WebException.h>
#include <Core/Strings/Tools.h>

#include <string.h>

#ifndef WIN32
#define LM_USE_CURLOPT_VERBOSE (0)
#endif


namespace Web
{
 
struct WriteThis {
  String msg;
  bool isSended;
};


static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
  struct WriteThis *pooh = (struct WriteThis *)userp;

  if(size*nmemb < 1)
    return 0;

  if (pooh->isSended == true) return 0;

  String8 msg8 (pooh->msg);
    size_t len = msg8.length();
    memcpy(ptr, msg8.c_str(), len);
    pooh->isSended = true;
    return len;

  return 0;
}


//-----------------------------------------------------------------------------
Mail::Mail()
{
}
//-----------------------------------------------------------------------------
String Mail::generate() const
{
    String msg;
    if (from!=L"")       msg += L"from:" + from + L"\n";
    if (replyTo!=L"")    msg += L"reply-to:" + replyTo + L"\n";
    if (subject!=L"")    msg += L"subject:" + subject + L"\n";

    msg += body;

    return msg;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MailSender::MailSender(const String& smtpServer, const String& userName, const String& password, const String& port):
    _smtpServer(L"smtp://" + smtpServer),
    _userName(userName),
    _password(password),
    _smtpPort(L":" + port)
{
}
//-----------------------------------------------------------------------------
MailSender::~MailSender()
{
}
//-----------------------------------------------------------------------------
void MailSender::sendMail(const Mail& mail, const String& sender, const String& receiver)
{
    if (!Core::isValidMail(sender) || !Core::isValidMail(receiver))
    {
        String message(L"mail not valid");
        throw WebException(message);
    }

    CURL* pCurl = curl_easy_init();

    if (!pCurl)
    {
        String message(L" Error when trying to curl_easy_init() a handle");
        throw WebException(message);
    }

    struct WriteThis pooh;
    struct curl_slist* rcpt_list = NULL;
 
    pooh.msg      = mail.generate();
    pooh.isSended = false;

    String from = L"<" + sender + L">";

    rcpt_list = curl_slist_append(rcpt_list, String8(receiver).c_str());
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl_easy_setopt(pCurl, CURLOPT_URL, String8(_smtpServer).c_str());
    curl_easy_setopt(pCurl, CURLOPT_USERNAME, String8(_userName).c_str());
    curl_easy_setopt(pCurl, CURLOPT_PASSWORD, String8(_password).c_str());
    curl_easy_setopt(pCurl, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(pCurl, CURLOPT_MAIL_FROM, String8(from).c_str());
    curl_easy_setopt(pCurl, CURLOPT_MAIL_RCPT, rcpt_list);
    curl_easy_setopt(pCurl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER,0);
    curl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(pCurl, CURLOPT_READDATA, &pooh);
    curl_easy_setopt(pCurl, CURLOPT_VERBOSE, LM_USE_CURLOPT_VERBOSE);
    curl_easy_setopt(pCurl, CURLOPT_SSLVERSION, 0);
    curl_easy_setopt(pCurl, CURLOPT_SSL_SESSIONID_CACHE, 0);

    if(curl_easy_perform(pCurl)!=0)
    {
        String message(L"Error when trying to curl_easy_perform()");
        throw WebException(message);
    }

    curl_slist_free_all(rcpt_list);
    curl_easy_cleanup(pCurl);
}
//-----------------------------------------------------------------------------
}
