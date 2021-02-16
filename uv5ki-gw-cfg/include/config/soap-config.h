#ifndef __SOAP_CONFIG__
#define __SOAP_CONFIG__

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

#include "../../include/rapidxml-1.13/rapidxml.hpp"

using namespace rapidxml;

#define RAPID_XML_NODE_VALUE(pnode, tag)	(pnode!=NULL ? (pnode->first_node(tag)!=NULL ? pnode->first_node(tag)->value() : tag) : tag)

typedef string (*remoteGetXdataFunc)(string proc, string p1, string p2, string p3);
enum TI_ {
	TI_None=-1,
	TI_Radio=0, TI_LCEN=1, TI_BC=2, TI_BL=3, TI_AB=4, TI_ATS_R2=5, TI_ATS_N5=6, TI_ATS_QSIG=7, TI_ISDN_2BD=8, TI_ISDN_30BD=9, TI_I_O=10, TI_DATOS=11,
	TI_EM_PP=13, TI_EM_MARC=14
};

enum THOST_ {
	THOST_None=-1,
	THOST_TOP = 0, THOST_TIFX = 1, THOST_EXTRADIO = 2, THOST_EXTTELEFONIA = 3, THOST_EXTSISTEMA = 4
};

/** */
class xml_data : public CodeBase
{
public:
	xml_data(){};
	virtual ~xml_data(){};
public:
	void XDeserialize(string xml_string, string rootNodeStr)
	{
		static xml_document<> doc;
		xml_node<> * root_node;

		doc.parse<0>((char *)xml_string.c_str());
		root_node = doc.first_node((char *)rootNodeStr.c_str());
		xread(root_node);
	}
	virtual void xread(xml_node<> * xnode)=0;

protected:
	void read_key(xml_node<> * xnode, const char *indice, bool &val) {
		string sval = string(RAPID_XML_NODE_VALUE(xnode, indice));
		val = sval == "false" ? false : true;
	}
	void read_key(xml_node<> * xnode, const char *indice, int &val, int vdef=-1) {
		string sval = RAPID_XML_NODE_VALUE(xnode, indice);
		val = sval == indice ? vdef : atoi(sval.c_str());
		// val = atoi(RAPID_XML_NODE_VALUE(xnode, indice));
	}
	void read_key(xml_node<> * xnode, const char *indice, TI_ &val) {
		string str_val = RAPID_XML_NODE_VALUE(xnode, indice);
		val = (TI_)(
			str_val=="TI_Radio" ? TI_Radio :
			str_val=="TI_LCEN" ? TI_LCEN :
			str_val=="TI_BC" ? TI_BC :
			str_val=="TI_BL" ? TI_BL :
			str_val=="TI_AB" ? TI_AB :
			str_val=="TI_ATS_R2" ? TI_ATS_R2 :
			str_val=="TI_ATS_N5" ? TI_ATS_N5 :
			str_val=="TI_EM_PP" ? TI_EM_PP:
			str_val=="TI_EM_MARC" ? TI_EM_MARC : TI_None);
	}
	void read_key(xml_node<> * xnode, const char *indice, THOST_ &val) {
		string str_val = RAPID_XML_NODE_VALUE(xnode, indice);
		val = (THOST_)(
			str_val=="TEH_TOP" ? THOST_TOP :
			str_val=="TEH_TIFX" ? THOST_TIFX :
			str_val=="TEH_EXTERNO_RADIO" ? THOST_EXTRADIO :
			str_val=="TEH_EXTERNO_TELEFONIA" ? THOST_EXTTELEFONIA :
			str_val=="TEH_EXTERNO_SISTEMA" ? THOST_EXTSISTEMA : THOST_None);
	}
	void read_key(xml_node<> * xnode, const char *indice, string &val) {
		val = RAPID_XML_NODE_VALUE(xnode, indice);
	}
	template <typename T> void read_key(string data, const char *indice, T &val) {
		val.XDeserialize(data, string(indice));
	}
	void read_key(xml_node<> * xnode, const char *root, vector<string> &val) {
		xml_node<> *root_node = xnode->first_node((char *)root);
		val.clear();
		if (root_node==NULL)
			return;
		for (xml_node<> * res_node = root_node->first_node("string"); res_node; res_node = res_node->next_sibling())
		{
			val.push_back(res_node->value());
		}
	}
	void read_key(xml_node<> * xnode, const char *root, vector<int> &val) {
		xml_node<> *root_node = xnode->first_node((char *)root);
		val.clear();
		if (root_node==NULL)
			return;
		for (xml_node<> * res_node = root_node->first_node("int"); res_node; res_node = res_node->next_sibling())
		{			
			val.push_back(atoi(res_node->value()));
		}
	}
	template <typename T> void read_key(xml_node<> * xnode, const char *root, const char *name, vector<T> &val) {
		xml_node<> *root_node = xnode->first_node((char *)root);
		val.clear();
		if (root_node==NULL)
			return;
		for (xml_node<> * res_node = root_node->first_node(name); res_node; res_node = res_node->next_sibling())
		{
			T obj;
			obj.xread(res_node);
			val.push_back(obj);
		}
	}
	template <typename T> void read_key(xml_document<> &doc, const char *root, const char *name, vector<T> &val) {
		xml_node<> * root_node;		
		
		root_node = doc.first_node((char *)root);
		val.clear();
		if (root_node==NULL)
			return;
		for (xml_node<> * res_node = root_node->first_node(name); res_node; res_node = res_node->next_sibling())
		{
			T obj;
			obj.xread(res_node);
			val.push_back(obj);
		}
	}

protected:
	static remoteGetXdataFunc remote_get_xdata_func;
};

