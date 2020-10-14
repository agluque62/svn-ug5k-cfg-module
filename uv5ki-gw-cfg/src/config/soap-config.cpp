#include "../../include/config/soap-config.h"

/** */
remoteGetXdataFunc xml_data::remote_get_xdata_func;

/** */
soap_config::soap_config(remoteGetXdataFunc remoteFunc, string iphw, string hwName, string ipServer)
{
	get(remoteFunc, iphw, hwName, ipServer);
}

/** */
soap_config::~soap_config(void)
{
}

void soap_config::get(remoteGetXdataFunc remoteFunc, string iphw, string hwName, string ipServer) 
{
	remote_get_xdata_func = remoteFunc;

	PLOG_DEBUG("SOAP: GetVersionConfiguracion desde %s", ipServer.c_str());
	xdata_idConfig = remote_get_xdata_func("GetVersionConfiguracion", "id_sistema=departamento", "", "");

	PLOG_DEBUG("SOAP: GetParametrosGenerales desde %s", ipServer.c_str());
	xdata_ParametrosGeneralesSistema = remote_get_xdata_func("GetParametrosGenerales", "id_sistema=departamento", "", "");

	PLOG_DEBUG("SOAP: GetParametrosMulticast desde %s", ipServer.c_str());
	xdata_ParametrosMulticast = remote_get_xdata_func("GetParametrosMulticast", "id_sistema=departamento", "", "");

	PLOG_DEBUG("SOAP: GetPlanAsignacionRecursos desde %s", ipServer.c_str());
	xdata_ArrayOfAsignacionRecursosGW = remote_get_xdata_func("GetPlanAsignacionRecursos", "id_sistema=departamento", "", "");

	PLOG_DEBUG("SOAP: GetPlanAsignacionUsuarios desde %s", ipServer.c_str());
	xdata_ArrayOfAsignacionUsuariosTV = remote_get_xdata_func("GetPlanAsignacionUsuarios", "id_sistema=departamento", "", "");

	PLOG_DEBUG("SOAP: GetPlanDireccionamientoIP desde %s", ipServer.c_str());
	xdata_ArrayOfDireccionamientoIP = remote_get_xdata_func("GetPlanDireccionamientoIP", "id_sistema=departamento", "", "");

	PLOG_DEBUG("SOAP: GetPlanDireccionamientoSIP desde %s", ipServer.c_str());
	xdata_ArrayOfDireccionamientoSIP = remote_get_xdata_func("GetPlanDireccionamientoSIP", "id_sistema=departamento", "", "");

	PLOG_DEBUG("SOAP: GetPlanRedes desde %s", ipServer.c_str());
	xdata_ArrayOfListaRedes = remote_get_xdata_func("GetPlanRedes", "id_sistema=departamento", "", "");

	PLOG_DEBUG("SOAP: GetPlanTroncales desde %s", ipServer.c_str());
	xdata_ArrayOfListaTroncales = remote_get_xdata_func("GetPlanTroncales", "id_sistema=departamento", "", "");

	PLOG_DEBUG("SOAP: GetPlanNumeracionATS desde %s", ipServer.c_str());
	xdata_ArrayOfNumeracionATS = remote_get_xdata_func("GetPlanNumeracionATS", "id_sistema=departamento", "", "");

	PLOG_DEBUG("SOAP: GetCfgPasarela desde %s", ipServer.c_str());
	xdata_CfgPasarela = remote_get_xdata_func("GetCfgPasarela", "id_sistema=departamento", "id_hw=" + hwName, "");
	///** Simulacion para Pruebas */
	//xdata_CfgPasarela = Tools::read_txt_file(onfs("/home/GetCfgPasarela_id_hw=CGW3_.xml"));

	XDeserialize(xdata_idConfig, "string");

	/** Obtener las Ip de la configuracion */
	this->IpHw = this->IpCol = this->IpVirt = iphw;
	for (vector<soap_DireccionamientoIP>::iterator host = ArrayOfDireccionamientoIP.begin();
		host != ArrayOfDireccionamientoIP.end(); host++)
	{
		if (host->IdHost == CfgPasarela.Nombre) {
			this->IpVirt = host->IpRed1;
			this->IpCol = host->IpRed2;		// sistema::ipColateral();	// host->IpRed2;
			break;
		}
	}
	this->Server = ipServer;
}
/** */
void soap_config::sip_servers(vector<string> &sipservers)
{
	sipservers.clear();
	for (size_t i=0; i<CfgPasarela.ListaRecursos.size(); i++)
	{
		if (CfgPasarela.ListaRecursos[i].TipoRecurso!=0)		// No Radio..
		{
			if (CfgPasarela.ListaRecursos[i].ServidorSIP != "")
			{
				sipservers.push_back(CfgPasarela.ListaRecursos[i].ServidorSIP);
				return;
			}
		}
	}
}

/** */
void soap_config::firstRadioParams(int &kap, int &kam, int &sm)
{
	kap = 200;
	kam = 10;
	sm = 0;
	for (size_t i=0; i<CfgPasarela.ListaRecursos.size(); i++)
	{
		if (CfgPasarela.ListaRecursos[i].TipoRecurso==0)		// Tipo radio..
		{
			kap = CfgPasarela.ListaRecursos[i].info.radio.KeepAlivePeriod;
			kam = CfgPasarela.ListaRecursos[i].info.radio.KeepAliveMultiplier;
			sm = CfgPasarela.ListaRecursos[i].info.SupresionSilencio;
			return;
		}
	}
}

/** */
void soap_config::TipoTelefonia(int stipo, int &jtipo)
{
	jtipo = stipo==1 ? 5 :			// LC
		stipo==2 ? 1 :				// BC
		stipo==3 ? 0 :				// BL
		stipo==4 ? 2 :				// AB
		stipo==5 ? 3 :				// R2
		stipo==6 ? 4 :				// N5
		stipo;
}

