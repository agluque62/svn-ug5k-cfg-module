#include "../../include/config/comm-config.h"

/** */
CommConfig::CommConfig(soap_config &sConfig)
{
	/** Configuración de ULISES */
	this->tipo = 1;
	this->idConf = "ULISES-CONF";
	this->fechaHora = sConfig.IdConfig;
	
	/** Parametros Generales */
	this->general = CommGenConfig(sConfig);
	
	/** Servicios */
	this->servicios = CommSerConfig(sConfig);

	/** Configuracion de Hardware */
	this->hardware = CommHarConfig(sConfig);

	/** Configuracion de Recursos */
	for (size_t irec=0; irec<sConfig.CfgPasarela.ListaRecursos.size(); irec++)
	{
		this->recursos.push_back(CommResConfig(sConfig, irec));
	}

	/** Configuracion especifica de ULISES */
	this->ulises = CommUv5Config(sConfig);

	/** En Ulises no hay usuarios */
	this->users.clear();

}

/** */
CommGenConfig::CommGenConfig(soap_config &sc)
{
	DatosLocales datos_locales;

	this->name = sc.CfgPasarela.Nombre;
	this->emplazamiento = "";									// Dejar Vacio. 
	// this->dualidad = datos_locales.dualidad==true ? 1 : 0;	// Leer DatosLocales.ini
	this->ipv = sc.IpVirt;
	this->ips = sc.Server;
	/** 20170714. AGL. En las nuevas versiones sc.IpCol puede venir vacio para Para pasarelas simples. */
	this->dualidad = ((sc.IpCol=="" || sc.IpVirt == sc.IpCol)  ? 0 : 1);

	//this->acGrupoMulticast = sc.ParametrosMulticast.GrupoMulticastConfiguracion;
	//this->uiPuertoMulticast = sc.ParametrosMulticast.PuertoMulticastConfiguracion;

	this->nivelconsola = datos_locales.nivel_consola;			// Leer DatosLocales.ini
	this->puertoconsola = datos_locales.puerto_consola;
	this->nivelIncidencias = datos_locales.nivel_incidencias;

	if (this->dualidad==0)
	{
		this->cpus.push_back(CommGenCpu(sc.IpHw,"255.255.255.0",sc.IpHw));
		this->cpus.push_back(CommGenCpu());
	}
	else 
	{
		if (sistema::ParImpar()==0)
		{
			this->cpus.push_back(CommGenCpu(sc.IpHw,"255.255.255.0",sc.IpHw));
			this->cpus.push_back(CommGenCpu(sc.IpCol,"255.255.255.0",sc.IpCol));
		}
		else
		{
			this->cpus.push_back(CommGenCpu(sc.IpCol,"255.255.255.0",sc.IpCol));
			this->cpus.push_back(CommGenCpu(sc.IpHw,"255.255.255.0",sc.IpHw));
		}
	}

	/** 20181016. U2510. SP#01-15*/
	this->SupervisionLanGW = sc.CfgPasarela.SupervisionLanGW;
	this->TmMaxSupervLanGW = sc.CfgPasarela.TmMaxSupervLanGW;
	/** 20190221. REDAN107*/
	this->dvrrp = sc.CfgPasarela.dvrrp;
}

