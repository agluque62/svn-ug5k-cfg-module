// uv5ki-gw-cfg.cpp: define el punto de entrada de la aplicaci�n de consola.
//
#include "./include/websrv/uv5kigwcfg-web-app.h"
#include "./include/cfg-proc.h"
#include "./include/man-proc.h"
#include "./include/his-proc.h"
#include "./include/file-supervisor.h"

#define VRS_ID_SOFTWARE			"UV5KI-GW CFG-SERVER"
#define VRS_VERSION_MAYOR		2
#define VRS_VERSION_MENOR_A		0
#define VRS_VERSION_MENOR_B		1

#if defined _WIN32
 char acBuildString[] =		"WIN-VER";
#else
#	if !defined __APPLE__
#		include <mcheck.h>
#	endif
#	include <execinfo.h>
	extern char acBuildString[]; 
#endif
	static int salida = 0;

/** */
char *acStrVersion()
{
	static char buffer[128];
#ifdef _WIN32
	sprintf_s(buffer, " %s   V%d.%d.%d (%s)",
		VRS_ID_SOFTWARE,
		VRS_VERSION_MAYOR, VRS_VERSION_MENOR_A, VRS_VERSION_MENOR_B,
		acBuildString );
#else
	sprintf(buffer, " %s   V%d.%d.%d (%s)",
		VRS_ID_SOFTWARE,
		VRS_VERSION_MAYOR, VRS_VERSION_MENOR_A, VRS_VERSION_MENOR_B,
		acBuildString );
#endif
	return buffer;
}

