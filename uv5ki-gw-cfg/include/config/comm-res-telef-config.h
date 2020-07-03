#ifndef __COMM_RES_TELEF_CONFIG_H__
#define __COMM_RES_TELEF_CONFIG_H__

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
class CommResTelefAtsRange : public jData
{
public:
	CommResTelefAtsRange(string Inicial="", string Final="") {
		inicial = Inicial;
		final = Final;
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "inicial", inicial);
		write_key(writer, "final", final);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "inicial", inicial);
		read_key(base, "final", final);
	}
public:
	string inicial;
	string final;
};

/** */
class CommResTelef : public jData
{
public:
	CommResTelef() {
		tipo = 0;
		lado = 1;
		t_eym = 0;
		h2h4 = 0;
		ladoeym = 0;
		modo = 0;
		no_test_local = no_test_remoto = "";
		it_release = 5;
		detect_vox = 0;
		umbral_vox = -10;
		tm_inactividad = 2;
		colateral_scv = 0;
		iT_Int_Warning = 5;

		idRed = "";
		idTroncal = "";
		/** 20180320. Nuevos Parámetros en interfaces analogicas */
		iTmLlamEntrante = 30;
		iTmDetFinLlamada = 6;
		/** 20181016. U2510. SP#01-15*/
		TReleaseBL = 3;
		iDetCallerId = 1;
		iTmCallerId = 3000;
		iDetInversionPol = 1;
		iPeriodoSpvRing = 200;
		iFiltroSpvRing = 2;
		iDetDtmf = 0;
		/** 20200703. Nuevos parámetros de líneas Telefónicas */
		iDetLineaAB = 0;
		iEnableNoED137 = 0;
		/** 20200703. Nueva Estructura de Colateral Remoto */
		r_automatica = 1;				// Elementos Existentes.
		tm_superv_options = 5;
		uri_remota = "";
		superv_options = 1;
		itiporespuesta = 0;	// Elementos Añadidos

		additional_uri_remota = "";
		additional_superv_options = 0;
		additional_itiporespuesta = 0;
		/** */
	}
	~CommResTelef() {
		clear_array(ats_rangos_dst);
		clear_array(ats_rangos_org);
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "tipo", tipo);
		write_key(writer, "lado", lado);
		write_key(writer, "t_eym", t_eym);
		write_key(writer, "h2h4", h2h4);
		write_key(writer, "ladoeym", ladoeym);
		write_key(writer, "modo", modo);
		write_key(writer, "no_test_local", no_test_local);
		write_key(writer, "no_test_remoto", no_test_remoto);
		write_key(writer, "it_release", it_release);
		write_key(writer, "detect_vox", detect_vox);
		write_key(writer, "umbral_vox", umbral_vox);
		write_key(writer, "tm_inactividad", tm_inactividad);
		write_key(writer, "colateral_scv", colateral_scv);
		write_key(writer, "ats_rangos_dst", ats_rangos_dst);
		write_key(writer, "ats_rangos_org", ats_rangos_org);
		write_key(writer, "iT_Int_Warning", iT_Int_Warning);

		write_key(writer, "idRed", idRed);
		write_key(writer, "idTroncal", idTroncal);
		write_key(writer, "listaEnlacesInternos", listaEnlacesInternos);

		write_key(writer, "ats_rangos_operador", ats_rangos_operador);
		write_key(writer, "ats_rangos_privilegiados", ats_rangos_privilegiados);
		write_key(writer, "ats_rangos_directos_ope", ats_rangos_directos_ope);
		write_key(writer, "ats_rangos_directos_pri", ats_rangos_directos_pri);
		/** 20180320. Nuevos Parámetros en interfaces analogicas */
		write_key(writer, "iTmLlamEntrante", iTmLlamEntrante);
		write_key(writer, "iTmDetFinLlamada", iTmDetFinLlamada);
		/** 20181016. U2510. SP#01-15*/
		write_key(writer, "TReleaseBL", TReleaseBL);
		write_key(writer, "iDetCallerId", iDetCallerId);
		write_key(writer, "iTmCallerId", iTmCallerId);
		write_key(writer, "iDetInversionPol", iDetInversionPol);
		write_key(writer, "iPeriodoSpvRing", iPeriodoSpvRing);
		write_key(writer, "iFiltroSpvRing", iFiltroSpvRing);
		write_key(writer, "iDetDtmf", iDetDtmf);
		/** 20200703. Nuevos parámetros de líneas Telefónicas */
		write_key(writer, "iDetLineaAB", iDetLineaAB);
		write_key(writer, "iEnableNoED137", iEnableNoED137);
		/** 20200703. Nueva Estructura de Colateral Remoto */
		write_key(writer, "r_automatica", r_automatica);	// Elementos Existentes.
		write_key(writer, "tm_superv_options", tm_superv_options);

		write_key(writer, "uri_remota", uri_remota);
		write_key(writer, "superv_options", superv_options);
		write_key(writer, "itiporespuesta", itiporespuesta);	// Elementos Añadidos

