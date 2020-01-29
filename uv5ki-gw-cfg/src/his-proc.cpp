#include "../include/his-proc.h"

/** */
HistClient *HistClient::p_hist;

/** */
HistClient::HistClient(void)
{
}

/** */
HistClient::~HistClient(void)
{
}

/** */
void HistClient::Run()
{
	CUDPSocket sck;
	LocalConfig snmpconfig(onfs(LocalConfig::p_cfg->get(strModulos, strItemModuloSnmp)/*.snmpModule()*/));
	char buffer[BUFSIZ+1];
	stHistAviso aviso;
	time_t tick =LocalConfig::p_cfg->getint(strRuntime, strRuntimeItemThreadActiveTick, "60");

	SetId("HistClient");
	PLOG_INFO("HistClient (%d) running...", pid());

	default_dst = CIPAddress("127.0.0.1", atoi(snmpconfig.get("SERVICIO","UDP_PORT_IN_AGSNMP","65000").c_str()));
	default_port = atoi(snmpconfig.get("SERVICIO","UDP_PORT_IN_AGSNMP","65000").c_str());
	PrepareSocket(sck, atoi(snmpconfig.get("SERVICIO","UDP_PORT_IN_CONFIG","65002").c_str()));

	while (IsRunning())
	{
		/** 20171102. Amplio de 10 a 100 para poder asegurar que se eliminan posibles respuestas a comandos 'anteriores' */
		this->sleep(100);
		if (Get(aviso))
		{
			CIPAddress from;
			try
			{
				PLOG_DEBUG("TO-SNMP-SERVICE (%s:%d): %s", aviso.dst.GetHostName().c_str(), aviso.dst.GetPort(), aviso.datos.c_str());
				/** 20171102. Limpio el buffer de lectura para eliminar posibles respuestas a comandos 'anteriores' */
#ifndef _WIN32
				sck.Recv(buffer, BUFSIZ-1, MSG_DONTWAIT);
#endif
				if (sck.SendTo(aviso.datos.c_str(), aviso.datos.size(), aviso.dst) == (int )aviso.datos.size())
				{
					if (aviso.respuesta != (callback )NULL)
					{	
						int leidos = sck.RecvFrom(buffer, BUFSIZ, &from);
						if (leidos > 0)
						{
							buffer[leidos<BUFSIZ ? leidos : BUFSIZ]=0;
							aviso.respuesta(true, leidos, buffer);
						}
						else
						{
							aviso.respuesta(false, 0, NULL);
						}
					}				
				}
				else
				{
					PLOG_ERROR("HistClient::Run. Sendto Error.");
				}
			}
			catch(socket_error e)
			{
				PLOG_EXCEP(e, "Local=%d, Remoto:%d", sck.GetLocalAddress().GetPort(), aviso.dst.GetPort());
				/** Si se esta esperando una respuesta, la genero con error */
				if (aviso.respuesta != (callback )NULL)
				{
					aviso.respuesta(false, 0, NULL);
				}
			}
			catch (...) 
			{
				PLOG_ERROR("Excepcion en HistClient::Run (Generica)");
			}
		}
		if (Tick.elapsed(tick)==true) {		
			PLOG_INFO("(%d)=>HistClient. TICK", pid());
		}
	}
	PLOG_INFO("HistClient leaving...");
}