/** */
class U5kiGwCfg : public CodeBase
{
private:
	static void traceSIGSEGV()
	{
		Tools::SafeLogFromSignal("********* SEGMENTATION FAULT *********");

#ifdef _WIN32
		size_t size = 0, i = 0;
		char** strings;
#else
		void* trace[32];
		size_t size, i;
		char** strings;

		size = backtrace(trace, 32);
		strings = backtrace_symbols(trace, size);
#endif

		Tools::SafeLogFromSignal("BACKTRACE:\n");

		for (i = 0; i < size; i++) {
			Tools::SafeLogFromSignal(strings[i]);
		}

		Tools::SafeLogFromSignal("***************************************");
	}
	static  void catchAllSignal(int sig)
	{
		Tools::SafeLogFromSignal("Recibida Signal...", sig, getpid()); 
#ifndef _WIN32
		switch (sig)
		{
		case SIGINT:
		case SIGTERM:
			salida = 1;         // Sale el Bucle Principal.
			break;
		case SIGSEGV:           // Error acceso a memoria.
			traceSIGSEGV();
			CfgProc::p_cfg_proc->p_working_config->dispose();
			CThread::sleep(2000);
			exit(-1);
			break;
		}
#endif
	}
	void setAllSignalCatch()
	{
#ifndef _WIN32
#if !defined __APPLE__
		mtrace();			// Para poder seguir los malloc...
#endif
		signal(SIGHUP, &catchAllSignal);    // 		1	/* Hangup (POSIX).  */
		signal(SIGINT, &catchAllSignal);    // 		2	/* Interrupt (ANSI).  */
		signal(SIGQUIT, &catchAllSignal);    // 		3	/* Quit (POSIX).  */
		signal(SIGILL, &catchAllSignal);    // 		4	/* Illegal instruction (ANSI).  */
		signal(SIGTRAP, &catchAllSignal);    // 		5	/* Trace trap (POSIX).  */
		signal(SIGABRT, &catchAllSignal);    // 		6	/* Abort (ANSI).  */
		signal(SIGIOT, &catchAllSignal);    // 		6	/* IOT trap (4.2 BSD).  */
		signal(SIGBUS, &catchAllSignal);    // 		7	/* BUS error (4.2 BSD).  */
		signal(SIGFPE, &catchAllSignal);    // 		8	/* Floating-point exception (ANSI).  */
		signal(SIGKILL, &catchAllSignal);    // 		9	/* Kill, unblockable (POSIX).  */
		signal(SIGUSR1, &catchAllSignal);    // 		10	/* User-defined signal 1 (POSIX).  */
		signal(SIGSEGV, &catchAllSignal);    // 		11	/* Segmentation violation (ANSI).  */
		signal(SIGUSR2, &catchAllSignal);    // 		12	/* User-defined signal 2 (POSIX).  */
		signal(SIGPIPE, &catchAllSignal);    // 		13	/* Broken pipe (POSIX).  */
		signal(SIGALRM, &catchAllSignal);    // 		14	/* Alarm clock (POSIX).  */
		signal(SIGTERM, &catchAllSignal);    // 		15	/* Termination (ANSI).  */
#ifndef __APPLE__
		signal(SIGSTKFLT, &catchAllSignal);    // 		16	/* Stack fault.  */
#endif
							//#define	SIGCLD		SIGCHLD	/* Same as SIGCHLD (System V).  */
		signal(SIGCHLD, &catchAllSignal);    // 		17	/* Child status has changed (POSIX).  */
		signal(SIGCONT, &catchAllSignal);    // 		18	/* Continue (POSIX).  */
		signal(SIGSTOP, &catchAllSignal);    // 		19	/* Stop, unblockable (POSIX).  */
		signal(SIGTSTP, &catchAllSignal);    // 		20	/* Keyboard stop (POSIX).  */
		signal(SIGTTIN, &catchAllSignal);    // 		21	/* Background read from tty (POSIX).  */
		signal(SIGTTOU, &catchAllSignal);    // 		22	/* Background write to tty (POSIX).  */
		signal(SIGURG, &catchAllSignal);    // 		23	/* Urgent condition on socket (4.2 BSD).  */
		signal(SIGXCPU, &catchAllSignal);    // 		24	/* CPU limit exceeded (4.2 BSD).  */
		signal(SIGXFSZ, &catchAllSignal);    // 		25	/* File size limit exceeded (4.2 BSD).  */
		signal(SIGVTALRM, &catchAllSignal);    // 		26	/* Virtual alarm clock (4.2 BSD).  */
#ifndef __APPLE__
		signal(SIGPROF, &catchAllSignal);    // 		27	/* Profiling alarm clock (4.2 BSD).  */
#endif                
		signal(SIGWINCH, &catchAllSignal);    // 		28	/* Window size change (4.3 BSD, Sun).  */
#ifndef __APPLE__
		signal(SIGPOLL, &catchAllSignal);    // 		SIGIO	/* Pollable event occurred (System V).  */
#endif                
		signal(SIGIO, &catchAllSignal);    // 		29	/* I/O now possible (4.2 BSD).  */
#ifndef __APPLE__
		signal(SIGPWR, &catchAllSignal);    // 		30	/* Power failure restart (System V).  */
#endif
		signal(SIGSYS, &catchAllSignal);    // 		31	/* Bad system call.  */
#ifndef __APPLE__
		signal(SIGUNUSED, &catchAllSignal);    // 			31
#endif
#endif // !_WIN32

		PLOG_INFO("setAllSignalCatch. PID=%d...", getpid());
	}

public:
	void Run(const char *pipe_name)
	{
		try 
		{
			plogInit();
			LocalConfig::p_cfg = new LocalConfig();
			
			/** 20170418. Timeout de procesos 'colgados' */
			hangup_timeout_min = LocalConfig::p_cfg->getint(strRuntime, strRuntimeItemThreadActiveTimeout, "180");
			/** 20171016. Timeout maximo que acota posibles actualizaciones de RELOJ */
			hangup_timeout_max = hangup_timeout_min + 60;
			bool mode = LocalConfig::p_cfg->get(strRuntime, strRuntimeItemModoGlobal, "0")=="1"/*.ModoUlises()*/;
			string redan_mode = LocalConfig::p_cfg->get(strRuntime, strRuntimeItemModoRedan, "0");

			PLOG_INFO("%s (%s) CfgServer(pid: %d): (%s) Iniciado en \"%s\". PIPE-ID: %s", 
				Tools::read_txt_file(ON_WORKING_DIR("VERSION.TXT")).c_str(),  
				mode==false ? string("REDAN-" + redan_mode).c_str() : "ULISES",
				(int )getpid(),
				acBuildString, 
				WORKING_DIR,
				pipe_name
				);
			/** 20190308. Poner la Configuracion al nivel ppal */
			WorkingConfig *pcfg = InitialConfiguration(mode);
			pwebApp = new Uv5kiGwCfgWebApp(pcfg);

			/** Crearlo seg�n el entorno */
			if (mode==false)
				CfgProc::p_cfg_proc = new JsonClientProc(pcfg);
			else
				CfgProc::p_cfg_proc = new SoapClientProc(pcfg);

			HistClient::p_hist = new HistClient();
			ManProc::p_man = new ManProc();
			FileSupervisor::p_fspv = new FileSupervisor();

			/** Arranque de Threads */
			HistClient::p_hist->Start();
			ManProc::p_man->Start();
			CfgProc::p_cfg_proc->Start();
			FileSupervisor::p_fspv->Start();		

			pwebApp->Start();

			/* Lazo Principal */
			// Si se ejecuta desde CARPER, nos pasan como primer parametro el descriptor de pipe para refrescar al perro (a CARPER).
			int iDescPerro = pipe_name == NULL ? -1 : atoi(pipe_name);
			unsigned char ucPerro = 0;			// Envio al Perro...		
			unsigned int MainLoopLogInfoCount = 0;
			
			/* Capturar las Se�ales */
			setAllSignalCatch();
			PLOG_INFO("Entrando en Bucle de Supervision. Pipe ID: %d.", iDescPerro);

#ifdef _WIN32
			/** Lazo de Windows */
			//while (std::cin.rdbuf()->in_avail() == 0)
			Tools::fatalerror("Test de fatal error");
			SHORT tecla;
			while((tecla=GetKeyState('Q'))>=0)
#else
			while (salida == 0)
#endif
			{
				CThread::sleep(100);

				if ((MainLoopLogInfoCount % 20) == 0) {
					if (iDescPerro != -1) {

						write(iDescPerro, &ucPerro, 1);
						PLOG_INFO("Pipe %d <= Write", iDescPerro);
					}
				}
				if ((MainLoopLogInfoCount % 600) == 0 && iDescPerro == -1) {
					PLOG_ERROR("Error PIPE to CARPER NO Abierta.");
				}
				MainLoopLogInfoCount++;

				SupervisaProcesos();
			}

			/** Finalizacion Comun */
			PLOG_INFO("UG5k-APPSERVER Deteniendo threads hijos...");

			/** Parada de Threads */
			pwebApp->Dispose();

			FileSupervisor::p_fspv->Stop();
			CfgProc::p_cfg_proc->Stop();
			ManProc::p_man->Dispose();
			HistClient::p_hist->Dispose();

			delete FileSupervisor::p_fspv;
			delete CfgProc::p_cfg_proc;
			delete ManProc::p_man;
			delete HistClient::p_hist;

			delete LocalConfig::p_cfg;

			PLOG_INFO("UG5k-APPSERVER Finalizado.");
			CThread::sleep(1000);

			plogDispose();
		}
		catch(Exception x)
		{
			PLOG_EXCEP(x, "Main AppException");
			CThread::sleep(1000);
		}
		catch(...)
		{
			PLOG_EXCEP(NULL, "Main Unknow Exception");
			CThread::sleep(1000);
		}
	}