/** */
class soap_IntEntry : public xml_data
{
public:
	soap_IntEntry() {
		valor = 0;
	}
	~soap_IntEntry(){}
public:
	void xread(xml_node<> *xnode) {
		read_key(xnode, "int", valor);
	}
public:
	int valor;
};

/** */
class soap_ParametrosGeneralesSistema : public xml_data
{
public:
	soap_ParametrosGeneralesSistema() 
	{
		TiempoMaximoPTT = 0;
		TiempoSinJack1 = 0;
		TiempoSinJack2 = 0;
		TamLiteralEnlExt = 0;
		TamLiteralEnlDA = 0;
		TamLiteralEnlIA = 0;
		TamLiteralEnlAG = 0;
		TamLiteralEmplazamiento = 0;
	}
	~soap_ParametrosGeneralesSistema() {}

public:
	void xread(xml_node<> * xnode) {
		read_key(xnode, "TiempoMaximoPTT", TiempoMaximoPTT);
		read_key(xnode, "TiempoSinJack1", TiempoSinJack1);
		read_key(xnode, "TiempoSinJack2", TiempoSinJack2);
		read_key(xnode, "TamLiteralEnlExt", TamLiteralEnlExt);
		read_key(xnode, "TamLiteralEnlDA", TamLiteralEnlDA);
		read_key(xnode, "TamLiteralEnlIA", TamLiteralEnlIA);
		read_key(xnode, "TamLiteralEnlAG", TamLiteralEnlAG);
		read_key(xnode, "TamLiteralEmplazamiento", TamLiteralEmplazamiento);
	}
public:
	int TiempoMaximoPTT;
	int TiempoSinJack1;
	int TiempoSinJack2;
	int TamLiteralEnlExt;
	int TamLiteralEnlDA;
	int TamLiteralEnlIA;
	int TamLiteralEnlAG;
	int TamLiteralEmplazamiento;

};

/** */
class soap_ParametrosMulticast : public xml_data
{
public:
	soap_ParametrosMulticast()
	{
		GrupoMulticastConfiguracion = "";
		PuertoMulticastConfiguracion = 0;
	}
	~soap_ParametrosMulticast(){}

public:
	void xread(xml_node<> * xnode) {
		read_key(xnode, "GrupoMulticastConfiguracion", GrupoMulticastConfiguracion);
		read_key(xnode, "PuertoMulticastConfiguracion",PuertoMulticastConfiguracion);
	}

public:
	string GrupoMulticastConfiguracion;
	int PuertoMulticastConfiguracion;
};

/** */
class soap_AsignacionRecursosGW : public xml_data
{
public:
	soap_AsignacionRecursosGW(){}
	~soap_AsignacionRecursosGW(){}

public:
	void xread(xml_node<> * xnode) {	
		read_key(xnode, "IdRecurso", IdRecurso);
		read_key(xnode, "IdHost", IdHost);		
	}
public:
	string IdRecurso;
	string IdHost;
};

/** */
class soap_AsignacionUsuariosTV : public xml_data
{
public:
	soap_AsignacionUsuariosTV(){}
	~soap_AsignacionUsuariosTV(){}

public:
	void xread(xml_node<> * xnode) {
		read_key(xnode, "IdUsuario", IdUsuario);
		read_key(xnode, "IdHost", IdHost);		
	}
public:
	string IdUsuario;
	string IdHost;
};

/** */
class soap_DireccionamientoIP : public xml_data
{
public:
	soap_DireccionamientoIP()
	{
		TipoHost = THOST_TOP;
		Interno = false;
		Min = 0;
		Max = 0;
		EsCentralIP = false;
	}
	~soap_DireccionamientoIP(){}

public:
	void xread(xml_node<> * xnode) {	
		read_key(xnode, "IdHost", IdHost);
		read_key(xnode, "IpRed1", IpRed1);
		read_key(xnode, "IpRed2", IpRed2);
		read_key(xnode, "TipoHost", TipoHost);
		read_key(xnode, "Interno", Interno);
		read_key(xnode, "Min", Min);
		read_key(xnode, "Max", Max);
	/** 20180214 Nuevos campos asociados a los PROXIES y SERVIDORES de Presencia */
		read_key(xnode, "IpRed3", IpRed3);
		read_key(xnode, "EsCentralIP", EsCentralIP);
		read_key(xnode, "SrvPresenciaIpRed1", SrvPresenciaIpRed1);
		read_key(xnode, "SrvPresenciaIpRed2", SrvPresenciaIpRed3);
		read_key(xnode, "SrvPresenciaIpRed3", SrvPresenciaIpRed3);
	}
public:
	string IdHost;
	string IpRed1;			// Proxy Principal de la Dependencia
	string IpRed2;			// Proxy Alternativo 1
	THOST_ TipoHost;
	bool Interno;
	int Min;
	int Max;
	/** 20180214 Nuevos campos asociados a los PROXIES y SERVIDORES de Presencia */
	string IpRed3;			// Proxy Alternativo 2
	bool EsCentralIP;
	string SrvPresenciaIpRed1;
	string SrvPresenciaIpRed2;
	string SrvPresenciaIpRed3;
};

