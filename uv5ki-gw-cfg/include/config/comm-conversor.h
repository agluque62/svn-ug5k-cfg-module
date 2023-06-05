#ifndef __COMM_CONFIG_CONVERSOR__
#define __COMM_CONFIG_CONVERSOR__

#include "../base/code-base.h"
#include "./comm-config.h"
#include "../his-proc.h"

/** */
class EvtConversor
{
public:
	static string EvtLadoR2N5(int lado);
	static string EvtNum2Sino(int num);
	static string EvtRespSipATS(int resp);
	static string EvtPeriodWarn(int valor);
	static string EvtTmoutRespStatus(int valor);
	static string EvtSpvColateral(int valor);
	static string EvtTipoTelefonia(int iItf);
	static string EvtRefresher(int valor);
	static string EvtTipoRadio(int tipo);
	static string EvtMetodoBss(int valor);
	static string EvtClimax(int valor);
	static string EvtPttPrio(int valor);
	static string EvtSessionPrio(int valor);
	static string EvtCalculoClimax(int valor);
	static string EvtPrecision(int valor);
	static string EvtAjusteCero(int valor);
};


/** */
class CfgConversor : public CodeBase
{
public:
	static int TelefoniaJ2M(int iItf);
	static int RadioPttPrioJ2M(int prio);
	static int RadioSessionPrioJ2M(int prio);
	static int AjusteCero(int valor);

protected:
	void SetInt(int *actual, int inuevo, int evento, string nombre, int (*Convierte)(int) = NULL, string (*EvtConvierte)(int) = NULL);
	int SetInt(LocalConfig &lcfg, string section, string key, int inuevo, int evento, string nombre);
	void SetString(char *actual, string snuevo, int evento, string nombre, int maxlen=1024);
	int SetString(LocalConfig &lcfg, string section, string key, string snuevo, int evento, string nombre, bool vacios=false);

protected:
	void *vp_mcfg;
	EventosHistoricos eventos;
	string _srec;
	vector<int> recadd;			// Recursos A�adidos...
};

/** */
class CommConversor : public CfgConversor , public EvtConversor
{
public:
	EventosHistoricos *convierte(CommConfig &cfgIn, void *p_mcfg, bool actualiza_ini, int *actsnmp, int *actrec);

protected:
	CommConfig *p_cfg_in;

protected:
	void SetTipoRadioTelefonia(struct cfgConfigRecurso *mrec, int Radio_o_Telefonia, int tipoRadio, int tipoTelefonia);
	void SetTipoPtt(char *actual, int nuevo);
	void SetTipoSqh(char *actual, int nuevo);
	void SetTipoConmutacionRadio(char *actual, int nuevo);

	/* **/
	void ParametrosGlobales();
	void Servidores();
	void Recurso(CommResConfig *p_rec, struct cfgConfigRecurso *mrec, bool modosistema=false);

	void RecursoGeneral(CommResConfig *p_rec, struct cfgConfigGeneralRecurso *mgen, bool modosistema=false);
	void RecursoColateralTPP(CommResConfig *p_rec, struct cfgColateralPP   *mcol, bool spvoptions=true);
	void RecursoAudio(CommResConfig *p_rec, struct cfgConfigRecAudio   *maud);
	void RecursoRadio(CommResConfig *p_rec, struct cfgConfigGeneralRecurso *mgen, struct cfgConfigIfRadio   *mrad);
	void ColateralesRadio(CommResRadioColateral *p_col, struct cfgColateralRadio *mcol);

	void RecursoLcen(CommResConfig *p_rec, struct cfgConfigIfLcen   *mlce, struct cfgConfigGeneralRecurso *mgen);
	void RecursoTelefoniaR2N5(CommResConfig *p_rec, struct cfgConfigIfR2N5   *mr2n5, struct cfgConfigGeneralRecurso *mgen);
	void RecursoTelefoniaAnalogica(CommResConfig *p_rec, struct cfgConfigIfTlf   *mtlf, struct cfgConfigGeneralRecurso *mgen);
	void RecursoTelefoniaDigital(CommResConfig *p_rec, struct cfgConfigIfRdsi  *mdig);

	void RecursosBorrados();
	void RecursosAnadidos();
	bool IsAdd(int grec);		// Mira si el Recurso es Añadido...

	void ActualizaSnmpIni();
	int ActualizaRecordIni();
	void ActualizaWebIni();

	void TablasUlises(CommUv5Config *p_tbs);
	void TablaUlises(vector<CommUlises_st_numeracionats> &plan, 
		vector<CommUlises_st_direccionamientoip> &planip, struct st_numeracionats *p_plan);
	void TablaUlises(vector<CommUlises_st_direccionamientoip> &plan, struct st_direccionamientoip *p_plan);
	void TablaUlises(vector<CommUlises_st_direccionamientoip> &plan, struct st_direccionamientoproxy *p_plan);
	void TablaUlises(vector<CommUlises_st_listatroncales> &plan, struct st_listatroncales *p_plan);
	void TablaUlises(vector<CommUlises_st_listaredes> &plan, struct st_listaredes *p_plan);
	void TablaUlises(vector<CommUlises_st_asignacionusuario_tv> &plan, struct st_asignacionusuario_tv *p_plan);
	void TablaUlises(vector<CommUlises_st_asignacionusuario_gw> &plan, struct st_asignacionrecursos_gw *p_plan);
	void TablaUlises(vector<CommUlises_st_direccionamiento_sip> &plan, struct st_direccionamiento_sip *p_plan);

	void ParametrosmodoSDC91();


};


#endif