/** */
CommSerConfig::CommSerConfig(soap_config &sc) 
{
	/** SIP */
	this->sip.PuertoLocalSIP = sc.CfgPasarela.PuertoLocalSIP;
	sc.firstRadioParams(this->sip.KeepAlivePeriod, this->sip.KeepAliveMultiplier, this->sip.SupresionSilencio);
	this->sip.PeriodoSupervisionSIP = sc.CfgPasarela.PeriodoSupervisionSIP;	
	
	vector<string> sipserver;
	sc.sip_servers(sipserver);

	if (sipserver.size() > 0) {
		this->sip.proxys[0] = sipserver[0];
		this->sip.registrars[0] = sipserver[0];
	}
	if (sipserver.size() > 1) {
		this->sip.proxys[1] = sipserver[1];
		this->sip.registrars[1] = sipserver[1];
	}

	//for(vector<string>::iterator it=sipserver.begin(); it!=sipserver.end(); it++) 
	//{
	//	if (this->sip.proxys.size() < 2)
	//	{
	//		this->sip.proxys.push_back(CommSerServidor(*it));
	//		this->sip.registrars.push_back(CommSerServidor(*it));
	//	}
	//}

	/** WEB */
	this->web.wport = atoi(LocalConfig::p_cfg->get(strSection, strItemWebPort)/*.PuertoEscucha()*/.c_str());
	this->web.stime = atoi(LocalConfig::p_cfg->get(strSection, strItemSessionTime)/*TiempoSesion()*/.c_str());

	/** SNMP */
	LocalConfig snmpini(onfs(LocalConfig::p_cfg->get(strModulos, strItemModuloSnmp)/*.snmpModule()*/));
	this->snmp.sport = 65000;
	this->snmp.snmpp = atoi(snmpini.get("AGENTE","PORT").c_str());
	this->snmp.agcomm = snmpini.get("AGENTE","SNMPV2COMM");
	this->snmp.agcont = snmpini.get("AGENTE","CONTACT");
	this->snmp.agloc = snmpini.get("AGENTE","LOCATION");
	this->snmp.agname = snmpini.get("AGENTE","NAME");
	this->snmp.agv2 = atoi(snmpini.get("AGENTE","SNMPV2").c_str());
	/** TODO... */
	this->snmp.traps.push_back("2," + sc.Server + "/162");

	/** Grabador */
	LocalConfig recini(onfs(LocalConfig::p_cfg->get(strModulos, strItemModuloGrabador)/*.recModule()*/));

	this->grab.rtsp_ip = sc.CfgPasarela.Grabador1;							// recini.get("RTSP","IP_REC_A");
	this->grab.rtspb_ip = sc.CfgPasarela.Grabador2;
	
	this->grab.rtsp_port = atoi(recini.get("RTSP","PORT_RTSP").c_str());

	/** SINCR */
	if (sc.CfgPasarela.MasterSincronizacion != "")
		this->sincr.servidores[0] = CommSerServidor(sc.CfgPasarela.MasterSincronizacion);
}

/** */
CommHarConfig::CommHarConfig(soap_config &sc)
{
	/** Inicializa los Arrays */
	for (int islv=0; islv<_NUM_SLAVES_; islv++) 
	{
		CommHarConfigSlv Slv;

		for (int ipos=0; ipos<_NUM_POS_; ipos++) 
		{
			Slv.pos.push_back(CommHarConfigPos());
		}

		this->slv.push_back(Slv);
	}

	/** Configura las posiciones con los recursos configurados */
	for (size_t irec=0; irec<sc.CfgPasarela.ListaRecursos.size(); irec++)
	{
		int islv = sc.CfgPasarela.ListaRecursos[irec].SlotPasarela;
		int ipos = sc.CfgPasarela.ListaRecursos[irec].NumDispositivoSlot;

		if (islv < _NUM_SLAVES_ && ipos < _NUM_POS_)
		{
			this->slv[islv].tp = 1;						// IA4
			this->slv[islv].pos[ipos].tp=0;
			this->slv[islv].pos[ipos].cfg=sc.CfgPasarela.ListaRecursos[irec].TipoRecurso==0 ? 1 : 2;
		}
	}

}