/** */
class soap_DireccionamientoSIP : public xml_data
{
public:
	class soap_StrNumeroAbonado : public xml_data
	{
	public:
		soap_StrNumeroAbonado(){}
		~soap_StrNumeroAbonado(){}
	public:
		void xread(xml_node<> * xnode) {	
			read_key(xnode, "Prefijo", Prefijo);
			read_key(xnode, "NumeroAbonado", NumeroAbonado);
		}
	public:
		string Prefijo;
		string NumeroAbonado;
	};

public:
	soap_DireccionamientoSIP(){}
	~soap_DireccionamientoSIP(){}

public:
	void xread(xml_node<> * xnode) {	
		read_key(xnode, "IdUsuario", IdUsuario);
		read_key(xnode, "NumerosAbonadoQueAtiende", "StrNumeroAbonado", NumerosAbonadoQueAtiende);
	}
public:
	string IdUsuario;
	vector<soap_StrNumeroAbonado> NumerosAbonadoQueAtiende;
};

/** */
class soap_ListaRedes : public xml_data
{
public:
	class soap_PlanRecursos : public xml_data
	{
	public:
		soap_PlanRecursos()
		{
			Tipo = TI_Radio;
		}
		~soap_PlanRecursos(){}
	public:
		void xread(xml_node<> * xnode) {	
			read_key(xnode, "IdRecurso", IdRecurso);
			read_key(xnode, "Tipo", Tipo);
		}
	public:
		string IdRecurso;
		TI_ Tipo;
	};

public:
	soap_ListaRedes(){}
	~soap_ListaRedes(){}

public:
	void xread(xml_node<> * xnode) {	
		read_key(xnode, "IdRed", IdRed);
		read_key(xnode, "Prefijo", Prefijo);
		read_key(xnode, "ListaRecursos", "PlanRecursos", ListaRecursos);
	}
public:
	string IdRed;
	string Prefijo;
	vector<soap_PlanRecursos> ListaRecursos;
};

/** */
class soap_ListaTroncales : public xml_data
{
public:
	class soap_PlanRecursos : public xml_data
	{
	public:
		soap_PlanRecursos()
		{
			Tipo = TI_Radio;
		}
		~soap_PlanRecursos(){}
	public:
		void xread(xml_node<> * xnode) {	
			read_key(xnode, "IdRecurso", IdRecurso);
			read_key(xnode, "Tipo", Tipo);
		}
	public:
		string IdRecurso;
		TI_ Tipo;
	};

public:
	soap_ListaTroncales(){}
	~soap_ListaTroncales(){}

public:
	void xread(xml_node<> * xnode) {	
		read_key(xnode, "IdTroncal", IdTroncal);
		read_key(xnode, "NumeroTest", NumeroTest);
		read_key(xnode, "ListaRecursos", "PlanRecursos", ListaRecursos);
	}
public:
	string IdTroncal;
	string NumeroTest;
	vector<soap_PlanRecursos> ListaRecursos;
};

/** */
class soap_NumeracionATS : public xml_data
{
public:
	class soap_RangosSCV : public xml_data
	{
	public:
		soap_RangosSCV()
		{
			Tipo = 0;
		}
		~soap_RangosSCV(){}
	public:
		void xread(xml_node<> * xnode) {	
			read_key(xnode, "IdPrefijo", IdPrefijo);
			read_key(xnode, "IdAbonado", IdAbonado);
			read_key(xnode, "Inicial", Inicial);
			read_key(xnode, "Final", Final);
			read_key(xnode, "IdSistema", IdSistema);
			read_key(xnode, "Central", Central);
			read_key(xnode, "Tipo", Tipo);
		}
	public:
		string IdPrefijo;
		string IdAbonado;
		string Inicial;
		string Final;
		string IdSistema;
		string Central;
		int Tipo;				// TODO: Es un string 'O', 'P', ???
	};