	/** 20170418. Timeout de procesos 'colgados' */
	/** 20171016. Timeout maximo que acota posibles actualizaciones de RELOJ */
	void SupervisaProcesos() {

		if (HistClient::p_hist->Tick.elapsed(hangup_timeout_min)==true && HistClient::p_hist->Tick.lastdiff() < hangup_timeout_max) {
			Tools::fatalerror("Reset por HISTCLIENT Colgado...");
			exit(-1);
		}
		if (CfgProc::p_cfg_proc->Tick.elapsed(hangup_timeout_min)==true && CfgProc::p_cfg_proc->Tick.lastdiff() < hangup_timeout_max) {
			Tools::fatalerror("Reset por CFGPROC Colgado...");
			exit(-1);
		}
		if (ManProc::p_man->Tick.elapsed(hangup_timeout_min)==true && ManProc::p_man->Tick.lastdiff() < hangup_timeout_max) {
			Tools::fatalerror("Reset por MANPROC Colgado...");
			exit(-1);
		}
		if (FileSupervisor::p_fspv->Tick.elapsed(hangup_timeout_min) == true && FileSupervisor::p_fspv->Tick.lastdiff() < hangup_timeout_max) {
			Tools::fatalerror("Reset por FILESUP Colgado...");
			exit(-1);
		}
		if (pwebApp->Tick.elapsed(hangup_timeout_min)==true && pwebApp->Tick.lastdiff() < hangup_timeout_max) {
			Tools::fatalerror("Reset por WEBSRV Colgado...");
			exit(-1);
		}
	}

