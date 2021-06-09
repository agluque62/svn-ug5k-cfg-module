#include "../../include/config/working-config.h"
#include "../../include/config/comm-conversor.h"
#include "../../include/his-proc.h"

#define mcfg	((ug5k_mem_config *)vp_mcfg)

int force_rdaudio_normal = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** */
int CfgConversor::TelefoniaJ2M(int iItf)
{
	int tbconv[]={
		CFG_IFREC_TIPO_BL,
		CFG_IFREC_TIPO_BC,
		CFG_IFREC_TIPO_AB,
		CFG_IFREC_TIPO_R2,
		CFG_IFREC_TIPO_N5,
		CFG_IFREC_TIPO_LCEN,
		CFG_IFREC_TIPO_QSIG,
		CFG_IFREC_TIPO_TUN_LOCAL,
		CFG_IFREC_TIPO_TUN_REMOTO
	};
	if (iItf < 9)
		return tbconv[iItf];
	return iItf;
}

/** */
int CfgConversor::RadioPttPrioJ2M(int prio)
{
	int tbconv[] = {1,3,4};
	return prio < 3 ? tbconv[prio] : 1;
}

/** */
int CfgConversor::RadioSessionPrioJ2M(int prio)
{
	int tbconv[] = {3,1};
	return prio < 2 ? tbconv[prio] : 3;
}


/** */
void CfgConversor::SetInt(int *actual, int inuevo, int evento, string nombre, int (*Convierte)(int))
{
	inuevo = Convierte==NULL ? inuevo : Convierte(inuevo);

	if (*actual != inuevo)
	{
		char buffer[16];
#ifdef _WIN32
		string data = _itoa(inuevo, buffer, 10);
#else
		sprintf(buffer, "%d", inuevo);
		string data = string(buffer);
#endif
		eventos.push_back(CommConvertEvent(evento, _srec, nombre, data));
	}
	*actual = inuevo;
}

/** */
void CfgConversor::SetInt(LocalConfig &lcfg, string section, string key, int inuevo, int evento, string nombre)
{
	int iactual = atoi(lcfg.get(section, key).c_str());
	if (inuevo != iactual)
	{
		char buffer[16];
#ifdef _WIN32
		eventos.push_back(CommConvertEvent(evento, "", nombre, _itoa(inuevo, buffer, 10)));
#else
		sprintf(buffer, "%d", inuevo);
		eventos.push_back(CommConvertEvent(evento, "", nombre, buffer));
#endif
	}
	lcfg.set(section, key, Tools::itoa(inuevo));
}

/** */
void CfgConversor::SetString(char *actual, string snuevo, int evento, string nombre, int maxlen)
{	
	if (snuevo != string(actual))
	{
		eventos.push_back(CommConvertEvent(evento, _srec, nombre, snuevo));
	}
	strncpy(actual,snuevo.c_str(),maxlen);
}

