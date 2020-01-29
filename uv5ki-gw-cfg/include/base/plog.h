//////////////////////////////////////////////////////////////////////////
//  Plog - portable and simple log for C++
//  Documentation and sources: https://github.com/SergiusTheBest/plog
//  License: MPL 2.0, http://mozilla.org/MPL/2.0/
#ifndef _PLog_h_
#define _PLog_h_

#include <fcntl.h>
#include <sys/stat.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <queue>
#include <iomanip>

#ifdef _WIN32
#   include <Windows.h>
#   include <time.h>
#   include <sys/timeb.h>
#   include <io.h>
#else
#   include <unistd.h>
#   include <sys/syscall.h>
#   include <sys/time.h>
#   include <pthread.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	include <errno.h>
#   ifndef __ANDROID__
#       include <iconv.h>
#   endif
#endif

#ifdef _WIN32
#   define _PLOG_NSTR(x)   L##x
#   define PLOG_NSTR(x)    _PLOG_NSTR(x)
#else
#   define PLOG_NSTR(x)    x
#endif

namespace plog
{
//----------------------------------------------------------------------------------
// #include "LogConfig.h"

// #include "Severity.h"
    enum Severity
    {
        none = 0,
        fatal = 1,
        error = 2,
        warning = 3,
        info = 4,
        debug = 5,
        verbose = 6
    };
        
    inline const char* getSeverityName(Severity severity)
    {
        switch (severity)
        {
        case fatal:
            return "FATAL";
        case error:
            return "ERROR";
        case warning:
            return "WARN";
        case info:
            return "INFO";
        case debug:
            return "DEBUG";
        case verbose:
            return "VERB";
        default:
            return "NONE";
        }
    }

	// #include "Util.h"
    namespace util
    {
#ifdef _WIN32
        typedef std::wstring nstring;
        typedef std::wstringstream nstringstream;
#else
        typedef std::string nstring;
        typedef std::stringstream nstringstream;
#endif
		/** */
        inline void localtime_s(struct tm* t, const time_t* time)
        {
#ifdef _WIN32
            ::localtime_s(t, time);
#else
            ::localtime_r(time, t);
#endif
        }

#ifdef _WIN32
        typedef timeb Time;

        inline void ftime(Time* t)
        {
            ::ftime(t);
        }
#else
        struct Time
        {
            time_t time;
            unsigned short millitm;
        };

        inline void ftime(Time* t)
        {
            timeval tv;
            ::gettimeofday(&tv, NULL);

            t->time = tv.tv_sec;
            t->millitm = static_cast<unsigned short>(tv.tv_usec / 1000);
        }
#endif
		/** */
        inline unsigned int gettid()
        {
#ifdef _WIN32
            return ::GetCurrentThreadId();
#elif defined(__unix__)
            return ::syscall(__NR_gettid);
#elif defined(__APPLE__)
            return static_cast<unsigned int>(::syscall(SYS_thread_selfid));
#endif
        }

#ifndef __ANDROID__
        inline std::string toNarrow(const wchar_t* wstr)
        {
            size_t wlen = ::wcslen(wstr);
            std::string str(wlen * sizeof(wchar_t), 0);

            if (!str.empty())
            {
#ifdef _WIN32
                int len = ::WideCharToMultiByte(CP_ACP, 0, wstr, static_cast<int>(wlen), &str[0], static_cast<int>(str.size()), 0, 0);
#else
                const char* in = reinterpret_cast<const char*>(&wstr[0]);
                char* out = &str[0];
                size_t inBytes = wlen * sizeof(wchar_t);
                size_t outBytes = str.size();

                iconv_t cd = ::iconv_open("UTF-8", "WCHAR_T");
                ::iconv(cd, const_cast<char**>(&in), &inBytes, &out, &outBytes);
                ::iconv_close(cd); 
                size_t len = str.size() - outBytes;
#endif
                str.resize(len);
            }

            return str;
        }
#endif

#ifdef _WIN32
        inline std::wstring toWide(const char* str)
        {
            size_t len = ::strlen(str);
            std::wstring wstr(len, 0);

            if (!wstr.empty())
            {
                int wlen = ::MultiByteToWideChar(CP_ACP, 0, str, static_cast<int>(len), &wstr[0], static_cast<int>(wstr.size()));
                wstr.resize(wlen);
            }

            return wstr;
        }

        inline std::string toUTF8(const std::wstring& wstr)
        {
            std::string str(wstr.size() * sizeof(wchar_t), 0);

            if (!str.empty())
            {
                int len = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], static_cast<int>(str.size()), 0, 0);
                str.resize(len);
            }

            return str;
        }

		/** AGL */
		inline std::string toNone(const std::wstring &wstr)
		{
		}
