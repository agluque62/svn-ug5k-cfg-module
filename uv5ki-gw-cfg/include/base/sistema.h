#ifndef _SISTEMA_
#define _SISTEMA_

#include "./code-base.h"

#include <time.h>

#ifdef _WIN32
	typedef __int32				int32_t;
	typedef unsigned __int32	uint32_t;
#else
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <sys/ioctl.h>
 #include <netinet/in.h>
 #include <net/if.h>
 #include <arpa/inet.h>

 #include <unistd.h>
 #include <string.h>
 #include <netdb.h> 

 #include <signal.h>

 /** 20180322. Se han añadido estos includes para el semaforo global */
 #include <sys/ipc.h>
 #include <sys/sem.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <errno.h>

#if defined(_PPC82xx_)
 #include <linux/config.h>
 #include <asm/ioctl.h>
#endif

#endif

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class sistema : CodeBase
{
public:
	sistema(void);
	~sistema(void);

public:
	static void ExecuteCommand(char *cmd);
    static string ResultExecuteCommand(char *cmd, string paraTest="");
        
	//static void GetIfconfigCommand(char *cmd, const char *ift, const char *ip, const char *msc);
	//static void GetIfenslaveCommand(char *cmd, const char *ifbond, const char *if1, const char *if2);
	//static void GetRouteCommand(char *cmd, const char *src, const char *dest);
	//static void GetRebootCommand(char *cmd);
	//static void GetTarCommand(char *cmd, const char *file, const char *dir, const char *src);
	//static void GetUnTarCommand(char *cmd, const char *file, const char *dest);
	//static void GetCdCommand(char *cmd, const char *dir);
	//static void GetMvCommand(char *cmd, const char *src, const char *dest);

	//static void GetRouteAddCommand(char *cmd, const char *dest, const char *via, const char *src);
	//static void GetRouteDelCommand(char *cmd, const char *dest);
	//static void GetRouteGet(char *cmd);

	static bool GetIpAddress(string &ip);
	static void GetMacAddress(char *lan, char *mac);
	static void GetWorkingIpAddressAndName(string &ip, string &ipserv, string &name);

	static void SignalNuevaConfiguracion(string npid);
	static int  pidof(string name);

	static string GetDelCommand(string path);
	static string GetCpCommand(string src, string dest);
	static string NormalizePath(string path);
	static string PathFileName(string path);

	static void DataSaveAs(string data, string filename);

	static bool MainOrStandby();
	static time_t _TIMER_CLK();

	/** */
	static string SnmpAgentVersion();
	static string RecordServiceVersion();

	/** */
	static int ParImpar();

	/** */
	static void fileattr(string path, int modo, string &date, string &size);

	/** 20171031. Obtener el colateral */
	static string ipColateral();

	/** 20180503. Obtener los Tick del sistema */
	static int TickCount();
	static time_t time();
	static time_t time_max();
	
};

/** Gestiona TEMPORIZADORES de Segundos */
#define _TIMER_SET( val, delta )	(val) = sistema::_TIMER_CLK() + (delta)
#define _TIMER_SUB( t1, t2 )		((int32_t)(((uint32_t)t1)-((uint32_t)t2)))
#define _TIMER_DELTA( val )			_TIMER_SUB( val, sistema::_TIMER_CLK() )
#define _TIMER_EXPIRED( val )		((val) && _TIMER_DELTA(val)<=0)
#define _TIMER_CLR( val ) 			(val) = 0
#define _TIMER_IS_RUNNING( val )	(val)

/** 20180322. Semaforo Global */
#ifndef _WIN32
	#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
	// La union ya está definida en sys/sem.h
	#else
	// Tenemos que definir la union
	union semun
	{
		int val;
		struct semid_ds *buf;
		unsigned short int *array;
		struct seminfo *__buf;
	};
	#endif
#endif
class global_semaphore : CodeBase
{
public:
	global_semaphore(int id, int count);
	~global_semaphore();
public:
	bool acquire(int seconds_timeout =5);
	bool release();
protected:
	int value();
	int Id;
	int procid;
	int initial_count;
public:
	static void Test();
};
 
#endif
