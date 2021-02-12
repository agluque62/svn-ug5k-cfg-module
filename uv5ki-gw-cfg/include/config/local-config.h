#ifndef _LOCALCONFIG_
#define _LOCALCONFIG_

#include "../base/code-base.h"

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <cstdlib>

#include "../tools/stlini.h"
#include "../tools/tools.h"

using namespace std;
#define strFile							ON_WORKING_DIR("u5kweb-config.ini")

#define strSection						((const char *)"U5KWEB")
#define strItemWebPort							((const char *)"WEB-PORT")
#define strItemSessionTime						((const char *)"SESSION-TIME")
#define strItemPath2Versiones					((const char *)"PATH-VERSIONES")
#define strItemLog								((const char *)"LOG")
#define strItemLogin							((const char *)"LOGIN")
#define strItemFtpGenTimeout					((const char *)"FTP-GEN-TIMEOUT")
#define strItemFtpSendTimeout					((const char *)"FTP-SEND-TIMEOUT")
#define strItemHttpGenTimeout					((const char *)"HTTP-GEN-TIMEOUT")
#define strItemClearResourcesOnBdt				((const char *)"CLEAR-RESOURCES-ONNOBDT")
#define strItemServidor							((const char *)"CONFIG-SERVER")
#define strItemNetworkInterfaceActiva			((const char *)"ETH-ACTIVA")
#define strItemConfigFile						((const char *)"CONFIG-FILE")
#define strItemConfigTSUP						((const char *)"CONFIG-TSUP")
#define strItemNucleoProcName					((const char *)"NUCLEO-PROC-NAME")
#define strItemSnmpProcName						((const char *)"SNMP-PROC-NAME")
#define strItemRecordProcName					((const char *)"RECORD-PROC-NAME")

#define strSectionStrings				((const char *)"U5KWEB-KEY-IDENT")
#define strSectionLast							((const char *)"U5KWEB-LAST")
#define strLastIpDest							((const char *)"IP-DEST")
#define strLastIpVia							((const char *)"IP-VIA")
#define strLastIpSource							((const char *)"IP-SRC")

#define strModulos						((const char *)"MODULOS")
#define strItemModuloSnmp						((const char *)"SNMP-CONFIG-FILE")
#define strItemModuloGrabador					((const char *)"GRAB-CONFIG-FILE")
#define strItemExeSnmp							((const char *)"SNMP-EXE-FILE")
#define strItemExeGrabador						((const char *)"GRAB-EXE-FILE")

#define strRuntime						((const char *)"G5K-RUNTIME")
#define strRuntimeItemModoGlobal				((const char *)"MODO")
#define strRuntimeItemModoRedan					((const char *)"MODO-REDAN")
#define strRuntimeItemLocalHttpTimeout			((const char *)"LOCAL_HTTP_TIMEOUT")
#define strRuntimeItemRedanHttpGetTimeout		((const char *)"REDAN_HTTP_GET_TIMEOUT")
#define strRuntimeItemRedanHttpPostTimeout		((const char *)"REDAN_HTTP_POST_TIMEOUT")
#define strRuntimeItemUlisesHttpTimeout			((const char *)"ULISES_HTTP_TIMEOUT")
#define strRuntimeItemThreadActiveTick			((const char *)"THREAD_ACTIVE_TICK")
#define strRuntimeItemThreadActiveTimeout		((const char *)"THREAD_ACTIVE_TIMEOUT")
#define strRuntimeItemHttpCharTimeout			((const char *)"HTTP_CHAR_TIMEOUT")
#define strRuntimeItemTcpConnTimeout			((const char *)"TCP_CONN_TIMEOUT")
#define strRuntimeItemColateralIpInfoPath		((const char *)"COLATERAL_IP_INFO_PATH")
#define strRuntimeItemSoapServicePath			((const char *)"SOAP-SERVICE-PATH")
#define strRuntimeItemForceRdAudioNormal		((const char *)"FORCE_RDAUDIO_NORMAL")


#define strFilesSupervidor				((const char *)"FILESUPERVISOR")
#define strKeySupervisedFile					((const char *)"FILE")
//#ifdef _WIN32
// #define strKeySupervisedFile			((const char *)"WFILE")
//#else
// #define strKeySupervisedFile			((const char *)"FILE")
//#endif
#define strSupervisedFileUnlockTime		((const char *)"UNLOCKTIME")