/** */
CommResConfig::CommResConfig(soap_config &sc, int irec)
{
	if (irec < (int )sc.CfgPasarela.ListaRecursos.size())
	{
		soap_RecursosSCV sres = sc.CfgPasarela.ListaRecursos[irec];

		/** Parametros generales de recurso */
		this->IdRecurso = sres.IdRecurso;
		this->Radio_o_Telefonia = sres.TipoRecurso==0 ? 1 : 2;
		this->SlotPasarela = sres.SlotPasarela;
		this->NumDispositivoSlot =sres. NumDispositivoSlot;
		this->TamRTP = sres.info.TamRTP;	
		this->Codec = sres.info.Codec;
		this->Uri_Local = "sip:" + sres.IdRecurso + "@" + sc.IpVirt;

		this->enableRegistro = 0;							// Dejar a 0. 
		this->szClave = "";									// Dejar vacio.
		this->Buffer_jitter.max = 0;							// Dejar a 0.
		this->Buffer_jitter.min = 0;							// Dejar a 0. 

		this->hardware.AD_AGC = sres.info.GananciaAGCTX;
		this->hardware.AD_Gain= (int )(Tools::atof(sres.info.GananciaAGCTXdBm)*10);
		this->hardware.DA_AGC = sres.info.GananciaAGCRX;
		this->hardware.DA_Gain= (int )(Tools::atof(sres.info.GananciaAGCRXdBm)*10);

		if (sres.TipoRecurso==0) 
		{
			/** Parametros Generales Radio */
			this->radio.tipo=sres.Tipo==0 ? 6 : sres.Tipo==1 ? 5 : 4;		// Tipos RemotoRx, RemotoTx o RemotoRxTx
			this->radio.sq=sres.info.radio.SQ=="v" ? 1 : 0;
			this->radio.ptt=sres.info.radio.PTT=="s" ? 1 : 0;
			this->radio.bss=sres.info.radio.BSS==false ? 0 : 1;
			this->radio.modoConfPtt=sres.info.radio.ModoConfPTT;				// TODO: Comprobar coherencia de valores.
			this->radio.repSqBss=sres.info.radio.RepSQyBSS;
			this->radio.desactivacionSq=sres.info.radio.DesactivacionSQ;
			this->radio.timeoutPtt=sres.info.radio.TimeoutPTT;
			this->radio.umbralVad=sres.info.radio.UmbralVAD;
			this->radio.numFlujosAudio=sres.info.radio.NumFlujosAudio;
			this->radio.tiempoPtt=sres.info.radio.TiempoPTT;
			this->radio.tmVentanaRx=0;										// Radio Local a 0
			this->radio.climaxDelay=0;										// Idem
			this->radio.tmRetardoFijo=0;										// Idem
			this->radio.bssRtp=0;											// TODO: Quitar...
			this->radio.retrasoSqOff=0;										// Dejar a 0

			/** 20170119. Este campo viene en UmbralTonoPTT. Valores mayores de 0 => evtPtt=1, */
			//  20170316. En nueva config, viene con campo específico.
			// this->radio.evtPTT=sres.info.radio.UmbralTonoPTT > 0 ? 1 : 0;
			/***************************/

			/** 20170316. Parámetros específicos para FD */
			this->radio.tGRSid = sres.info.radio.GrsDelay;								// GRS Delay
			/** 201806020. En Ulises vienen los codigos cambiados */
			// this->radio.metodoBss = sres.info.radio.MetodoBSS;							// Metodo QIDX
			this->radio.metodoBss = sres.info.radio.MetodoBSS==0 ? 1 : 0;

			sres.TablaBss(this->radio.tabla_indices_calidad);						//  
			this->radio.iEnableGI=sres.info.radio.GrabacionEd137==false ? 0 : 1;		// 
			this->radio.evtPTT=sres.info.radio.EnableEventPttSq  ? 1 : 0;
			/*********************************************/

			this->radio.tjbd=30;												// Dejar a 30		
			this->radio.iPttPrio=0;											// Idem
			this->radio.iPrecisionAudio=0;									// Estricta.

			this->radio.iSesionPrio=0;										// Radio Local a 0
			this->radio.iModoCalculoClimax = 0;

			this->radio.colateral.name=sres.info.IdDestino;					// FID. Frecuencia
			this->radio.colateral.tipoConmutacion=0;							// Quitar...
																			// Es en local. Quitar.
			this->radio.colateral.emplazamientos.clear();
			this->radio.colateral.emplazamientos.push_back(CommResRadioEmplazamiento());
			this->radio.colateral.emplazamientos.push_back(CommResRadioEmplazamiento());
			this->radio.colateral.emplazamientos.push_back(CommResRadioEmplazamiento());

			this->radio.FrqTonoSQ = sres.info.radio.FrqTonoSQ;
			this->radio.UmbralTonoSQ = sres.info.radio.UmbralTonoSQ;
			this->radio.FrqTonoPTT = sres.info.radio.FrqTonoPTT;
			this->radio.UmbralTonoPTT = sres.info.radio.UmbralTonoPTT;							// => iNivelTonoPtt
			this->radio.SupervPortadoraTx = sres.info.radio.SupervPortadoraTx==true ? 1 : 0;		// => iSupervisionPortadoraTx
			this->radio.SupervModuladoraTx = sres.info.radio.SupervModuladoraTx==true ? 1 : 0;	// => iSupervisionModuladoraTx

		}
		else 
		{
			/** Parametros Generales Telefonia */
			sc.TipoTelefonia((int )sres.Interface, this->telefonia.tipo);
			this->telefonia.lado = sres.info.telef.Lado;
			this->telefonia.t_eym = 0;								 // TODO: Confirmar.
			this->telefonia.h2h4 = 0;								 // TODO: Confirmar.
			this->telefonia.ladoeym = 0;								 // TODO: Confirmar.
			this->telefonia.modo = 0;								 // TODO: Confirmar.
			this->telefonia.no_test_local = AtsLocalTestNumber(sc);						// Numero de la Central PROPIA.
			this->telefonia.no_test_remoto = AtsRemoteTestNumber(sc, this->IdRecurso);	// Numero del troncal que contiene al recurso.
			this->telefonia.it_release = 5;							 // Dejar a 5.
			this->telefonia.detect_vox = 0;							 // Dejar a 0. En ULISES no hay deteccion de cuelgue.
			this->telefonia.umbral_vox = -26;						 // 
			this->telefonia.tm_inactividad = 0;						 // 20170628. Se cambia de 12 a 0 para que en ULISES no se supervise inactividad.
		
		    /** 20181016. U2510. SP#01-15*/
			this->telefonia.TReleaseBL = sres.info.telef.TReleaseBL;
			this->telefonia.iDetCallerId = sres.info.telef.iDetCallerId;
			this->telefonia.iTmCallerId = sres.info.telef.iTmCallerId;
			this->telefonia.iDetInversionPol = sres.info.telef.iDetInversionPol;
			this->telefonia.iPeriodoSpvRing = sres.info.telef.iPeriodoSpvRing;
			this->telefonia.iFiltroSpvRing = sres.info.telef.iFiltroSpvRing;
			this->telefonia.iDetDtmf = sres.info.telef.iDetDtmf;
			
			this->telefonia.colateral_scv = 0;						 // Dejar a 0.
			this->telefonia.iT_Int_Warning = 5;						 // Dejar a 5.
			/** 20180320. Nuevos Parámetros en interfaces analogicas */
			this->telefonia.iTmLlamEntrante = sres.info.telef.iTmLlamEntrante;
			this->telefonia.iTmDetFinLlamada = sres.info.telef.iTmDetFinLlamada;

			this->telefonia.ats_rangos_dst.clear();					 // Dejar Vacio.
			this->telefonia.ats_rangos_org.clear();					 // Dejar Vacio.

			/** 20200703. Nuevos parámetros de líneas Telefónicas */
			this->telefonia.iDetLineaAB = 0;
			this->telefonia.iEnableNoED137 = 0;
			/** 20200703. Nueva Estructura de Colateral Remoto */
				// Elementos existentes.
			this->telefonia.r_automatica = 0; // Dejar a Cero.
			this->telefonia.tm_superv_options = sres.info.telef.tm_superv_options;
			this->telefonia.uri_remota = "";							 // Dejar vacio.
			this->telefonia.superv_options = sres.info.telef.superv_options;
				// Elementos Añadidos.
			this->telefonia.itiporespuesta = 0;

			this->telefonia.additional_uri_remota = "";
			this->telefonia.additional_superv_options = 0;
			this->telefonia.additional_itiporespuesta = 0;

				// Exclusivas de Ulises.
			this->telefonia.idRed = sres.info.telef.IdRed;
			this->telefonia.idTroncal = sres.info.telef.IdTroncal;
			this->telefonia.listaEnlacesInternos = sres.info.telef.ListaEnlacesInternos;

			this->telefonia.ats_rangos_operador = AtsRangosOperador(sc);
			this->telefonia.ats_rangos_privilegiados = AtsRangosPrivilegiados(sc);
			this->telefonia.ats_rangos_directos_ope = AtsRangosDirectosOpe(sc, this->IdRecurso);
			this->telefonia.ats_rangos_directos_pri = AtsRangosDirectorPri(sc, this->IdRecurso);
		}

		this->LlamadaAutomatica=0;									// Dejar a 0

		this->restriccion=0;											// Dejar a 0.
		this->blanca.clear();										// Dejar Vacio.
		this->negra.clear();											// Dejar Vacio.

		/** */
		this->iFlgUsarDiffServ = sres.Diffserv;
		this->szDestino = sres.info.IdDestino;
	}
}

