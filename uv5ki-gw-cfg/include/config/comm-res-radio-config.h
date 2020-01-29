#ifndef __COMM_RES_RADIO_CONFIG_H__
#define __COMM_RES_RADIO_CONFIG_H__

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
class CommResRadioEmplazamiento : public jData
{
public:
	CommResRadioEmplazamiento() {
		activoTx=0;
		activoRx=0;
		uriTxA = uriTxB = uriRxA = uriRxB = "";
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "activoTx", activoTx);
		write_key(writer, "activoRx", activoRx);
		write_key(writer, "uriTxA", uriTxA);
		write_key(writer, "uriTxB", uriTxB);
		write_key(writer, "uriRxA", uriRxA);
		write_key(writer, "uriRxB", uriRxB);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "activoTx", activoTx);
		read_key(base, "activoRx", activoRx);
		read_key(base, "uriTxA", uriTxA);
		read_key(base, "uriTxB", uriTxB);
		read_key(base, "uriRxA", uriRxA);
		read_key(base, "uriRxB", uriRxB);
	}
public:
	int activoTx;
	int activoRx;
	string uriTxA;
	string uriTxB;
	string uriRxA;
	string uriRxB;
};

/** */
class CommResRadioColateral : public jData
{
public:
	CommResRadioColateral() {
		name = "default";
		tipoConmutacion = 0;
	}
	~CommResRadioColateral() {
		clear_array(emplazamientos);
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "name", name);
		write_key(writer, "tipoConmutacion", tipoConmutacion);
		write_key(writer, "emplazamientos", emplazamientos);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "name", name);
		read_key(base, "tipoConmutacion", tipoConmutacion);
		read_key(base, "emplazamientos", emplazamientos);
	}
public:
	string name;
	int tipoConmutacion;
#if __POR_REFERENCIA__
	vector<CommResRadioEmplazamiento> emplazamientos;
#else
	vector<CommResRadioEmplazamiento *> emplazamientos;
#endif
};

/** */
class CommResRadio : public jData
{
public:
	CommResRadio() {
		tipo=0;
		sq=0;
		ptt=0;
		bss=0;
		modoConfPtt=0;
		repSqBss=1;
		desactivacionSq=1;
		timeoutPtt=200;
		metodoBss=0;
		umbralVad=-33;
		numFlujosAudio=1;
		tiempoPtt=0;
		tmVentanaRx=100;
		climaxDelay=1;
		tmRetardoFijo=100;
		bssRtp=0;
		retrasoSqOff=50;
		evtPTT=0;
		tjbd=20;
		tGRSid=10;
		iEnableGI=0;
		iSesionPrio=0;
		iPttPrio=0;
		iPrecisionAudio=1;

		FrqTonoSQ = 0;				// TODO: Definir los valores por defecto.
		UmbralTonoSQ = -15;			
		FrqTonoPTT = 0;					
		UmbralTonoPTT = -15;			 
		SupervPortadoraTx = 0;		 
		SupervModuladoraTx = 0;		
		iModoCalculoClimax = 0;		// 20170130. Nuevo. 0: Relativo (defaecto), 1: absoluto
	}
	~CommResRadio() {
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "tipo", tipo);
		write_key(writer, "sq", sq);
		write_key(writer, "ptt", ptt);
		write_key(writer, "bss", bss);
		write_key(writer, "modoConfPtt", modoConfPtt);
		write_key(writer, "repSqBss", repSqBss);
		write_key(writer, "desactivacionSq", desactivacionSq);
		write_key(writer, "timeoutPtt", timeoutPtt);
		write_key(writer, "metodoBss", metodoBss);
		write_key(writer, "umbralVad", umbralVad);
		write_key(writer, "numFlujosAudio", numFlujosAudio);
		write_key(writer, "tiempoPtt", tiempoPtt);
		write_key(writer, "tmVentanaRx", tmVentanaRx);
		write_key(writer, "climaxDelay", climaxDelay);
		write_key(writer, "tmRetardoFijo", tmRetardoFijo);
		write_key(writer, "bssRtp", bssRtp);
		write_key(writer, "retrasoSqOff", retrasoSqOff);
		write_key(writer, "evtPTT", evtPTT);
		write_key(writer, "tjbd", tjbd);
		write_key(writer, "tGRSid", tGRSid);
		write_key(writer, "iEnableGI", iEnableGI);
		write_key(writer, "tabla_indices_calidad", tabla_indices_calidad);
		write_key(writer, "iSesionPrio", iSesionPrio);
		write_key(writer, "iPttPrio", iPttPrio);
		write_key(writer, "colateral", colateral);
		write_key(writer, "iPrecisionAudio", iPrecisionAudio);
		write_key(writer, "iModoCalculoClimax", iModoCalculoClimax);