#ifdef _WIN32
 #define strWindowsTest					((const char *)"WINDOWS-TEST")
 #define strItemWindowsTestIp					((const char *)"IP-WINDOWS")
 #define strItemWindowsTestSyncFtp				((const char *)"SINCR-FTP")
 #define strItemWindowsTestSnmpStd				((const char *)"SNMPSTD")
 #define strItemWindowsTestServidor				((const char *)"SINCR-SERV")
 #define strItemWindowsWebPort					((const char *)"WEBPORT")
#endif

#define ON_SWREP(p)						(LocalConfig::onswrep(p).c_str())

/** */
#define LAST_CFG						(onflash(LocalConfig::p_cfg->get(strSection, strItemConfigFile).c_str()))
#define LAST_CFG_PRUEBA					(onflash("gw_config_prueba.json"))
#define LAST_SAVE(n)					(onflash(string("last_gw_config_")+string(n)+string(".json")))
#define LAST_JSON_REC(n)				(string("last_json_rec_")+string(n)+string(".json"))
#define LAST_SOAP_REC(n)				(string("last_soap_rec_")+string(n)+string(".soap"))

///** */
//typedef map<string,string> MapString;
//
///** */
//class stStrings
//{
//	public:
//		MapString ToIdent;
//		MapString ToKey;
//};
//
///** */
//class LocalConfig : CodeBase
//{
//public:
//	static LocalConfig cfg;
//
//private:
//	INIFile ini;
//	string namefile;
//
//private:
//	stStrings _strings;
//#ifdef _LOCAL_LOCK_
//	CCritSec _lock;
//#endif
//
//public:
//	LocalConfig(void);
//	LocalConfig(string strfile);
//	~LocalConfig(void);
//
//	string getString(string section, string item, string def="");
//	int getInt(string section, string item, string def="");
//	void setString(string section, string item, string valor);
//	void setInt(string section, string item, int valor);
//
//public:
//	void save(void);
//	void sync();
//
//	string PuertoEscucha(string puerto="NoPuerto");
//	string TiempoSesion(string segundos="NoSegundos");
//	string ServerURL();
//
//	string PathToPreconf(string path="NoPath");
//	string PathToVersiones();
//	int FtpGenTimeout() {return atoi(getString(strSection, strItemFtpGenTimeout,"5").c_str());}
//	int FtpSendTimeout(){return atoi(getString(strSection, strItemFtpSendTimeout,"6000").c_str());}
//	int HttpGenTimeout(){return atoi(getString(strSection, strItemHttpGenTimeout,"5").c_str());}
//	int ClearResourcesOnBdt(){return atoi(getString(strSection, strItemClearResourcesOnBdt,"1").c_str());}
//
//	string Log();
//	string Login();
//	int ConfigTsup();
//
//    string NucleoProcName();
//    string SnmpProcName();
//    string RecordProcName();
//    string NetworkInterfaceActiva();
//	string ConfigFile();
//
//	/** */
//	string LastRouteDest(string ip="GetIp");
//	string LastRouteVia(string ip="GetIp");
//	string LastRouteSource(string ip="GetIp");
//
//	/** SECCION ULISES */
//	bool ModoUlises() {
//		int md = getInt(strUlises, strItemUlisesModo, "0");
//		return md==1 ? true : false;
//	}
//
//	/** */
//	static string onswrep(string filename);
//	static string snmpModule()	{return onfs(LocalConfig::cfg.getString(strModulos, strItemModuloSnmp,"./ug5ksnmp-config.ini"));} 
//	static string recModule()	{return onfs(LocalConfig::cfg.getString(strModulos, strItemModuloGrabador, "./ug5krec-config.ini"));} 
//	/** */
//	static string snmpExe()	{return onfs(LocalConfig::cfg.getString(strModulos, strItemExeSnmp,"/home/snmp/ug5ksnmp"));} 
//	static string recExe()	{return onfs(LocalConfig::cfg.getString(strModulos, strItemExeGrabador, "/home/rec/UG5KEd137b4Service"));} 
//
//	static int SupervisedFileUnlocktime() {
//		return LocalConfig::cfg.getInt(strFilesSupervidor, strSupervisedFileUnlockTime, "10");
//	}
//
//	/** */
//	INISection getSection(string section); 
//	void deleteSection(string section);
//
//private:
//	/** */
//	void LoadStrings();
//	/** */
//	string KeyToIdent(string key);
//	string IdentToKey(string ident);
//	/** */
//	void GetKeyConfigLocal(INISection &config);
//	void SetKeyConfigLocal(string key, string data);
//
//#ifdef _WIN32
//public:
//	string ipWindows() { return GetIniSetting(ini, strWindowsTest, strItemWindowsTestIp, "192.168.1.129");}
//	bool winSyncFtp() { return GetIniSetting(ini, strWindowsTest, strItemWindowsTestSyncFtp, "0") == "0" ? false : true;}
//	bool winStdSnmp() { return GetIniSetting(ini, strWindowsTest, strItemWindowsTestSnmpStd, "0") == "0" ? false : true;}
//	bool winSyncSer() { return GetIniSetting(ini, strWindowsTest, strItemWindowsTestServidor, "0") == "0" ? false : true;}
//#endif
//};