	class soap_PlanRutas : public xml_data
	{
	public:
		soap_PlanRutas(){}
		~soap_PlanRutas(){}
	public:
		void xread(xml_node<> * xnode) {	
			read_key(xnode, "TipoRuta", TipoRuta);
			read_key(xnode, "ListaTroncales", ListaTroncales);
		}
	public:
		string TipoRuta;
		vector<string> ListaTroncales;
	};

public:
	soap_NumeracionATS()
	{
		CentralPropia = false;
		Throwswitching = false;
	}
	~soap_NumeracionATS(){}

public:
	void xread(xml_node<> * xnode) {	
		read_key(xnode, "CentralPropia", CentralPropia);
		read_key(xnode, "Throwswitching", Throwswitching);
		read_key(xnode, "NumTest", NumTest);
		read_key(xnode, "RangosOperador", "RangosSCV", RangosOperador);
		read_key(xnode, "RangosPrivilegiados", "RangosSCV", RangosPrivilegiados);
		read_key(xnode, "ListaRutas", "PlanRutas", ListaRutas);
	}
public:
	bool CentralPropia;
	bool Throwswitching;
	string NumTest;
	vector<soap_RangosSCV> RangosOperador;
	vector<soap_RangosSCV> RangosPrivilegiados;
	vector<soap_PlanRutas> ListaRutas;
};

