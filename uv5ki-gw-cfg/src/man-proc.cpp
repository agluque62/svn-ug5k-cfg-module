#include "../include/man-proc.h"
#include "../include/his-proc.h"
#include "../include/cfg-proc.h"

#define _CFG_PROC_				1
#define _NOTIFICA_ESTADOS_CPU_	0

/** */
ManProc *ManProc::p_man=NULL;

/** */
void ManProc::st_estado_cpu0(bool res, int len, void *data)
{
	static int _cnt = 0;
	if (res==true && (_cnt++%10)==0)
		PLOG_DEBUG("RECIBIDO ESTADO_CPU0: %s", data);
	ManProc::p_man->estado_cpu(0, len, data, res!=true);
}

/** */
void ManProc::st_estado_cpu1(bool res, int len, void *data)
{
	static int _cnt = 0;
	if (res==true && (_cnt++%10)==0)
		PLOG_DEBUG("RECIBIDO ESTADO_CPU1: %s", data);
	ManProc::p_man->estado_cpu(1, len, data, res!=true);
}

/** */
ManProc::ManProc(void)
{
	LocalConfig snmpconfig(onfs(LocalConfig::p_cfg->get(strModulos, strItemModuloSnmp)/*.snmpModule()*/));
	snmpServicePort = atoi(snmpconfig.get("SERVICIO","UDP_PORT_IN_AGSNMP","65000").c_str());
}

/** */
ManProc::~ManProc(void)
{
}

/** */
void ManProc::Run()
{
	time_t tick = LocalConfig::p_cfg->getint(strRuntime, strRuntimeItemThreadActiveTick, "60");
#if _NOTIFICA_ESTADOS_CPU_
	int _cntEstados = 1;
#endif
	SetId("ManProc");
	PLOG_INFO("ManProc (%d) running...", pid());


	/** Inicializando las tablas */
	ManProc::p_man->resetCpu(epriCpu0, estado.cpu0);
	ManProc::p_man->resetCpu(epriCpu1, estado.cpu1);
	estado.itf = estado.cpu0.itfs;

	///** */
	//snmpVersion = sistema::SnmpAgentVersion();
	//recVersion = sistema::RecordServiceVersion();

	_cnt = 10;
	while (IsRunning())
	{
		sleep(1000);
#ifdef _WIN32
		if (LocalConfig::p_cfg->get(strWindowsTest, strItemWindowsTestSnmpStd, "0")=="1"/*.winStdSnmp()*/)
#endif
		{
			CCSLock _lock(m_lock);
			if ((--_cnt) <= 0)
			{
				GetEstadoCpu(0);
				if (P_WORKING_CONFIG->DualCpu() == true)
					GetEstadoCpu(1);

				/** Polling Normal cada minuto */
				_cnt = 60;
			}

#if _NOTIFICA_ESTADOS_CPU_
			if ((--_cntEstados) <= 0)
			{
				NotificaEstadosCpu();
				_cntEstados = atoi(LocalConfig::p_cfg->get("MODULOS", "ACTUALIZAESTADOSEN", "15").c_str());
			}
#endif
		}
		if (Tick.elapsed(tick)==true) {		
			PLOG_INFO("(%d)=>ManProc. TICK", pid());
		}
	}
	PLOG_INFO("ManProc leaving...");
}

/** */
void ManProc::Dispose()
{
	Stop();
}

/** */
string ManProc::jestado()
{
	CCSLock _lock(m_lock);

	/** Esta activa la pagina acelero el Polling */
	_cnt = _cnt >= 2 ? 2 : _cnt;

	/** Actualizo las Versiones */
	estado.serv = VersionConfiguracion();		// string(acStrVersion());
	estado.snmp = VersionSnmp();
	estado.record = VersionGrabador();

	return estado.JSerialize();
}

/** */
void ManProc::resetCpu(ePRIdentity quien, jCpu &cpu)
{
	setEstadoCpu(quien, cpu, 174,
		(void *)",R03,0;0;0;0;0;0;0;0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0;--:10:0:0:0",
		true);
}

