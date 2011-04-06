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
#include "HttpRequest.h"
#include <Web/WebException.h>
#include <curl/curl.h>
#include <Core/List.h>

#ifndef WIN32
#define LM_USE_CURLOPT_VERBOSE (0)
#endif

namespace Web
{
//-----------------------------------------------------------------------------
static size_t
curlWriteCallback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    LM_ASSERT (userp!=NULL);

    String8 *string = (String8 *) userp;
    string->reserve(size * nmemb);

    string->append((char *) ptr, size * nmemb);

    return (size * nmemb);
}
//-----------------------------------------------------------------------------
Uri::Uri()
{
}
//-----------------------------------------------------------------------------
Uri::Uri(const String8& strUri)
{
    Core::List<String8> list;
	String8::size_type  lastPos; 
    String8::size_type  pos;

    //query param
    list.clear();
    lastPos = strUri.find('?');
    if (lastPos!=String8::npos)
    {
	    ++lastPos;
	    pos = strUri.find_first_of("&=", lastPos);
        
	    while (String8::npos != pos || String8::npos != lastPos)
	    {
		    list.push_back(strUri.substr(lastPos, pos - lastPos));
		    lastPos = strUri.find_first_not_of("&=", pos);
		    pos = strUri.find_first_of("&=", lastPos);
	    }

	    LM_ASSERT(list.size() % 2 == 0);

        for (int32 ii=0; ii<list.size(); ii+=2)
        {
            addQueryParam(doUrlDecoding(list[ii]), doUrlDecoding(list[ii+1]));
        }
    }


    //fragment param
    list.clear();
    lastPos = strUri.find('#');
    if (lastPos!=String8::npos)
    {
        ++lastPos;
        pos = strUri.find_first_of("&=", lastPos);
        while (String8::npos != pos || String8::npos != lastPos)
	    {
		    list.push_back(strUri.substr(lastPos, pos - lastPos));
		    lastPos = strUri.find_first_not_of("&=", pos);
		    pos = strUri.find_first_of("&=", lastPos);
	    }

	    LM_ASSERT(list.size() % 2 == 0);

        for (int32 ii=0; ii<list.size(); ii+=2)
        {
            addFragmentParam(doUrlDecoding(list[ii]), doUrlDecoding(list[ii+1]));
        }
    }


    //base URI
    lastPos = strUri.find('?');
    if (String8::npos !=lastPos)
        baseUri = strUri.substr(0, lastPos);
    else
    {
        lastPos = strUri.find('#');
        if (String8::npos != lastPos)
            baseUri = strUri.substr(0, lastPos);
        else
            baseUri = strUri;
    }
}
//-----------------------------------------------------------------------------
String8 doUrlEncoding(const String8& in)
{
    String8 ret;
    char* p = curl_escape(in.c_str(), (int)in.size());

    if(!p)
    {
        String message(L"doUrlEncoding : unable to escape the string");
        throw WebException(message);
    }
    else
    {
        ret = p;
        curl_free(p);
    }

    return ret;
}
//-----------------------------------------------------------------------------
String8 doUrlDecoding(const String8& in)
{
    String8 ret;
    char* p = curl_unescape(in.c_str(), (int)in.size());
    if (!p)
    {
        String message(L"doUrlDecoding : unable to unescape the string");
        throw WebException(message);
    }
    else
    {
        ret = p;
        curl_free(p);
    }
    return ret;
}
//-----------------------------------------------------------------------------
String8 Uri::getUri() const
{
    LM_ASSERT(!baseUri.empty());

    String8 uri;

    uri = baseUri;

    if(queryParams.size() > 0)
	{
		uri += '?';

        Core::Map<String8, String8>::const_iterator it = queryParams.begin();

        uri += doUrlEncoding(it->first) + '=' + doUrlEncoding(it->second);
	    ++it;

		while(it != queryParams.end())
		{
			uri += '&';
			uri += doUrlEncoding(it->first) + '=' + doUrlEncoding(it->second);
			++it;
		}
	}

    //TODO faire fragment param

    return uri;
}
//-----------------------------------------------------------------------------
String8 Uri::getPost() const
{
    //on copie les parametres
    Core::Map<String8, String8> sortedQParams (queryParams);

	// on construit le post
    String8 post;
    if (sortedQParams.size()>0)
    {
        Core::Map<String8, String8>::const_iterator it = sortedQParams.begin();

        post += doUrlEncoding(it->first) + '=' + doUrlEncoding(it->second);
        ++it;

	    while(it != sortedQParams.end())
	    {
		    post += '&';
		    post += doUrlEncoding(it->first) + '=' + doUrlEncoding(it->second);
		    ++it;
	    }
    }

	return post;
}
//-----------------------------------------------------------------------------
void Uri::addQueryParam(const String8& name, const String8& value)
{
    queryParams.insert(std::pair<String8, String8>(name, value));
}
//-----------------------------------------------------------------------------
void Uri::addFragmentParam(const String8& name, const String8& value)
{
    fragmentParams.insert(std::pair<String8, String8>(name, value));
}
//-----------------------------------------------------------------------------
void Uri::clear()
{
    baseUri.clear();
    queryParams.clear();
    fragmentParams.clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HttpRequest::HttpRequest() : 
    _pCurl(NULL)
{
}
//-----------------------------------------------------------------------------
void HttpRequest::initialize()
{
    _pCurl = curl_easy_init();

    if (!_pCurl)
    {
        String message(L"HttpRequest : Error when trying to curl_easy_init() a handle");
        throw WebException(message);
    }
}
//-----------------------------------------------------------------------------
void HttpRequest::finalize()
{
    LM_ASSERT(_pCurl!=NULL);
    curl_easy_cleanup(_pCurl);
}
//-----------------------------------------------------------------------------
void HttpRequest::reset()
{
    LM_ASSERT(_pCurl!=NULL);
    curl_easy_reset(_pCurl);
}
//-----------------------------------------------------------------------------
String8 HttpRequest::makeRequest(const Uri& uri)
{
    LM_ASSERT(_pCurl!=NULL);

    String8 storage;

    curl_easy_setopt(_pCurl, CURLOPT_URL,               uri.baseUri.c_str());
    curl_easy_setopt(_pCurl, CURLOPT_WRITEFUNCTION,     curlWriteCallback);
    curl_easy_setopt(_pCurl, CURLOPT_WRITEDATA,         &storage);

    String8 post = uri.getPost();
    if(post != "")
    {
        curl_easy_setopt(_pCurl, CURLOPT_POST, true);
        curl_easy_setopt(_pCurl, CURLOPT_COPYPOSTFIELDS, post.c_str()); 
    }

    curl_easy_setopt(_pCurl, CURLOPT_VERBOSE, LM_USE_CURLOPT_VERBOSE);

    if(curl_easy_perform(_pCurl)!=0)
    {
        String message(L"HttpRequest : Error when trying to curl_easy_perform()");
        throw WebException(message);
    }


    return storage;
}
//-----------------------------------------------------------------------------
void HttpRequest::setOption(EHttpRequestOption opt)
{
    LM_ASSERT(_pCurl!=NULL);
    switch(opt)
    {
        case HTTP_REQUEST_SSL_NOT_VERIFY_SERVER:    curl_easy_setopt(_pCurl, CURLOPT_SSL_VERIFYPEER, false); break;
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}  //namespace Social
