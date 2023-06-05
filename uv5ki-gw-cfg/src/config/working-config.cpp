#include "../../include/config/working-config.h"
#include "../../include/tools/base64.h"

/** */
WorkingConfig::WorkingConfig(WorkingConfigMode mode)
{
	cfg_mode = mode;
	init();
}

/** */
WorkingConfig::~WorkingConfig() 
{
	dispose();
}

/** */
void WorkingConfig::init()
{
	p_mem_config_sem = new global_semaphore(33,1);
	if (p_mem_config_sem->acquire()) {
#ifdef _WIN32
		p_mem_config =(ug5k_mem_config *)(new char[sizeof(ug5k_mem_config)+4]);	// new ug5k_mem_config();
#else
		/** Shared memory segment at 1234 */
		key_mem = 75218977; 
		/** Create the segment and set permissions.*/
		if ((shmid = shmget(key_mem, sizeof(ug5k_mem_config), IPC_CREAT | 0666)) < 0) 
		{
			PLOG_ERROR("shmget error %i", errno);
		}
		   /** Now we attach the segment to our data space. */
		else if ((p_mem_config = (ug5k_mem_config *)shmat(shmid, NULL, 0)) == (ug5k_mem_config *) -1) 
		{
			PLOG_ERROR("shmat error %i", errno);
		}
		else 
		{
			/** Marco la configuracion como vacia */
			strncpy(((ug5k_mem_config *)p_mem_config)->acIdSistema, "ESTA-VACIA", CFG_MAX_LONG_ID);
		}
#endif
		p_mem_config_sem->release();
	}
}

/** */
void WorkingConfig::dispose()
{
	/** 20180411. Si se cae la apliacion con el semaforo cogido, no se podr�a limpiar la misma */
	//PLOG_DEBUG("WorkingConfig (global semaphore) disposing...");
	//if (p_mem_config_sem->acquire()) {
#ifdef _WIN32
	delete p_mem_config;
#else
	if(shmdt(p_mem_config) != 0)
		fprintf(stderr, "Could not close memory segment.\n");
#endif
	delete p_mem_config_sem;
	p_mem_config_sem = NULL;
	PLOG_DEBUG("WorkingConfig (global semaphore) disposed.");
	//}
}

/** */
/** 20171102. Callback para las respuestas del modulo de Grabacion */
void WorkingConfig::_recResponse(bool res, int len, void *data)
{
	if (data != NULL) {
		((char *)data)[len] = 0;
		PLOG_INFO("_recResponse: %s", string(((char *)data)).c_str());
	}
	else {
		PLOG_ERROR("_recResponse....");
	}
}
EventosHistoricos *WorkingConfig::set(CommConfig &redanCfg, bool actualiza_ini, bool bsave) 
{
	EventosHistoricos *his = NULL;
	int actualiza_snmp, actualiza_rec;
	config = redanCfg;

	if (p_mem_config_sem->acquire()) {
		/** Actualizar la memoria y los ficheros INI */
		his = redanConv.convierte(config, p_mem_config, actualiza_ini, &actualiza_snmp, &actualiza_rec);
		p_mem_config_sem->release();
	}

	if (bsave == true) {
		save_to(LAST_CFG);
	}

	/** Mandar las se�ales a los procesos colaterales */
	if (actualiza_ini==true)
	{
		LocalConfig snmpconfig(onfs(LocalConfig::p_cfg->get(strModulos, strItemModuloSnmp)/*.snmpModule()*/));
		HistClient::p_hist->Signal(snmpconfig.getint("SERVICIO","UDP_PORT_IN_AGSNMP","65000"));
		if (actualiza_rec)
		{
			LocalConfig recconfig(onfs(LocalConfig::p_cfg->get(strModulos, strItemModuloGrabador)/*.snmpModule()*/));
			HistClient::p_hist->Signal(recconfig.getint("SERVICIO","PORT_IN_SERVICIO","65003"), _recResponse);
		}
	}
	/** Mandar el SIGNAL USR2 */
	RealWorkingThread(WorkingConfig::DelayedSignal, this).Do();

	return his;
}

/** */
void WorkingConfig::set(soap_config &sConfig)
{
	/** Parse */
	CommConfig newConfig(sConfig);
	set(newConfig, true, true);
}

/** */
void WorkingConfig::set() {
	set(config, false, false);
}