/** */
CommUv5Config::CommUv5Config(soap_config &sc)
{
	this->MulticastGroup = sc.ParametrosMulticast.GrupoMulticastConfiguracion;
	this->MulticastPort = sc.ParametrosMulticast.PuertoMulticastConfiguracion;

	// Plan ATS.
	for(size_t iPlanAts=0; iPlanAts<sc.ArrayOfNumeracionATS.size(); iPlanAts++)
	{
		soap_NumeracionATS sPlanAts = sc.ArrayOfNumeracionATS[iPlanAts];
		CommUlises_st_numeracionats numeracionats(sPlanAts);
		this->plannumeracionats.push_back(numeracionats);
	}
	// Plan IP
	for (size_t iPlanIp=0; iPlanIp<sc.ArrayOfDireccionamientoIP.size(); iPlanIp++)
	{
		this->plandireccionamientoip.push_back(CommUlises_st_direccionamientoip(sc.ArrayOfDireccionamientoIP[iPlanIp]));
	}

	// Plan Troncales.
	for (size_t iPlanTronc=0; iPlanTronc<sc.ArrayOfListaTroncales.size(); iPlanTronc++)
	{
		this->plantroncales.push_back(CommUlises_st_listatroncales(sc.ArrayOfListaTroncales[iPlanTronc]));
	}

	// Plan Redes.
	for (size_t iPlanRed=0; iPlanRed<sc.ArrayOfListaRedes.size(); iPlanRed++)
	{
		this->planredes.push_back(CommUlises_st_listaredes(sc.ArrayOfListaRedes[iPlanRed]));
	}

	// Plan Usuario TV
	for (size_t iTv=0; iTv<sc.ArrayOfAsignacionUsuariosTV.size(); iTv++)
	{
		this->planasignacionusuarios.push_back(CommUlises_st_asignacionusuario_tv(sc.ArrayOfAsignacionUsuariosTV[iTv]));
	}

	// Plan Recursos GW.
	for (size_t iGw=0; iGw<sc.ArrayOfAsignacionRecursosGW.size(); iGw++)
	{
		this->planasignacionrecursos.push_back(CommUlises_st_asignacionusuario_gw(sc.ArrayOfAsignacionRecursosGW[iGw]));
	}

	// Plan SIP
	for (size_t iSip=0; iSip<sc.ArrayOfDireccionamientoSIP.size(); iSip++)
	{
		this->plandireccionamientosip.push_back(CommUlises_st_direccionamiento_sip(sc.ArrayOfDireccionamientoSIP[iSip]));
	}
}