/** */
class soap_ResourceInfo : public xml_data
{
public:
	soap_ResourceInfo()	{

		GananciaAGCTX = 0;
		GananciaAGCRX = 0;
		SupresionSilencio = false;
		TamRTP = 0;
		Codec = 0;
		GrabacionEd137 = false;
		TipoDestino = 0;

		radio.UmbralTonoSQ = -30;
		/** 20180320. Nuevos Parámetros en interfaces analogicas */
		telef.iTmLlamEntrante = 30;
		telef.iTmDetFinLlamada= 6;
		/** 20181016. U2510. SP#01-15*/
		telef.superv_options = 0;
		telef.tm_superv_options = 10;
		telef.TReleaseBL = 3;
		telef.iDetCallerId = 1;
		telef.iTmCallerId = 3000;
		telef.iDetInversionPol = 1;
		telef.iPeriodoSpvRing = 200;
		telef.iFiltroSpvRing = 2;
		telef.iDetDtmf = 0;
	}
	~soap_ResourceInfo(){}
public:
	void xread(xml_node<> * xnode) {
		read_key(xnode, "GananciaAGCTX", GananciaAGCTX);
		read_key(xnode, "GananciaAGCTXdBm", GananciaAGCTXdBm);
		read_key(xnode, "GananciaAGCRX", GananciaAGCRX);
		read_key(xnode, "GananciaAGCRXdBm", GananciaAGCRXdBm);
		read_key(xnode, "SupresionSilencio", SupresionSilencio);
		read_key(xnode, "TamRTP", TamRTP);
		read_key(xnode, "Codec", Codec);
		read_key(xnode, "GrabacionEd137", GrabacionEd137);
		read_key(xnode, "TipoDestino", TipoDestino);
		read_key(xnode, "IdDestino", IdDestino);

		/** Telefonia */
		read_key(xnode, "ListaEnlacesInternos", telef.ListaEnlacesInternos);
		read_key(xnode, "IdPrefijo", telef.IdPrefijo);

		read_key(xnode, "RefrescoEstados", telef.RefrescoEstados);
		read_key(xnode, "Timeout", telef.Timeout);
		read_key(xnode, "LongRafagas", telef.LongRafagas);

		read_key(xnode, "Lado", telef.Lado);
		read_key(xnode, "Modo", telef.Modo);
		read_key(xnode, "TipoEM", telef.TipoEM);
		read_key(xnode, "IdRed", telef.IdRed);
		read_key(xnode, "IdTroncal", telef.IdTroncal);

		/** Radio */
		read_key(xnode, "EM", radio.EM);
		read_key(xnode, "SQ", radio.SQ);
		read_key(xnode, "PTT", radio.PTT);
		read_key(xnode, "FrqTonoE", radio.FrqTonoE);

		read_key(xnode, "UmbralTonoE", radio.UmbralTonoE);
		read_key(xnode, "FrqTonoM", radio.FrqTonoM);
		read_key(xnode, "UmbralTonoM", radio.UmbralTonoM);
		read_key(xnode, "BSS", radio.BSS);

		read_key(xnode, "NTZ", radio.NTZ);
		read_key(xnode, "TipoNTZ", radio.TipoNTZ);
		read_key(xnode, "Cifrado", radio.Cifrado);
		read_key(xnode, "SupervPortadoraTx", radio.SupervPortadoraTx);

		read_key(xnode, "SupervModuladoraTx", radio.SupervModuladoraTx);
		read_key(xnode, "ModoConfPTT", radio.ModoConfPTT);
		read_key(xnode, "RepSQyBSS", radio.RepSQyBSS);
		read_key(xnode, "DesactivacionSQ", radio.DesactivacionSQ);

		read_key(xnode, "TimeoutPTT", radio.TimeoutPTT);
		read_key(xnode, "UmbralVAD", radio.UmbralVAD);
		read_key(xnode, "TiempoPTT", radio.TiempoPTT);

		read_key(xnode, "NumFlujosAudio", radio.NumFlujosAudio);
		read_key(xnode, "KeepAlivePeriod", radio.KeepAlivePeriod);
		read_key(xnode, "KeepAliveMultiplier", radio.KeepAliveMultiplier);
		read_key(xnode, "IdEmplazamiento", radio.IdEmplazamiento);
		read_key(xnode, "Tipo", radio.Tipo);

		read_key(xnode, "FrqTonoSQ", radio.FrqTonoSQ);
		read_key(xnode, "UmbralTonoSQ", radio.UmbralTonoSQ);
		read_key(xnode, "FrqTonoPTT", radio.FrqTonoPTT);
		read_key(xnode, "UmbralTonoPTT", radio.UmbralTonoPTT);

		/** 20170316. Nuevos Parámetros para FD */
		read_key(xnode, "Metodos_bss_idmetodos_bss", radio.MetodoBSS);
		read_key(xnode, "GrsDelay", radio.GrsDelay);
		read_key(xnode, "EnableEventPttSq", radio.EnableEventPttSq);
		read_key(xnode, "GrabacionEd137", radio.GrabacionEd137);
		read_key(xnode, "ValuesTablaBss", radio.ValuesTablaBss);
		// radio.TablaBss="1, 2, 3,  4 ,5,   6,7,8,9,10,11,mmm,11,";
		/** 20180320. Nuevos Parámetros en interfaces analogicas */
		read_key(xnode, "iTmLlamEntrante", telef.iTmLlamEntrante, 30);
		read_key(xnode, "iTmDetFinLlamada", telef.iTmDetFinLlamada, 6);

		/** 20181016. U2510. SP#01-15*/
		read_key(xnode, "isuperv_options", telef.superv_options, 0);
		read_key(xnode, "itm_superv_options", telef.tm_superv_options, 10);
		read_key(xnode, "TReleaseBL", telef.TReleaseBL, 3);
		read_key(xnode, "iDetCallerId", telef.iDetCallerId, 1);
		read_key(xnode, "iTmCallerId", telef.iTmCallerId, 3000);
		read_key(xnode, "iDetInversionPol", telef.iDetInversionPol, 1);
		read_key(xnode, "iPeriodoSpvRing", telef.iPeriodoSpvRing, 200);
		read_key(xnode, "iFiltroSpvRing", telef.iFiltroSpvRing, 2);
		read_key(xnode, "iDetDtmf", telef.iDetDtmf, 0);
	}

public:
	/** Comunes */
	int GananciaAGCTX;
	string GananciaAGCTXdBm;
	int GananciaAGCRX;
	string GananciaAGCRXdBm;
	bool SupresionSilencio;
	int TamRTP;
	int Codec;
	bool GrabacionEd137;
	int TipoDestino;					// TODO: No se asigna.
	string IdDestino;
	struct Telef {
		/** Telefonia */
		vector<string> ListaEnlacesInternos;
		int IdPrefijo;					// TODO: No se asigna.
		/** LC */
		int RefrescoEstados;			// TODO: No se asigna.
		int Timeout;					// TODO: No se asigna.
		int LongRafagas;				// TODO: No se asigna.
		/** BC/BL/AB */
		int Lado;						// TODO: No se asigna.
		string Modo;					// TODO: No se asigna.
		string TipoEM;					// TODO: No se asigna.
		/** AB */
		string IdRed;					// TODO: Pendiente.
		/** R2/N5 */
		string IdTroncal;				// TODO: Pendiente.
		/** 20180320. Nuevos Parámetros en interfaces analogicas */
		int iTmLlamEntrante;
		int iTmDetFinLlamada;
		/** 20181016. U2510. SP#01-15*/
		int superv_options;
		int tm_superv_options;
		int TReleaseBL;
		int iDetCallerId;
		int iTmCallerId;
		int iDetInversionPol;
		int iPeriodoSpvRing;
		int iFiltroSpvRing;
		int iDetDtmf;
		Telef () : IdPrefijo(0), RefrescoEstados(0), Timeout(0), LongRafagas(0), Lado(0), iTmLlamEntrante(0), iTmDetFinLlamada(0),
			superv_options(0), tm_superv_options(0), TReleaseBL(0), iDetCallerId(0), iTmCallerId(0), iDetInversionPol(0),
			iPeriodoSpvRing(0), iFiltroSpvRing(0), iDetDtmf(0)
		{}
	} telef;
	struct Radio {
		bool EM;						// TODO: No se asigna.
		string SQ;
		string PTT;
		int FrqTonoE;					// TODO: No se asigna.
		int UmbralTonoE;				// TODO: No se asigna.
		int FrqTonoM;					// TODO: No se asigna.
		int UmbralTonoM;				// TODO: No se asigna.
		bool BSS;
		bool NTZ;						// TODO: No se asigna.
		int TipoNTZ;					// TODO: No se asigna.
		bool Cifrado;					// TODO: No se asigna.
		bool SupervPortadoraTx;			// => iSupervisionPortadoraTx
		bool SupervModuladoraTx;		// => iSupervisionModuladoraTx
		int ModoConfPTT;
		int RepSQyBSS;
		int DesactivacionSQ;
		int TimeoutPTT;
		int UmbralVAD;
		int TiempoPTT;
		int NumFlujosAudio;
		int KeepAlivePeriod;
		int KeepAliveMultiplier;
		string IdEmplazamiento;			// TODO: No se asigna.
		int Tipo;						// TODO: No se asigna.

		int FrqTonoSQ;
		int UmbralTonoSQ;
		int FrqTonoPTT;
		int UmbralTonoPTT;				// => iNivelTonoPtt