		write_key(writer, "FrqTonoSQ", FrqTonoSQ);
		write_key(writer, "UmbralTonoSQ", UmbralTonoSQ);
		write_key(writer, "FrqTonoPTT", FrqTonoPTT);
		write_key(writer, "UmbralTonoPTT", UmbralTonoPTT);
		write_key(writer, "SupervPortadoraTx", SupervPortadoraTx);
		write_key(writer, "SupervModuladoraTx", SupervModuladoraTx);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "tipo", tipo);
		read_key(base, "sq", sq);
		read_key(base, "ptt", ptt);
		read_key(base, "bss", bss);
		read_key(base, "modoConfPtt", modoConfPtt);
		read_key(base, "repSqBss", repSqBss);
		read_key(base, "desactivacionSq", desactivacionSq);
		read_key(base, "timeoutPtt", timeoutPtt);
		read_key(base, "metodoBss", metodoBss);
		read_key(base, "umbralVad", umbralVad);
		read_key(base, "numFlujosAudio", numFlujosAudio);
		/** 20170119. En REDAN venga lo que venga este valor vale 200 */
		/** 20170327. En REDAN venga lo que venga este valor vale 0 */
		// read_key(base, "tiempoPtt", tiempoPtt);
		tiempoPtt=0;
		/****************/
		read_key(base, "tmVentanaRx", tmVentanaRx);
		read_key(base, "climaxDelay", climaxDelay);
		read_key(base, "tmRetardoFijo", tmRetardoFijo);
		read_key(base, "bssRtp", bssRtp);
		read_key(base, "retrasoSqOff", retrasoSqOff);
		read_key(base, "evtPTT", evtPTT);
		read_key(base, "tjbd", tjbd);
		read_key(base, "tGRSid", tGRSid);
		read_key(base, "iEnableGI", iEnableGI);
		read_key(base,"iSesionPrio", iSesionPrio);
		read_key(base,"iPttPrio", iPttPrio);
		read_key(base,"colateral", colateral);
		read_key(base,"iPrecisionAudio", iPrecisionAudio);
		read_key(base,"iModoCalculoClimax", iModoCalculoClimax);

		read_key(base,"FrqTonoSQ", FrqTonoSQ);
		read_key(base,"UmbralTonoSQ", UmbralTonoSQ);
		read_key(base,"FrqTonoPTT", FrqTonoPTT);
		read_key(base,"UmbralTonoPTT", UmbralTonoPTT);
		read_key(base,"SupervPortadoraTx", SupervPortadoraTx);
		read_key(base,"SupervModuladoraTx", SupervModuladoraTx);
		read_key(base,"tabla_indices_calidad", tabla_indices_calidad);
	}

public:
	int tipo;
	int sq;
	int ptt;
	int bss;
	int modoConfPtt;
	int repSqBss;
	int desactivacionSq;
	int timeoutPtt;
	int metodoBss;
	int umbralVad;
	int numFlujosAudio;
	int tiempoPtt;
	int tmVentanaRx;
	int climaxDelay;
	int tmRetardoFijo;
	int bssRtp;
	int retrasoSqOff;
	int evtPTT;
	int tjbd;
	int tGRSid;
	int iEnableGI;
	vector<int> tabla_indices_calidad;
	int iSesionPrio;
	int iPttPrio;
	CommResRadioColateral colateral;
	int iPrecisionAudio;

	/** */
	int iModoCalculoClimax;

	/** Nuevos desde ULISES */
	int FrqTonoSQ;					
	int UmbralTonoSQ;					
	int FrqTonoPTT;					
	int UmbralTonoPTT;				// => iNivelTonoPtt
	int SupervPortadoraTx;			// => iSupervisionPortadoraTx
	int SupervModuladoraTx;			// => iSupervisionModuladoraTx
};

#endif