		write_key(writer, "additional_uri_remota",  additional_uri_remota);
		write_key(writer, "additional_superv_options", additional_superv_options);
		write_key(writer, "additional_itiporespuesta", additional_itiporespuesta);
		/** */
	}
	virtual void jread(Value &base)
	{
		read_key(base, "tipo", tipo);
		read_key(base, "lado", lado);
		read_key(base, "t_eym", t_eym);
		read_key(base, "h2h4", h2h4);
		read_key(base, "ladoeym", ladoeym);
		read_key(base, "modo", modo);
		read_key(base, "no_test_local", no_test_local);
		read_key(base, "no_test_remoto", no_test_remoto);
		read_key(base, "it_release", it_release);
		read_key(base, "detect_vox", detect_vox);
		read_key(base, "umbral_vox", umbral_vox);
		read_key(base, "tm_inactividad", tm_inactividad);
		read_key(base, "colateral_scv", colateral_scv);
		read_key(base, "ats_rangos_dst", ats_rangos_dst);
		read_key(base, "ats_rangos_org", ats_rangos_org);
		read_key(base, "iT_Int_Warning", iT_Int_Warning);

		read_key(base, "idRed", idRed);
		read_key(base, "idTroncal", idTroncal);
		read_key(base, "listaEnlacesInternos", listaEnlacesInternos);

		read_key(base, "ats_rangos_operador", ats_rangos_operador);
		read_key(base, "ats_rangos_privilegiados", ats_rangos_privilegiados);
		read_key(base, "ats_rangos_directos_ope", ats_rangos_directos_ope);
		read_key(base, "ats_rangos_directos_pri", ats_rangos_directos_pri);
		/** 20180320. Nuevos Parámetros en interfaces analogicas */
		read_key(base, "iTmLlamEntrante", iTmLlamEntrante, 30);
		read_key(base, "iTmDetFinLlamada", iTmDetFinLlamada, 6);
		/** 20181016. U2510. SP#01-15*/
		read_key(base, "TReleaseBL", TReleaseBL, 3);
		read_key(base, "iDetCallerId", iDetCallerId, 1);
		read_key(base, "iTmCallerId", iTmCallerId, 3000);
		read_key(base, "iDetInversionPol", iDetInversionPol, 1);
		read_key(base, "iPeriodoSpvRing", iPeriodoSpvRing, 200);
		read_key(base, "iFiltroSpvRing", iFiltroSpvRing, 2);
		read_key(base, "iDetDtmf", iDetDtmf, 0);
		/** 20200703. Nuevos parámetros de líneas Telefónicas */
		read_key(base, "iDetLineaAB", iDetLineaAB);
		read_key(base, "iEnableNoED137", iEnableNoED137);
		/** 20200703. Nueva Estructura de Colateral Remoto */
		read_key(base, "r_automatica", r_automatica);	// Elementos Existentes.
		read_key(base, "tm_superv_options", tm_superv_options);

		read_key(base, "uri_remota", uri_remota);
		read_key(base, "superv_options", superv_options);
		read_key(base, "itiporespuesta", itiporespuesta);	// Elementos Añadidos

		read_key(base, "additional_uri_remota", additional_uri_remota);
		read_key(base, "additional_superv_options", additional_superv_options);
		read_key(base, "additional_itiporespuesta", additional_itiporespuesta);
		/** */
	}

public:
	int tipo;
	int lado;
	int t_eym;
	int h2h4;
	int ladoeym;
	int modo;
	string no_test_local;
	string no_test_remoto;
	int it_release;
	int detect_vox;
	int umbral_vox;
	int tm_inactividad;
	int colateral_scv;
	int iT_Int_Warning;
	/** 20180320. Nuevos Parámetros en interfaces analogicas */
	int iTmLlamEntrante;
	int iTmDetFinLlamada;

	/** 20181016. U2510. SP#01-15*/
	int TReleaseBL;
	int iDetCallerId;
	int iTmCallerId;
	int iDetInversionPol;
	int iPeriodoSpvRing;
	int iFiltroSpvRing;
	int iDetDtmf;
	/** 20200703. Nuevos parámetros de líneas Telefónicas */
	int iDetLineaAB;
	int iEnableNoED137;
	/** 20200703. Nueva Estructura de Colateral Remoto */
	int r_automatica;			// Elementos Existentes
	int tm_superv_options;
	string uri_remota;
	int superv_options;
	int itiporespuesta;	// Elementos Añadidos

	string additional_uri_remota;
	int additional_superv_options;
	int additional_itiporespuesta;
	/** */


#if __POR_REFERENCIA__
	vector<CommResTelefAtsRange> ats_rangos_dst;
	vector<CommResTelefAtsRange> ats_rangos_org;
#else
	vector<CommResTelefAtsRange *> ats_rangos_dst;
	vector<CommResTelefAtsRange *> ats_rangos_org;
#endif

	/** Proviene de Ulises */
	string idRed;
	string idTroncal;
	vector<string> listaEnlacesInternos;

	vector<CommResTelefAtsRange> ats_rangos_operador;
	vector<CommResTelefAtsRange> ats_rangos_privilegiados;
	vector<CommResTelefAtsRange> ats_rangos_directos_ope;
	vector<CommResTelefAtsRange> ats_rangos_directos_pri;

};

#endif