#endif

        inline std::string processFuncName(const char* func)
        {
#if defined(_WIN32) || defined(__OBJC__)
            return std::string(func);
#else
            //const char* funcBegin = func;
            //const char* funcEnd = ::strchr(funcBegin, '(');

            //for (const char* i = funcEnd - 1; i >= funcBegin; --i)
            //{
            //    if (*i == ' ')
            //    {
            //        funcBegin = i + 1;
            //        break;
            //    }
            //}

            //return std::string(funcBegin, funcEnd);
            return std::string(func);
#endif
        }

        inline void splitFileName(const char* fileName, std::string& fileNameNoExt, std::string& fileExt)
        {
            const char* dot = std::strrchr(fileName, '.');

            if (dot)
            {
                fileNameNoExt.assign(fileName, dot);
                fileExt.assign(dot + 1);
            }
            else
            {
                fileNameNoExt.assign(fileName);
                fileExt.clear();
            }
        }

        class NonCopyable
        {
        protected:
            NonCopyable()
            {
            }

        private:
            NonCopyable(const NonCopyable&);
            NonCopyable& operator=(const NonCopyable&);
        };

        class File : NonCopyable
        {
        public:
            File() : m_file(-1)
            {
            }

            File(const char* fileName) : m_file(-1)
            {
                open(fileName);
            }

            ~File()
            {
                close();
            }

            off_t open(const char* fileName)
            {
#ifdef _WIN32
                ::_sopen_s(&m_file, fileName, _O_CREAT | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IREAD | _S_IWRITE);
#else
                m_file = ::open(fileName, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
                return seek(0, SEEK_END);
            }

            int write(const void* buf, size_t count)
            {
#ifdef _WIN32
                return m_file != -1 ? ::_write(m_file, buf, static_cast<unsigned int>(count)) : -1;
#else
                return m_file != -1 ? static_cast<int>(::write(m_file, buf, count)) : -1;
#endif
            }

            template<class CharType>
            int write(const std::basic_string<CharType>& str)
            {
                return write(str.data(), str.size() * sizeof(CharType));
            }

            off_t seek(off_t offset, int whence)
            {
#ifdef _WIN32
                return m_file != -1 ? ::_lseek(m_file, offset, whence) : -1;
#else
                return m_file != -1 ? ::lseek(m_file, offset, whence) : -1;
#endif
            }

            void close()
            {
                if (m_file != -1)
                {
#ifdef _WIN32
                    ::_close(m_file);
#else
                    ::close(m_file);
#endif
                    m_file = -1;
                }
            }

            static int unlink(const char* fileName)
            {
#ifdef _WIN32
                return ::_unlink(fileName);
#else
                return ::unlink(fileName);
#endif
            }

            static int rename(const char* oldFilename, const char* newFilename)
            {
                return ::rename(oldFilename, newFilename);
            }

        private:
            int m_file;
        };

        class Mutex : NonCopyable
        {
        public:
            Mutex()
            {
#ifdef _WIN32
                ::InitializeCriticalSection(&m_sync);
#else
                ::pthread_mutex_init(&m_sync, 0);
#endif
            }

            ~Mutex()
            {
#ifdef _WIN32
                ::DeleteCriticalSection(&m_sync);
#else
                ::pthread_mutex_destroy(&m_sync);
#endif
            }

            friend class MutexLock;

        private:
            void lock()
            {
#ifdef _WIN32
                ::EnterCriticalSection(&m_sync);
#else
                ::pthread_mutex_lock(&m_sync);
#endif
            }

            void unlock()
            {
#ifdef _WIN32
                ::LeaveCriticalSection(&m_sync);
#else
                ::pthread_mutex_unlock(&m_sync);
#endif
            }

        private:
#ifdef _WIN32
            CRITICAL_SECTION m_sync;
#else
            pthread_mutex_t m_sync;
#endif
        };

        class MutexLock : NonCopyable
        {
        public:
            MutexLock(Mutex& mutex) : m_mutex(mutex)
            {
                m_mutex.lock();
            }

            ~MutexLock()
            {
                m_mutex.unlock();
            }

        private:
            Mutex& m_mutex;
        };

        template<class T> 
        class Singleton : NonCopyable
        {
        public:
            Singleton()
            {
                assert(!m_instance);
                m_instance = static_cast<T*>(this);
            }

            ~Singleton()
            {
                assert(m_instance);
                m_instance = 0;
            }

            static T* getInstance()
            {
                return m_instance;
            }

        private:
            static T* m_instance;
        };

        template<class T> 
        T* Singleton<T>::m_instance = NULL;
    }

	static struct stPlogProfile
	{
		bool toFile;
		bool toConsole;
		bool toNetwork;
	} pLogProfiles[]= {
		 {false,false,false}			// plog::none
		,{true, true, true }			// plog::fatal
		,{true, false,true }			// plog::error
		,{true, false,false}			// plog::warning
		,{true, true, false}			// plog::info
		,{false,false,false}			// plog::debug
		,{false,false,false}			// plog::verbose
	};

	/** Para el control de Nueva Configuracion */
#ifdef _WIN32
	static struct _stat _lastMod;
#else
	static struct stat _lastMod;
