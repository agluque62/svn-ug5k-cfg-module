#ifndef __COMM_GEN_CONFIG_H__
#define __COMM_GEN_CONFIG_H__

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream#include <utility>
#include <queue>
#include <algorithm>

#include "../base/code-base.h"
#include "../websrv/uv5kigwcfg-web-app-data.h"
#include "../tools/tools.h"
#include "../config/local-config.h"
#include "../base/sistema.h"

/** */
class CommGenCpu : public jData
{
public:
	CommGenCpu() {
		tlan=1;
		ip1 = ipb = ip0 = ipg = "";
		ms0 = ms1 = msb = "255.255.255.0";
	}
	CommGenCpu(string ip, string msc, string gw) {
		tlan=1;
		ip0 = ip1 = ms0 = ms1 = "";
		ipb = ip;
		msb = msc;
		ipg = gw;
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "tlan", tlan);
		write_key(writer, "ip0", ip0);
		write_key(writer, "ms0", ms0);
		write_key(writer, "ip1", ip1);
		write_key(writer, "ms1", ms1);
		write_key(writer, "ipb", ipb);
		write_key(writer, "msb", msb);
		write_key(writer, "ipg", ipg);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "tlan", tlan);
		read_key(base, "ip0", ip0);
		read_key(base, "ms0", ms0);
		read_key(base, "ip1", ip1);
		read_key(base, "ms1", ms1);
		read_key(base, "ipb", ipb);
		read_key(base, "msb", msb);
		read_key(base, "ipg", ipg);
	}

public:
	int tlan;
	string ip0;
	string ip1;
	string ipb;
	string ms0;
	string ms1;
	string msb;
	string ipg;
};

/** */
class CommGenConfig : public jData
{
public:
	CommGenConfig() {
		/** Leo los datos del Hardware */
		sistema::GetWorkingIpAddressAndName(ipv, ips, name);

		name += "-DEF";
		emplazamiento="EMPL-DEF";

		nivelconsola = 0;
		puertoconsola = 0;
		nivelIncidencias = 0;

		/** En Modo ULISES la configuracion por defecto es NoDual en Redan es Dual */
		dualidad = LocalConfig::p_cfg->get(strRuntime, strRuntimeItemModoGlobal, "0")=="1" ? 0 : 1;

		if (this->dualidad==0)
		{
			this->cpus.push_back(CommGenCpu(ipv,"255.255.255.0",ipv));
			this->cpus.push_back(CommGenCpu());
		}
		else 
		{
			string ipColDef = "";
			if (sistema::ParImpar()==0)
			{
				this->cpus.push_back(CommGenCpu(ipv,"255.255.255.0",ipv));
				this->cpus.push_back(CommGenCpu(ipColDef,"255.255.255.0",ipColDef));
			}
			else
			{
				this->cpus.push_back(CommGenCpu(ipColDef,"255.255.255.0",ipColDef));
				this->cpus.push_back(CommGenCpu(ipv,"255.255.255.0",ipv));
			}
		}
		//cpus.push_back(CommGenCpu());
		//cpus.push_back(CommGenCpu());
	
		/** 20181016. U2510. SP#01-15*/
		SupervisionLanGW=0;
		TmMaxSupervLanGW=1;
		/** 20190221. REDAN107 */
		dvrrp = 2000;
	}
	CommGenConfig(soap_config &sc);
	~CommGenConfig() {
		clear_array(cpus);
	}
public:
	bool IpColateral(string ipPropia, string &ip) {

		if (cpus.size() < 2)
			return false;
		
		if (ipPropia==cpus[0].ipb)
		{
			ip = cpus[1].ipb;
			return Tools::ip_format_test(ip);
		}
		else if (ipPropia==cpus[1].ipb)
		{
			ip = cpus[0].ipb;
			return Tools::ip_format_test(ip);
		}		
		
		PLOG_ERROR("CommGenConfig::IpColateral: ERROR. NO IP PROPIA!!!");
		return false;
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "name", name);
		write_key(writer, "emplazamiento", emplazamiento);
		write_key(writer, "dualidad", dualidad);
		write_key(writer, "ipv", ipv);
		write_key(writer, "ips", ips);
		//write_key(writer, "acGrupoMulticast", acGrupoMulticast);
		//write_key(writer, "uiPuertoMulticast", uiPuertoMulticast);
		write_key(writer, "nivelconsola", nivelconsola);
		write_key(writer, "puertoconsola", puertoconsola);
		write_key(writer, "nivelIncidencias", nivelIncidencias);
		write_key(writer, "cpus", cpus);

		/** 20181016. U2510. SP#01-15*/
		write_key(writer, "SupervisionLanGW", SupervisionLanGW);
		write_key(writer, "TmMaxSupervLanGW", TmMaxSupervLanGW);
		/** 20190221. REDAN107 */
		write_key(writer, "dvrrp", dvrrp);
	}
	virtual void jread(Value &base)
	{
		DatosLocales datos_locales;

		read_key(base, "name", name);
		read_key(base, "emplazamiento", emplazamiento);
		read_key(base,"dualidad", dualidad);
		read_key(base, "ipv", ipv);
		// read_key(base, "ips", ips);							// El servidor no se lee del fichero sino de la configuracion...
		ips = datos_locales.ipServ;
		//read_key(base,"acGrupoMulticast", acGrupoMulticast);
		//read_key(base,"uiPuertoMulticast", uiPuertoMulticast);
		read_key(base,"nivelconsola", nivelconsola);
		read_key(base,"puertoconsola", puertoconsola);
		read_key(base,"nivelIncidencias", nivelIncidencias);
		read_key(base, "cpus", cpus);

		/** 20181016. U2510. SP#01-15*/
		read_key(base,"SupervisionLanGW", SupervisionLanGW, 0);
		read_key(base, "TmMaxSupervLanGW", TmMaxSupervLanGW, 1);
		/** 20190221. REDAN107 */
		read_key(base, "dvrrp", dvrrp, 2000);
	}

public:
	string name;
	string emplazamiento;
	int dualidad;
	string ipv;
	string ips;
	int nivelconsola;
	int puertoconsola;
	int nivelIncidencias;
#if __POR_REFERENCIA__
	vector<CommGenCpu> cpus;
#else
	vector<CommGenCpu *> cpus;
#endif
	/** 20181016. U2510. SP#01-15*/
	int SupervisionLanGW;
	int TmMaxSupervLanGW;
	/** 20190221. REDAN107 */
	int dvrrp;

	//string acGrupoMulticast;
	//int uiPuertoMulticast;
};

#endif
