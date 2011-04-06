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
#include <Core/Logger.h>
#include <Core/IO/FileStream.h>
#include <Core/System/Files.h>

namespace Core
{
using namespace Core::System;

//-----------------------------------------------------------------------------
LoggerMessage::LoggerMessage() :
    _msg(new Core::BufferStream())
{
}
//-----------------------------------------------------------------------------
LoggerMessage::~LoggerMessage()
{
}
//-----------------------------------------------------------------------------
String LoggerMessage::getLastLine()
{
    String8 string8;
    char    oneChar;
    int32   readBytes = _msg->readUpTo(&oneChar, sizeof(char));
    bool    isEndLine = (oneChar == '\n');

    while(readBytes == sizeof(char) && !isEndLine)
    {
        if(oneChar != '\r')
        {
            string8.push_back(oneChar);
        }

        readBytes = _msg->readUpTo(&oneChar, sizeof(char));
        isEndLine = (oneChar == '\n');
    }

    return String(string8);
}
//-----------------------------------------------------------------------------
String LoggerMessage::computeMessage (const String& loggerTag)
{
    String str;
    String line;

    do
    {
        line = getLastLine();
        str << loggerTag << _tagContainer << line << L"\n";
    }
    while(!_msg->eof());

    _tagContainer = L"";

    return str;
}
//-----------------------------------------------------------------------------
Logger::Logger(Ptr<OutputStream> stream, String name)
{
    Ptr<TextOutputStream > tos  = Ptr<TextOutputStream > (new TextOutputStream (stream, true));
    addStream(tos, LOGGER_DEFAULT_STREAM_NAME);
    
    _name = name;
    _nbMsg = 0;

    _logMsgLevel = Core::L_DEBUG;
    setDefaultMsgLevel(L_INFO);

    _showTagIdMsg   = true;
    _showTagAppTime   = true;
    _showTagLocalTime   = false;
    _showTagThreadId   = false;
    _showTagMsgLevel   = true;
    
    _DBGtoStdOut = true;
}
//-----------------------------------------------------------------------------
Logger::~Logger()
{
}
//-----------------------------------------------------------------------------
void Logger::destroyStream(const String& name)
{
    _streamMap.erase(name);
}
//-----------------------------------------------------------------------------
void Logger::addStream(Ptr<TextOutputStream > tos, const String& name)
{
    _streamMap[name] = tos;
}
//-----------------------------------------------------------------------------
String Logger::getLoggerTag()
{
    String str;

    if (_showTagIdMsg)
    {
        str += Core::toString(_nbMsg, 6);
        str += LOGGER_TAG_SEPARATOR;
    }

    if (_showTagAppTime)
    {
        str += Clock::applicationTime().toString(false, TD_PRINT_HOURS);
        str += LOGGER_TAG_SEPARATOR;
#ifdef _WIN32
        str += Clock::applicationTime().toString(false, TD_PRINT_COUNTER);
        str += LOGGER_TAG_SEPARATOR;
#endif
    }

    if (_showTagLocalTime)
    {
        String time(Core::Clock::localTime().toString().slice(0, 22));
        time[10] = L'_'; // Vire le 'T' tout pourri
        str += time;
        str += LOGGER_TAG_SEPARATOR;
    }

    if (_showTagThreadId)
    {
        str += Core::toStringHex(Thread::getCurrentThreadID(), 4);
        str += LOGGER_TAG_SEPARATOR;
    }

    if (_showTagMsgLevel)
    {
        switch(_msgLevel)
        {
            case L_ERROR:
                str += L"ERR";
            break;
            case L_WARNING:
                str += L"WAR";
            break;
            case L_INFO:
                str += L"INF";
            break;
            case L_DEBUG:
                str += L"DBG";
            break;
        }
        str += LOGGER_TAG_SEPARATOR;
    }

    return str;
}
//-----------------------------------------------------------------------------
void Logger::operator= (LoggerMessage& lm)
{
    String loggerTag = getLoggerTag();
    String msg = lm.computeMessage (loggerTag);
    
#if defined _MSC_VER
    OutputDebugStringW(msg.c_str());
#endif

    if (_DBGtoStdOut == true && _msgLevel == L_DEBUG)
    {
        LM_COUT<<msg;
    }
   
    for(Map<String, Ptr<TextOutputStream > >::iterator it = _streamMap.begin(); it != _streamMap.end(); ++it)
    {       
        (*it->second)<<msg;
        it->second->flush();
    }
    
    

    _nbMsg++;
}
//-----------------------------------------------------------------------------
Logger& Logger::configMessage(ELogMsgLevel eLevel)
{
    setMsgLevel(eLevel);

    return *this;
}
//-----------------------------------------------------------------------------
LoggerManager::LoggerManager()
{
    _defautLogger = getFileName(getFileBaseName(getExecutableName()));
    String defaultFileName = _defautLogger;
    String ext = L".log";

    String  pathFile = getUserLocalAppDir() + L"/AnimaGames/";
    createDirectory(pathFile);
    String  pathAndFileName = pathFile + defaultFileName;

    Ptr<Core::FileOutputStream> defaultStreamLog  (new Core::FileOutputStream(pathAndFileName + ext));

    int32 idLog = 0;
    while (defaultStreamLog->isOpened() == false)
    {
        idLog++;
        defaultStreamLog->open(pathAndFileName + L"_" + Core::toString(idLog) + ext);
    }
    Ptr<Logger > defaultLog = (Ptr<Logger >)new Logger (defaultStreamLog, _defautLogger);

    
    defaultLog->setShowTagIdMsg(LOGGER_DEFAULT_TAG_ID_MSG);
    defaultLog->setShowTagAppTime(LOGGER_DEFAULT_TAG_APP_TIME);
    defaultLog->setShowTagLocalTime(LOGGER_DEFAULT_TAG_LOCAL_TIME);
    defaultLog->setShowTagThreadId(LOGGER_DEFAULT_TAG_THREAD_ID);
    defaultLog->setShowTagMsgLevel(LOGGER_DEFAULT_TAG_MSG_LEVEL);

    defaultLog->setLogMsgLevel(LM_LOGGER_DEFAULT_MSG_LEVEL);

    addLogger(defaultLog);
    
    defaultLog->addStream(Ptr<TextOutputStream > (new TextOutputStream (LM_COUT_PTR, true)), STDCOUT_STREAM_NAME);
    defaultLog->setDBGtoStdOut(false);
}
//-----------------------------------------------------------------------------
LoggerManager::~LoggerManager()
{

}
//-----------------------------------------------------------------------------
void LoggerManager::destroyLogger(const String& name)
{
    _loggerMap.erase(name);
}
//-----------------------------------------------------------------------------
void LoggerManager::addLogger(Ptr<Logger > log)
{
    _loggerMap[log->getName()] = log;
}
//-----------------------------------------------------------------------------
Ptr<Logger > LoggerManager::getLogger(const String& name)
{
    return _loggerMap[name];
}
//-----------------------------------------------------------------------------

LoggerManager& LoggerManager::getInstance()
{
	static LoggerManager sInstance;
    return sInstance;
}
//-----------------------------------------------------------------------------
} // namespace Core