#endif

	/** */
	class LogConfig
	{
	public:
		LogConfig() {
			_fileformat="csv";
			_filename="plog.csv";
			_filesize=500000;
			_nfiles = 3;
			_ipcshare=false;
			_fileConfig="";

			_udpId="gw31";
			_ipUdp="127.0.0.1";
			_portUdp = 878;
		}
	public:
		inline std::string FileFormat() {return _fileformat;}
		inline std::string FileName() { return _filename;}
		inline unsigned long FileSize() { return _filesize;}
		inline int NumberOfFile() { return _nfiles;}
		inline bool IpcShare() {return _ipcshare;}
		inline std::string IpUpd(){return _ipUdp;}
		inline int PortUdp() { return _portUdp;}
		inline std::string UdpId(){return _udpId;}

	public:
		void Configure(std::string fileconfig)
		{
#ifdef _WIN32
			_stat( fileconfig.c_str(), &plog::_lastMod );
#else
			lstat( fileconfig.c_str(), &plog::_lastMod );
#endif // _WIN32

			_fileConfig = fileconfig;

			std::ifstream infile(fileconfig.c_str());
			std::string line;
			while (std::getline(infile, line))
			{
				if (line=="") continue;
				line.erase(std::remove_if(line.begin(), line.end(), &plog::LogConfig::IsSpace), line.end());
				std::vector<std::string> valores = split(line,'=');
				if (valores.size()==2)
				{
					if (valores[0]=="FILEFORMAT")
					{
						_fileformat=valores[1];
					} 
					else if (valores[0]=="FILENAME")
					{
						_filename = valores[1];
					}
					else if (valores[0]=="FILESIZE")
					{
						_filesize = atoi(valores[1].c_str());
					}
					else if (valores[0]=="NUMOFFILES")
					{
						_nfiles = atoi(valores[1].c_str());
					}
					else if (valores[0]=="IPC_SHARE")
					{
						if (valores[1]=="false")
							_ipcshare = false;
						else if (valores[1]=="true")
							_ipcshare = true;
					}
					else if (valores[0]=="UDP_IP")
					{
						_ipUdp = valores[1];
					}
					else if (valores[0]=="UDP_PORT")
					{
						_portUdp = atoi(valores[1].c_str());
					}
					else if (valores[0]=="UDP_PLOG_ID")
					{
						_udpId = valores[1];
					}
					else if (valores[0]=="TO_STANDARD_OUTPUT")
					{
						pLogProfiles[plog::info].toConsole=false;
						pLogProfiles[plog::error].toConsole=false;
						pLogProfiles[plog::debug].toConsole=false;

						std::vector<std::string> options = split(valores[1],',');
						for (int ival=0; ival < (int )options.size(); ival++)
						{
							if (options[ival]=="INFO")
								pLogProfiles[plog::info].toConsole=true;
							else if (options[ival]=="DEBUG")
								pLogProfiles[plog::debug].toConsole=true;
							else if (options[ival]=="ERROR")
								pLogProfiles[plog::error].toConsole=true;
						}
					}
					else if (valores[0]=="TO_FILE")
					{
						pLogProfiles[plog::info].toFile=false;
						pLogProfiles[plog::error].toFile=false;
						pLogProfiles[plog::debug].toFile=false;

						std::vector<std::string> options = split(valores[1],',');
						for (int ival=0; ival < (int )options.size(); ival++)
						{
							if (options[ival]=="INFO")
								pLogProfiles[plog::info].toFile=true;
							else if (options[ival]=="DEBUG")
								pLogProfiles[plog::debug].toFile=true;
							else if (options[ival]=="ERROR")
								pLogProfiles[plog::error].toFile=true;
						}
					}
					else if (valores[0]=="TO_NETWORK")
					{
						pLogProfiles[plog::info].toNetwork=false;
						pLogProfiles[plog::error].toNetwork=false;
						pLogProfiles[plog::debug].toNetwork=false;

						std::vector<std::string> options = split(valores[1],',');
						for (int ival=0; ival < (int )options.size(); ival++)
						{
							if (options[ival]=="INFO")
								pLogProfiles[plog::info].toNetwork=true;
							else if (options[ival]=="DEBUG")
								pLogProfiles[plog::debug].toNetwork=true;
							else if (options[ival]=="ERROR")
								pLogProfiles[plog::error].toNetwork=true;
						}
					}
				}
			}

		}
		/** */
		void TestCfgChange()
		{
			//static int counter=0;

			util::MutexLock lock(m_mutex);

			//counter = (counter + 1) % 5;		// Chequea cada 5 LOGS...
			//if (counter != 0)
			//	return;

			bool change = false;
#ifdef _WIN32
			struct _stat _currentMod;
			_stat( _fileConfig.c_str(), &_currentMod );
			if (_lastMod.st_mtime != _currentMod.st_mtime)
			{
				_lastMod.st_mtime = _currentMod.st_mtime;
				change = true;
			}
#else
			struct stat _currentMod;
			lstat( _fileConfig.c_str(), &plog::_lastMod );
			if (_lastMod.st_mtime != _currentMod.st_mtime)
			{
				_lastMod.st_mtime = _currentMod.st_mtime;
				change = true;
			}
#endif // _WIN32
			if (change==true)
				Configure(_fileConfig);
		}

	protected:
		std::vector<std::string> split(std::string text, char sep)
		{
			std::vector<std::string> tokens;
			/*unsigned int*/size_t start = 0, end = 0;

			while ((end = text.find(sep, start)) != std::string::npos) 
			{
				tokens.push_back(text.substr(start, end - start));
				start = end + 1;
			}

			tokens.push_back(text.substr(start));
			return tokens;
		}
		static bool IsSpace(char c) {return c==' ';}

	private:
		std::string _fileformat;
		std::string _filename;
		unsigned long _filesize;
		int _nfiles;
		std::string _ipUdp;
		int _portUdp;
		std::string _udpId;
		bool _ipcshare;


		std::string _fileConfig;
		std::string _consoleProfile;
		std::string _fileProfile;
		std::string _networkProfile;

		/** */
		util::Mutex     m_mutex;
	};