/** Rutinas Publicas... */
/** */
void HistClient::SetEvent(int evento, string user, string rec, string p1, string val, string p2, string p3)
{
	/** Lock */
	CCSLock lock(_acceso);

	char btemp[16];

	std::replace( user.begin(), user.end(), ':', '@');	
	std::replace( rec.begin(),  rec.end(), ':', '@');

	/** FORMATO: C,H01,xxx,USER:REC:PAR1=VALOR:PAR2:PAR3 */
#ifdef _WIN32
	string strAviso="C,H01," + string(_itoa(evento,btemp,10))+","+user+(rec=="" ? "" : ":"+rec);
#else
	sprintf(btemp,"%d", evento);
	string strAviso="C,H01," + string(btemp)+","+user+(rec=="" ? "" : ":"+rec);
#endif

	if (p1!="")
	{
		std::replace( p1.begin(), p1.end(), ':', '@');
		strAviso = strAviso + ":" + p1 + "=" + val;
		if (p2!="")
		{
			std::replace( p2.begin(), p2.end(), ':', '@');
			strAviso = strAviso + ":" + p2;
			if (p3!="")
			{
				std::replace( p3.begin(), p3.end(), ':', '@');
				strAviso = strAviso + ":" + p3;
			}
		}
	}
	stHistAviso aviso;
	aviso.datos = string(strAviso);
	aviso.respuesta = NULL;
	aviso.dst = default_dst;

	avisos.set(aviso);
}

/** */
void HistClient::SetBite(callback respuesta)
{
	/** Lock */
	CCSLock lock(_acceso);

	stHistAviso aviso;
	aviso.datos = string("C,T00");
	aviso.respuesta = respuesta;
	aviso.dst = default_dst;
	avisos.set(aviso);
}

/** */
void HistClient::SetReset()
{
	/** Lock */
	CCSLock lock(_acceso);

	stHistAviso aviso;
	aviso.datos = string("C,T02");
	aviso.respuesta = NULL;
	aviso.dst = default_dst;
	avisos.set(aviso);
}

/** */
void HistClient::SetSincrState(string snmpStatus)
{
	/** Lock */
	CCSLock lock(_acceso);

	// string snmpStatus = std==slcSincronizado ? "1" : std==slcConflicto ? "2" : "0";

	stHistAviso aviso;
	aviso.datos = string("C,C00,"+snmpStatus);
	aviso.respuesta = NULL;
	aviso.dst = default_dst;
	avisos.set(aviso);
}

/** */
void HistClient::GetEstado(CIPAddress from, callback respuesta)
{
	/** Lock */
	CCSLock lock(_acceso);

	stHistAviso aviso;
	aviso.datos = string("C,C03");
	aviso.respuesta = respuesta;
	aviso.dst = from;
	avisos.set(aviso);
}

/** */
void HistClient::Signal(int toPort, callback rsp)
{
	/** Lock */
	CCSLock lock(_acceso);

	stHistAviso aviso;
	
	aviso.datos = string("C,H02");
	aviso.respuesta = rsp;
	aviso.dst = CIPAddress("127.0.0.1", toPort);
	avisos.set(aviso);
}

/** */
void HistClient::SetEstadoCpu(string local, string remoto)
{
	/** Lock */
	CCSLock lock(_acceso);

	stHistAviso aviso;
	
	aviso.datos = string("V,N04,") + local +  string(",") + remoto;

	aviso.respuesta = NULL;
	aviso.dst = default_dst;
	avisos.set(aviso);
}

/** */
void HistClient::SetEventosHistoricos(string user, EventosHistoricos *eventos)
{	
	EventosHistoricos::iterator it;
	for (it=eventos->begin(); it!=eventos->end(); it++)
	{
		if (it->evento != INCI_NONE)
			SetEvent(it->evento, user, it->res, it->name, it->data);
	}
}


/** Rutinas Protected.... **/
/** */
void HistClient::Dispose()
{
	Stop();
}

/** */
void HistClient::PrepareSocket(CUDPSocket &sck, int port)
{
	try
	{
#ifdef _WIN32
		port = 65003;
#endif
		sck.Bind(port);
		sck.SetRecvTimeout(5);
	}
	catch(socket_error e)
	{
		PLOG_ERROR("Excepcion en HistClient::PrepareSocket (port=%d): %s", port, e.what().c_str());
	}
}

/** */
bool HistClient::Get(stHistAviso &aviso)
{
	/** Lock */
	CCSLock lock(_acceso);
	return avisos.get(aviso);
}