/** */
void WorkingConfig::load_from(string file)
{
	if (cfg_mode == cfgRedan || cfg_mode==cfgSoap)
	{
		try 
		{
			ifstream f(file.c_str(), ios_base::in);
			CommConfig cfg(f);
			//
			if (cfg.test() == false)
			{
				PLOG_ERROR("load_from: JSON-No Valido Indice Desconocido");
				throw Exception("JSON-No Valido Indice Desconocido");
			}
			else //
			{
				set(cfg, false, false);
			}
		}
		catch(Exception x)
		{
			PLOG_EXCEP(x, "Error cargando fichero de configuracion: %s", file.c_str());
			// Poner configuracion por defecto.
			CommConfig def = get_default();
			def.fechaHora = "01/01/1970 00:00:00 UTC";				// Pone Fecha 01/01/1970, para que sea actualizado.
			set(def, false, false);
			//set_to_default(false);
			//CommConfig cfg;
			PLOG_DEBUG("Generada CFG Por Defecto, %s", def.fechaHora.c_str());
			//set(cfg, false);
			//PLOG_DEBUG("Activada CFG Por Defecto");
		}

		return;
	}
	throw Exception("Modo de Configuracion no implementado leyendo desde fichero...");
}

/** */
void WorkingConfig::save_to(string file)
{
	PLOG_INFO("save_to %s ", file.c_str());
	ofstream ff(file.c_str());	
	if (cfg_mode == cfgRedan || cfg_mode == cfgSoap) 
	{
		string data = config.JSerialize();
		ff.write(data.c_str(), data.length());
#ifdef _WIN32
#endif
		return;
	}
	throw Exception("Modo de Configuracion no implementado escribiendo fichero...");
}

CommConfig WorkingConfig::get_default() {
	CommConfig cfg;
	PLOG_INFO("WorkingConfig::get_default");
	return cfg;
}

///** */
//void WorkingConfig::set_to_default(bool bsave)
//{
//	// Poner configuracion por defecto.
//	CommConfig cfg;
//	PLOG_DEBUG("Generada CFG Por Defecto,");
//	set(cfg, false, bsave);
//	PLOG_DEBUG("Activada CFG Por Defecto");
//}

/** */
void WorkingConfig::TimeStamp()
{
	if (cfg_mode == cfgRedan || cfg_mode == cfgSoap) 
	{
		config.fechaHora = Tools::Ahora_Servidor();
		return;
	}
	throw Exception("Modo de Configuracion no implementado obteniendo timestamp...");
}

/** */
void WorkingConfig::TimeStamp(RedanTestComm &remota)
{
	if (cfg_mode == cfgRedan || cfg_mode == cfgSoap) 
	{
		config.fechaHora = remota.fechaHora;
		return;
	}
	throw Exception("Modo de Configuracion no implementado obteniendo timestamp...");
}

/** */
void WorkingConfig::ResourcesClear()
{
	if (cfg_mode == cfgRedan || cfg_mode == cfgSoap) 
	{
		// Borrar las referencias en la configuracion HW.
		config.hardware.clear();

		// Borrar la Lista de recursos.
		config.recursos.clear();
		return;
	}
	throw Exception("Modo de Configuracion no implementado borrando recursos...");
}

/** */
string WorkingConfig::JConfig()
{
	if (cfg_mode == cfgRedan || cfg_mode == cfgSoap)
		return config.JSerialize();
	throw Exception("Modo de Configuracion no implementado obteniendo configuracion...");
}

/** */
bool WorkingConfig::UserAccess(string user, string pwd, int *profile)
{
	if (cfg_mode == cfgRedan || cfg_mode == cfgSoap) 
	{
		string cpwd = base64_encode((const unsigned char *)pwd.c_str(), pwd.length());
		vector<UserData>::iterator it;
		for (it=config.users.begin(); it!=config.users.end(); it++)
		{
			if (it->name==user && it->clave==cpwd) 
			{
				if (profile != NULL) *profile=it->perfil;
				return true;
			}
		}
		return false;
	}
	throw Exception("Modo de Configuracion no implementado borrando recursos...");
}

/** */
void *WorkingConfig::DelayedSignal(void *arg)
{
	Sleep(2000);
	sistema::SignalNuevaConfiguracion(LocalConfig::p_cfg->get(strSection, strItemNucleoProcName)/*.NucleoProcName()*/);
#ifndef _NO_WORKING_THREAD_
	pthread_exit(NULL);
#endif
	return NULL;
}

/** */
void WorkingConfig::FromExternalSet(bool isFromWebPage, string user, CommConfig& cfg, bool ActualizaIni) {

	/** Salva ultima configuracion */
	save_to(LAST_SAVE(Tools::Int2String(_lastcfg++ & 3)));

	/** Activa la configuracion recibida */
	cfg.tipo = 0; // TODO ???
	// REDAN V2. Solo se Marca la CFG si la orden no procede del servidor...
	if (isFromWebPage) {
		cfg.fechaHora = Tools::Ahora_Servidor();
	}
	// Activa y actualiza la configuracion recibida.
	EventosHistoricos* ev = set(cfg, true, true);
	// REDAN V2. Solo generar los eventos si la orden no procede del servidor...
	if (isFromWebPage) {
		// Generar los historicos de cambios.
		P_HIS_PROC->SetEventosHistoricos(user, ev);
	}
	// Actualiza la configuracion recibida... */
	// save_to(LAST_CFG);
}