/** */
void CfgConversor::SetString(LocalConfig &lcfg, string section, string key, string snuevo, int evento, string nombre, bool vacios)
{
	if (snuevo != "" || vacios == true)
	{
		string sactual = lcfg.get(section, key);
		if (sactual != snuevo) 
		{
			eventos.push_back(CommConvertEvent(evento, "", nombre, snuevo));
			lcfg.set(section, key, snuevo);
		}
	}
	else
		PLOG_ERROR("Intento Escritura String Vacio en INI (%s-%s).", section.c_str(), key.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** */
void CommConversor::SetTipoRadioTelefonia(struct cfgConfigRecurso *mrec, int Radio_o_Telefonia, int tipoRadio, int tipoTelefonia)
{
	if (Radio_o_Telefonia == 1)	// Tipo Radio
	{
		SetInt(&mrec->sGeneral.iTipo, tipoRadio, INCI_MPSW, "TIPO-GRAL-RADIO");
		mrec->sGeneral.iTipoIf = CFG_IFREC_TIPO_RADIO;
	}
	else if (Radio_o_Telefonia==2)
	{
		SetInt(&mrec->sGeneral.iTipoIf, tipoTelefonia, INCI_MPSW, "TIPO-TELEFONIA", CommConversor::TelefoniaJ2M);		
		mrec->sGeneral.iTipo = CFG_REC_TIPO_AUDIO_TXRX;
	}
	else
		PLOG_ERROR("Valor Radio_o_Telefonia incorrecto: %d", Radio_o_Telefonia);
}

/** */
void CommConversor::SetTipoPtt(char *actual, int nuevo )
{
	char ctipo = nuevo==0 ? 'h' : 's';
	if (ctipo != *actual)
		eventos.push_back(CommConvertEvent(INCI_MPSW, _srec, "TIPO-PTT", nuevo==0 ? "HARDWARE" : "VOX"));
	*actual = ctipo;
}

/** */
void CommConversor::SetTipoSqh(char *actual, int nuevo)
{
	char ctipo = nuevo==1 ? 'v' : 'h';
	if (ctipo != *actual)
		eventos.push_back(CommConvertEvent(INCI_MPSW, _srec, "TIPO-SQUELCH", nuevo==0 ? "HARDWARE" : "VOX"));
	*actual = ctipo;
}

/** */
void CommConversor::SetTipoConmutacionRadio(char *actual, int nuevo)
{
	char ctipo = nuevo==0 ? 'M' : 'A';
	if (ctipo != *actual)
		eventos.push_back(CommConvertEvent(INCI_MPSW, _srec, "TIPO-CONMUTACION", nuevo==0 ? "M" : "A"));
	*actual = ctipo;
}

/** */
vector<CommConvertEvent> *CommConversor::convierte(CommConfig &cfgIn, void *p_mcfg, bool actualiza_ini)
{
	/** 20200508 Inicializar opciones */
	force_rdaudio_normal = LocalConfig::p_cfg->getint(strRuntime, strRuntimeItemForceRdAudioNormal, "0");

	/** Inicializar Variables Internas. */
	p_cfg_in = &cfgIn;
	vp_mcfg = p_mcfg;
	_srec = "";
	eventos.clear();

	ParametrosGlobales();
	Servidores();

	/** Genero Eventos de Recursos Borrados y A�adidos */
	RecursosBorrados();
	RecursosAnadidos();

	/** Recursos. */
	mcfg->iNumRecursos = p_cfg_in->recursos.size() > 16 ? 16 : p_cfg_in->recursos.size();

	for (int irec=0; irec<mcfg->iNumRecursos; irec++)
	{
		/** Los localizadores de recursos */
#if __POR_REFERENCIA__
		mcfg->asLocRec[irec].iSlot = p_cfg_in->recursos[irec].SlotPasarela;	
		mcfg->asLocRec[irec].iDispositivo = p_cfg_in->recursos[irec].NumDispositivoSlot;
		
		/** El identificador global del recurso */
		int igrec = mcfg->asLocRec[irec].iSlot*4 + mcfg->asLocRec[irec].iDispositivo;
		Recurso(&p_cfg_in->recursos[irec], &mcfg->sRecurso[igrec]);
#else
		mcfg->asLocRec[irec].iSlot = p_cfg_in->recursos[irec]->SlotPasarela;	
		mcfg->asLocRec[irec].iDispositivo = p_cfg_in->recursos[irec]->NumDispositivoSlot;
		
		/** El identificador global del recurso */
		int igrec = mcfg->asLocRec[irec].iSlot*4 + mcfg->asLocRec[irec].iDispositivo;
		Recurso(p_cfg_in->recursos[irec], &mcfg->sRecurso[igrec]);
#endif
		/** Modo de Pasarela en Recursos */
		mcfg->sRecurso[igrec].sGeneral.iCfgModoSistema = p_cfg_in->tipo==1 ? 0 : 1;						// 0: Ulises, 1: Redan.
	}

	/** ULISES */
	TablasUlises(&p_cfg_in->ulises);

	/** Actualizar datos en INI's */
	if (actualiza_ini) 
	{
		ActualizaRecordIni();
		ActualizaSnmpIni();
		/** 20190308. Ya no hace falta actualizar el ficheros INI */
		//ActualizaWebIni();
	}

	return &eventos;
}

/** */
void CommConversor::ParametrosGlobales()
{
	/** */
	_srec = p_cfg_in->general.name;

	// strcpy(mcfg->acIdSistema,"");
	SetString(mcfg->acIdSistema, p_cfg_in->general.emplazamiento, INCI_MPGP, "ID-EMPLAZAMIENTO",CFG_MAX_LONG_ID);
	SetString(mcfg->szNombre, p_cfg_in->general.name, INCI_MPGP, "ID-PASARELA",CFG_MAX_LONG_NOMBRE_PASARELA);		
	SetString(mcfg->szDirVirtual, p_cfg_in->general.ipv, INCI_MPGP, "IP-VIRTUAL",CFG_MAX_LONG_URL);
	SetString(mcfg->szDirSrvConfig, p_cfg_in->general.ips, INCI_MPGP, "URL-SERVIDOR",CFG_MAX_LONG_URL);
	SetInt(&mcfg->iNivelConsola, p_cfg_in->general.nivelconsola, INCI_MPGP, "NIVEL-CONSOLA");
	SetInt(&mcfg->iPuertoConsola, p_cfg_in->general.puertoconsola, INCI_MPGP, "PUERTO-CONSOLA");
	SetInt(&mcfg->iNivelIncidencias, p_cfg_in->general.nivelIncidencias, INCI_MPGP, "NIVEL-INCIDENCIAS");
	SetInt(&mcfg->iDual, p_cfg_in->general.dualidad, INCI_MPGP, "DUALIDAD");
	strcpy(mcfg->acIdConfig,"");

	// TODO: De momento se queda asi...
	mcfg->iModoSincronizacion = 2;

	for (size_t icpu=0; icpu<p_cfg_in->general.cpus.size() && icpu < 2; icpu++)
	{
#if __POR_REFERENCIA__
		if (icpu==0)
			SetString(mcfg->szDirCPU0, p_cfg_in->general.cpus[0].ipb, INCI_MPGP, "IP CPU0", CFG_MAX_LONG_URL);
		else if (icpu==1)
			SetString(mcfg->szDirCPU1, p_cfg_in->general.cpus[1].ipb, INCI_MPGP, "IP CPU1", CFG_MAX_LONG_URL);
#else
		if (icpu==0)
			SetString(mcfg->szDirCPU0, p_cfg_in->general.cpus[0]->ipb, INCI_MPGP, "IP CPU0", CFG_MAX_LONG_URL);
		else if (icpu==1)
			SetString(mcfg->szDirCPU1, p_cfg_in->general.cpus[1]->ipb, INCI_MPGP, "IP CPU1", CFG_MAX_LONG_URL);
#endif
	}

	/** Tipo de Slots */
	for (size_t slv=0; slv < p_cfg_in->hardware.slv.size() && slv < MAX_SLOTS_PASARELA; slv++)
	{
#if __POR_REFERENCIA__
		mcfg->acTipoSlot[slv] = (char )p_cfg_in->hardware.slv[slv].tp;
#else
		mcfg->acTipoSlot[slv] = (char )p_cfg_in->hardware.slv[slv]->tp;
#endif
	}

	SetInt(&mcfg->iSipPuertoLocal, p_cfg_in->servicios.sip.PuertoLocalSIP, INCI_MPGP, "PUERTO-SIP");
	SetInt(&mcfg->iSipPeriodoSuperv, p_cfg_in->servicios.sip.PeriodoSupervisionSIP, INCI_MPGP, "SUPERVISION-SIP");

	SetInt(&mcfg->iSnmpPuertoRemoto, p_cfg_in->servicios.snmp.sport, INCI_MPGP, "Puerto Servicio SNMP");
	SetInt(&mcfg->iRecPuertoRemoto, p_cfg_in->servicios.grab.sport, INCI_MPGP, "Puerto Servicio Grabador");

	// ULISES
	SetString(mcfg->acGrupoMulticast, p_cfg_in->ulises.MulticastGroup, INCI_MPGP, "IP MCAST", CFG_MAX_LONG_URL);
	SetInt((int *)&mcfg->uiPuertoMulticast, p_cfg_in->ulises.MulticastPort, INCI_MPGP, "Puerto MCAST");

	// MODO en Pasarela
	mcfg->iCfgModoSistema = p_cfg_in->tipo==1 ? 0 : 1;						// 0: Ulises, 1: Redan.

	/** 20181016. U2510. SP#01-15*/
	mcfg->iSupervLanGW = p_cfg_in->general.SupervisionLanGW;
	mcfg->itmmaxSupervLanGW = p_cfg_in->general.TmMaxSupervLanGW;

	/** 20190221. REDAN107*/
	mcfg->idelayIniciaVrrp = p_cfg_in->general.dvrrp;
}

/** */
void CommConversor::Servidores()
{
	size_t i;

	/** SIP Proxy */
	for (i=0; i<p_cfg_in->servicios.sip.proxys.size() && i<NUM_SIP_PROXY; i++)
	{
#if __POR_REFERENCIA__
		memcpy(mcfg->szDirSipProxy[i], p_cfg_in->servicios.sip.proxys[i].ip.c_str(), MAX_LONG_DIRIP);
#else
		memcpy(mcfg->szDirSipProxy[i], p_cfg_in->servicios.sip.proxys[i]->ip.c_str(), MAX_LONG_DIRIP);
#endif
	}

	/** SIP Registrars */
	for (i=0; i< p_cfg_in->servicios.sip.registrars.size() && i<NUM_SIP_REGISTRAR; i++)
	{
#if __POR_REFERENCIA__
		memcpy(mcfg->szDirSipRegistrar[i], p_cfg_in->servicios.sip.registrars[i].ip.c_str(), MAX_LONG_DIRIP);
#else
		memcpy(mcfg->szDirSipRegistrar[i], p_cfg_in->servicios.sip.registrars[i]->ip.c_str(), MAX_LONG_DIRIP);
#endif
	}

	/** NTP Master */	
	for (i=0; i<2;i++)
		strcpy(mcfg->szDirSrvNtp[i], "");
	for (i=0; i<p_cfg_in->servicios.sincr.servidores.size() && i<2; i++)
	{
#if __POR_REFERENCIA__
		memcpy(mcfg->szDirSrvNtp[i], p_cfg_in->servicios.sincr.servidores[i].ip.c_str(),MAX_LONG_DIRIP);
#else
		memcpy(mcfg->szDirSrvNtp[i], p_cfg_in->servicios.sincr.servidores[i]->ip.c_str(),MAX_LONG_DIRIP);
#endif
	}
				// El tercer servidor siempre apunta al servidor de configuracion.
	memcpy(mcfg->szDirSrvNtp[2], mcfg->szDirSrvConfig, CFG_MAX_LONG_URL);
				// Debo quitar la extension de PUERTO.
	char * pch = strchr(mcfg->szDirSrvNtp[2],':');
	if (pch!=NULL)
		*pch = 0;

}

/** */
void CommConversor::Recurso(CommResConfig *p_rec, struct cfgConfigRecurso *mrec, bool add)
{
	SetString(mrec->sGeneral.szNombre, p_rec->IdRecurso, INCI_MPSW, "ID-RECURSO",CFG_MAX_LONG_NOMBRE_RECURSO);
	_srec = string(mrec->sGeneral.szNombre);

	SetTipoRadioTelefonia(mrec, p_rec->Radio_o_Telefonia, p_rec->radio.tipo, p_rec->telefonia.tipo);

	RecursoGeneral(p_rec, &mrec->sGeneral);
	RecursoAudio(p_rec, &mrec->uRec.sAudio);

	switch(mrec->sGeneral.iTipoIf)
	{
	case CFG_IFREC_TIPO_RADIO:
		RecursoRadio(p_rec, &mrec->sGeneral, &mrec->uIf.sRadio);
		break;
	
	case CFG_IFREC_TIPO_LCEN:
		RecursoColateralTPP(p_rec, &mrec->sGeneral.sColateral, true);
		RecursoLcen(p_rec, &mrec->uIf.sLcen, &mrec->sGeneral);
		break;

	case CFG_IFREC_TIPO_R2:
	case CFG_IFREC_TIPO_N5:
		RecursoColateralTPP(p_rec, &mrec->sGeneral.sColateral, true);
		RecursoTelefoniaR2N5(p_rec, &mrec->uIf.sR2N5, &mrec->sGeneral);
		break;

	case CFG_IFREC_TIPO_BC:
	case CFG_IFREC_TIPO_BL:
	case CFG_IFREC_TIPO_AB:
	case CFG_IFREC_TIPO_EYM_PP:
	case CFG_IFREC_TIPO_EYM_MARC:
		RecursoColateralTPP(p_rec, &mrec->sGeneral.sColateral);
		RecursoTelefoniaAnalogica(p_rec, &mrec->uIf.sTlf, &mrec->sGeneral);
		break;

	case CFG_IFREC_TIPO_TUN_LOCAL:
	case CFG_IFREC_TIPO_TUN_REMOTO:
		RecursoColateralTPP(p_rec, &mrec->sGeneral.sColateral);
		break;

	case CFG_IFREC_TIPO_QSIG:
	case CFG_IFREC_TIPO_RDSI_2BD:
	case CFG_IFREC_TIPO_RDSI_30BD:
		RecursoColateralTPP(p_rec, &mrec->sGeneral.sColateral);
		RecursoTelefoniaDigital(p_rec, &mrec->uIf.sRdsi);
		break;

	case CFG_IFREC_TIPO_IO:
	case CFG_IFREC_TIPO_DATOS:
	case CFG_IFREC_TIPO_RRC:
		break;
	
	default:
		break;
	}
}

/** */
void CommConversor::RecursoGeneral(CommResConfig *p_rec, struct cfgConfigGeneralRecurso *mgen)
{
	SetInt(&mgen->iSlot, p_rec->SlotPasarela, INCI_MPSW, "SLOT TARJETA");
	SetInt(&mgen->iDispositivo, p_rec->NumDispositivoSlot, INCI_MPSW, "DISPOSITIVO TARJETA");

	SetInt(&mgen->iEnableRegistro, p_rec->enableRegistro, INCI_MPSW, "ENABLE REGISTRO");
	SetString(mgen->szClave, p_rec->szClave, INCI_MPSW, "CLAVE",MAX_LONG_CLAVE);

	SetString(mgen->szUriLocal, p_rec->Uri_Local, INCI_MPSW, "URI-LOCAL",MAX_LONG_DIR_URI);
	SetInt(&mgen->iLLamadaAutomatica, p_rec->LlamadaAutomatica, INCI_MPSW, "LLAMADA AUTOMATICA");
	SetInt(&mgen->iRestriccion, p_rec->restriccion, INCI_MPSW, "RESTRICCION LLAMADA");

	/** Lista Blancas y Negras */
	for (size_t i=0; i<p_rec->blanca.size() && i < MAX_NUM_LISTA_BLANCA; i++)
	{
		memcpy(mgen->szListaBlanca[(int )i], p_rec->blanca[i].c_str(), MAX_LONG_DIR_URI);
	}
	for (size_t i=0; i<p_rec->negra.size() && i < MAX_NUM_LISTA_NEGRA; i++)
	{
		memcpy(mgen->szListaNegra[(int )i], p_rec->negra[i].c_str(), MAX_LONG_DIR_URI);
	}

	/** */
	mgen->iSnmpPuertoRemoto = mcfg->iSnmpPuertoRemoto;
	mgen->iRecPuertoRemoto = mcfg->iRecPuertoRemoto;
	mgen->iRecPuertoBase = 65004;

	/** 20180913. Se utiliza en ULISES para los Display NAME de los recursos PP */
	memcpy(mgen->szDestino, p_rec->szDestino.c_str(), CFG_MAX_LONG_NOMBRE_RECURSO);

	mgen->iFlgUsarDiffServ = p_rec->iFlgUsarDiffServ;

	/** Lista de enlaces externos ??? */
	mgen->size_aListaEnlacesExternos = p_rec->telefonia.listaEnlacesInternos.size();
	for (int iex=0; iex<mgen->size_aListaEnlacesExternos; iex++) 
	{
		memcpy(mgen->aListaEnlacesExternos[iex].szId, p_rec->telefonia.listaEnlacesInternos[iex].c_str(), MAX_ENLACES_EXTERNOS_POR_REC);
	}
#ifdef V110
	for (size_t i = 0; i < p_rec->telefonia.ats_rangos_dst.size() && i < N_MAX_RANGOS_ATS; i++)
	{
#if __POR_REFERENCIA__
		memcpy(mgen->rangos_dst[i].inicial, p_rec->telefonia.ats_rangos_dst[i].inicial.c_str(), LONG_AB_ATS);
		memcpy(mgen->rangos_dst[i].final, p_rec->telefonia.ats_rangos_dst[i].final.c_str(), LONG_AB_ATS);
#else
		memcpy(mgen->rangos_dst[i].inicial, p_rec->telefonia.ats_rangos_dst[i]->inicial.c_str(), LONG_AB_ATS);
		memcpy(mgen->rangos_dst[i].final, p_rec->telefonia.ats_rangos_dst[i]->final.c_str(), LONG_AB_ATS);
#endif
	}
	for (size_t i = 0; i < p_rec->telefonia.ats_rangos_org.size() && i < N_MAX_RANGOS_ATS; i++)
	{
#if __POR_REFERENCIA__
		memcpy(mgen->rangos_org[i].inicial, p_rec->telefonia.ats_rangos_org[i].inicial.c_str(), LONG_AB_ATS);
		memcpy(mgen->rangos_org[i].final, p_rec->telefonia.ats_rangos_org[i].final.c_str(), LONG_AB_ATS);
#else
		memcpy(mgen->rangos_org[i].inicial, p_rec->telefonia.ats_rangos_org[i]->inicial.c_str(), LONG_AB_ATS);
		memcpy(mgen->rangos_org[i].final, p_rec->telefonia.ats_rangos_org[i]->final.c_str(), LONG_AB_ATS);
#endif
	}
	mgen->iEnableNoED137 = p_rec->telefonia.iEnableNoED137;
#endif

}

/** */
void CommConversor::RecursoColateralTPP(CommResConfig *p_rec, struct cfgColateralPP   *mcol, bool spvoptions)
{
	SetInt(&mcol->iRespuestaAutomatica,p_rec->telefonia.r_automatica, INCI_MPSW, "RESPUESTA AUTOMATICA TPP");
#ifndef V110
	SetString(mcol->szUriRemota, p_rec->telefonia.uri_remota, INCI_MPSW, "URI-REMOTA1 TPP", MAX_LONG_DIR_AMPLIADA);
	if (spvoptions == true) {
		SetInt(&mcol->itm_superv_options, p_rec->telefonia.tm_superv_options, INCI_MPSW, "TIEMPO OPTIONS");
		SetInt(&mcol->isuperv_options, p_rec->telefonia.superv_options, INCI_MPSW, "HABILITA OPTIONS1");
	}
	else {
		//p_rec->telefonia.tm_superv_options = 10;
		//p_rec->telefonia.superv_options = 0;
		mcol->itm_superv_options = 10;
		mcol->isuperv_options = 0;
	}
#else
	SetString(mcol->szUriRemota1, p_rec->telefonia.uri_remota, INCI_MPSW, "URI-REMOTA1 TPP", MAX_LONG_DIR_AMPLIADA);
	SetString(mcol->szUriRemota2, p_rec->telefonia.additional_uri_remota, INCI_MPSW, "URI-REMOTA2 TPP", MAX_LONG_DIR_AMPLIADA);
	if (spvoptions == true) {
		SetInt(&mcol->itm_superv_options, p_rec->telefonia.tm_superv_options, INCI_MPSW, "TIEMPO OPTIONS");
		SetInt(&mcol->isuperv_options1, p_rec->telefonia.superv_options, INCI_MPSW, "HABILITA OPTIONS1");
		SetInt(&mcol->isuperv_options2, p_rec->telefonia.additional_superv_options, INCI_MPSW, "HABILITA OPTIONS2");
		SetInt(&mcol->itiporespuesta1, p_rec->telefonia.itiporespuesta, INCI_MPSW, "TIPO RSP-OPTIONS1");
		SetInt(&mcol->itiporespuesta2, p_rec->telefonia.additional_itiporespuesta, INCI_MPSW, "TIPO RSP-OPTIONS1");
	}
	else {
		//p_rec->telefonia.tm_superv_options = 10;
		//p_rec->telefonia.superv_options = 0;
		mcol->itm_superv_options = 10;
		mcol->isuperv_options1 = 0;
		mcol->isuperv_options2 = 0;
		mcol->itiporespuesta1 = 0;
		mcol->itiporespuesta2 = 0;
	}
	
#endif
	SetInt(&mcol->iColateralSCV, p_rec->telefonia.colateral_scv, INCI_MPSW, "TIPO COLATERAL");
}

/** */
void CommConversor::RecursoAudio(CommResConfig *p_rec, struct cfgConfigRecAudio   *maud)
{
	SetInt(&maud->iSelGananciaAgcTx, p_rec->hardware.AD_AGC, INCI_MPSW, "ACG EN TX");
	SetInt(&maud->iGananciaAgcTx, p_rec->hardware.AD_Gain, INCI_MPSW, "GANANCIA ACG EN TX");
	SetInt(&maud->iSelGananciaAgcRx, p_rec->hardware.DA_AGC, INCI_MPSW, "ACG EN RX");
	SetInt(&maud->iGananciaAgcRx, p_rec->hardware.DA_Gain, INCI_MPSW, "GANANCIA ACG EN RX");

	maud->iHaySupresionSilencio = 0;

	SetInt(&maud->iTamanoRtp,p_rec->TamRTP, INCI_MPSW, "LONG. TRAMA RTP");
	SetInt(&maud->iCodecPreferido, p_rec->Codec, INCI_MPSW, "CODEC");

	SetInt(&maud->iBufferJitterMin, p_rec->Buffer_jitter.min, INCI_MPSW, "JITTER MIN");
	SetInt(&maud->iBufferJitterMax,	p_rec->Buffer_jitter.max, INCI_MPSW, "JITTER MAX");
}

/** */
void CommConversor::RecursoRadio(CommResConfig *p_rec, struct cfgConfigGeneralRecurso *mgen, struct cfgConfigIfRadio   *mrad)
{
	SetTipoPtt(&mrad->cTipoPtt, p_rec->radio.ptt);
	SetTipoSqh(&mrad->cTipoSq, p_rec->radio.sq);

	/** 20106407. SQUELCH Forzado.*/
	int forzado = p_rec->radio.sq;
	forzado = forzado==2 ? 1 : 0;
	if (mrad->iNumPaquetesSqOff != forzado) 
	{
		mrad->iNumPaquetesSqOff = forzado;
		eventos.push_back(CommConvertEvent(INCI_MPSW, _srec, "TIPO-SQUELCH", forzado==0 ? "FORZADO ON" : "FORZADO OFF"));
	}

	SetInt(&mrad->iTipoRadio, p_rec->radio.tipo, INCI_MPSW, "TIPO RADIO");
	SetInt(&mrad->iUmbralDetSq, p_rec->radio.umbralVad, INCI_MPSW, "UMBRAL SQUELCH");
	SetInt(&mrad->iMetodoBss, p_rec->radio.metodoBss, INCI_MPSW, "METODO BSS");
	mrad->iNtz = 0;
	SetInt(&mrad->TiempoMaxPtt, p_rec->radio.tiempoPtt, INCI_MPSW, "TIEMPO MAXIMO PTT");
	SetInt(&mrad->iModoConfirmacionPtt, p_rec->radio.modoConfPtt, INCI_MPSW, "MODO CONFIRMAR PTT");
	SetInt(&mrad->iPeriodoRtpSqBss, p_rec->radio.repSqBss, INCI_MPSW, "RepSQyBSS");

	mrad->iSupervisionPortadoraTx = p_rec->radio.SupervPortadoraTx;
	mrad->iSupervisionModuladoraTx = p_rec->radio.SupervModuladoraTx;
	mrad->iNivelTonoPtt = p_rec->radio.UmbralTonoPTT;

	// 20160804. Este campo se utiliza para el SQH forzado...
	// SetInt(&mrad->iNumPaquetesSqOff, jradio, "desactivacionSq", INCI_MPSW, "PAQUETES SQ-OFF");
	// 20190226. El valor depende si estamos en ULISES (tipo==1) o en REDAN (tipo==0)
	if (p_cfg_in->tipo == 0) 
		mrad->iPttTimeOut = 0;
	else 
		SetInt(&mrad->iPttTimeOut, p_rec->radio.timeoutPtt, INCI_MPSW, "TIMEOUT PTT");

	SetInt(&mrad->iNumFlujosMezcla, p_rec->radio.numFlujosAudio, INCI_MPSW, "NUMERO DE FLUJOS EN MEZCLA");

	mrad->iEyM = 0;
	mrad->iTiempoPttBloqueado = 2000;
	mrad->iRetrasoPttOff = 0;

	SetInt((int *)&mrad->KeepAlivePeriod, p_cfg_in->servicios.sip.KeepAlivePeriod, INCI_MPSW, "KeepAlivePeriod");
	SetInt((int *)&mrad->KeepAliveMultiplier,p_cfg_in->servicios.sip.KeepAliveMultiplier, INCI_MPSW, "KeepAliveMultiplier");

	/** 20170191. */
	switch(mrad->iTipoRadio) {
		case TIPO_RD_LOCAL_FD_SIMPLE:
		case TIPO_RD_LOCAL_FD_PR:
			if (p_rec->radio.bss == 1)
				SetInt((int *)&mrad->iClimaxDelay, p_rec->radio.climaxDelay, INCI_MPSW, "CLIMAX");
			else
				mrad->iClimaxDelay = 0;
			SetInt(&mrad->iRetrasoSqOff, p_rec->radio.retrasoSqOff, INCI_MPSW, "COLA SQUELCH OFF");
			SetInt((int *)&mrad->iTiempo_VentanaRx, p_rec->radio.tmVentanaRx, INCI_MPSW, "VENTANA ANALISIS BSS");
			SetInt((int *)&mrad->iTmRetardoFijo, p_rec->radio.tmRetardoFijo, INCI_MPSW, "RETRASO FIJO CLIMAX");
			break;
		default:
			mrad->iClimaxDelay = 0;
			mrad->iRetrasoSqOff= 0;
			mrad->iTiempo_VentanaRx = 0;
			mrad->iTmRetardoFijo = 0;
			break;
	}

	switch(mrad->iTipoRadio) {
	case TIPO_RD_REMOTO_RX:
		mgen->iTipo = CFG_REC_TIPO_AUDIO_RX;
		break;
	case TIPO_RD_REMOTO_TX:
		mgen->iTipo = CFG_REC_TIPO_AUDIO_TX;
		break;
	default:
	//case TIPO_RD_LOCAL_SIMPLE:
	//case TIPO_RD_LOCAL_PR:
	//case TIPO_RD_LOCAL_FD_SIMPLE:
	//case TIPO_RD_LOCAL_FD_PR:
	//case TIPO_RD_REMOTO_RXTX:
		mgen->iTipo = CFG_REC_TIPO_AUDIO_TXRX;
		break;
	}
	/*****************************************/

	SetInt((int *)&mrad->iBssRtp, p_rec->radio.bssRtp, INCI_MPSW, "BSS EN RTP");
	SetInt((int *)&mrad->iEvtPTT, p_rec->radio.evtPTT, INCI_MPSW, "GENERACION HISTORICO PTT-SQH");
	SetInt((int *)&mrad->iTjbd, p_rec->radio.tjbd, INCI_MPSW, "JITTER BUFFER DELAY");
	SetInt((int *)&mrad->iTGRSid, p_rec->radio.tGRSid, INCI_MPSW, "GRS INTERNAL DELAY");

	/** 20160830. Precision de Audio.. */
	SetInt((int *)&mrad->iPrecisionAudio, p_rec->radio.iPrecisionAudio, INCI_MPSW, "PRECISION DE AUDIO");
	/** 20200508. Precision de Audio No Estricta para Version de 16 Canales Radio / Remoto */
	if (force_rdaudio_normal == 1) {
		mrad->iPrecisionAudio = PRECISION_NORMAL;
	}

	SetInt((int *)&mgen->iEnableGI, p_rec->radio.iEnableGI, INCI_MPSW, "IP-REC ENABLE");
	/** PTT y Sesiones Prioritarias */
	SetInt((int *)&mrad->iPttPrio, p_rec->radio.iPttPrio, INCI_MPSW, "PTT-PRIO", CommConversor::RadioPttPrioJ2M);
	SetInt((int *)&mrad->iSessionPrio, p_rec->radio.iSesionPrio, INCI_MPSW, "SESSION-PRIO", CommConversor::RadioSessionPrioJ2M);

	SetTipoConmutacionRadio(&mrad->cTipoConmutacion, p_rec->radio.colateral.tipoConmutacion);
	SetString(mgen->szDestino, p_rec->radio.colateral.name, INCI_MPSW, "FID", CFG_MAX_LONG_NOMBRE_RECURSO);
	ColateralesRadio(&p_rec->radio.colateral, &mrad->sColateral);

	/** Indices de Calidad */
	for (size_t icad = 0; icad < BSSC_QIDXVAL; icad++) {
		if (icad < p_rec->radio.tabla_indices_calidad.size()) {
			mrad->qidxNdf2Rssi[icad] = (char )p_rec->radio.tabla_indices_calidad[icad];
		}
		else  {
			/** 20210609 RM4849. Cuando no hay tabla de calidad, los valores deben estar a '0' en vez de a 15 */
			mrad->qidxNdf2Rssi[icad] = (char )0x00;
		}
	}

	/** 20170130. Modo de Calculo Climax */
	SetInt((int *)&mrad->iModoCalculoClimax, p_rec->radio.iModoCalculoClimax, INCI_MPSW, "PRECISION DE AUDIO");
}

/** */
void CommConversor::ColateralesRadio(CommResRadioColateral *p_col, struct cfgColateralRadio *mcol)
{
	for (size_t emp=0; emp<p_col->emplazamientos.size() && emp<MAX_NUM_EMPL; emp++) 
	{		
		/** No genero historico de esto */
#if __POR_REFERENCIA__
		memcpy(mcol->asEmpl[emp].UriTxA, p_col->emplazamientos[emp].uriTxA.c_str(),MAX_LONG_DIR_URI);
		memcpy(mcol->asEmpl[emp].UriTxB, p_col->emplazamientos[emp].uriTxB.c_str(),MAX_LONG_DIR_URI);
		mcol->asEmpl[emp].cActivoRx = p_col->emplazamientos[emp].activoTx==0 ? 'A' : 'B';

		memcpy(mcol->asEmpl[emp].UriRxA, p_col->emplazamientos[emp].uriRxA.c_str(),MAX_LONG_DIR_URI);
		memcpy(mcol->asEmpl[emp].UriRxB, p_col->emplazamientos[emp].uriRxB.c_str(),MAX_LONG_DIR_URI);
		mcol->asEmpl[emp].cActivoRx = p_col->emplazamientos[emp].activoRx==0 ? 'A' : 'B';
#else
		memcpy(mcol->asEmpl[emp].UriTxA, p_col->emplazamientos[emp]->uriTxA.c_str(),MAX_LONG_DIR_URI);
		memcpy(mcol->asEmpl[emp].UriTxB, p_col->emplazamientos[emp]->uriTxB.c_str(),MAX_LONG_DIR_URI);
		mcol->asEmpl[emp].cActivoRx = p_col->emplazamientos[emp]->activoTx==0 ? 'A' : 'B';

		memcpy(mcol->asEmpl[emp].UriRxA, p_col->emplazamientos[emp]->uriRxA.c_str(),MAX_LONG_DIR_URI);
		memcpy(mcol->asEmpl[emp].UriRxB, p_col->emplazamientos[emp]->uriRxB.c_str(),MAX_LONG_DIR_URI);
		mcol->asEmpl[emp].cActivoRx = p_col->emplazamientos[emp]->activoRx==0 ? 'A' : 'B';
#endif
	}
}

/** */
void CommConversor::RecursoLcen(CommResConfig *p_rec, struct cfgConfigIfLcen   *mlce, struct cfgConfigGeneralRecurso *mgen)
{
	mlce->iUmbralRxTonoLcen = -35;
	mlce->iNivelTxTonoLcen = -18;
	mlce->iPeriodoRefrescoEstado = 200;
	mlce->iTimeoutEstado = 600;
	mlce->iLongRafagas = 6;
	mlce->iT1 = 20;
	mlce->iT1Max = 100;
	mlce->iT2 = 20;
	mlce->iT2Max = 100;
	mlce->iT3 = 40;
	mlce->iT4 = 40;
	mlce->iT4Max = 160;
	mlce->iT5 = 60;
	mlce->iT5Max = 100;
	mlce->iT6 = 	5000;
	mlce->iT6Max = 6000;
	mlce->iT8 = 80;
	mlce->iT8Max = 150;
	mlce->iT9 = 40;
	mlce->iT9Max = 60;
	mlce->iT10= 20;
	mlce->iT10Max = 130;
	mlce->iT11 = 20;
	mlce->iT11Max = 130;
	mlce->iT12 = 200;

	SetInt(&mlce->iT_Release, p_rec->telefonia.it_release, INCI_MPSW, "TIEMPO TONOS RELEASE");

	/* 20170119. */
	mgen->iRestriccion = 0;
	// mgen->iEnableRegistro = 0; 
	/*************/
}

/** */
void CommConversor::RecursoTelefoniaR2N5(CommResConfig *p_rec, struct cfgConfigIfR2N5   *mr2n5, struct cfgConfigGeneralRecurso *mgen)
{
	mr2n5->sTiemA.iP1Min = 0;
	mr2n5->sTiemA.iP1Nom = 0;
	mr2n5->sTiemA.iP1Max = 0;
	mr2n5->sTiemA.iP2 = 0;
	mr2n5->sTiemA.iP2Tout = 0;
	mr2n5->sTiemA.iP3 = 0;
	mr2n5->sTiemA.iP3Tout = 0;
	mr2n5->sTiemA.iP4 = 0;
	mr2n5->sTiemA.iP4Tout = 0;
	mr2n5->sTiemA.iP5 = 0;
	mr2n5->sTiemA.iP5Tout = 0;
	mr2n5->sTiemA.iP6 = 0;
	mr2n5->sTiemA.iP6Tout = 0;
	mr2n5->sTiemA.iP7 = 0;
	mr2n5->sTiemA.iP7Tout = 0;
	mr2n5->sTiemA.iP8 = 0;
	mr2n5->sTiemA.iP8Tout = 0;
	mr2n5->sTiemA.iP9Min = 0;
	mr2n5->sTiemA.iP9Nom = 0;
	mr2n5->sTiemA.iP9Max = 0;
	mr2n5->sTiemA.iP11 = 0;
	mr2n5->sTiemA.iP16Min = 0;
	mr2n5->sTiemA.iP16Nom = 0;
	mr2n5->sTiemA.iP16Max = 0;
	mr2n5->sTiemA.iP18 = 0;

	mr2n5->sTiemB.iP1Min = 0;
	mr2n5->sTiemB.iP1Nom = 0;
	mr2n5->sTiemB.iP1Max = 0;
	mr2n5->sTiemB.iP2 = 0;
	mr2n5->sTiemB.iP2Tout = 0;
	mr2n5->sTiemB.iP3 = 0;
	mr2n5->sTiemB.iP3Tout = 0;
	mr2n5->sTiemB.iP4 = 0;
	mr2n5->sTiemB.iP4Tout = 0;
	mr2n5->sTiemB.iP5 = 0;
	mr2n5->sTiemB.iP5Tout = 0;
	mr2n5->sTiemB.iP6 = 0;
	mr2n5->sTiemB.iP6Tout = 0;
	mr2n5->sTiemB.iP7 = 0;
	mr2n5->sTiemB.iP7Tout = 0;
	mr2n5->sTiemB.iP8 = 0;
	mr2n5->sTiemB.iP8Tout = 0;
	mr2n5->sTiemB.iP9Min = 0;
	mr2n5->sTiemB.iP9Nom = 0;
	mr2n5->sTiemB.iP9Max = 0;
	mr2n5->sTiemB.iP11 = 0;
	mr2n5->sTiemB.iP16Min = 0;
	mr2n5->sTiemB.iP16Nom = 0;
	mr2n5->sTiemB.iP16Max = 0;
	mr2n5->sTiemB.iP18 = 0;

	//strcpy(mr2n5->szIdTroncal,"");
	//strcpy(mr2n5->szIdRed, "");

	SetInt(&mr2n5->iLado, p_rec->telefonia.lado, INCI_MPSW, "LADOR2-N5");
	SetInt(&mr2n5->iT_Release, p_rec->telefonia.it_release, INCI_MPSW, "TIEMPO TONOS RELEASE");

	memcpy(mr2n5->ntest_local, p_rec->telefonia.no_test_local.c_str(),LONG_AB_ATS);
	memcpy(mr2n5->ntest_remoto, p_rec->telefonia.no_test_remoto.c_str(),LONG_AB_ATS);

	/** 20160830. Periodo de Interrupt Warning... */
	/** 20210212. El Byte Bajo es Periodo. El Byte alto es el 'P6-P22' Modo Normal o Transito */
	SetInt((int *)&mr2n5->iT_Int_Warning, p_rec->telefonia.iT_Int_Warning, INCI_MPSW, "PERIODO INTERRUPT WARNING");

#ifndef V110


	/** */
	for (size_t i=0; i<p_rec->telefonia.ats_rangos_dst.size() && i<N_MAX_RANGOS_ATS; i++)
	{
#if __POR_REFERENCIA__
		memcpy(mr2n5->rangos_dst[i].inicial, p_rec->telefonia.ats_rangos_dst[i].inicial.c_str(),LONG_AB_ATS);
		memcpy(mr2n5->rangos_dst[i].final, p_rec->telefonia.ats_rangos_dst[i].final.c_str(),LONG_AB_ATS);
#else
		memcpy(mr2n5->rangos_dst[i].inicial, p_rec->telefonia.ats_rangos_dst[i]->inicial.c_str(),LONG_AB_ATS);
		memcpy(mr2n5->rangos_dst[i].final, p_rec->telefonia.ats_rangos_dst[i]->final.c_str(),LONG_AB_ATS);
#endif
	}
	for (size_t i=0; i<p_rec->telefonia.ats_rangos_org.size() && i<N_MAX_RANGOS_ATS; i++)
	{
#if __POR_REFERENCIA__
		memcpy(mr2n5->rangos_org[i].inicial, p_rec->telefonia.ats_rangos_org[i].inicial.c_str(),LONG_AB_ATS);
		memcpy(mr2n5->rangos_org[i].final, p_rec->telefonia.ats_rangos_org[i].final.c_str(),LONG_AB_ATS);
#else
		memcpy(mr2n5->rangos_org[i].inicial, p_rec->telefonia.ats_rangos_org[i]->inicial.c_str(),LONG_AB_ATS);
		memcpy(mr2n5->rangos_org[i].final, p_rec->telefonia.ats_rangos_org[i]->final.c_str(),LONG_AB_ATS);
#endif
	}
#endif // !V110

	/** */
	memcpy(mr2n5->szIdRed, p_rec->telefonia.idRed.c_str(), CFG_MAX_LONG_NOMBRE_RED);
	memcpy(mr2n5->szIdTroncal, p_rec->telefonia.idTroncal.c_str(), CFG_MAX_LONG_NOMBRE_TRONCAL);

	/** */
	/*
    int iNumRangosOperador;
    int iNumRangosPrivilegiados;
    struct st_rango_binario asMisRangosOperador[N_MAX_RANGOS],
    struct st_rango_binario asMisRangosPrivilegiados[N_MAX_PRIV];
    int iNumRangosDirectosOpe;
    int iNumRangosDirectosPriv;
    struct st_rango_binario asRangosDirectosOpe[N_MAX_RANGOS];
    struct st_rango_binario asRangosDirectosPriv[N_MAX_PRIV];
	*/
	mr2n5->iNumRangosOperador = p_rec->telefonia.ats_rangos_operador.size();
	for (int i = 0; i< mr2n5->iNumRangosOperador; i++)
	{
		mr2n5->asMisRangosOperador[i].lwFinal = (LongWord )atoi(p_rec->telefonia.ats_rangos_operador[i].final.c_str());
		mr2n5->asMisRangosOperador[i].lwInicial = (LongWord )atoi(p_rec->telefonia.ats_rangos_operador[i].inicial.c_str());
	}

	mr2n5->iNumRangosPrivilegiados = p_rec->telefonia.ats_rangos_privilegiados.size();
	for (int i=0; i<mr2n5->iNumRangosPrivilegiados; i++)
	{
		mr2n5->asMisRangosPrivilegiados[i].lwFinal = (LongWord)atoi(p_rec->telefonia.ats_rangos_privilegiados[i].final.c_str());
		mr2n5->asMisRangosPrivilegiados[i].lwInicial = (LongWord)atoi(p_rec->telefonia.ats_rangos_privilegiados[i].inicial.c_str());
	}

	mr2n5->iNumRangosDirectosOpe = p_rec->telefonia.ats_rangos_directos_ope.size();
	for (int i=0; i<mr2n5->iNumRangosDirectosOpe; i++)
	{
		mr2n5->asRangosDirectosOpe[i].lwFinal = (LongWord)atoi(p_rec->telefonia.ats_rangos_directos_ope[i].final.c_str());
		mr2n5->asRangosDirectosOpe[i].lwInicial = (LongWord)atoi(p_rec->telefonia.ats_rangos_directos_ope[i].inicial.c_str());
	}

	mr2n5->iNumRangosDirectosPriv = p_rec->telefonia.ats_rangos_directos_pri.size();
	for (int i=0; i<mr2n5->iNumRangosDirectosPriv; i++)
	{
		mr2n5->asRangosDirectosPriv[i].lwFinal = (LongWord)atoi(p_rec->telefonia.ats_rangos_directos_pri[i].final.c_str());
		mr2n5->asRangosDirectosPriv[i].lwInicial = (LongWord)atoi(p_rec->telefonia.ats_rangos_directos_pri[i].inicial.c_str());
	}

	/* 20170119. */
	mgen->iRestriccion = 0;
	// mgen->iEnableRegistro = 0; 
	/*************/
}

/** */
void CommConversor::RecursoTelefoniaAnalogica(CommResConfig *p_rec, struct cfgConfigIfTlf   *mtlf, struct cfgConfigGeneralRecurso *mgen)
{
	mtlf->iPatronRing = 	0;
	mtlf->iPeriodoRing = 50;
	mtlf->iUmbralAltoPeriodoRing = 0x0E;
	mtlf->iUmbralBajoPeriodoRing = 0x3F;
	mtlf->iVRing = 71300;
	mtlf->iVBat = 50000;


	SetInt((int *)(&mtlf->eTypeEyM), p_rec->telefonia.t_eym, INCI_MPSW, "TIPO DE EYM");
	SetInt((int *)(&mtlf->iModoEyM), p_rec->telefonia.h2h4, INCI_MPSW, "MODO DE EYM");
	SetInt((int *)(&mtlf->iLadoEyM), p_rec->telefonia.ladoeym, INCI_MPSW, "LADO DE EYM");

	// 20161213. Este valor siempre debe valer 1.
	// SetInt((int *)(&mtlf->iDetectVox), p_rec->telefonia.detect_vox, INCI_MPSW, "DETECCION VOX");
	// mtlf->iDetectVox = 1;
	///////////////////////////////////////////////////////////////////////////////////////////////
	// 20170215. El valor depende si estamos en ULISES (tipo==1) o en REDAN (tipo==0)
	// mtlf->iDetectVox = p_cfg_in->tipo==0 ? 1 : 0;
	// 20210301. Para REDAN se habilita la edición, y se deshabilita la opción para ULISES.
	if (p_cfg_in->tipo == 0) {
		SetInt((int*)(&mtlf->iDetectVox), p_rec->telefonia.detect_vox, INCI_MPSW, "DETECCION VOX");
	}
	else {
		mtlf->iDetectVox = 0;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////

	SetInt((int *)(&mtlf->iUmbralVox), p_rec->telefonia.umbral_vox, INCI_MPSW, "UMBRAL VOX");
	SetInt((int *)(&mtlf->iTmInactividad), p_rec->telefonia.tm_inactividad, INCI_MPSW, "TIEMPO INACTIVIDAD");

	/** */
	memcpy(mtlf->szIdRed, p_rec->telefonia.idRed.c_str(), CFG_MAX_LONG_NOMBRE_RED);

	// 20180320. Nuevos Parametros...
	// mtlf->iTmLlamEntrante = p_rec->telefonia.iTmLlamEntrante == -1 ? 30 : p_rec->telefonia.iTmLlamEntrante;
	// mtlf->iTmDetFinLlamada= p_rec->telefonia.iTmDetFinLlamada == -1 ? 6 : p_rec->telefonia.iTmDetFinLlamada;

	/** 20181016. U2510. SP#01-15*/
	mtlf->TReleaseBL = 3;
	mtlf->iDetCallerId = 0;
	mtlf->iTmCallerId = 3000;
	mtlf->iDetInversionPol = 0;
	mtlf->iTmLlamEntrante = 30;
	mtlf->iTmDetFinLlamada = 6;
	mtlf->iPeriodoSpvRing = 200;
	mtlf->iFiltroSpvRing = 2;
	mtlf->iDetDtmf = 0;

	/* 20170119. */
	switch ( mgen->iTipoIf)
	{
	case CFG_IFREC_TIPO_BL:
		mtlf->iModoEyM = 0;
		mtlf->TReleaseBL = p_rec->telefonia.TReleaseBL;
		mtlf->iTmLlamEntrante = p_rec->telefonia.iTmLlamEntrante;
		mtlf->iPeriodoSpvRing = p_rec->telefonia.iPeriodoSpvRing;
		mtlf->iFiltroSpvRing = p_rec->telefonia.iFiltroSpvRing;
		break;
	case CFG_IFREC_TIPO_BC:
		mtlf->iDetectVox = 0;
		mtlf->iTmLlamEntrante = p_rec->telefonia.iTmLlamEntrante;
		mtlf->iDetDtmf = p_rec->telefonia.iDetDtmf;
		break;
	case CFG_IFREC_TIPO_AB:
		mtlf->iDetectVox = 0;
		mtlf->iDetCallerId = p_rec->telefonia.iDetCallerId;
		mtlf->iTmCallerId = p_rec->telefonia.iTmCallerId;
		mtlf->iDetInversionPol = p_rec->telefonia.iDetInversionPol;
		mtlf->iTmLlamEntrante = p_rec->telefonia.iTmLlamEntrante;
		mtlf->iTmDetFinLlamada = p_rec->telefonia.iTmDetFinLlamada;
		mtlf->iPeriodoSpvRing = p_rec->telefonia.iPeriodoSpvRing;
		mtlf->iFiltroSpvRing = p_rec->telefonia.iFiltroSpvRing;
		break;

	default:
		break;
	}
	mgen->iRestriccion=0;
	// mgen->iEnableRegistro = 0;
	/*************/
#ifdef V110
	mtlf->iDetLineaAB = p_rec->telefonia.iDetLineaAB;
#endif // V110


}

/** */
void CommConversor::RecursoTelefoniaDigital(CommResConfig *p_rec, struct cfgConfigIfRdsi  *mdig)
{
	SetInt((int *)(&mdig->iLado), p_rec->telefonia.lado, INCI_MPSW, "LADO");
	SetInt((int *)(&mdig->iModo), p_rec->telefonia.modo, INCI_MPSW, "MODO");
	//strcpy(mdig->szIdRed,"");
	//strcpy(mdig->szIdTroncal, "");
}

/** Busca los recursos borrados y genera el historico correspondiente */
void CommConversor::RecursosBorrados()
{
	int jnrec = p_cfg_in->recursos.size();		// jrecs.Size();

	for (int irec=0; irec<mcfg->iNumRecursos; irec++)
	{
		bool borrado = true;

		int grec = mcfg->asLocRec[irec].iSlot*4 + mcfg->asLocRec[irec].iDispositivo;

		for (int jirec=0; jirec<jnrec; jirec++)
		{
			//Value &jrec = jrecs[jirec];
#if __POR_REFERENCIA__
			int jgrec = p_cfg_in->recursos[jirec].SlotPasarela*4 + p_cfg_in->recursos[jirec].NumDispositivoSlot;
#else
			int jgrec = p_cfg_in->recursos[jirec]->SlotPasarela*4 + p_cfg_in->recursos[jirec]->NumDispositivoSlot;
#endif
			if (jgrec == grec)
			{
				borrado = false;
				break;
			}						
		}
		if (borrado)
		{
			int evento = mcfg->sRecurso[grec].sGeneral.iTipoIf == CFG_IFREC_TIPO_RADIO ? INCI_BRCR : INCI_BRCT;
			eventos.push_back(CommConvertEvent(evento, mcfg->sRecurso[grec].sGeneral.szNombre,"En Posicion ", 
				Tools::Int2String(mcfg->asLocRec[irec].iSlot*4 )+"-"+Tools::Int2String(mcfg->asLocRec[irec].iDispositivo)));
		}
	}
}

/** Busca los recursos A�adidos y general el historico correspondiente */
void CommConversor::RecursosAnadidos()
{
	int jnrec = p_cfg_in->recursos.size();		// jrecs.Size();
	recadd.clear();

	for (int jirec=0; jirec<jnrec; jirec++)
	{
		//Value &jrec = jrecs[jirec];
#if __POR_REFERENCIA__
		int jgrec = p_cfg_in->recursos[jirec].SlotPasarela*4 + p_cfg_in->recursos[jirec].NumDispositivoSlot;
#else
		int jgrec = p_cfg_in->recursos[jirec]->SlotPasarela*4 + p_cfg_in->recursos[jirec]->NumDispositivoSlot;
#endif
		bool anadido = true;
		for (int irec=0; irec<mcfg->iNumRecursos; irec++)
		{
			int grec = mcfg->asLocRec[irec].iSlot*4 + mcfg->asLocRec[irec].iDispositivo;
			if (grec==jgrec) 
			{
				anadido = false;
				break;
			}
		}
		if (anadido)
		{
#if __POR_REFERENCIA__
			string srec = p_cfg_in->recursos[jirec].IdRecurso;
			int tipo = p_cfg_in->recursos[jirec].Radio_o_Telefonia;
#else
			string srec = p_cfg_in->recursos[jirec]->IdRecurso;
			int tipo = p_cfg_in->recursos[jirec]->Radio_o_Telefonia;
#endif
			int evento = tipo==1 ? INCI_ARCR : INCI_ARCT;
			eventos.push_back(CommConvertEvent(evento, srec,"En Posicion ",
				Tools::Int2String(jgrec/4)+"-"+Tools::Int2String(jgrec%4)));
			recadd.push_back(jgrec);
		}
	}
}

/** */
bool CommConversor::IsAdd(int grec)		// Mira si el Recurso ha sido A�adido...
{
	return std::find(recadd.begin(), recadd.end(), grec)!=recadd.end();
}

/** */
void CommConversor::ActualizaSnmpIni()
{
	LocalConfig snmpini(onfs(LocalConfig::p_cfg->get(strModulos, strItemModuloSnmp)/*.snmpModule()*/));

	SetInt(snmpini, "AGENTE", "PORT", p_cfg_in->servicios.snmp.snmpp, INCI_MPGP, "SNMP PUERTO ESCUCHA");
	SetString(snmpini, "AGENTE", "NAME", p_cfg_in->servicios.snmp.agname, INCI_MPGP, "SNMP-NAME");
	SetString(snmpini, "AGENTE", "LOCATION", p_cfg_in->servicios.snmp.agloc, INCI_MPGP, "SNMP-LOCATION");
	SetString(snmpini, "AGENTE", "CONTACT", p_cfg_in->servicios.snmp.agcont, INCI_MPGP, "SNMP-CONTACT");
	SetString(snmpini, "AGENTE", "SNMPV2COMM", p_cfg_in->servicios.snmp.agcomm, INCI_MPGP, "SNMPV2-COMMUNITY");
	SetInt(snmpini, "AGENTE", "SNMPV2", p_cfg_in->servicios.snmp.agv2, INCI_MPGP, "HABILITA SNMPV2");

	int ikey=0;
	snmpini.del("TRAPS");
	for (vector<string>::iterator itr = p_cfg_in->servicios.snmp.traps.begin(); itr != p_cfg_in->servicios.snmp.traps.end(); ++itr,ikey++)
	{
		string leido = *itr;
		if (leido==string(""))
				continue;
		char strkey[32];
		sprintf(strkey,"DST%02d",ikey);
		snmpini.set("TRAPS", strkey, leido.c_str());
	}

	snmpini.save();
}

/** */
void CommConversor::ActualizaRecordIni()
{
	LocalConfig recini(onfs(LocalConfig::p_cfg->get(strModulos, strItemModuloGrabador)/*.recModule()*/));

	/** 20160721. Solo son configurables la IP y Puerto de Grabador */
	//setString(recini, "RTSP", "IP", p_cfg_in->servicios.grab.rtsp_ip, INCI_MPGP, "GRABADOR IP");
	//setString(recini, "RTSP", "URI", p_cfg_in->servicios.grab.rtsp_uri, INCI_MPGP, "GRABADOR URI");
	//setInt(recini, "RTSP", "PORT", p_cfg_in->servicios.grab.rtsp_port, INCI_MPGP, "GRABADOR PUERTO RTSP");

	//setInt(recini, "RTSP", "RTP", p_cfg_in->servicios.grab.rtsp_rtp, INCI_MPGP, "GRABADOR GENERA RTP");
	//setInt(recini, "RTP", "SAMPLE_RATE", p_cfg_in->servicios.grab.rtp_sr, INCI_MPGP, "GRABADOR MUESTREO RTP");
	//setInt(recini, "RTP", "PAYLOAD_FORMAT", p_cfg_in->servicios.grab.rtp_pl, INCI_MPGP, "GRABADOR PAYLOAD RTP");
	
	int numRecorder = 0;
	if (Tools::ValidateIpAddress(p_cfg_in->servicios.grab.rtsp_ip)==true) {
		SetString(recini, "RTSP", "IP_REC_A", p_cfg_in->servicios.grab.rtsp_ip, INCI_MPGP, "IP GRABADOR A");
		numRecorder += 1;
	}
	else {
		recini.set("RTSP", "IP_REC_A", "");
	}
	if (Tools::ValidateIpAddress(p_cfg_in->servicios.grab.rtspb_ip)==true) {
		SetString(recini, "RTSP", "IP_REC_B", p_cfg_in->servicios.grab.rtspb_ip, INCI_MPGP, "IP GRABADOR B");
		numRecorder += 1;
	}
	else {
		recini.set("RTSP", "IP_REC_B", "");
	}
	recini.set("GENERAL", "DUAL_RECORDER", numRecorder==2 ? "1" : "0");

	SetInt(recini, "RTSP", "PORT_RTSP", p_cfg_in->servicios.grab.rtsp_port, INCI_MPGP, "GRABADOR PUERTO RTSP");
	recini.save();
}

/** */
void CommConversor::ActualizaWebIni()
{
	LocalConfig &webini = *LocalConfig::p_cfg;

	SetString(webini,"U5KWEB","CONFIG-SERVER", p_cfg_in->general.ips,INCI_MPGP,"URL-SERVIDOR", true);		// Permite string vacios...
	SetInt(webini, "U5KWEB", "SESSION-TIME", p_cfg_in->servicios.web.stime, INCI_MPGP, "TIEMPO SESION");
	if (p_cfg_in->servicios.web.wport != 0)
		SetInt(webini, "U5KWEB", "WEB-PORT",p_cfg_in->servicios.web.wport, INCI_MPGP, "PUERTO DE ESCUCHA");
	webini.save();
}

/*******************************************************************************************************************/
/** */
void CommConversor::TablasUlises(CommUv5Config *p_tbs)
{
	TablaUlises(p_tbs->plannumeracionats, p_tbs->plandireccionamientoip, mcfg->plannumeracionats);
	TablaUlises(p_tbs->plandireccionamientoip, mcfg->plandireccionamientoip);
	TablaUlises(p_tbs->plandireccionamientoip, mcfg->plandireccionamientoproxy);
	TablaUlises(p_tbs->plantroncales, mcfg->plantroncales);
	TablaUlises(p_tbs->planredes, mcfg->planredes);
	TablaUlises(p_tbs->planasignacionusuarios, mcfg->planasignacionusuarios);
	TablaUlises(p_tbs->planasignacionrecursos, mcfg->planasignacionrecursos);
	TablaUlises(p_tbs->plandireccionamientosip, mcfg->plandireccionamientosip);
}

/** */
void CommConversor::TablaUlises(vector<CommUlises_st_numeracionats> &plan,
						vector<CommUlises_st_direccionamientoip> &planip, 
						struct st_numeracionats *p_plan)
{
	/** Plan numeracion ATS */
	size_t ats;
	for (ats=0; ats<plan.size(); ats++)
	{
		if (ats >= (N_MAX_TV+N_MAX_TIFX)) {
			PLOG_ERROR("CommConversor::TablasUlises: Demasiados elementos en el Plan de numeracion ATS");
			break;
		}
		plan[ats].NameSet(planip);
		plan[ats].copyto(&p_plan[ats]);
	}
	if (ats < (N_MAX_TV+N_MAX_TIFX))
		p_plan[ats].centralpropia = CENTRAL_NO_DEF;
}

/** */
void CommConversor::TablaUlises(vector<CommUlises_st_direccionamientoip> &plan, struct st_direccionamientoip *p_plan)
{
	/** Plan direccionamiento IP */
	size_t ip;
	for (ip=0; ip<plan.size(); ip++)
	{
		if (ip >= (N_MAX_TV+N_MAX_TIFX)) {
			PLOG_ERROR("CommConversor::TablasUlises: Demasiados elementos en el Plan de DIRECCIONAMIENTO IP");
			break;
		}

		plan[ip].copyto(&p_plan[ip]);
	}
	if (ip < (N_MAX_TV+N_MAX_TIFX))
		p_plan[ip].idhost[0]=NO_NAME;
}

/** */
void CommConversor::TablaUlises(vector<CommUlises_st_direccionamientoip> &plan, struct st_direccionamientoproxy *p_plan)
{
	/** Plan direccionamiento IP-PROXY */
	size_t ip,icen=0;
	vector<string> centrales;
	for (ip=0; ip<plan.size(); ip++)
	{
		if (plan[ip].centralip == true)
		{
			if (plan[ip].interno==true) 
			{
			/** 20180214. Chequeo la central propia y relleno los proxies. */
				strncpy(mcfg->szDirSipProxy[0], plan[ip].ipred1.c_str(), MAX_LONG_DIRIP);
				strncpy(mcfg->szDirSipProxy[1], plan[ip].ipred2.c_str(), MAX_LONG_DIRIP);
				strncpy(mcfg->szDirSipProxy[2], plan[ip].ipred3.c_str(), MAX_LONG_DIRIP);
			}

			if (icen >= (N_MAX_CENTRALES)) {
				PLOG_ERROR("CommConversor::TablasUlises: Demasiados elementos en el Plan de DIRECCIONAMIENTO PROXY");
				break;
			}
			/** Chequear que no haya centrales repetidas.*/
			if (std::find(centrales.begin(), centrales.end(), plan[ip].idhost) == centrales.end())
			{
				plan[ip].copyto(&p_plan[icen]);
				centrales.push_back(plan[ip].idhost);
				icen++;
			}
		}
	}

	if (icen < (N_MAX_CENTRALES))
		p_plan[icen].idhost[0]=NO_NAME;
}


/** */
void CommConversor::TablaUlises(vector<CommUlises_st_listatroncales> &plan, struct st_listatroncales *p_plan)
{
	/** Plan de troncales */
	size_t tr;
	for (tr=0; tr<plan.size(); tr++)
	{
		if (tr >= N_MAX_TRONCALES) {
			PLOG_ERROR("CommConversor::TablasUlises: Demasiados elementos en el Plan de TRONCALES");
			break;
		}
		plan[tr].copyto(&p_plan[tr]);
	}
	if (tr < N_MAX_TRONCALES)
		p_plan[tr].idtroncal[0]=0;
}

/** */
void CommConversor::TablaUlises(vector<CommUlises_st_listaredes> &plan, struct st_listaredes *p_plan)
{
	/** Plan de redes */
	size_t red;
	for (red=0; red<plan.size(); red++)
	{
		if (red >= N_MAX_REDES) {
			PLOG_ERROR("CommConversor::TablasUlises: Demasiados elementos en el Plan de REDES");
			break;
		}
		plan[red].copyto(&p_plan[red]);
	}
	if (red < N_MAX_REDES)
		p_plan[red].prefijo=255;
}

/** */
void CommConversor::TablaUlises(vector<CommUlises_st_asignacionusuario_tv> &plan, struct st_asignacionusuario_tv *p_plan)
{
	/** Plan de asignacion de Usuarios TV */
	size_t tv;
	for (tv=0; tv<plan.size(); tv++)
	{
		if (tv >= N_MAX_TV) {
			PLOG_ERROR("CommConversor::TablasUlises: Demasiados elementos en el Plan de Asignacion de Usuarios TV");
			break;
		}
		plan[tv].copyto(&p_plan[tv]);
	}
	if (tv < N_MAX_TV)
		p_plan[tv].idhost[0] = NO_NAME;
}

/** */
void CommConversor::TablaUlises(vector<CommUlises_st_asignacionusuario_gw> &plan, struct st_asignacionrecursos_gw *p_plan)
{
	/** Plan de asignacion de recursos GW */
	size_t gw;
	for (gw=0; gw<plan.size(); gw++)
	{
		if (gw >= N_MAX_REC_SISTEMA) {
			PLOG_ERROR("CommConversor::TablasUlises: Demasiados elementos en el Plan de Asignacion de Recursos GW");
			break;
		}
		plan[gw].copyto(&p_plan[gw]);
	}
	if (gw < N_MAX_REC_SISTEMA)
		p_plan[gw].idhost[0] = NO_NAME;
}

/** */
void CommConversor::TablaUlises(vector<CommUlises_st_direccionamiento_sip> &plan, struct st_direccionamiento_sip *p_plan)
{
	/** Plan direccionamiento SIP */
	size_t iSip;
	for (iSip=0; iSip<plan.size(); iSip++)
	{
		if (iSip >= N_MAX_TV) 
		{
			PLOG_ERROR("CommConversor::TablasUlises: Demasiados elementos en el Plan de Direccionamiento SIP");
			break;
		}		
		plan[iSip].copyto(&p_plan[iSip]);
	}

	if (iSip < N_MAX_TV)
		p_plan[iSip].idusuario[0] = NO_NAME;
}