/** */
CommUlises_st_numeracionats::CommUlises_st_numeracionats(soap_NumeracionATS sPlanAts)
{
	this->centralpropia = sPlanAts.CentralPropia ? 1 : 0;
	this->throwswitching = sPlanAts.Throwswitching ? 1 : 0;
	this->no_test = sPlanAts.NumTest;

	// Rangos Operadores
	for (size_t iRango=0; iRango<sPlanAts.RangosOperador.size(); iRango++)
	{
		soap_NumeracionATS::soap_RangosSCV sRango = sPlanAts.RangosOperador[iRango];
		CommUlises_st_rango rango(sRango);

		this->rangosoperador.push_back(rango);
	}

	// Rangos Operadores Privilegiados.
	for (size_t iRango=0; iRango<sPlanAts.RangosPrivilegiados.size(); iRango++)
	{
		soap_NumeracionATS::soap_RangosSCV sRango = sPlanAts.RangosPrivilegiados[iRango];
		CommUlises_st_rango rango(sRango);

		this->rangosprivilegiados.push_back(rango);
	}

	// Plan Rutas.
	for (size_t iRuta=0; iRuta<sPlanAts.ListaRutas.size(); iRuta++)
	{
		soap_NumeracionATS::soap_PlanRutas sRuta = sPlanAts.ListaRutas[iRuta];
		CommUlises_st_planrutas ruta(sRuta);

		this->listarutas.push_back(ruta);
	}
}