/** */
class LocalConfig : public CodeBase
{
public:
	static LocalConfig *p_cfg;
	/** 20171016. Mutex Global para controlar los accesos a ficheros INI */
	static pthread_mutex_t global_mutex;

public:
	LocalConfig(string filename=strFile) {
		_filename = filename;

		/** 20171016. Mutex Global para controlar los accesos a ficheros INI */
		GlobalAccessAcquire();
		ini = LoadIni(_filename.c_str());
		GlobalAccessRelease();
	}
	~LocalConfig() {}
public:
	string get(string section, string key, string def="") {
		return GetIniSetting(ini, section.c_str(), key.c_str(), def.c_str());
	}
	int getint(string section, string key, string def="") {
		return atoi(get(section, key, def).c_str());
	}
	INISection &get(string section) {
		return ini[section.c_str()];
	}
	void set(string section, string key, string val, bool save=false) {
		PutIniSetting(ini, section.c_str(), key.c_str(), val.c_str());
		if (save) {
			/** 20171016. Mutex Global para controlar los accesos a ficheros INI */
			GlobalAccessAcquire();
			SaveIni(ini, _filename.c_str());
			GlobalAccessRelease();
		}
	}
	void del(string strsection) {
		INISection vacia;
		ini[strsection.c_str()] = vacia;	
	}
	void save() {			
		/** 20171016. Mutex Global para controlar los accesos a ficheros INI */			
		GlobalAccessAcquire();
		SaveIni(ini, _filename.c_str() );
		GlobalAccessRelease();
	}
protected:
	void GlobalAccessAcquire() 
	{
		pthread_mutex_lock(&LocalConfig::global_mutex);
	}
	void GlobalAccessRelease()
	{
		pthread_mutex_unlock(&LocalConfig::global_mutex);
	}
private:
	INIFile ini;
	string _filename;
};

/** */
class DatosLocales : public CodeBase
{
public:
	DatosLocales() {
		INIFile ini = LoadIni(onram("DatosLocales.ini").c_str());

		idGw = GetIniSetting(ini, "DATOS_LOCALES", "Identificador_Pasarela", "GWDEF");
		dualidad = GetIniSetting(ini, "DATOS_LOCALES", "Dual_CGW", "n")=="s" ? true : false;
		ipServ = GetIniSetting(ini, "DATOS_LOCALES", "Url_Servidor", "127.0.0.1");
		ipServ = url(ipServ).host_;
		nivel_consola = atoi(GetIniSetting(ini, "DATOS_LOCALES", "ConsolaNivel", "0").c_str());
		puerto_consola = atoi(GetIniSetting(ini, "DATOS_LOCALES", "ConsolaPuerto", "19710").c_str());
		nivel_incidencias = atoi(GetIniSetting(ini, "DATOS_LOCALES", "IncidenciasNivel", "3").c_str());
	}
	~DatosLocales(){}
public:
	string idGw;
	bool dualidad;
	string ipServ;
	int nivel_consola;
	int puerto_consola;
	int nivel_incidencias;
};

#endif
