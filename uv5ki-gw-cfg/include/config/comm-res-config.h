#ifndef __COMM_RES_CONFIG_H__
#define __COMM_RES_CONFIG_H__

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream#include <utility>
#include <queue>
#include <algorithm>

#include "../base/code-base.h"
#include "../websrv/uv5kigwcfg-web-app-data.h"
#include "../tools/tools.h"
#include "../config/local-config.h"
#include "./comm-res-radio-config.h"
#include "./comm-res-telef-config.h"

/** */
class CommResBJitter : public jData
{
public:
	CommResBJitter() {
		max = min = 0;
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "min", min);
		write_key(writer, "max", max);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "min", min);
		read_key(base, "max", max);
	}
public:
	int min;
	int max;
};

/** */
class CommResHardware : public jData
{
public:
	CommResHardware() {
		AD_AGC = DA_AGC = 0;
		AD_Gain = DA_Gain = 0;
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "AD_AGC", AD_AGC);
		write_key(writer, "AD_Gain", AD_Gain);
		write_key(writer, "DA_AGC", DA_AGC);
		write_key(writer, "DA_Gain", DA_Gain);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "AD_AGC", AD_AGC);
		read_key(base, "AD_Gain", AD_Gain);
		read_key(base, "DA_AGC", DA_AGC);
		read_key(base, "DA_Gain", DA_Gain);
	}
public:
	int AD_AGC;
	int AD_Gain;
	int DA_AGC;
	int DA_Gain;
};

/** */
class CommResConfig : public jData
{
public:
	CommResConfig() {
		IdRecurso = "Vacio";
		Radio_o_Telefonia = 1;
		SlotPasarela = 0;
		NumDispositivoSlot = 0;
		TamRTP = 20;
		Codec = 0;
		Uri_Local = "sip:"+IdRecurso+"@127.0.0.1";
		enableRegistro = 0;
		szClave = "";
		LlamadaAutomatica=0;
		restriccion=0;
	}
	CommResConfig(soap_config &sc, int irec);
	~CommResConfig() {
		//clear_array(cpus);
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "IdRecurso", IdRecurso);
		write_key(writer, "Radio_o_Telefonia", Radio_o_Telefonia);
		write_key(writer, "SlotPasarela", SlotPasarela);
		write_key(writer, "NumDispositivoSlot", NumDispositivoSlot);
		write_key(writer, "TamRTP", TamRTP);
		write_key(writer, "Codec", Codec);
		write_key(writer, "Uri_Local", Uri_Local);
		write_key(writer, "enableRegistro", enableRegistro);
		write_key(writer, "szClave", szClave);
		write_key(writer, "Buffer_jitter", Buffer_jitter);
		write_key(writer, "hardware", hardware);
		write_key(writer, "radio", radio);
		write_key(writer, "telefonia", telefonia);
		write_key(writer, "LlamadaAutomatica", LlamadaAutomatica);
		write_key(writer, "restriccion", restriccion);
		write_key(writer, "blanca", blanca);
		write_key(writer, "negra", negra);
		write_key(writer, "iFlgUsarDiffServ", iFlgUsarDiffServ);
		write_key(writer, "szDestino", szDestino);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "IdRecurso", IdRecurso);
		read_key(base, "Radio_o_Telefonia", Radio_o_Telefonia);
		read_key(base, "SlotPasarela", SlotPasarela);
		read_key(base, "NumDispositivoSlot", NumDispositivoSlot);
		read_key(base,"TamRTP", TamRTP);
		read_key(base,"Codec", Codec);
		read_key(base,"Uri_Local", Uri_Local);
		read_key(base,"enableRegistro", enableRegistro);
		read_key(base, "szClave", szClave);
		read_key(base,"Buffer_jitter", Buffer_jitter);
		read_key(base,"hardware", hardware);
		read_key(base, "radio", radio);
		read_key(base,"telefonia", telefonia);
		read_key(base,"LlamadaAutomatica", LlamadaAutomatica);
		read_key(base, "restriccion", restriccion);
		read_key(base,"blanca", blanca);
		read_key(base,"negra", negra);
		read_key(base,"iFlgUsarDiffServ", iFlgUsarDiffServ);
		read_key(base,"szDestino", szDestino);
	}

public:
	string IdRecurso;
	int  Radio_o_Telefonia;
	int SlotPasarela;
	int NumDispositivoSlot;
	int TamRTP;
	int Codec;
	string Uri_Local;
	int enableRegistro;
	string szClave;

	CommResBJitter Buffer_jitter;
	CommResHardware hardware;
	CommResRadio radio;
	CommResTelef telefonia;

	int LlamadaAutomatica;
	int restriccion;
	vector<string> blanca;
	vector<string> negra;

	int iFlgUsarDiffServ;
	string szDestino;

public:
	vector<CommResTelefAtsRange> AtsRangosOperador(soap_config &sc) {
		vector<CommResTelefAtsRange> rangos;	
		soap_NumeracionATS *p_CentralPropia = sc.CentralPropia();
		if (p_CentralPropia != NULL) {
			for (size_t r=0; r<p_CentralPropia->RangosOperador.size(); r++) {
				rangos.push_back(CommResTelefAtsRange(p_CentralPropia->RangosOperador[r].Inicial,
					p_CentralPropia->RangosOperador[r].Final));
			}
		}
		return rangos;
	}
	vector<CommResTelefAtsRange> AtsRangosPrivilegiados(soap_config &sc){
		vector<CommResTelefAtsRange> rangos;	
		soap_NumeracionATS *p_CentralPropia = sc.CentralPropia();
		if (p_CentralPropia != NULL) {
			for (size_t r=0; r<p_CentralPropia->RangosPrivilegiados.size(); r++) {
				rangos.push_back(CommResTelefAtsRange(p_CentralPropia->RangosPrivilegiados[r].Inicial,
					p_CentralPropia->RangosPrivilegiados[r].Final));
			}
		}
		return rangos;
	}
	vector<CommResTelefAtsRange> AtsRangosDirectosOpe(soap_config &sc, string res){
		vector<CommResTelefAtsRange> rangos;	
		soap_NumeracionATS *p_Central = sc.CentralConRutaDirecta(res);
		if (p_Central != NULL) {
			for (size_t r=0; r<p_Central->RangosOperador.size(); r++) {
				rangos.push_back(CommResTelefAtsRange(p_Central->RangosOperador[r].Inicial,
					p_Central->RangosOperador[r].Final));
			}
		}
		return rangos;
	}
	vector<CommResTelefAtsRange> AtsRangosDirectorPri(soap_config &sc, string res){
		vector<CommResTelefAtsRange> rangos;	
		soap_NumeracionATS *p_Central = sc.CentralConRutaDirecta(res);
		if (p_Central != NULL) {
			for (size_t r=0; r<p_Central->RangosPrivilegiados.size(); r++) {
				rangos.push_back(CommResTelefAtsRange(p_Central->RangosPrivilegiados[r].Inicial,
					p_Central->RangosPrivilegiados[r].Final));
			}
		}
		return rangos;
	}
	string AtsLocalTestNumber(soap_config &sc) {
		soap_NumeracionATS *p_CentralPropia = sc.CentralPropia();
		if (p_CentralPropia != NULL)
			return p_CentralPropia->NumTest;
		return "";
	}
	string AtsRemoteTestNumber(soap_config &sc, string res) {
		soap_ListaTroncales *p_trunk = sc.TrunckOfResource(res);
		if (p_trunk != NULL)
			return p_trunk->NumeroTest;
		return "";
	}
};

#endif
