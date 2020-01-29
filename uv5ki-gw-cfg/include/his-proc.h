#ifndef __HIS_PROC_H__
#define __HIS_PROC_H__
#include <utility>

#include "./base/code-base.h"
#include "./base/thread.h"
#include "./base/socket.h"
#include "./tools/tools.h"
#include "./config/local-config.h"

//#include "jgw-config.h"
//#include "http-client.h"

using namespace std;

#define P_HIS_PROC								(HistClient::p_hist)

//#define _EVENTONPAIR_

/** Codigos de Eventos */
#define INCI_MPGP		150		// Modificación de Parámetros Generales de Pasarela.			/ USER:GW:PAR=VALOR /
#define INCI_MRATS		151		// Modificación Rutas ATS.										*/
#define INCI_MPHW		152		// Modificación de Parámetros de Recurso Hardware.				/ USER:RECURSO:PAR=VALOR /
#define INCI_MPSW		153		// Modificación de Parámetros Lógico de Recurso Hardware.		/ USER:RECURSO:PAR=VALOR /
#define INCI_GCFG		154		// Generación de Configuración por Defecto.						/ USER:CFG-NAME /
#define INCI_ACFG		155		// Activación de Configuración por Defecto.						/ USER:CFG-NAME /
#define INCI_DCFG		156		// Borrado de Configuración por Defecto.						/ USER:CFG-NAME /
#define INCI_ARCR		157		// Alta Recurso Radio											/ USER:RECURSO /
#define INCI_BRCR		158		// Baja Recurso Radio											/ USER:RECURSO /
#define INCI_ARCT		159		// Alta Recurso Telefonia										/ USER:RECURSO /
#define INCI_BRCT		160		// Baja Recuso Telefonia										/ USER:RECURSO /
#define INCI_CONFLICTO	161		// Conflicto de configuraciones.								/ IDGW:GW=IP /

#define INCI_CGSW		180		// Carga de Versión Software.									*/ USER:VER=VERSION /
#define INCI_ATSW		181		// Activación de Versión Software.								*/ USER:VER=VERSION /
#define INCI_RESET		182		// RESET Remoto.												/ USER /
#define INCI_BCLP		183		// Ejecución de Bucle de Prueba.								*/ USER:BLUCE=IDBUCLE:COL1:COL2:RESULTADO /
#define INCI_BITE		184		// Ejecucion BITE desde Local.									/ USER /
#define INCI_SELPR		2100	// SP-Radio	Selección Principal/Reserva							*/ USER:REC=RECURSO /

#define INCI_NONE		-1

/** */
class CommConvertEvent 
{
public:
	CommConvertEvent(int Evento, string Res="", string Name="", string Data="")
	{
		evento = Evento;
		res = Res;
		name = Name;
		data = Data;
	}
public:
	int evento;
	string res;
	string name;
	string data;
};
/** */
typedef vector<CommConvertEvent> EventosHistoricos;

/** */
class HistClientException
{
public:
	HistClientException(string msg)		
	{
		_msg="HistClientException: " + msg;
	}
	HistClientException(socket_error error)
	{
		_msg = "Socket Error: " + string(error.what());
	}

private:
	string _msg;
public:
	string what(){return _msg;}

};

//
typedef pair<string, bool>	stHistAviso1;
//
typedef void(*callback)(bool result, int len, void *datos);
#ifdef _EVENTONPAIR_
	typedef pair<string, callback> stHistAviso;
#else
	typedef struct {
		CIPAddress dst;
		string	   datos;
		callback   respuesta;
	} stHistAviso;
#endif

/** */
class HistClient : public CThread
{
public:
	static HistClient *p_hist;
	
public:
	HistClient(void);
	~HistClient(void);

public:
	void Run();
	void Dispose();

protected:
	void PrepareSocket(CUDPSocket &sck, int port);
	bool Get(stHistAviso &aviso);

public:
	void SetEvent(int evento, string user, string rec, string p1="", string val="", string p2="", string p3="");
	void SetReset();
	void SetBite(callback respuesta);
	void GetEstado(CIPAddress from, callback respuesta);
	void SetSincrState(string snmpStatus);

	void Signal(int toPort = 65000, callback rsp=NULL);
	void SetEstadoCpu(string local, string remoto);

	void SetEventosHistoricos(string user, EventosHistoricos *eventos);

protected:
	ColaAventos<stHistAviso> avisos;
	CIPAddress default_dst;
	int default_port;
	CCritSec   _acceso;
};

#endif

