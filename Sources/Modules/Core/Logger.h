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
#ifndef LOGGER_H_
#define LOGGER_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Map.h>
#include <Core/RT/Thread.h>
#include <Core/IO/Streams.h>
#include <Core/IO/BufferStream.h>
#include <Core/IO/TextStream.h>
#include <Core/IO/StandardStream.h>
#include <Core/DateAndTime.h>

/**
 * Ce fichier contient les classes nécessaires pour la gestion des fichiers Log
 *
 * L'ensemble des log sont geres par le LoggerManager. Par defaut celui-ci ne contient qu'un fichier Log, celui de l'application.
 * Le message envoyé au log sera écrit ou non en fonction de son niveau de priorité
 * Les quatre niveaux de priorité sont les suivants :
 * L_ERROR      // message critique ne permettant pas au programme de pouvoir continuer son execution
 * L_WARNING    // signale la presence d'une erreur qui à pu etre gere
 * L_INFO       // concerne les messages d'information
 * L_DEBUG      // message utilise pour debugger (en mode Debug ou Release) note : ce type de message ne doit etre utilise que de manière temporaire
 *
 *
 * Pour poster un message :
 * ERR << L"ici le message sera poste dans le log par defaut avec une priorite L_ERROR";
 * WAR << L"ici le message sera poste dans le log par defaut avec une priorite L_WARNING";
 * INF << L"ici le message sera poste dans le log par defaut avec une priorite L_INFO";
 * DBG << L"ici le message sera poste dans le log par defaut avec une priorite L_DEBUG";
 * Note : les messages du type DBG sont également redirigés vers la console
 *
 *
 * Les Tags:
 * un log dispose des tags suivants :
 * IdMsg : le tag contient le numéro du message posté
 * AppTime : linux : le nombre de micro seconde écoulé depuis le lancement de l'application
 *           windows : le temps écoulé à la milliseconde (enfion plutot la dizaine de miliseconde) plus la valeur du QueryPerformanceCounter
 * LocalTime : L'heure actuel sur la machine exécutant l'application
 * ThreadId : l'identifiant du thread exécutant l'écriture du log
 * ModuleName : le nom du module faisant appel au log
 * MsgLevel : la priorité du message (ERR, WAR, INF, DBG)
 * L'utilisateur peut activer ou désactiver chacun de ces tags
 *
 * Il est possible d'ajouter pour un message les tags de "localisation" permettant de situer le message du log dans un des fichiers du programme
 * LM_LOG_TAG_FL : pour obtenir le nom du fichier et la ligne ou le log est appelé
 * LM_LOG_TAG_FUNCTION : permets d'obtenir le  nom de la fonction ou le log est appelé
 *
 * pour utiliser ces tags :
 * ERR << LM_LOG_TAG_FL << LM_LOG_TAG_FUNCTION << L"le programme a généré une erreur";
 *
 * pour activer l'ecriture du log par defaut sur la sortie standard il suffit de faire appel a la fonction : logStdOut_On();
 * De meme logStdOut_Off()
 * exemple: ici le fichier log contiendra les message : 'msg 1', 'msg 2', 'msg 3', et seul le message : 'msg 2' sera également envoye sur la sortie standard
 *
 * INF << L"msg 1";
 * logStdOut_On();
 * INF << L"msg 2";
 * logStdOut_Off();
 * INF << L"msg 3";
 *
 *
 */

#define LM_GET_LOG(strName) (*(Core::LoggerManager::getInstance().getLogger(strName)))
#define LM_LAUNCH_LOG(strName, eLevel) LM_GET_LOG(strName).configMessage(eLevel) = LM_GET_LOG(strName).lm << LM_LOG_TAG_MODULE
#define LM_LAUNCH_LOG_DEFAULT_LEVEL(strName) LM_LAUNCH_LOG(strName, LM_GET_LOG(strName).getDefaultMsgLevel())
#define LM_LOG_LEVEL_TEST(strName, eLevel) LM_GET_LOG(strName).levelIsOk(eLevel)
#define LM_LAUNCH_LOG_IF_LEVEL(strName, eLevel) if (!LM_LOG_LEVEL_TEST(strName, eLevel)) ; else LM_LAUNCH_LOG(strName, eLevel)