//----------------------------------------------------------------------------------
// #include "Record.h"
    namespace detail
    {
        //////////////////////////////////////////////////////////////////////////
        // Stream output operators as free functions

        inline void operator<<(util::nstringstream& stream, const char* data)
        {
            data = data ? data : "(null)";

#ifdef _WIN32
            std::operator<<(stream, util::toWide(data));
#else            
            std::operator<<(stream, data);
#endif
        }

        inline void operator<<(util::nstringstream& stream, const std::string& data)
        {
            plog::detail::operator<<(stream, data.c_str());
        }

#ifndef __ANDROID__
        inline void operator<<(util::nstringstream& stream, const wchar_t* data)
        {
            data = data ? data : L"(null)";

#ifdef _WIN32
            std::operator<<(stream, data);
#else
            std::operator<<(stream, util::toNarrow(data));
#endif
        }

        inline void operator<<(util::nstringstream& stream, const std::wstring& data)
        {
            plog::detail::operator<<(stream, data.c_str());
        }
#endif
    }

    class Record
    {
    public:
		Record(const Record &other) 
			: m_severity(other.getSeverity()), 
			  m_tid(other.getTid()), 
			  m_object(other.getObject()),
			  m_line(other.getLine()), 
			  m_message(other.getMessage()),
			  m_func(NULL)
		{
			m_time = other.m_time;
			//m_func = other.m_func;
		}
		/** */
        Record(Severity severity, const char* func, size_t line, const void* object)
            : m_severity(severity), m_tid(util::gettid()), m_object(object), m_line(line), m_func(func)
        {
            util::ftime(&m_time);
        }

        //////////////////////////////////////////////////////////////////////////
        // Stream output operators
        Record& operator<<(char data)
        {
            char str[] = { data, 0 };
            *this << str;
            return *this;
        }

#ifndef __ANDROID__
        Record& operator<<(wchar_t data)
        {
            wchar_t str[] = { data, 0 };
            *this << str;
            return *this;
        }
#endif

        template<typename T>
        Record& operator<<(const T& data)
        {
            using namespace plog::detail;

            m_message << data;
            return *this;
        }

        //////////////////////////////////////////////////////////////////////////
        // Getters
        const util::Time& getTime() const
        {
            return m_time;
        }

        Severity getSeverity() const
        {
            return m_severity;
        }

        unsigned int getTid() const
        {
            return m_tid;
        }

        const void* getObject() const
        {
            return m_object;
        }

        size_t getLine() const
        {
            return m_line;
        }

        const util::nstring getMessage() const
        {
            return m_message.str();
        }

        std::string getFunc() const
        {
            return util::processFuncName(m_func);
        }

    private:
        util::Time          m_time;
        const Severity      m_severity;
        const unsigned int  m_tid;
        const void* const   m_object;
        const size_t        m_line;
        util::nstringstream m_message;
        const char* const   m_func;
	public:
		std::string			ipFrom;
    };

//----------------------------------------------------------------------------------
// #include "Logger.h"
// #include "Appenders/IAppender.h"
    class IAppender
    {
    public:
        virtual ~IAppender()
        {
        }

        virtual void write(const Record& record) = 0;
    };
	
	template<int instance>
	class Logger : public util::Singleton<Logger<instance> >, public IAppender
	{
#ifdef _PLOG_MT_
	private:
		static void *loggingTh(void *arg)
		{
			Logger *pLog = (Logger *)arg;
			pLog->_thLog();
			return NULL;
		}
	protected:
		void _thLog()
		{
			while (bstarted)
			{
				if (!m_queue.empty())
				{
					util::MutexLock lock(m_mutex);
					
					Record data = m_queue.front();
					m_queue.pop();
					for (std::vector<IAppender*>::iterator it = m_appenders.begin(); it != m_appenders.end(); ++it)
					{
						(*it)->write(data);
					}
				}
				Sleep(10);
			}
		}


#endif // _PLOG_MT_

	public:
		/** */
		Logger(string id, Severity maxSeverity = none) : m_maxSeverity(maxSeverity)
		{
#ifdef _PLOG_MT_
			bstarted=true;
			pthread_create(&m_hThread, NULL, Logger::loggingTh, this);
#ifndef _WIN32
#ifndef _PPC82xx_
			pthread_setname_np(m_hThread, id.c_str());
#endif // !_PPC82xx_
#endif // !_WIN32
#endif // _PLOG_MT_
		}

#ifdef _PLOG_MT_
		~Logger()
		{
			bstarted = false;
			Sleep(20);
		}
#endif // _PLOG_MT_


		/** */
		Logger& addAppender(IAppender* appender)
		{
			assert(appender != this);
			m_appenders.push_back(appender);
			return *this;
		}

		/** */
		Severity getMaxSeverity() const
		{
			return m_maxSeverity;
		}

		/** */
		void setMaxSeverity(Severity severity)
		{
			m_maxSeverity = severity;
		}

		/** */
		bool checkSeverity(Severity severity) const
		{
			return severity <= m_maxSeverity;
		}

		/** */
		virtual void write(const Record& record)
		{
			if (checkSeverity(record.getSeverity()))
			{
				*this += record;
			}
		}

		/** */
		void operator+=(const Record& record)
		{
#ifdef _PLOG_MT_
			util::MutexLock lock(m_mutex);
			m_queue.push(record);
#else
			for (std::vector<IAppender*>::iterator it = m_appenders.begin(); it != m_appenders.end(); ++it)
			{
				(*it)->write(record);
			}
#endif // _PLOG_MT_
		}

	private:
		Severity m_maxSeverity;
		std::vector<IAppender*> m_appenders;
#ifdef _PLOG_MT_
		bool bstarted;
		pthread_t m_hThread;
		std::queue<Record >   m_queue;
		util::Mutex     m_mutex;
#endif // _PLOG_MT_

	};

	template<int instance>
	inline Logger<instance>* get()
	{
		return Logger<instance>::getInstance();
	}

	inline Logger<0>* get()
	{
		return Logger<0>::getInstance();
	}