/** */
void ManProc::setEstadoCpu(ePRIdentity quien, jCpu &cpu, int len, void *data, bool inicializando)
{
	CCSLock _lock(m_lock);

	vector<std::string> nivel1;
	string rsp = string((char *)data);
	Tools::split(nivel1,rsp,',');
	if (nivel1.size()==3 && nivel1[1]=="R03")
	{
		vector<std::string> nivel2;
		Tools::split(nivel2, nivel1[2],';');
		if (nivel2.size()==25)							
		{
			cpu.stdg = atoi(nivel2[0].c_str());
			cpu.stdf = atoi(nivel2[1].c_str());
			cpu.stl1 = atoi(nivel2[2].c_str());
			cpu.stl2 = atoi(nivel2[3].c_str());
			// cpu.st_cpu= atoi(nivel2[4].c_str());
			SetEstadoPRCpu(quien, /*epriCpu0, */atoi(nivel2[4].c_str()));
			cpu.st_col = atoi(nivel2[5].c_str());
			// SetEstadoPRCpu(quien, epriCpu1, atoi(nivel2[5].c_str()));
			cpu.stdn = atoi(nivel2[6].c_str());
			cpu.sync = atoi(nivel2[7].c_str());
			for (int ift=0; ift<16; ift++)
			{
				vector<string> nivel3;
				Tools::split(nivel3, nivel2[8+ift],':');
				if (nivel3.size()==5)
				{
#if __POR_REFERENCIA__
					cpu.itfs[ift].id = nivel3[0];
					cpu.itfs[ift].tp = atoi(nivel3[1].c_str());
					cpu.itfs[ift].st = atoi(nivel3[2].c_str());
					cpu.itfs[ift].md = atoi(nivel3[3].c_str());
					cpu.itfs[ift].bc = atoi(nivel3[4].c_str());
#else
					cpu.itfs[ift]->id = nivel3[0];
					cpu.itfs[ift]->tp = atoi(nivel3[1].c_str());
					cpu.itfs[ift]->st = atoi(nivel3[2].c_str());
					cpu.itfs[ift]->md = atoi(nivel3[3].c_str());
					cpu.itfs[ift]->bc = atoi(nivel3[4].c_str());
#endif
				}				
				else
				{
					PLOG_ERROR("ManProc::setEstadoCpu. ERROR1 FORMATO: %s", rsp.c_str());
				}
			}
		}
		else
		{
			PLOG_ERROR("ManProc::setEstadoCpu. ERROR2 FORMATO: %s", rsp.c_str());
		}
	}
	else
	{
		PLOG_ERROR("ManProc::setEstadoCpu. ERROR3 FORMATO: %s", rsp.c_str());
	}
	if (!inicializando)
		actualizaPrincipal();
}

/** */
void ManProc::actualizaPrincipal()
{
	if (estado.cpu0.st_cpu==1 && estado.cpu1.st_cpu != 1)		// La CPU0 dice que es la principal..
	{
		estado.stdg = estado.cpu0.stdg;
		estado.cpua = 1;
		estado.stdf = estado.cpu0.stdf;
		estado.stdn = estado.cpu0.stdn;
		estado.itf = estado.cpu0.itfs;
	}
	else if (estado.cpu0.st_cpu !=1 && estado.cpu1.st_cpu == 1)	// La CPU1 dice que es la principal..
	{
		estado.stdg = estado.cpu1.stdg;
		estado.cpua = 2;
		estado.stdf = estado.cpu1.stdf;
		estado.stdn = estado.cpu1.stdn;
		estado.itf = estado.cpu1.itfs;
	}
	else
	{															// Incongruencia.....
		estado.stdg = 4;
		estado.cpua = 4;
		estado.stdf = 4;
		estado.stdn = 4;
		estado.itf = estado.cpu0.itfs;
	}
}

/** */
void ManProc::SetEstadoPRCpu(ePRIdentity quien, /*ePRIdentity que, */int nestado)
{
	jCpu *pCpu = quien==epriCpu0 ? &estado.cpu0 : &estado.cpu1;
	int *pval = &pCpu->st_cpu; // que==epriCpu0 ? &pCpu->stcp0 : &pCpu->stcp1;
	if (/*quien==que && */nestado != *pval)
	{
		//_autpr.SetEvent(quien, nestado==0 ? epreOff : nestado==1 ? eprePrincipal : epreReserva);
	}
	*pval = nestado;
}

/** */
void ManProc::setBite(int len, void *data)
{
	CCSLock _lock(m_lock);

	((char *)data)[len]=0;

	vector<std::string> nivel1;
	string rsp = string((char *)data);
	Tools::split(nivel1,rsp,',');

	/** 20160830. Han cambiado el formato de la respuesta. No viene el campo resultado... */
	if (nivel1.size()==4 && nivel1[1]=="R00")		// Formato Antiguo...
	{
		/** Estado del Equipo... */
		bite.res = nivel1[2]=="0" ? "No Inicializado" :
				   nivel1[2]=="1" ? "Ok" :
				   nivel1[2]=="2" ? "Error" : "Error Interno";
		
		/** Mensaje.. */
		//bite.msg = nivel1[3];
		bite.setMsg(nivel1[3]);
	}
	else if (nivel1.size()==3 && nivel1[1]=="R00")	// Formato Nuevo.
	{
		bite.res = "Ok";
		/** Mensaje.. */
		//bite.msg = nivel1[2];
		bite.setMsg(nivel1[2]);
	}
	else
	{
		bite.res = "Error Interno";
		//bite.msg = "Error en Formato de Trama";
		bite.setMsg("Error en Formato de Trama");
		PLOG_ERROR("ManProc::setBite. Error Formato => %s",rsp.c_str());
	}
}

