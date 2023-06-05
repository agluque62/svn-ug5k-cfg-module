#ifndef __COMM_CONFIG_H__
#define __COMM_CONFIG_H__

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream#include <utility>
#include <queue>
#include <algorithm>

#include "../base/code-base.h"
#include "../base/thread.h"
#include "../base/socket.h"
#include "../base/sistema.h"
#include "../websrv/uv5kigwcfg-web-app-data.h"
#include "../tools/tools.h"
#include "../config/local-config.h"

#include "./soap-config.h"
#include "./users-config.h"
#include "./comm-gen-config.h"
#include "./comm-ser-config.h"
#include "./comm-har-config.h"
#include "./comm-res-config.h"
#include "./comm-uv5-config.h"
#include "./comm-mSDC91-config.h"

/** Estructura de CONFIGURACION. Se parte de REDAN */
class CommConfig : public jData
{
public:
	CommConfig() {
		/** Modo para las configuraciones por defecto */
		tipo = LocalConfig::p_cfg->get(strRuntime, strRuntimeItemModoGlobal, "0")=="1" ? 1 : 0;
		idConf = string("defecto_") + (tipo==0 ? string("redan") : string("ulises"));
		origenCfg = "CFGDEF";
		fechaHora = Tools::Ahora_Servidor();
	}
	CommConfig(string jstring) {
											// Configuracion desde REDAN
		JDeserialize(jstring);
		tipo = 0;
	}
	CommConfig(ifstream &f) {               // Configuracion desde fichero JSON
		string data,linea;
		while (std::getline(f, linea))
			data += linea;
		JDeserialize(data);
	}
	CommConfig(soap_config &sConfig);		// Configuracion desde servidor ULISES. (el tipo se pone a 1)
	~CommConfig() {
		clear_array(users);
		clear_array(recursos);
	}
//public:
//	bool operator == (CommConfig &otra) {
//		return (idConf == otra.idConf && fechaHora == otra.fechaHora) ? true : false;
//	}
//	bool operator < (CommConfig &otra) {
//		struct tm time_l,time_o;
//		time_t loctime,othtime;
//
//		Tools::DateString2time(fechaHora, time_l);
//		Tools::DateString2time(otra.fechaHora, time_o);
//
//		loctime = mktime(&time_l);
//		othtime = mktime(&time_o);
//
//		return loctime < othtime;	
//	}
public:
	bool IpColateral(string &ip) {
		if (general.dualidad != 0) 
		{
			string ippropia;
			if (sistema::GetIpAddress(ippropia))
			{
				return general.IpColateral(ippropia, ip);
			}	
			PLOG_ERROR("CommConfig::IpColateral: ERROR. NO IP PROPIA !!!");
		}
		return false;
	}
	bool test() {
		if (idConf == "Indice Desconocido")
			return false;
		bool bret = general.test() && servicios.test() && hardware.test();
		for (vector<CommResConfig>::iterator res = recursos.begin(); res != recursos.end(); res++)
			bret = bret && res->test();
		return bret;
	}

public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "tipo", tipo);
		write_key(writer, "idConf", idConf);
		write_key(writer, "fechaHora", fechaHora);
		write_key(writer, "origenCfg", origenCfg);
		write_key(writer, "general", general);
		write_key(writer, "hardware", hardware);
		write_key(writer, "recursos", recursos);
		write_key(writer, "users", users);
		write_key(writer, "servicios", servicios);
		write_key(writer, "ulises", ulises);
		write_key(writer, "modoSDC91", modoSDC91);
	}
	virtual void jread(Value &base) {
		read_key(base, "tipo", tipo);
		read_key(base, "idConf", idConf);
		read_key(base, "fechaHora", fechaHora);
		read_key(base, "origenCfg", origenCfg);
		read_key(base, "users", users);
		read_key(base, "general", general);
		read_key(base, "servicios", servicios);
		read_key(base, "hardware", hardware);
		read_key(base, "recursos", recursos);
		read_key(base, "ulises", ulises);
		read_key(base, "modoSDC91", modoSDC91);
		if (origenCfg == "Indice Desconocido")
			origenCfg = "CFGR";
	}

public:
	int tipo;
	string idConf;
	string fechaHora;
	string origenCfg;
	CommGenConfig general;
	CommSerConfig servicios;
	CommHarConfig hardware;
#if __POR_REFERENCIA__
	vector<UserData> users;
	vector<CommResConfig> recursos;
#else
	vector<UserData *> users;
	vector<CommResConfig *> recursos;
#endif
	CommUv5Config ulises;
	CommModoSDC91Config modoSDC91;

};

/** Para los Polling de Test */
class RedanTestComm : public jData
{
public:
	RedanTestComm(string jstr) {
		JDeserialize(jstr);
		tipo = 0;
	}
	RedanTestComm(CommConfig &cfg) {
		tipo = 0;
		idConf = cfg.idConf;
		fechaHora = cfg.fechaHora;
		localtime = Tools::Ahora_Servidor();
	}
public:
	bool isEqual(CommConfig &otra) {
		return (idConf == otra.idConf && fechaHora == otra.fechaHora) ? true : false;
	}
	bool isSameTime(CommConfig &otra) {
		return (fechaHora == otra.fechaHora) ? true : false;
	}
	bool isNewer(CommConfig &otra) {
		//struct tm time_l,time_o;
		//time_t loctime,othtime;

		//Tools::DateString2time(fechaHora, time_l);
		//Tools::DateString2time(otra.fechaHora, time_o);

		//loctime = mktime(&time_l);
		//othtime = mktime(&time_o);
		time_t loctime = ToTimet(fechaHora);
		time_t othtime = ToTimet(otra.fechaHora);

		return loctime > othtime;	
	}
	bool isSync(int margin) {
		time_t cfgloctime = ToTimet(localtime);
		time_t loctime = ToTimet(Tools::Ahora_Servidor());

		return abs(cfgloctime - loctime) <= margin;
	}
protected:
	time_t ToTimet(string date) {
		struct tm stm;
		Tools::DateString2time(date, stm);
		return mktime(&stm);
	}

public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "idConf", idConf);
		write_key(writer, "fechaHora", fechaHora);
		write_key(writer, "localtime", localtime);
	}
	virtual void jread(Value &base) {
		read_key(base, "idConf", idConf);
		read_key(base, "fechaHora", fechaHora);
		read_key(base, "localtime", localtime);
	}
public:
	int tipo;
	string idConf;
	string fechaHora;
	string localtime;
};

#endif