//----------------------------------------------------------------------------------
// #include "Init.h"
// #include "Logger.h"
// #include "Formatters/CsvFormatter.h"
    class CsvFormatter
    {
    public:
        static util::nstring header()
        {
            //return PLOG_NSTR("Date;Time;Severity;TID;This;Function;Message\n");
            return PLOG_NSTR("Date;Time;Severity;From;Line;Message\n");
        }

        static util::nstring format(const Record& record)
        {
            tm t;
            util::localtime_s(&t, &record.getTime().time);

            util::nstringstream ss;
            ss << t.tm_year + 1900 << "/" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << "/" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << ";";
            ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << ":" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << ":" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << "." << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << ";";
            ss << getSeverityName(record.getSeverity()) << ";";
            //ss << record.getTid() << ";";
            //ss << record.getObject() << ";";
            //ss << record.getFunc().c_str() << "@" << record.getLine() << ";";
			ss << record.getFunc().c_str() << ";";
			ss << record.getLine() << ";";

            util::nstring message = record.getMessage();

            if (message.size() > kMaxMessageSize)
            {
                message.resize(kMaxMessageSize);
                message.append(PLOG_NSTR("..."));
            }

            util::nstringstream split(message);
            util::nstring token;

            while (!split.eof())
            {
                std::getline(split, token, PLOG_NSTR('"'));
                ss << "\"" << token << "\"";
            }

            ss << "\n";

            return ss.str();
        }

        static const size_t kMaxMessageSize = 32000;
    };

// #include "Formatters/TxtFormatter.h"
    class TxtFormatter
    {
    public:
        static util::nstring header()
        {
            return util::nstring();
        }

        static util::nstring format(const Record& record)
        {
            tm t;
            util::localtime_s(&t, &record.getTime().time);

            util::nstringstream ss;
            ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << " ";
            ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << ":" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << ":" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << "." << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << " ";
            ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << getSeverityName(record.getSeverity()) << " ";
            ss << "[" << record.getTid() << "] ";
            ss << "[" << record.getFunc().c_str() << "@" << record.getLine() << "] ";
            ss << record.getMessage().c_str() << "\n";

            return ss.str();
        }
    };

// #include "Formatters/ConsoleFormatter.h"
    class ConsoleFormatter
    {
    public:
        static util::nstring header()
        {
            return util::nstring();
        }

        static util::nstring format(const Record& record)
        {
            tm t;
            util::localtime_s(&t, &record.getTime().time);

            util::nstringstream ss;
//          ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << " ";
//            ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << ":" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << ":" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << "." << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << " ";
            ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << ":" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << ":" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << " ";
            ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << getSeverityName(record.getSeverity()) << " ";
//            ss << "[" << record.getTid() << "] ";
//            ss << "[" << record.getFunc().c_str() << "@" << record.getLine() << "] ";
//			ss << record.getFunc().c_str();
            ss << record.getMessage().c_str() << "\n";
            return ss.str();
        }
    };

// #include "Formatters/Log4viewFormatter.h"
	/** Formato a Generar por Cada mensaje para verlo en Log4View...
		<log4j:event logger="TestNLog.Program" level="INFO" timestamp="1448022393457" thread="8">
			<log4j:message>Esto es una prueba...</log4j:message>
			<log4j:properties>
				<log4j:data name="log4japp" value="TestNLog.vshost.exe(5688)" />
				<log4j:data name="log4jmachinename" value="DF1501" />
			</log4j:properties>
		</log4j:event>
	*/
    class Log4viewFormatter
    {
    public:
        static util::nstring header()
        {
            return util::nstring();
        }

        static util::nstring format(const Record& record)
        {
            //tm t;
            //util::localtime_s(&t, &record.getTime().time);
            util::nstringstream ss;

			ss << "<log4j:event logger=\"" << record.ipFrom.c_str() << "\" level=\"" << getSeverityName(record.getSeverity());
			ss << "\" timestamp=\"" << std::setfill(PLOG_NSTR('0')) << std::setw(10) << record.getTime().time << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm;
			ss << "\" thread=\"" << record.getTid() << "\">";
			ss << "<log4j:message>" << record.getMessage().c_str() << "</log4j:message>";
			ss << "<log4j:properties>";
			ss << "<log4j:data name=\"log4japp\" value=\"" << "PLOG" << "\" />";
			ss << "<log4j:data name=\"log4jmachinename\" value=\"" << "PLOG-M" << "\" />";
			ss << "</log4j:properties>";
			ss << "</log4j:event>";

            return ss.str();
        }
    };