#define LM_LOG_CONVERT_TO_STRING_BASE(x) #x
#define LM_LOG_CONVERT_TO_STRING(x) LM_LOG_CONVERT_TO_STRING_BASE(x)
#define LM_LOG_FILELINE __FILE__ ":" LM_LOG_CONVERT_TO_STRING(__LINE__)
#define LM_LOG_LINE LM_LOG_CONVERT_TO_STRING(__LINE__)

#define LM_LOG_TAG_FL Core::Tag(LM_LOG_FILELINE)
#define LM_LOG_TAG_LINE Core::Tag("line : " LM_LOG_LINE)
#define LM_LOG_TAG_FUNCTION Core::Tag(__FUNCTION__)
#define LM_LOG_TAG_PRETTY_FUNCTION Core::Tag(__PRETTY_FUNCTION__)
#define LM_LOG_TAG_MODULE Core::Tag(LM_LOG_MODULE_NAME)
#define LM_LOG_TAG_THREAD_ID Core::Tag(Core::toString(int32(Thread::getCurrentThreadID())))
#define LM_LOG_TAG_TIME Core::Tag(Core::Clock::localTime().toString())

#define ERR LM_LAUNCH_LOG_IF_LEVEL(Core::LoggerManager::getInstance().getDefaultLoggerName(), Core::L_ERROR)
#define WAR LM_LAUNCH_LOG_IF_LEVEL(Core::LoggerManager::getInstance().getDefaultLoggerName(), Core::L_WARNING)
#define INF LM_LAUNCH_LOG_IF_LEVEL(Core::LoggerManager::getInstance().getDefaultLoggerName(), Core::L_INFO)
#define DBG LM_LAUNCH_LOG_IF_LEVEL(Core::LoggerManager::getInstance().getDefaultLoggerName(), Core::L_DEBUG)

namespace Core
{
const Core::String LOGGER_TAG_SEPARATOR (L"|");

const Core::String LOGGER_DEFAULT_STREAM_NAME (L"DefaultStream");

const bool LOGGER_DEFAULT_TAG_ID_MSG = true;
const bool LOGGER_DEFAULT_TAG_APP_TIME = true;
const bool LOGGER_DEFAULT_TAG_LOCAL_TIME = false;
const bool LOGGER_DEFAULT_TAG_THREAD_ID = false;
const bool LOGGER_DEFAULT_TAG_MSG_LEVEL = true;
#define LM_LOGGER_DEFAULT_MSG_LEVEL Core::L_DEBUG
} // namespace Core

namespace Core
{
//-----------------------------------------------------------------------------
LM_ENUM_4 (ELogMsgLevel,
        L_ERROR,
        L_WARNING,
        L_INFO,
        L_DEBUG)
//-----------------------------------------------------------------------------
struct LM_API_COR Tag
{
    Tag(const char* val) { String8 str (val); String b(str); _value = b; }
    Tag(const wchar_t* val = L"") { String str (val); _value = str; }
    Tag(String str) : _value(str) {; }
    String _value;
};
//-----------------------------------------------------------------------------
class LM_API_COR LoggerMessage
{
public:
    LoggerMessage();
    virtual ~LoggerMessage();

    LoggerMessage& operator<<(const Tag& tag)
    {
        _tagContainer += tag._value;
        _tagContainer += LOGGER_TAG_SEPARATOR;

        return *this;
    }

    template<class T>
    LoggerMessage& operator<<(const T& val)
    {
        String str = toString(val);
        String8 string8(str);
        _msg->writeData(string8.c_str(), string8.size());
        return *this;
    }

    //necessaire pour : L"texte"
    LoggerMessage& operator<<(const wchar_t* val)
    {
       String str (val);

       String8 string8(str);
       _msg->writeData(string8.c_str(), string8.size());

       return *this;
    }

    //necessaire pour : "texte"
    LoggerMessage& operator<<(const char* val)
    {
        String8 string8(val);
        _msg->writeData(string8.c_str(), string8.size());

        return *this;
    }

    //gestion des manipulateurs
    LoggerMessage& operator<< (LoggerMessage& (*pfunc)(LoggerMessage&))
    {
        pfunc(*this);
        return *this;
    }