		/** 20170316. Nuevos Parámetros para FD */
		int MetodoBSS;
		int GrsDelay;
		bool EnableEventPttSq;
		bool GrabacionEd137;
		vector<int> ValuesTablaBss;

		Radio() : EM(false), FrqTonoE(0), UmbralTonoE(0), FrqTonoM(0), UmbralTonoM(0), BSS(false), NTZ(false), TipoNTZ(0), Cifrado(false),
			SupervPortadoraTx(false), SupervModuladoraTx(false), ModoConfPTT(0), RepSQyBSS(0), DesactivacionSQ(0), TimeoutPTT(0),
			UmbralVAD(0), TiempoPTT(0), NumFlujosAudio(0), KeepAlivePeriod(0), KeepAliveMultiplier(0), Tipo(0), FrqTonoSQ(0),
			UmbralTonoSQ(0), FrqTonoPTT(0), UmbralTonoPTT(0), MetodoBSS(0), GrsDelay(0), EnableEventPttSq(false), GrabacionEd137(false)
		{}
	} radio;
};

/** */
class soap_RecursosSCV : public xml_data
{
public:
	soap_RecursosSCV()
	{
		IdRecurso = "";
		Tipo = 0;
		Interface = TI_Radio;
		SlotPasarela = 0;
		NumDispositivoSlot = 0;
		ServidorSIP="";
		Diffserv = false;
		IdSistema = "";
		TipoRecurso = 0;
		IdTifX = "";
	}
	~soap_RecursosSCV(){}
public:
	void xread(xml_node<> * xnode) {	
		read_key(xnode, "IdRecurso", IdRecurso);
		read_key(xnode, "Tipo", Tipo);
		read_key(xnode, "Interface", Interface);
		read_key(xnode, "SlotPasarela", SlotPasarela);
		read_key(xnode, "NumDispositivoSlot", NumDispositivoSlot);
		read_key(xnode, "ServidorSIP", ServidorSIP);
		read_key(xnode, "Diffserv", Diffserv);
		read_key(xnode, "IdSistema", IdSistema);
		read_key(xnode, "TipoRecurso", TipoRecurso);
		read_key(xnode, "IdTifX", IdTifX);

		/** Lee Recursos de forma Remota */
		xdata_ResourceInfo = remote_get_xdata_func("GetParametrosRecursoById",
			"idSistema=departamento", 
			"idRecurso="+IdRecurso, 
			"tipo=" + Tools::Int2String((int)Interface));
		///** Simulacion de una carga */
		//xdata_ResourceInfo = Tools::read_txt_file(onfs("/home/GetParametrosRecursoById_idRecurso=RD-SIM_tipo=0.xml"));

		read_key(xdata_ResourceInfo, "Tablas", info);

		/** 20171107. Para los recursos sin destino, como el caso de los TX-HF, se pone el destino igual al recurso... */
		if (info.IdDestino=="IdDestino") {
			info.IdDestino = IdRecurso;
		}
		/***************/
	}
	void TablaBss(vector<int> &v) {
		v.clear();
		for (vector<int>::iterator i=info.radio.ValuesTablaBss.begin(); i!=info.radio.ValuesTablaBss.end(); i++) {
			v.push_back(*i);
		}
		/** si hay menos de 6, completo */
		if (v.size() < 6) {
			int add = 6 - v.size();
			for (int i=0; i<add; i++) {
				v.push_back(15);
			}
		}
	}

public:
	string IdRecurso;
	int Tipo;						// En los recursos radio Modo (0: RX, 1: TX y 2:RXTX).
	TI_ Interface;
	int SlotPasarela;
	int NumDispositivoSlot;
	string ServidorSIP;
	bool Diffserv;					
	string IdSistema;
	int TipoRecurso;
	string IdTifX;
	soap_ResourceInfo info;

protected:
	string xdata_ResourceInfo;
public:
	void saveIn(ofstream &ff) {
		ff << "[Recurso " << IdRecurso << "]" << endl;
		ff << xdata_ResourceInfo << endl << "[-----------------]" << endl;
	}
};

