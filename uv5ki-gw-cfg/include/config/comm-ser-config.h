#ifndef __COMM_SER_CONFIG_H__
#define __COMM_SER_CONFIG_H__

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream#include <utility>
#include <queue>
#include <algorithm>

#include "../base/code-base.h"
#include "../websrv/uv5kigwcfg-web-app-data.h"
#include "../tools/tools.h"
#include "../config/local-config.h"

/** */
class CommSerServidor : public jData
{
public:
	CommSerServidor(string Ip="", int Selected=0) {
		ip=Ip;
		selected = Selected;
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "ip", ip);
		write_key(writer, "selected", selected);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "ip", ip);
		read_key(base, "selected", selected);
	}
public:
	string ip;
	int selected;
};

/** */
class CommSerSip : public jData
{
public:
	CommSerSip() {
		PuertoLocalSIP=5060;
		KeepAlivePeriod=200;
		KeepAliveMultiplier=10;
		SupresionSilencio=0;
		SupervisionTlf=0;
		PeriodoSupervisionSIP=100;
		Refresher=0;

		proxys.push_back(CommSerServidor());
		proxys.push_back(CommSerServidor());
		proxys.push_back(CommSerServidor());

		registrars.push_back(CommSerServidor());
		registrars.push_back(CommSerServidor());
		registrars.push_back(CommSerServidor());
	}
	~CommSerSip() {
		clear_array(proxys);
		clear_array(registrars);
	}

public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "PuertoLocalSIP", PuertoLocalSIP);
		write_key(writer, "KeepAlivePeriod", KeepAlivePeriod);
		write_key(writer, "KeepAliveMultiplier", KeepAliveMultiplier);
		write_key(writer, "SupresionSilencio", SupresionSilencio);
		write_key(writer, "SupervisionTlf", SupervisionTlf);
		write_key(writer, "PeriodoSupervisionSIP", PeriodoSupervisionSIP);
		write_key(writer, "Refresher", Refresher);
		write_key(writer, "proxys", proxys);
		write_key(writer, "registrars", registrars);

	}
	virtual void jread(Value &base) {
		read_key(base, "PuertoLocalSIP", PuertoLocalSIP);
		read_key(base, "KeepAlivePeriod", KeepAlivePeriod);
		read_key(base, "KeepAliveMultiplier", KeepAliveMultiplier);
		read_key(base, "SupresionSilencio", SupresionSilencio);
		read_key(base, "SupervisionTlf", SupervisionTlf);
		read_key(base, "PeriodoSupervisionSIP", PeriodoSupervisionSIP);
		read_key(base, "Refresher", Refresher);
		read_key(base, "proxys", proxys);
		read_key(base, "registrars", registrars);
	}

public:
	int PuertoLocalSIP;
	int KeepAlivePeriod;
	int KeepAliveMultiplier;
	int SupresionSilencio;
	int SupervisionTlf;
	int PeriodoSupervisionSIP;
	int Refresher;

#if __POR_REFERENCIA__
	vector<CommSerServidor> proxys;
	vector<CommSerServidor> registrars;
#else
	vector<CommSerServidor *> proxys;
	vector<CommSerServidor *> registrars;
#endif
};

/** */
class CommSerWeb : public jData
{
public:
	CommSerWeb() {
		wport=8080;
		stime=0;
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "wport", wport);
		write_key(writer, "stime", stime);
	}
	virtual void jread(Value &base) {
		read_key(base, "wport", wport);
		read_key(base, "stime", stime);
	}

public:
	int wport;
	int stime;
};

/** */
class CommSerSnmp : public jData
{
public:
	CommSerSnmp() {
		sport=65000;
		snmpp=161;
		agcomm = "public";
		agcont = "AMPER SISTEMAS. MADRID. SPAIN";
		agloc = "AMPER SISTEMAS";
		agname = "ULISESG5000i";
		agv2 = 1;
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "agcomm", agcomm);
		write_key(writer, "agcont", agcont);
		write_key(writer, "agloc", agloc);
		write_key(writer, "agname", agname);
		write_key(writer, "agv2", agv2);
		write_key(writer, "sport", sport);
		write_key(writer, "snmpp", snmpp);
		write_key(writer, "traps", traps);
	}
	virtual void jread(Value &base) {
		read_key(base, "agcomm", agcomm);
		read_key(base, "agcont", agcont);
		read_key(base, "agloc", agloc);
		read_key(base, "agname", agname);
		read_key(base, "agv2", agv2);
		read_key(base, "sport", sport);
		read_key(base, "snmpp", snmpp);
		read_key(base, "traps", traps);
	}