    Ptr<Core::BufferStream> getMsgStream()
    {
        return _msg;
    }

    String computeMessage (const String& loggerTag);

protected:
    String getLastLine();

protected:
    Ptr<Core::BufferStream> _msg;
    String _tagContainer;
};
//-----------------------------------------------------------------------------
class LM_API_COR Logger : public Object
{
public:
    Logger(Ptr<OutputStream> stream, String name);
    ~Logger();

    const String& getName() const { return _name; }
    String getLoggerTag();
    Logger& configMessage(ELogMsgLevel eLevel);
    void operator= (LoggerMessage& lm);
    void setDBGtoStdOut(const bool value) {_DBGtoStdOut = value;};
    //level
    ELogMsgLevel getDefaultMsgLevel() {return _defaultMsgLevel;}
    void setDefaultMsgLevel(const ELogMsgLevel& lvl) {_defaultMsgLevel = lvl;}
    void setMsgLevel(const ELogMsgLevel& lvl) {_msgLevel = lvl;}
    void setLogMsgLevel(ELogMsgLevel lvl) {_logMsgLevel = lvl;}
    bool levelIsOk(ELogMsgLevel lvl) {return lvl<=_logMsgLevel;}

    //tag
    void setShowTagIdMsg(const bool show) {_showTagIdMsg = show;}
    void setShowTagAppTime(const bool show) {_showTagAppTime = show;}
    void setShowTagLocalTime(const bool show) {_showTagLocalTime = show;}
    void setShowTagThreadId(const bool show) {_showTagThreadId = show;}
    void setShowTagMsgLevel(const bool show) {_showTagMsgLevel = show;}

    //stream
    void destroyStream(const String& name);
    void addStream(Ptr<TextOutputStream > tos, const String& name);

public:
    Core::LoggerMessage lm;
protected:
    String _name;
    uint64 _nbMsg;

    ELogMsgLevel _defaultMsgLevel;  //le level de priorité utilisée par défaut
    ELogMsgLevel _msgLevel;         //la priorité du message en cours
    ELogMsgLevel _logMsgLevel;      //le log post les msg ayant une priorité <= à _logMsgLevel


    bool _showTagIdMsg;
    bool _showTagAppTime;
    bool _showTagLocalTime;
    bool _showTagThreadId;

    bool _showTagMsgLevel;

    bool _DBGtoStdOut;

    Map<String, Ptr<TextOutputStream > > _streamMap;
};
//-----------------------------------------------------------------------------
class LM_API_COR LoggerManager : public Object
{
public:
    typedef Map<String, Ptr<Logger > > ContainerLogger_t;


    ~LoggerManager();


    void destroyLogger(const String& name);
    void addLogger(Ptr<Logger > log);
    Ptr<Logger > getLogger(const String& name);
    static LoggerManager& getInstance();

    Ptr<Logger > getDefaultLogger() {return getLogger(_defautLogger);}
    const String& getDefaultLoggerName() {return _defautLogger;}
    void setDefaultLoggerName(const String& str) {_defautLogger = str;}
protected:

    LoggerManager();
    String                 _defautLogger;
    ContainerLogger_t     _loggerMap;
};
//-----------------------------------------------------------------------------
inline String toString(const Logger& log)
{
    String str (log.getName());
    return str;
}
//-----------------------------------------------------------------------------
inline Core::LoggerMessage& endl (Core::LoggerMessage& lm)
{
    char n = '\n';
    lm.getMsgStream()->writeData(&n, 1);

    return lm;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
inline void logStdOut_On()
{
    Core::LoggerManager::getInstance().getDefaultLogger()->addStream(
        Ptr<TextOutputStream > (new TextOutputStream (LM_COUT_PTR, true)),
        STDCOUT_STREAM_NAME);

    Core::LoggerManager::getInstance().getDefaultLogger()->setDBGtoStdOut(false);
}
//-----------------------------------------------------------------------------
inline void logStdOut_Off()
{
    Core::LoggerManager::getInstance().getDefaultLogger()->destroyStream(STDCOUT_STREAM_NAME);
    Core::LoggerManager::getInstance().getDefaultLogger()->setDBGtoStdOut(true);
}
//-----------------------------------------------------------------------------
}//namespace Core

#endif /* LOGGER_H_ */
