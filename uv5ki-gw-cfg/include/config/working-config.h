#ifndef __WORKING_CONFIG_H__
#define __WORKING_CONFIG_H__

#ifdef _WIN32
#else
	#include <sys/types.h>
	#include <sys/ipc.h>
	#include <sys/shm.h>
#endif 

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream#include <utility>
#include <queue>
#include <algorithm>

#include "../base/code-base.h"
#include "../base/thread.h"
#include "../base/socket.h"
#include "../base/sistema.h"
#include "../tools/tools.h"
#include "../config/local-config.h"
#include "./comm-config.h"
#include "./soap-config.h"
#include "./comm-conversor.h"

#include "./cfgpasa.hpp"

/** La configuracion en memoria */
typedef struct cfgConfigPasarela ug5k_mem_config ;
enum WorkingConfigMode { cfgRedan, cfgSoap };

/** Estructura de CONFIGURACION. Se parte de REDAN */
class WorkingConfig : public CodeBase
{
public:
	WorkingConfig(WorkingConfigMode mode);
	~WorkingConfig();

public:
	EventosHistoricos *set(CommConfig &redanConfig, bool actualiza_ini, bool bsave);
	void set(soap_config &sConfig);
	void set();
	void load_from(string file);
	void save_to(string file);
	//void set_to_default(bool bsave);
	CommConfig get_default();

	void FromExternalSet(bool isServer, string user, CommConfig& redanConfig, bool actualiza_ini);

public:
	void TimeStamp();
	void TimeStamp(RedanTestComm &remota);
	void ResetTimeStamp() {
		if (cfg_mode == cfgRedan || cfg_mode == cfgSoap) {
			config.fechaHora = "01/01/1970 00:00:00 UTC";
			return;	
		}
		throw Exception("Modo de Configuracion no implementado obteniendo timestamp...");	
	}
	bool HasResources() {
		return (config.recursos.size() != 0);
	}
	void ResourcesClear();
	string JConfig();
	string IdConfig() {
		return config.idConf;
	}
	string TimConfig() {
		return config.fechaHora;
	}
	string OrigenConfig() {
		return config.origenCfg;
	}

	bool UserAccess(string user, string pwd, int *profile);
	bool IpColateral(string &ipCol) {
		return config.IpColateral(ipCol);
	}
	bool DualCpu() {
		return config.general.dualidad==1;
	}
	bool TestConfig() {
		return config.test();
	}
	int cpu1cpu2() {
		string ippropia;
		if (sistema::GetIpAddress(ippropia))
		{
			if (ippropia == ipcpu(0))
				return 0;
			if (ippropia == ipcpu(1))
				return 1;
			PLOG_ERROR("CommConfig::cpu1cpu2: ERROR CPU0-CPU1 No coincide con IPPROPIA !!!");
			return -1;
		}	
		PLOG_ERROR("CommConfig::cpu1cpu2: ERROR. NO IP PROPIA !!!");
		return -1;
	}
	string ipcpu(int cpu) {
		return cpu==0 ? config.general.cpus[0].ipb : cpu==1 ? config.general.cpus[1].ipb : "ip-erronea";
	}
	string ippropia() {
		return ipcpu(cpu1cpu2());
	}

	bool IsCfgDefecto() {
		if (config.origenCfg == "CFGDEF")
			return true;
		else
			return false;
	}


	/** Para ulises */
	void UlisesParamsMulticast(string &ip, int &port) {
		ip = config.ulises.MulticastGroup;
		port = config.ulises.MulticastPort;
	}

	/** 20171031. Obtener dinamicamente la ip del colaterl */
	void refreshCpuPair() {
		string ipCol = sistema::ipColateral();
		if (sistema::ParImpar()==0)
		{
			config.general.cpus[1].ipb = ipCol;
			config.general.cpus[1].ipg = ipCol;
		}
		else
		{
			config.general.cpus[0].ipb = ipCol;
			config.general.cpus[0].ipg = ipCol;
		}
	}

public:
	bool IdConfig(string &idConfig, string &fechaHora);
	void dispose();

	/** */
	unsigned int WebPort() {
		unsigned int wport = config.servicios.web.wport;
		wport = wport <= 0 || wport > 32767 ? 8080 : wport;
		return wport;
	}
	int WebSessionTime() {
		int stime = config.servicios.web.stime;
		stime = stime < 0 || stime > 120 ? 0 : stime;
		return stime;
	}

protected:
	void init();
	static void *DelayedSignal(void *arg);
	static void _recResponse(bool res, int len, void *data);

private:
	WorkingConfigMode cfg_mode;
	global_semaphore *p_mem_config_sem;
	ug5k_mem_config *p_mem_config;
	CommConversor redanConv;
#ifdef _WIN32
#else
	key_t key_mem;
	int  shmid;
#endif 
public:
	CommConfig config;
};

#endif