	time_t hangup_timeout_min;
	time_t hangup_timeout_max;
	Uv5kiGwCfgWebApp *pwebApp;
	/** 20190308. Poner la Configuracion al nivel ppal */
	WorkingConfig *p_wcfg;
	WorkingConfig *InitialConfiguration(bool mode) {
		p_wcfg = new WorkingConfig(mode == false ? cfgRedan : cfgSoap);

		string ipPropia, hwServer, hwName;
		sistema::GetWorkingIpAddressAndName(ipPropia, hwServer, hwName);

		/** Leer la ultima CFG recibida */
		p_wcfg->load_from(LAST_CFG);
		PLOG_INFO("Leida LAST_CFG");
		if (mode == false) {
			/** MODO REDAN */
			p_wcfg->config.tipo = 0;
			// Comprobar que _ip_propia es igual a la configurada en LAST_CFG. Si es diferente implica que ha habido algún cambio HW
			// y hay que obtener la configuracion del Servidor...
			// Como la Configuracion actual no sirve para nada... se poden a DEFAULT...

			if (p_wcfg->ippropia() != ipPropia) {
				CommConfig cfg = p_wcfg->get_default();
				cfg.fechaHora = "01/01/1970 00:00:00 UTC";				// Pone Fecha 01/01/1970, para que sea actualizado.
				p_wcfg->set(cfg, false, true);
				//p_wcfg->set_to_default();
				//p_wcfg->ResetTimeStamp();				// Pone Fecha 01/01/1970, para que sea actualizado.
				//p_wcfg->save_to(LAST_CFG);

				PLOG_INFO("Detectado Cambio de IP-LOCAL. Nueva IP: %s", ipPropia.c_str());
			}
		}
		else {
			/** MODO ULISES */
			p_wcfg->config.tipo = 1;
			// Comprobar que el NOMBRE es igual a la configurada en LAST_CFG. Si es diferente implica que ha habido algún cambio HW
			// y hay que obtener la configuracion del Servidor...
			// Como la Configuracion actual no sirve para nada... se ponen a DEFAULT...
			if (mode == true && p_wcfg->config.general.name != hwName) {
				CommConfig cfg = p_wcfg->get_default();
				cfg.general.name = hwName;
				cfg.fechaHora = "01/01/1970 00:00:00 UTC";
				p_wcfg->set(cfg, false, true);
				//p_wcfg->set_to_default(true);
				//p_wcfg->config.general.name = hwName;
				//p_wcfg->ResetTimeStamp();
				//p_wcfg->save_to(LAST_CFG);
				PLOG_INFO("Detectado Cambio de ID. Nuevo ID: %s", hwName.c_str());
			}
		}
		return p_wcfg;
	}
};

/** */
#ifdef _WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
	/** Imprime la Version */
	if ( 2==argc )
	{
		if (!strncmp(argv[1],"/V",2))
		{
			printf( " %s   V%d.%d.%d (%s)\n",
				VRS_ID_SOFTWARE,
				VRS_VERSION_MAYOR, VRS_VERSION_MENOR_A, VRS_VERSION_MENOR_B,
				acBuildString );
			return -1;
		}
		else if (!strncmp(argv[1],"/JV",3)) {
			webData_VersionNucleoNew versiones;
			versiones.loadfrom(versiones.onfs("/home/serv/versiones.json"));
			versiones.printComponents();
			return -1;
		}
	}

	/** Arranca la aplicacion */
	U5kiGwCfg app;
	app.Run(argc==2 ? argv[1] : NULL);
	return 0;
}






