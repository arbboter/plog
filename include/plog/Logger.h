#pragma once
#include <plog/Appenders/IAppender.h>
#include <plog/Util.h>
#include <vector>

#ifndef PLOG_DEFAULT_INSTANCE
#   define PLOG_DEFAULT_INSTANCE 0
#endif

namespace plog
{
    template<int instance>
    class Logger : public util::Singleton<Logger<instance> >, public IAppender
    {
    public:
        Logger(Severity maxSeverity = none) : m_maxSeverity(maxSeverity)
        {
        }

        Logger& addAppender(IAppender* appender)
        {
            assert(appender != this);
            m_appenders.push_back(appender);
            return *this;
        }

        Severity getMaxSeverity() const
        {
            return m_maxSeverity;
        }

        void setMaxSeverity(Severity severity)
        {
            m_maxSeverity = severity;
        }

        bool checkSeverity(Severity severity) const
        {
            return severity <= m_maxSeverity;
        }

        virtual void write(const Record& record)
        {
            if (checkSeverity(record.getSeverity()))
            {
                *this += record;
            }
        }

        void operator+=(const Record& record)
        {
            for (std::vector<IAppender*>::iterator it = m_appenders.begin(); it != m_appenders.end(); ++it)
            {
                (*it)->write(record);
            }
            unsigned int nTid = plog::util::gettid();
            m_mapLastLog[nTid] = record.getMessage();
        }
        void setLastMeg(const Record& record)
        {
            if(this == NULL)
            {
                return;
            }
            unsigned int nTid = plog::util::gettid();
            m_mapLastLog[nTid] = record.getMessage();
        }
        util::nstring getLastMsg()
        {
            unsigned int nTid = plog::util::gettid();
            if((this==NULL) || (m_mapLastLog.find(nTid)==m_mapLastLog.end()))
            {
#ifdef _WIN32
                return L"";
#else
                return "";
#endif
            }
            return m_mapLastLog[nTid];
        }
        std::string getLastMsgA()
        {
            unsigned int nTid = plog::util::gettid();
            if((this==NULL) || (m_mapLastLog.find(nTid)==m_mapLastLog.end()))
            {
                return "";
            }
            util::nstring strMsg = getLastMsg();
#if _WIN32
            _bstr_t t = strMsg.c_str();
            const char* pchar = (const char*)t;
            string result = pchar;
            return result;
#else
            return strMsg.c_str();
#endif
        }

    private:
        Severity m_maxSeverity;
        std::vector<IAppender*> m_appenders;
        map<unsigned int, util::nstring> m_mapLastLog;
    };

    template<int instance>
    inline Logger<instance>* get()
    {
        return Logger<instance>::getInstance();
    }

    inline Logger<PLOG_DEFAULT_INSTANCE>* get()
    {
        return Logger<PLOG_DEFAULT_INSTANCE>::getInstance();
    }
}
