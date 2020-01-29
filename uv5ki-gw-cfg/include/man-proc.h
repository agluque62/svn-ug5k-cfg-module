#ifndef __MAN_PROC_H__
#define __MAN_PROC_H__
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream#include <utility>
#include <queue>
#include <algorithm>

#define _NUM_HW_ITFS_			16
#define P_MAN_PROC				(ManProc::p_man)


#include "./base/code-base.h"
#include "./base/thread.h"
#include "./base/socket.h"
#include "./base/sistema.h"
#include "./websrv/uv5kigwcfg-web-app-data.h"
#include "./tools/tools.h"
#include "./config/local-config.h"
#include "./config/comm-config.h"


using namespace std;
///** */
//extern char *acStrVersion();

/** */
enum ePRIdentity {epriError, epriCpu0, epriCpu1, epriNone }; 

/** */
class jInterface : public jData
{
public:
	jInterface(){
		id="";
		tp = st = md = bc = -1;
	}
	~jInterface(){}

public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "id", id);
		write_key(writer, "tp", tp);
		write_key(writer, "st", st);
		write_key(writer, "md", md);
		write_key(writer, "bc", bc);
	}
	virtual void jread(Value &base){}

public:
	string id;
	int tp;
	int st;
	int md;
	int bc;
};

/** */
class jCpu : public jData
{
public:
	jCpu() {
		ip = "";
		stdg = stdf = stl1 = stl2 = st_cpu = st_col = stdn = sync = -1;
		for (int i=0; i<_NUM_HW_ITFS_; i++)
		{
#if __POR_REFERENCIA__
			itfs.push_back(jInterface());
#else
			itfs.push_back(new jInterface());
#endif
		}
	}
	~jCpu() {
		clear_array(itfs);
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "ip", ip);
		write_key(writer, "stdg", stdg);
		write_key(writer, "stdf", stdf);
		write_key(writer, "stl1", stl1);
		write_key(writer, "stl2", stl2);
		write_key(writer, "st_cpu", st_cpu);
		write_key(writer, "st_col", st_col);
		write_key(writer, "stdn", stdn);
		write_key(writer, "sync", sync);
		write_key(writer, "itfs", itfs);
	}
	virtual void jread(Value &base){}

public:
	int stdg;
	int stdf;
	int stl1;
	int stl2;
	int st_cpu;
	int st_col;
	int stdn;
	int sync;
	string ip;
#if __POR_REFERENCIA__
	vector<jInterface> itfs;
#else
	vector<jInterface *> itfs;
#endif
};

/** */
class jEstado : public jData
{
public:
	jEstado() {
		
		serv = VersionConfiguracion();
		stdg = cpua = stdf = stdn = -1;

		//snmp = Versiones::snmpVersion;
		//record = Versiones::recVersion;
		//for (int i=0; i<_NUM_HW_ITFS_; i++)
		//	itf.push_back(new jInterface());
	}
	~jEstado() {
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "serv", serv);
		write_key(writer, "snmp", snmp);
		write_key(writer, "record", record);

		write_key(writer, "stdg", stdg);
		write_key(writer, "cpua", cpua);
		write_key(writer, "stdf", stdf);
		write_key(writer, "stdn", stdn);

		write_key(writer, "cpu0", cpu0);
		write_key(writer, "cpu1", cpu1);

		write_key(writer, "itf", itf);
	}
	virtual void jread(Value &base){}

public:
	string	serv;
	string	snmp;
	string	record;
	int		stdg;
	int		cpua;
	int		stdf;
	int		stdn;
	jCpu	cpu0;
	jCpu	cpu1;
#if __POR_REFERENCIA__
	vector<jInterface> itf;
#else
	vector<jInterface *> itf;
#endif
};

/** */
class jDataBite : public jData
{
public:
	jDataBite() {
		res = "";		
	}
	~jDataBite() {
		clear_array(msg);
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "res", res);
		write_key(writer, "msg", msg);
	}
	virtual void jread(Value &base){}
	void setMsg(string msg);

public:
	string res;						// 0: NoInicializado, 1: OK, 2: Error
#if __POR_REFERENCIA__
	vector<webData_line> msg;
#else
	vector<webData_line *> msg;
#endif
};

/** */
class ManProc :
	public CThread
{
public:
	static ManProc *p_man;
	jEstado estado;
	jDataBite bite;

public:
	ManProc(void);
	~ManProc(void);

public:
	void Run();
	void Dispose();

public:
	string jestado();
	string jbite();
	void setEstadoCpu(ePRIdentity quien, jCpu &cpu, int len, void *data, bool inicializando=false);
	void setBite(int len, void *data);
	void resetCpu(ePRIdentity quien, jCpu &cpu);
	void NotificaCambioConfig() {/*_autpr.SetEvent(epriNone, epreInicio);*/}

public:
	static void st_estado_cpu0(bool res, int len, void *data);
	static void st_estado_cpu1(bool res, int len, void *data);
	void estado_cpu(int cpu1cpu2, int len, void *data, bool Reset=false) {
		if (Reset==true)
			resetCpu(cpu1cpu2==0 ? epriCpu0 : epriCpu1, cpu1cpu2==0 ? estado.cpu0 : estado.cpu1);
		else
			setEstadoCpu(cpu1cpu2==0 ? epriCpu0 : epriCpu1, cpu1cpu2==0 ? estado.cpu0 : estado.cpu1, len, data);
	}
	bool IsCollateralInStandby();
protected:
	void actualizaPrincipal();
	void SetEstadoPRCpu(ePRIdentity quien, /*ePRIdentity que, */int nestado);
	void NotificaEstadosCpu();
	void GetEstadoCpu(int cpu);

protected:
	CCritSec m_lock;
	int _cnt;
	//string snmpVersion;
	//string recVersion;
	int snmpServicePort;
};

#endif