/** 
	{res: "resultado", msg:[{line: "line1"}, {line:"linea2"}, ... ]}
*/
string ManProc::jbite()
{
	CCSLock _lock(m_lock);

	return bite.JSerialize();
}

/** */
void ManProc::GetEstadoCpu(int cpu)
{
	try 
	{
		//LocalConfig snmpconfig(onfs(LocalConfig::p_cfg->get(strModulos, strItemModuloSnmp)/*.snmpModule()*/));
		if (cpu==0)
		{
#if _CFG_PROC_
			if (P_WORKING_CONFIG->DualCpu() == true)
				estado.cpu0.ip = P_WORKING_CONFIG->ipcpu(0);
			else
			{
				estado.cpu0.ip="127.0.0.1";
			}
#else
			estado.cpu0.ip = "192.168.0.71";
#endif
			if (Tools::ip_format_test(estado.cpu0.ip))
			{
				CIPAddress to(estado.cpu0.ip, snmpServicePort/* atoi(snmpconfig.get("SERVICIO","UDP_PORT_IN_AGSNMP","65000").c_str())*/);
				HistClient::p_hist->GetEstado(to, st_estado_cpu0);
			}
			else
				PLOG_ERROR("ManProc::GetEstadoCpu %d. IP NO Valida: %s", cpu, estado.cpu0.ip.c_str());
		}
		else if (cpu==1)
		{
#if _CFG_PROC_
			estado.cpu1.ip = P_WORKING_CONFIG->ipcpu(1);
#else
			estado.cpu1.ip = "192.168.0.72";
#endif
			if (Tools::ip_format_test(estado.cpu1.ip))
			{
				CIPAddress to(estado.cpu1.ip, snmpServicePort/*atoi(snmpconfig.get("SERVICIO","UDP_PORT_IN_AGSNMP","65000").c_str())*/);
				HistClient::p_hist->GetEstado(to ,st_estado_cpu1);
			}
			else
				PLOG_ERROR("ManProc::GetEstadoCpu %d. IP NO Valida: %s", cpu, estado.cpu1.ip.c_str());
		}
	}
	catch(socket_error e) 
	{
		PLOG_ERROR("ManProc::GetEstadoCpu %d. socket_error: %s.", cpu, e.what().c_str());
	}
	catch(...) 
	{
		PLOG_ERROR("ManProc::GetEstadoCpu %d. Excepcion no controlada", cpu);
	}
}

#if _NOTIFICA_ESTADOS_CPU_
/** */
void ManProc::NotificaEstadosCpu()
{
	try 
	{
#if _CFG_PROC_
		int quiensoy =  P_WORKING_CONFIG->cpu1cpu2();
#else
		int quiensoy = 0;
#endif
		bool soymain = sistema::MainOrStandby();
		jCpu *pOtraCpu =  quiensoy==0 ? &estado.cpu1 : quiensoy==1 ? &estado.cpu0 : NULL;
		if (pOtraCpu != NULL)
		{
			string stdlocal = soymain==true ? "1" : "2";
			string stdcol = pOtraCpu->stdg == 0 ? "0" : pOtraCpu->stdg==1 ? "1" : "2";
			HistClient::p_hist->SetEstadoCpu(stdlocal, stdcol);
			return;
		}
		PLOG_ERROR("ManProc::NotificaEstadosCpu. Error No puedo decodificar si soy Cpu0 o Cpu1");
	}
	catch(socket_error e) 
	{
		PLOG_ERROR("ManProc::NotificaEstadosCpu. socket_error: %s.", e.what());
	}
	catch(...) 
	{
		PLOG_ERROR("ManProc::NotificaEstadosCpu. Excepcion No controlada...");
	}        
}

#endif

bool ManProc::IsCollateralInStandby() 
{
	if (P_WORKING_CONFIG->DualCpu() == true) {
		string ipCol = sistema::ipColateral();
		if (ipCol == estado.cpu0.ip) {
			GetEstadoCpu(0);
			this->sleep(200);
			return estado.cpu0.st_cpu == 2 ? true : false;
		}
		else if (ipCol == estado.cpu1.ip) {
			GetEstadoCpu(1);
			this->sleep(200);
			return estado.cpu1.st_cpu == 2 ? true : false;
		}
	}
	return false;
}


///////////////////////////////////////////////////////////////////
//
void jDataBite::setMsg(string msgIn) 
{
	istringstream instr(msgIn);
	string line;

	clear_array(msg);
	while (std::getline(instr, line))
	{
		if (line=="") continue;

		line.erase(std::remove_if(line.begin(), line.end(), &Tools::IsNoAscii), line.end());
#if __POR_REFERENCIA__
		msg.push_back(webData_line(line));
#else
		msg.push_back(new webData_line(line));
#endif
	}
}