/** */
class soap_CfgPasarela : public xml_data
{
public:
	soap_CfgPasarela()
	{
		NumRecursos = 0;
		ModoSincronizacion = 0;
		PuertoLocalSNMP = 0;
		PuertoRemotoSNMP = 0;
		PuertoRemotoTrapsSNMP = 0;
		PuertoLocalSIP = 0;
		PeriodoSupervisionSIP = 0;
		SupervisionLanGW = 0;
		TmMaxSupervLanGW = 0;
		dvrrp = 0;
	}
	~soap_CfgPasarela(){}
public:
	void xread(xml_node<> * xnode) {	
		read_key(xnode, "Nombre", Nombre);
		read_key(xnode, "NumRecursos", NumRecursos);
		read_key(xnode, "ModoSincronizacion", ModoSincronizacion);
		read_key(xnode, "MasterSincronizacion", MasterSincronizacion);
		read_key(xnode, "PuertoLocalSNMP", PuertoLocalSNMP);
		read_key(xnode, "PuertoRemotoSNMP", PuertoRemotoSNMP);
		read_key(xnode, "PuertoRemotoTrapsSNMP", PuertoRemotoTrapsSNMP);
		read_key(xnode, "PuertoLocalSIP", PuertoLocalSIP);
		read_key(xnode, "PeriodoSupervisionSIP", PeriodoSupervisionSIP);
		read_key(xnode, "ListaRecursos", "RecursosSCV", ListaRecursos);
		/** */
		read_key(xnode, "IpGrabador1", Grabador1);
		read_key(xnode, "IpGrabador2", Grabador2);

		/** 20181016. U2510. SP#01-15*/
		read_key(xnode, "iSupervLanGW", SupervisionLanGW, 0);
		read_key(xnode, "itmmaxSupervLanGW", TmMaxSupervLanGW, 1);
		/** 20190221. REDAN107 */
		dvrrp = 2000;
	}

public:
	string Nombre;
	int NumRecursos;
	int ModoSincronizacion;
	string MasterSincronizacion;
	int PuertoLocalSNMP;
	int PuertoRemotoSNMP;
	int PuertoRemotoTrapsSNMP;
	int PuertoLocalSIP;
	int PeriodoSupervisionSIP;
	vector<soap_RecursosSCV> ListaRecursos;
	/** */
	string Grabador1;
	string Grabador2;
	
	/** 20181016. U2510. SP#01-15*/
	int SupervisionLanGW;
	int TmMaxSupervLanGW;
	/** 20190221. REDAN107 */
	int dvrrp;
};