// #include "Appenders/RollingFileAppender.h"
	// #include "../Converters/UTF8Converter.h"
    class UTF8Converter
    {
    public:
        static std::string header(const util::nstring& str)
        {
            const char kBOM[] = "\xEF\xBB\xBF";

            return std::string(kBOM) + convert(str);
        }

#ifdef WIN32
        static std::string convert(const util::nstring& str)
        {
            return util::toUTF8(str);
        }
#else
        static const std::string& convert(const util::nstring& str)
        {
            return str;
        }
#endif
    };

	
    template<class Formatter, class Converter = UTF8Converter>
    class RollingFileAppender : public IAppender
    {
    public:
        RollingFileAppender(const char* fileName, size_t maxFileSize = 0, int maxFiles = 0) 
            : m_fileSize()
            , m_maxFileSize((std::max)(maxFileSize, static_cast<size_t>(1000))) // set a lower limit for the maxFileSize
            , m_lastFileNumber((std::max)(maxFiles - 1, 0))
            , m_firstWrite(true)
        {
            util::splitFileName(fileName, m_fileNameNoExt, m_fileExt);
        }

        virtual void write(const Record& record)
        {
            util::MutexLock lock(m_mutex);

            if (m_firstWrite)
            {
                openLogFile();
                m_firstWrite = false;
            }
            else if (m_lastFileNumber > 0 && m_fileSize > m_maxFileSize && static_cast<size_t>(-1) != m_fileSize)
            {
                rollLogFiles();
            }

            int bytesWritten = m_file.write(Converter::convert(Formatter::format(record)));

            if (bytesWritten > 0)
            {
                m_fileSize += bytesWritten;
            }
        }

    private:
        void rollLogFiles()
        {
            m_file.close();

            std::string lastFileName = buildFileName(m_lastFileNumber);
            util::File::unlink(lastFileName.c_str());

            for (int fileNumber = m_lastFileNumber - 1; fileNumber >= 0; --fileNumber)
            {
                std::string currentFileName = buildFileName(fileNumber);
                std::string nextFileName = buildFileName(fileNumber + 1);

                util::File::rename(currentFileName.c_str(), nextFileName.c_str());
            }

            openLogFile();
        }

        void openLogFile()
        {
            std::string fileName = buildFileName();
            m_fileSize = m_file.open(fileName.c_str());

            if (0 == m_fileSize)
            {
                int bytesWritten = m_file.write(Converter::header(Formatter::header()));

                if (bytesWritten > 0)
                {
                    m_fileSize += bytesWritten;
                }
            }
        }

        std::string buildFileName(int fileNumber = 0)
        {
            std::stringstream ss;
            ss << m_fileNameNoExt;
            
            if (fileNumber > 0)
            {
                ss << '.' << fileNumber;
            }
            
            if (!m_fileExt.empty())
            {
                ss << '.' << m_fileExt;
            }

            return ss.str();
        }

    private:
        util::Mutex     m_mutex;
        util::File      m_file;
        size_t          m_fileSize;
        const size_t    m_maxFileSize;
        const int       m_lastFileNumber;
        std::string     m_fileExt;
        std::string     m_fileNameNoExt;
        bool            m_firstWrite;
    };

// #include "Appenders/ConsoleAppender.h"
    template<class Formatter>
    class ConsoleAppender : public IAppender
    {
    public:
        ConsoleAppender()
        {
#ifdef _WIN32
            ::setlocale(LC_ALL, "");
#endif
        }

        virtual void write(const Record& record)
        {
            util::MutexLock lock(m_mutex);
#ifdef _WIN32
            std::wcout << Formatter::format(record);
#else
            std::cout << Formatter::format(record);
#endif
        }
    private:
        util::Mutex     m_mutex;
    };

