#ifndef __CFG_PROC__
#define __CFG_PROC__

#include "./base/code-base.h"
#include "./base/thread.h"
#include "./base/socket.h"
#include "./tools/tools.h"
#include "./tools/http-client.h"
#include "./config/local-config.h"
#include "./websrv/uv5kigwcfg-web-app.h"
#include "./config/working-config.h"
#include "./config/comm-preconf.h"

using namespace std;

#define P_CFG_PROC					(CfgProc::p_cfg_proc)
#define P_WORKING_CONFIG			(P_CFG_PROC->p_working_config)

#define MAIN_PIDE_CONFIG			((const char *)"pideconfig")
#define MAIN_TEST_CONFIG			((const char *)"testconfig")
#define MAIN_SUBIR_CONFIG			((const char *)"subirconfig")
#define LAST_CONFIG_ONLINE			((const char *)"Ultima Configuracion online")

/** Estado de la Configuracion respecto a la Base de Datos */
enum eStdLocalConfig
{
	slcSincronizado=0,
	slcNoBdt = -1,
	slcNoActiveCfg = -2,
	slcAislado=-3,
	slcConflicto=-4,
	slcNoInicializado=-5
};

/** */
typedef struct
{
	string ip;
	string main;
	string cmd;
} stAviso;


/** */
class CfgProc : public CThread
{
public:
	CfgProc();
	~CfgProc();

public:
	static CfgProc *p_cfg_proc;
	WorkingConfig *p_working_config;

public:
	//void AvisaCambioConfiguracion(string host);
	void AvisaSubirConfiguracion();
	void AvisaPideConfiguracion(string cfg="");
	void AvisaChequearConfiguracion();
	bool Get(stAviso &aviso);

public:
	bool IsIdle() {
		return (_stdLocalConfig==slcAislado || _stdLocalConfig==slcNoInicializado);
	}
	bool IdConfig(int &std, string &id, string &tim, string &origen);
	eStdLocalConfig GetStdLocalConfig()
	{
		CCSLock _lock(m_lock);
		return _stdLocalConfig;
	}
	string Modo() {
		return modo;
	}
	string LastServCfgTime(){return _last_cfgr_time;}

protected:
	void StdSincrSet(eStdLocalConfig nstd/*, jgw_config &local*/);
	string StdSincrGet();
	//void GeneraAvisosCpu(string host, string cmd);
	void ResourcesConfigClear();

protected:
	CCritSec m_lock;
	ColaAventos<stAviso> avisos;
	string _ip_propia;
	eStdLocalConfig _stdLocalConfig;
	//int _lastcfg;
	int _cntticks;
	int _maxticks;
	bool _bconflicto;
	string modo;
protected:
	static string hwIp;
	static string hwName;
	static string hwServer;
	string _last_cfgr_time;
};


/** */
class JsonClientProc : public CfgProc
{
public:
	JsonClientProc(WorkingConfig *pcfg) {
		p_working_config = pcfg;	//  new WorkingConfig(cfgRedan);
		_modo_redan = "0";
	}
	~JsonClientProc(void)
	{
	};

protected:
	void Run();
	void Dispose();

	void SupervisaProcesoConfiguracion();
	void ChequearConfiguracion();
	void PedirConfiguracion(string cfg);
	void SubirConfiguracion();

	// REDAN V2. Para mantener sincronizadas las CFG's
	eStdLocalConfig ConfigurationSync();

private:
	// CommConfig cfg_redan;
	string _modo_redan;
};

/** */
class SoapClientProc : public CfgProc
{
public:
	SoapClientProc(WorkingConfig *pcfg) {
		p_working_config = pcfg;	// new WorkingConfig(cfgSoap);
		p_mcast_socket = NULL;
	}
	~SoapClientProc() {
	}

protected:
	void Run();
	void Dispose();

	void SupervisaProcesoConfiguracion();
	void ChequearConfiguracion();
	void PedirConfiguracion(string cfg);
	void SubirConfiguracion();

	void McastActivateOrDeactivate(bool activate);
	void McastTest();

protected:
	static string getXml(string rtName, string p1, string p2="", string p3="");

private:
	CommConfig cfg_soap;
	CUDPSocket *p_mcast_socket;
};

#endif

