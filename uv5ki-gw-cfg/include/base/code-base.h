#ifndef _DFN_CODEBASE_
#define _DFN_CODEBASE_

/*
#if defined _WIN32
#elif defined __APPLE__
#elif defined _PPC82xx_
#else
#endif
*/
#ifdef _WIN32
	#define _CRTDBG_MAP_ALLOC		// Para el Debugger de Memory leaks...
	#include <Windows.h>
	#include <stdlib.h>
	#include <crtdbg.h>
	#include <tchar.h>
	#define HAVE_STRUCT_TIMESPEC
	#include "../../include/pthreads/include/pthread.h"
	#define WORKING_DIR		".\\"
	#define __FILENAME__			(strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
	#include <unistd.h>
	#include <signal.h>
	#include <limits.h>
	#define Sleep(m)	usleep(1000*m)

	#if defined _PPC82xx_
		#define WORKING_DIR		"/home/serv/"
	#elif defined __APPLE__
		#define WORKING_DIR		"./"
	#else
		#define WORKING_DIR		"./"
		#define _SIGACTION_		1
	#endif
	#include <stdarg.h>
	#include <stdlib.h>
	#include <pthread.h>
	#define __FILENAME__			(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

	typedef unsigned char byte;
#endif

#ifndef _PPC82xx_
#	define LOCAL_TEST		1
#endif


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>

#include <string>
#include <list>
#include <map>
#include <queue>
#include <fstream>

#include "exception.h"
#include "plog.h"

#define _PARAMS_IN_INI_			0
#define ON_WORKING_DIR(m)		(((std::string)WORKING_DIR)+((std::string)m)).c_str()

#define ON_RAM(p)				(onram(p).c_str())
#define ON_FLASH(p)				(onflash(p).c_str())


using namespace plog;
using namespace std;

#define PLOG_INFO(format, ...)		LogInfo(__FILENAME__, __LINE__, format, ##__VA_ARGS__)
#define PLOG_ERROR(format, ...)		LogError(__FILENAME__, __LINE__, format,##__VA_ARGS__)
#define PLOG_DEBUG(format, ...)		LogDebug(__FILENAME__, __LINE__, format,##__VA_ARGS__)
#define PLOG_EXCEP(x, format, ...)	LogException(__FILENAME__, __LINE__, x, format, ##__VA_ARGS__)

/** */
class PLogEvent {
public:
	PLogEvent(plog::Severity sevIn=info, string fromIn="", int lineIn=0, string msgIn="") {
		sev = sevIn;
		from = fromIn;
		msg = msgIn;
		line = lineIn;
	}
public:
	plog::Severity	sev;
	string from;
	int line;
	string msg;
};

/** */
class CodeBase
{
	enum emPlogs
	{
		plogFile = 0,
		plogConsole= 1,
		plogNetwork= 2
	};

public:
	CodeBase(void);
	~CodeBase(void);

	static void plogInit();
	static void plogDispose();

private:
	static void _Log(plog::Severity level, const char *from, int line, const char *fmt, va_list args);
	static void _FormatLog(plog::Severity level, const char *file, int line, const char *fmt, va_list args );
	static void _FormatLog(plog::Severity level, const char *file, int line, const char *fmt, ... );

protected:	
	static void LogException(const char *file, int line, Exception x, const char *fmt, ... )
	{
		va_list args;
		std::stringstream ss;
		ss << "Excepcion: " << x.what() << ". " << fmt;
		va_start ( args, fmt );
		_FormatLog(error, file, line, ss.str().c_str(), args);
		va_end(args);
	}
	static void LogError(const char *file, int line, const char *fmt, ... ) 
	{
		va_list args;
		va_start ( args, fmt );
		_FormatLog(error, file, line, fmt, args);
		va_end(args);
	}
	static void LogInfo(const char *file, int line, const char *fmt, ... )
	{
		va_list args;
		va_start ( args, fmt );
		_FormatLog(info, file, line, fmt, args);
		va_end(args);
	}
	static void LogDebug(const char *file, int line, const char *fmt, ... )
	{
		va_list args;
		va_start ( args, fmt );
		_FormatLog(debug, file, line, fmt, args);
		va_end(args);
	}
	/** */
public:
	static string onfs(string filename) {
#if defined(_WIN32)
		return "./fs-win" + filename;
#elif defined(_PPC82xx_)
		return filename;
#else
		// return "./fs-lnx" + filename;
		return "./fs-win" + filename;
#endif
	}
	static string onram(string filename) {
		return onfs("/mnt/ramfs/" + filename);
	}
	static string onflash(string filename) {
		return onfs("/home/serv/" + filename);
	}
	static string VersionGlobal();
	static string VersionConfiguracion();
	static string VersionSnmp();
	static string VersionGrabador();

	static bool IsNtpSync();

private:
	static plog::ConsoleAppender<plog::ConsoleFormatter> *p_consoleAppender;
	static plog::NetUdpAppender<plog::Log4viewFormatter> *p_log4viewAppender;
	static plog::LogConfig cfg;

	static bool _plog_iniciado;
	static pthread_t plog_thread_id;
	static void *plog_thread_routine(void *arg);
	static bool plog_queue_event_get(PLogEvent *p_evento);
	static std::queue<PLogEvent> plog_queue;
	static util::Mutex plog_mutex;
};

#endif