public:
	string agcomm;
	string agcont;
	string agloc;
	string agname;
	int agv2;
	int sport;
	int snmpp;
	vector<string> traps;
};

/** */
class CommSerGrab : public jData
{
public:
	CommSerGrab() {
		sport = 65001;
		grabacionEd137=0;
		rtsp_ip = rtspb_ip = "127.0.0.1";
		rtsp_port = rtspb_port = 554;
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "grabacionEd137", grabacionEd137);
		write_key(writer, "rtsp_ip", rtsp_ip);
		write_key(writer, "rtspb_ip", rtspb_ip);
		write_key(writer, "rtsp_port", rtsp_port);
		write_key(writer, "rtspb_port", rtspb_port);

		write_key(writer, "rtsp_uri", rtsp_uri);
		write_key(writer, "sport", sport);
		write_key(writer, "rtsp_rtp", rtsp_rtp);
		write_key(writer, "rtp_pl", rtp_pl);
		write_key(writer, "rtp_sr", rtp_sr);
	}
	virtual void jread(Value &base) {
		read_key(base, "grabacionEd137", grabacionEd137);
		read_key(base, "rtsp_ip", rtsp_ip);
		read_key(base, "rtspb_ip", rtspb_ip);
		read_key(base, "rtsp_port", rtsp_port);
		read_key(base, "rtspb_port", rtspb_port);

		read_key(base, "rtsp_uri", rtsp_uri);
		read_key(base, "sport", sport);
		read_key(base, "rtsp_rtp", rtsp_rtp);
		read_key(base, "rtp_pl", rtp_pl);
		read_key(base, "rtp_sr", rtp_sr);
		if (grabacionEd137 == -1)
		{
			if (rtsp_ip != "" && rtsp_port != -1)
				grabacionEd137 = 1;
			else
				grabacionEd137 = 0;
		}
		if (rtspb_port == -1)
			rtspb_port = rtsp_port;
	}

public:
	int grabacionEd137;
	string rtsp_ip;
	string rtspb_ip;
	int rtsp_port;
	int rtspb_port;

	/** No se Utilizan */
	string rtsp_uri;
	int sport;
	int rtsp_rtp;
	int rtp_pl;
	int rtp_sr;
};

/** */
class CommSerSincr : public jData
{
public:
	CommSerSincr() {
		ntp = 2;

		servidores.push_back(CommSerServidor());
		servidores.push_back(CommSerServidor());
	}
	~CommSerSincr() {
		clear_array(servidores);
	}

public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "ntp", ntp);
		write_key(writer, "servidores", servidores);
	}
	virtual void jread(Value &base) {
		read_key(base, "ntp", ntp);
		read_key(base, "servidores", servidores);
	}

public:
	int ntp;
#if __POR_REFERENCIA__
	vector<CommSerServidor> servidores;
#else
	vector<CommSerServidor *> servidores;
#endif
};

/** */
class CommSerConfig : public jData
{
public:
	CommSerConfig() {
	}
	CommSerConfig(soap_config &sc);
	~CommSerConfig() {
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)	{
		write_key(writer, "name", name);
		write_key(writer, "idSERVICIOS", idSERVICIOS);
		write_key(writer, "sip", sip);
		write_key(writer, "web", web);
		write_key(writer, "snmp", snmp);
		write_key(writer, "grab", grab);
		write_key(writer, "sincr", sincr);
	}
	virtual void jread(Value &base)	{
		read_key(base, "name", name);
		read_key(base, "idSERVICIOS", idSERVICIOS);
		read_key(base, "sip", sip);
		read_key(base, "web", web);
		read_key(base, "snmp", snmp);
		read_key(base, "grab", grab);
		read_key(base, "sincr", sincr);
	}

public:
	int	idSERVICIOS;
	string name;
	CommSerSip sip;
	CommSerWeb web;
	CommSerSnmp snmp;
	CommSerGrab grab;
	CommSerSincr sincr;
};

#endif