// #include "Appenders/NetUdpAppender.h"
	template<class Formatter, class Converter = UTF8Converter>
	class NetUdpAppender : public IAppender
	{
	public:
		NetUdpAppender() 
		{
		}
		NetUdpAppender(const char* ipTo, int udpPort, const char* ipFrom) 
		{
			m_socket=-1;
			m_firstWrite=true;

			m_addr.sin_family = AF_INET;
			m_addr.sin_port = htons(udpPort);
			m_addr.sin_addr.s_addr = inet_addr(ipTo);

			m_ipFrom =std::string(ipFrom);
		}

		virtual void write(const Record& record)
		{
			util::MutexLock lock(m_mutex);

			if (m_firstWrite)
			{
				m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				if (m_socket == -1)
				{
#ifdef _WIN32
					wprintf(L"socket failed with error: %d\n", WSAGetLastError());
#else
					wprintf(L"socket failed with error: %d\n", errno);
#endif
				}
				m_firstWrite = false;
			}

			if (m_socket != -1)
			{
				((Record *)&record)->ipFrom = m_ipFrom;

				string data = Converter::convert(Formatter::format(record));
				// string data = Formatter::format(record);
#ifdef _WIN32
				int iResult = sendto(m_socket, data.c_str(), data.length(), 0, (SOCKADDR *) & m_addr, sizeof (m_addr));
				if (iResult == SOCKET_ERROR) 
				{
					wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
				}    			
#else
				int iResult = sendto(m_socket, data.c_str(), data.length(), 0, (const sockaddr *)&m_addr, sizeof (m_addr));
				if (iResult == -1) 
				{
					wprintf(L"sendto failed with error: %d\n", errno);
				}    			
#endif
			}

		}

	private:
		util::Mutex			m_mutex;
		bool				m_firstWrite;
		int					m_socket;
#ifdef _WIN32
		sockaddr_in			m_addr;
#else
		struct sockaddr_in  m_addr;
#endif
		std::string			m_ipFrom;
	};

    namespace
    {
        bool isCsv(const char* fileName)
        {
            const char kCsvExt[] = ".csv";
            const size_t kCsvExtLength = sizeof(kCsvExt) - 1;

            size_t len = std::strlen(fileName);
            return len >= kCsvExtLength && 0 == std::strcmp(&fileName[len - kCsvExtLength], kCsvExt);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Empty initializer / one appender

    template<int instance>
    inline Logger<instance>& init(string id, Severity maxSeverity = none, IAppender* appender = NULL)
    {
        static Logger<instance> logger(id, maxSeverity);
        return appender ? logger.addAppender(appender) : logger;
    }

    inline Logger<0>& init(string id, Severity maxSeverity = none, IAppender* appender = NULL)
    {
        return init<0>(id, maxSeverity, appender);
    }

    //////////////////////////////////////////////////////////////////////////
    // RollingFileAppender with any Formatter

    template<class Formatter, int instance>
    inline Logger<instance>& init(Severity maxSeverity, const char* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        static RollingFileAppender<Formatter> rollingFileAppender(fileName, maxFileSize, maxFiles);
        return init<instance>("plog_file", maxSeverity, &rollingFileAppender);
    }

    template<class Formatter>
    inline Logger<0>& init(Severity maxSeverity, const char* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return init<Formatter, 0>(maxSeverity, fileName, maxFileSize, maxFiles);
    }

    //////////////////////////////////////////////////////////////////////////
    // RollingFileAppender with TXT/CSV chosen by file extension

    template<int instance>
    inline Logger<instance>& init(Severity maxSeverity, const char* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return isCsv(fileName) ? init<CsvFormatter, instance>(maxSeverity, fileName, maxFileSize, maxFiles) : init<TxtFormatter, instance>(maxSeverity, fileName, maxFileSize, maxFiles);
    }

    inline Logger<0>& init(Severity maxSeverity, const char* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return init<0>(maxSeverity, fileName, maxFileSize, maxFiles);
    }

    //////////////////////////////////////////////////////////////////////////
    // WCHAR variants for Windows

#ifdef _WIN32
    template<class Formatter, int instance>
    inline Logger<instance>& init(Severity maxSeverity, const wchar_t* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return init<Formatter, instance>(maxSeverity, util::toNarrow(fileName).c_str(), maxFileSize, maxFiles);
    }

    template<class Formatter>
    inline Logger<0>& init(Severity maxSeverity, const wchar_t* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return init<Formatter, 0>(maxSeverity, fileName, maxFileSize, maxFiles);
    }

    template<int instance>
    inline Logger<instance>& init(Severity maxSeverity, const wchar_t* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return init<instance>(maxSeverity, util::toNarrow(fileName).c_str(), maxFileSize, maxFiles);
    }

    inline Logger<0>& init(Severity maxSeverity, const wchar_t* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return init<0>(maxSeverity, fileName, maxFileSize, maxFiles);
    }
#endif


//----------------------------------------------------------------------------------
// #include "Compatibility.h"
    const Severity trace = verbose;
    const Severity minimum_log_level = none;
    const Severity maximum_log_level = verbose;
    
    typedef Severity Level;
    typedef Severity log_level;
    typedef Record message;

    //inline void free(bool = true)
    //{
    //}

    template<class CharType>
    inline void init_csv(const CharType* fileName, Severity maxSeverity)
    {
        init<CsvFormatter, 0>(maxSeverity, fileName);
    }

    template<class CharType>
    inline void init_txt(const CharType* fileName, Severity maxSeverity)
    {
        init<TxtFormatter, 0>(maxSeverity, fileName);
    }

    template<int instance, class CharType>
    inline Logger<instance>& init(const CharType* fileName, Severity maxSeverity, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return init<instance>(maxSeverity, fileName, maxFileSize, maxFiles);
    }

    template<class CharType>
    inline Logger<0>& init(const CharType* fileName, Severity maxSeverity, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return init<0>(maxSeverity, fileName, maxFileSize, maxFiles);
    }

}

//////////////////////////////////////////////////////////////////////////
// Helper macros that get context info

#ifdef _MSC_BUILD
#   if _MSC_VER >= 1600 // Visual Studio 2010
#       define PLOG_GET_THIS()      __if_exists(this) { this } __if_not_exists(this) { 0 } 
#   else
#       define PLOG_GET_THIS()      0
#   endif
#   define PLOG_GET_FUNC()          __FUNCTION__
#else
#   define PLOG_GET_THIS()          0
#   define PLOG_GET_FUNC()          __PRETTY_FUNCTION__
#endif

//////////////////////////////////////////////////////////////////////////
// Log severity level checker
#define IF_LOG_(instance, severity)     if (plog::get<instance>() && plog::get<instance>()->checkSeverity(severity))
#define IF_LOG(severity)	            IF_LOG_(0, severity)

//////////////////////////////////////////////////////////////////////////// Main logging macros

#define LOG_(instance, severity)        IF_LOG_(instance, severity) (*plog::get<instance>()) += plog::Record(severity, PLOG_GET_FUNC(), __LINE__, PLOG_GET_THIS())
/** */
#define NDFLOG_(instance, severity, from, line)    IF_LOG_(instance, severity) (*plog::get<instance>()) += plog::Record(severity, from, line, PLOG_GET_THIS())
#define LOG(severity)                   LOG_(0, severity)

#define LOG_VERBOSE                     LOG(plog::verbose)
#define LOG_DEBUG                       LOG(plog::debug)
#define LOG_INFO                        LOG(plog::info)
#define LOG_WARNING                     LOG(plog::warning)
#define LOG_ERROR                       LOG(plog::error)
#define LOG_FATAL                       LOG(plog::fatal)

#define LOG_VERBOSE_(instance)          LOG_(instance, plog::verbose)
#define LOG_DEBUG_(instance)            LOG_(instance, plog::debug)
#define LOG_INFO_(instance)             LOG_(instance, plog::info)
#define LOG_WARNING_(instance)          LOG_(instance, plog::warning)
#define LOG_ERROR_(instance)            LOG_(instance, plog::error)
#define LOG_FATAL_(instance)            LOG_(instance, plog::fatal)

#define LOGV                            LOG_VERBOSE
#define LOGD                            LOG_DEBUG
#define LOGI                            LOG_INFO
#define LOGW                            LOG_WARNING
#define LOGE                            LOG_ERROR
#define LOGF                            LOG_FATAL

#define LOGV_(instance)                 LOG_VERBOSE_(instance)
#define LOGD_(instance)                 LOG_DEBUG_(instance)
#define LOGI_(instance)                 LOG_INFO_(instance)
#define LOGW_(instance)                 LOG_WARNING_(instance)
#define LOGE_(instance)                 LOG_ERROR_(instance)
#define LOGF_(instance)                 LOG_FATAL_(instance)

//////////////////////////////////////////////////////////////////////////
// Conditional logging macros

#define LOG_IF_(instance, severity, condition)  if (condition) LOG_(instance, severity)
#define LOG_IF(severity, condition)             LOG_IF_(0, severity, condition)

#define LOG_VERBOSE_IF(condition)               LOG_IF(plog::verbose, condition)
#define LOG_DEBUG_IF(condition)                 LOG_IF(plog::debug, condition)
#define LOG_INFO_IF(condition)                  LOG_IF(plog::info, condition)
#define LOG_WARNING_IF(condition)               LOG_IF(plog::warning, condition)
#define LOG_ERROR_IF(condition)                 LOG_IF(plog::error, condition)
#define LOG_FATAL_IF(condition)                 LOG_IF(plog::fatal, condition)

#define LOG_VERBOSE_IF_(instance, condition)    LOG_IF_(instance, plog::verbose, condition)
#define LOG_DEBUG_IF_(instance, condition)      LOG_IF_(instance, plog::debug, condition)
#define LOG_INFO_IF_(instance, condition)       LOG_IF_(instance, plog::info, condition)
#define LOG_WARNING_IF_(instance, condition)    LOG_IF_(instance, plog::warning, condition)
#define LOG_ERROR_IF_(instance, condition)      LOG_IF_(instance, plog::error, condition)
#define LOG_FATAL_IF_(instance, condition)      LOG_IF_(instance, plog::fatal, condition)

#define LOGV_IF(condition)                      LOG_VERBOSE_IF(condition)
#define LOGD_IF(condition)                      LOG_DEBUG_IF(condition)
#define LOGI_IF(condition)                      LOG_INFO_IF(condition)
#define LOGW_IF(condition)                      LOG_WARNING_IF(condition)
#define LOGE_IF(condition)                      LOG_ERROR_IF(condition)
#define LOGF_IF(condition)                      LOG_FATAL_IF(condition)

#define LOGV_IF_(instance, condition)           LOG_VERBOSE_IF_(instance, condition)
#define LOGD_IF_(instance, condition)           LOG_DEBUG_IF_(instance, condition)
#define LOGI_IF_(instance, condition)           LOG_INFO_IF_(instance, condition)
#define LOGW_IF_(instance, condition)           LOG_WARNING_IF_(instance, condition)
#define LOGE_IF_(instance, condition)           LOG_ERROR_IF_(instance, condition)
#define LOGF_IF_(instance, condition)           LOG_FATAL_IF_(instance, condition)



#endif