/** */
class soap_config : public xml_data
{
public:
	soap_config(){}
	soap_config(remoteGetXdataFunc remoteFunc, string ip, string hwName, string ipServer);
	~soap_config(void);
public:
	void get(remoteGetXdataFunc remoteFunc, string ip, string hwName, string ipServer);

public:
	void xread(xml_node<> * xnode) {
		static xml_document<> doc;

		IdConfig = xnode->value();
		read_key(xdata_ParametrosGeneralesSistema, "ParametrosGeneralesSistema", ParametrosGeneralesSistema);
		read_key(xdata_ParametrosMulticast, "ParametrosMulticast", ParametrosMulticast);

		//read_key(xdata_ArrayOfAsignacionRecursosGW, "ArrayOfAsignacionRecursosGW", "AsignacionRecursosGW", ArrayOfAsignacionRecursosGW);
		doc.parse<0>((char *)xdata_ArrayOfAsignacionRecursosGW.c_str());
		read_key(doc, "ArrayOfAsignacionRecursosGW", "AsignacionRecursosGW", ArrayOfAsignacionRecursosGW);
		//read_key(xdata_ArrayOfAsignacionUsuariosTV, "ArrayOfAsignacionUsuariosTV", "AsignacionUsuariosTV", ArrayOfAsignacionUsuariosTV);
		doc.parse<0>((char *)xdata_ArrayOfAsignacionUsuariosTV.c_str());
		read_key(doc, "ArrayOfAsignacionUsuariosTV", "AsignacionUsuariosTV", ArrayOfAsignacionUsuariosTV);
		//read_key(xdata_ArrayOfDireccionamientoIP, "ArrayOfDireccionamientoIP", "DireccionamientoIP", ArrayOfDireccionamientoIP);
		doc.parse<0>((char *)xdata_ArrayOfDireccionamientoIP.c_str());
		read_key(doc, "ArrayOfDireccionamientoIP", "DireccionamientoIP", ArrayOfDireccionamientoIP);
		//read_key(xdata_ArrayOfDireccionamientoSIP, "ArrayOfDireccionamientoSIP", "DireccionamientoSIP", ArrayOfDireccionamientoSIP);
		doc.parse<0>((char *)xdata_ArrayOfDireccionamientoSIP.c_str());
		read_key(doc, "ArrayOfDireccionamientoSIP", "DireccionamientoSIP", ArrayOfDireccionamientoSIP);
		//read_key(xdata_ArrayOfListaRedes, "ArrayOfListaRedes", "ListaRedes", ArrayOfListaRedes);
		doc.parse<0>((char *)xdata_ArrayOfListaRedes.c_str());
		read_key(doc, "ArrayOfListaRedes", "ListaRedes", ArrayOfListaRedes);
		//read_key(xdata_ArrayOfListaTroncales, "ArrayOfListaTroncales", "ListaTroncales", ArrayOfListaTroncales);
		doc.parse<0>((char *)xdata_ArrayOfListaTroncales.c_str());
		read_key(doc, "ArrayOfListaTroncales", "ListaTroncales", ArrayOfListaTroncales);
		//read_key(xdata_ArrayOfNumeracionATS, "ArrayOfNumeracionATS", "NumeracionATS", ArrayOfNumeracionATS);
		doc.parse<0>((char *)xdata_ArrayOfNumeracionATS.c_str());
		read_key(doc, "ArrayOfNumeracionATS", "NumeracionATS", ArrayOfNumeracionATS);

		read_key(xdata_CfgPasarela, "CfgPasarela", CfgPasarela);
	}

public:
	string IdConfig;
	soap_ParametrosGeneralesSistema ParametrosGeneralesSistema;
	soap_ParametrosMulticast ParametrosMulticast;
	vector<soap_AsignacionRecursosGW> ArrayOfAsignacionRecursosGW;
	vector<soap_AsignacionUsuariosTV> ArrayOfAsignacionUsuariosTV;
	vector<soap_DireccionamientoIP> ArrayOfDireccionamientoIP;
	vector<soap_DireccionamientoSIP> ArrayOfDireccionamientoSIP;
	vector<soap_ListaRedes> ArrayOfListaRedes;
	vector<soap_ListaTroncales> ArrayOfListaTroncales;
	vector<soap_NumeracionATS> ArrayOfNumeracionATS;
	soap_CfgPasarela CfgPasarela;

public:
	soap_NumeracionATS *CentralPropia() {
		for (size_t i=0; i<ArrayOfNumeracionATS.size(); i++) {
			if (ArrayOfNumeracionATS[i].CentralPropia==true)
				return &ArrayOfNumeracionATS[i];
		}
		return NULL;
	}
	soap_NumeracionATS *CentralConRutaDirecta(string idres) {
		for (size_t it=0; it<ArrayOfListaTroncales.size(); it++) {
			for (size_t ir=0; ir<ArrayOfListaTroncales[it].ListaRecursos.size(); ir++) {
				if (ArrayOfListaTroncales[it].ListaRecursos[ir].IdRecurso == idres) {
					for (size_t ic=0; ic<ArrayOfNumeracionATS.size(); ic++) {
						if (ArrayOfNumeracionATS[ic].CentralPropia==false) {
							for (size_t iru=0; iru<ArrayOfNumeracionATS[ic].ListaRutas.size(); iru++) {
								if (ArrayOfNumeracionATS[ic].ListaRutas[iru].TipoRuta=="D") {
									for (size_t itr=0; itr<ArrayOfNumeracionATS[ic].ListaRutas[iru].ListaTroncales.size(); itr++) {
										if (ArrayOfNumeracionATS[ic].ListaRutas[iru].ListaTroncales[itr] == ArrayOfListaTroncales[it].IdTroncal) {
											return &ArrayOfNumeracionATS[ic];
										}
									}
								}
							}
						}
					}

				}
			}
		}
		return NULL;
	}
	soap_ListaTroncales *TrunckOfResource(string idres) {
		for (size_t it=0; it<ArrayOfListaTroncales.size(); it++) {
			for (size_t ir=0; ir<ArrayOfListaTroncales[it].ListaRecursos.size(); ir++) {
				if (ArrayOfListaTroncales[it].ListaRecursos[ir].IdRecurso == idres) {
					return &ArrayOfListaTroncales[it];
				}
			}
		}
		return NULL;
	}

public:
	string Server;
	string IpVirt;
	string IpHw;
	string IpCol;

public:
	string xdata_idConfig;
	string xdata_ParametrosGeneralesSistema;
	string xdata_ParametrosMulticast;
	string xdata_ArrayOfAsignacionRecursosGW;
	string xdata_ArrayOfAsignacionUsuariosTV;
	string xdata_ArrayOfDireccionamientoIP;
	string xdata_ArrayOfDireccionamientoSIP;
	string xdata_ArrayOfListaRedes;
	string xdata_ArrayOfListaTroncales;
	string xdata_ArrayOfNumeracionATS;
	string xdata_CfgPasarela;

public:
	void saveIn(string tofile) {
		ofstream ff(ON_FLASH(tofile));
		
		ff << "[IdConfig] " << endl << xdata_idConfig << endl << endl;
		ff << "[Parametros Generales] " << endl << xdata_ParametrosGeneralesSistema << endl << endl;
		ff << "[Parametros Multicast] " << endl << xdata_ParametrosMulticast << endl << endl;
		
		ff << "[CfgPasareala] " << endl << xdata_CfgPasarela << endl;
		for (size_t res = 0; res <CfgPasarela.ListaRecursos.size(); res++) {
			CfgPasarela.ListaRecursos[res].saveIn(ff);
		}
		ff << endl;

		ff << "[Asignacion Recursos GW] " << endl << xdata_ArrayOfAsignacionRecursosGW << endl << endl;
		ff << "[Asignacion Usuarios TV] " << endl << xdata_ArrayOfAsignacionUsuariosTV << endl << endl;
		ff << "[Plan IP] " << endl << xdata_ArrayOfDireccionamientoIP << endl << endl;
		ff << "[Plan SIP] " << endl << xdata_ArrayOfDireccionamientoSIP << endl << endl;
		ff << "[Troncales] " << endl << xdata_ArrayOfListaTroncales << endl << endl;
		ff << "[Redes] " << endl << xdata_ArrayOfListaRedes << endl << endl;
		ff << "[Numeracion ATS] " << endl << xdata_ArrayOfNumeracionATS << endl << endl;
	}

public:
	void sip_servers(vector<string> &sipservers);
	void firstRadioParams(int &kap, int &kam, int &sm);
	void TipoTelefonia(int stipo, int &jtipo);
};

#endif
