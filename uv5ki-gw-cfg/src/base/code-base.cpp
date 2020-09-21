#include "../../include/base/code-base.h"
#include "../../include/base/sistema.h"
#include "../../include/tools/tools.h"

/** MACROS */
#define LOGCONF	ON_WORKING_DIR("plog.conf")

/** Variables Estaticas */
bool CodeBase::_plog_iniciado=false;
plog::LogConfig CodeBase::cfg;
plog::ConsoleAppender<plog::ConsoleFormatter> *CodeBase::p_consoleAppender;
plog::NetUdpAppender<plog::Log4viewFormatter> *CodeBase::p_log4viewAppender;

static SysMutex global_mutex;

#if !defined(_PPC82xx_)
 /** Para pruebas... */
 int cntEvents = 0;
#endif

/** */
CodeBase::CodeBase(void)
{
}

/** */
CodeBase::~CodeBase(void)
{
}

/** */
void CodeBase::plogInit() 
{
	//if (_plog_iniciado == true)
	//	return;

#ifdef _WIN32
	cfg.Configure("./wplog.conf");
#else
	cfg.Configure(LOGCONF);
#endif

	string id=cfg.UdpId();

	p_consoleAppender = new plog::ConsoleAppender<plog::ConsoleFormatter>();
	p_log4viewAppender = new plog::NetUdpAppender<plog::Log4viewFormatter>(cfg.IpUpd().c_str(), cfg.PortUdp(), id.c_str());

	plog::init<plogFile>(plog::verbose, cfg.FileName().c_str(), cfg.FileSize(), cfg.NumberOfFile());
	plog::init<plogConsole>("plog_con", plog::verbose, p_consoleAppender);
	plog::init<plogNetwork>("plog_net", plog::verbose, p_log4viewAppender);

	_plog_iniciado = true;
	pthread_create(&plog_thread_id, NULL, plog_thread_routine, NULL);
#if !defined(_WIN32) && !defined(__APPLE__) && !defined(_PPC82xx_)
	pthread_setname_np(plog_thread_id, id.c_str());
#endif 
}

/** */
void CodeBase::plogDispose() 
{
	if (_plog_iniciado==true)
	{
		_plog_iniciado = false;
		pthread_join(plog_thread_id, NULL);
		delete p_consoleAppender;
		delete p_log4viewAppender;
	}
}

/** */
void CodeBase::_Log(plog::Severity level, const char *from, int line, const char *fmt, va_list args)
{
	if (_plog_iniciado == false) {
		// plogInit();
		return;
	}

	//util::MutexLock lock(plog_mutex);
	Tools::Trace("PID %d. Locking Log Mutex...", Tools::Pid());
	SysMutexLock global_lock(global_mutex);

	static char textString[1024] = {'\0'};
	memset(textString, '\0', sizeof(textString));

#ifdef _WIN32
	vsnprintf_s ( textString, sizeof(textString), fmt, args );
#else
	vsnprintf ( textString, sizeof(textString), fmt, args );
#endif

	PLogEvent evento(level, from, line, textString);

	/** 20170418. Control de la COLA de LOGS */
	unsigned int mcola = 100;
	if (plog_queue.size() < mcola)
		plog_queue.push(evento);
	else
		Tools::fatalerror(string("Cola de Log llena... Tamaño Maximo: ") + Tools::itoa(mcola));

	Tools::Trace("PID %d. unlocking Log Mutex...", Tools::Pid());
}

/** */
void CodeBase::_FormatLog(plog::Severity level, const char *file, int line, const char *fmt, va_list args )
{
	try
	{
		_Log(level, file, line, fmt, args);
	}
	catch(...)
	{
		/** 
			20170418. Si la función genera una excepción, no puedo volver a llamarla porque podría dar lugar a un bucle infinito.
		 */
		Tools::fatalerror("Error en _FormatLog");
		return;
	}
}

/** */
pthread_t CodeBase::plog_thread_id;
std::queue<PLogEvent > CodeBase::plog_queue;
util::Mutex CodeBase::plog_mutex;
bool CodeBase::plog_queue_event_get(PLogEvent *p_evento) 
{
	//util::MutexLock lock(plog_mutex);
	Tools::Trace("PID %d. Locking Log Mutex (1) ...", Tools::Pid());
	SysMutexLock global_lock(global_mutex);
	if (!plog_queue.empty())
	{
		*p_evento = plog_queue.front();
		plog_queue.pop();
		Tools::Trace("PID %d. Unlocking Log Mutex (1)...", Tools::Pid());
		return true;
	}
	Tools::Trace("PID %d. Unlocking Log Mutex (2)...", Tools::Pid());
	return false;
}

void *CodeBase::plog_thread_routine(void *arg) 
{
#if !defined(_WIN32)
	PLOG_INFO("PlogThread (%d) running...", (int )getpid());
#endif
	while (_plog_iniciado == true)
	{
		try 
		{
			PLogEvent evento;
			if (plog_queue_event_get(&evento)==true)
			{
				cfg.TestCfgChange(); 
				if (plog::pLogProfiles[(int)evento.sev].toFile)
					NDFLOG_(plogFile, evento.sev, evento.from.c_str(), evento.line) << evento.msg;
				if (plog::pLogProfiles[(int)evento.sev].toConsole)
					NDFLOG_(plogConsole, evento.sev, evento.from.c_str(), evento.line) << evento.msg;
				if (plog::pLogProfiles[(int)evento.sev].toNetwork)
					NDFLOG_(plogNetwork, evento.sev, evento.from.c_str(), evento.line) << evento.msg;			
#if !defined(_PPC82xx_)
				/** Para pruebas... */
			if (++cntEvents == 32)
					throw new Exception("Excepcion provocada para pruebas");
#endif
			}
		}
		catch(...) {
#if !defined(_WIN32)
//			PLOG_ERROR("PlogThread (%d) Exception...", (int )getpid());
			Tools::fatalerror("PlogThread Exception...");
#else
//			PLOG_ERROR("PlogThread Exception...");
			Tools::fatalerror("PlogThread Exception...");
#endif
		}
		//Sleep(10);
		Sleep(50);
	}
	return NULL;
}

/** */
string CodeBase::VersionGlobal() 
{
	return Tools::read_txt_file(ON_WORKING_DIR("version.txt"));
}

/** */
extern char *acStrVersion();
string CodeBase::VersionConfiguracion()
{
	return (string )acStrVersion();
}
/** */
string CodeBase:: VersionSnmp() 
{
	return sistema::SnmpAgentVersion();
}

/** */
string CodeBase::VersionGrabador()
{
	return sistema::RecordServiceVersion();
}

/** */
bool CodeBase::IsNtpSync()
{
	string ntp_sync_info = Tools::read_txt_file(onfs("/var/ntpq_report"));
	PLOG_INFO("IsNtpSync: %s", ntp_sync_info.c_str());
	return ntp_sync_info=="" || ntp_sync_info[0]=='n' ? false : true;
}

