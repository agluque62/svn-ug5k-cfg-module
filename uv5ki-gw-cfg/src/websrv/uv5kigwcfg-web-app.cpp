#include "../../include/websrv/uv5kigwcfg-web-app.h"

#define FILEOFVERSION	onfs("/home/serv/versiones.json")
/** */
BiteControl *BiteControl::This = NULL;

/** */
restHandler Uv5kiGwCfgWebApp::_handlers_list;
web_config  Uv5kiGwCfgWebApp::_web_config;
webData_VersionNucleoNew Uv5kiGwCfgWebApp::_versiones;

/** */
void *Uv5kiGwCfgWebApp::file_version_load_thread_routine(void *arg) 
{
	PLOG_INFO("Uv5kiGwCfgWebApp. Cargando Fichero de versiones...");
	_versiones.loadfrom(FILEOFVERSION);
	PLOG_INFO("Uv5kiGwCfgWebApp. Fichero de versiones cargado...");
	return NULL;
}

/** */
Uv5kiGwCfgWebApp::Uv5kiGwCfgWebApp(void *pcfg)
{
	GetHandlers();
	GetConfig(pcfg);
}

/** */
Uv5kiGwCfgWebApp::~Uv5kiGwCfgWebApp(void)
{
}

/** */
void Uv5kiGwCfgWebApp::GetHandlers() 
{
	_handlers_list["/tses"]=stCb_tses;				// GET					TICK de Sesssion
	_handlers_list["/logout"]=stCb_logout;			// POST					LOGOUT.
	_handlers_list["/config"]=stCb_config;			// GET, POST, PUT		Configuracion Activa. Obtener (GET), Modificar (POST), Subir (PUT)
	_handlers_list["/preconf"]=stCb_preconfig;		// GET, POST, PUT, DEL	Preconfiguraciones
	_handlers_list["/impexp"]=stCb_importexport;	// GET, POST			EXPORTAR (GET) IMPORTAR (POST)

	_handlers_list["/mant"]=stCb_mtto;				// GET, POST
	//_handlers_list["/mant/std"]=stCb_mtto;		// GET					.
	//_handlers_list["/mant/ver"]=stCb_mtto;		// GET					.
	//_handlers_list["/mant/lver"]=stCb_mtto;		// GET					.
	//_handlers_list["/mant/bite"]=stCb_mtto;		// GET					.
	//_handlers_list["/mant/reset"]=stCb_mtto;		// RESET de UNIDAD		POST
	//_handlers_list["/mant/swactiva"]=stCb_mtto;	// ACTIVAR SW-VER		POST
	//_handlers_list["/mant/swrestore"]=stCb_mtto;	// RETAURAR SW-VER		POST
	_handlers_list["/ntpstatus"]=stCb_ntpstatus;	// GET.					INFO CLIENTE NTP.
	_handlers_list["/cpu2cpu"]=stCb_internos;		// PUT.					COMUNICACIONES INTERNAS

	_handlers_list["/test"]=stCb_;					// GET, POST.			PARA PRUEBAS...
	_handlers_list["/dev"]=stCb_dev;				// GET, POST			PARA DESARROLLO...
	_handlers_list["/force_logout"] = stCb_logout;
}

/** */
void Uv5kiGwCfgWebApp::GetConfig(void *pvcfg)
{
	WorkingConfig *pcfg = (WorkingConfig *)pvcfg;

//	_web_config.web_port = LocalConfig::p_cfg->get(strSection, strItemWebPort, "8080")/*.PuertoEscucha()*/; 
	_web_config.web_port = Tools::itoa(pcfg->WebPort());
	_web_config.document_root = ON_WORKING_DIR("appweb");
	_web_config.default_page = "ug5kweb-index.html";
	_web_config.login_uri = "/login.html";
	_web_config.bad_user_uri = "/error1.html";
	_web_config.closed_session_uri = "/error2.html";
	_web_config.secret = ":-)";
#ifdef _WIN32
	_web_config.enable_login = true;
#else
	_web_config.enable_login = true;
#endif
	_web_config.enable_ssession = true;
	// _web_config.session_time = 0;					// TODO. Leer de la configuracion.
	_web_config.session_time = pcfg->WebSessionTime();

	_web_config.sec_uris.push_back("/styles/bootstrap/bootstrap.min.css");
	_web_config.sec_uris.push_back("/styles/uv5ki-styles.css");
	_web_config.sec_uris.push_back("/scripts/jquery/jquery-2.1.3.min.js");
	_web_config.sec_uris.push_back("/images/corporativo-a.png");
	_web_config.sec_uris.push_back("/images/noaccess.jpg");
	_web_config.sec_uris.push_back(_web_config.login_uri);
	_web_config.sec_uris.push_back(_web_config.bad_user_uri);
	_web_config.sec_uris.push_back(_web_config.closed_session_uri);
	_web_config.sec_uris.push_back("/cpu2cpu");
	_web_config.sec_uris.push_back("/test");
	_web_config.sec_uris.push_back("/hojaExterna.css");
	_web_config.sec_uris.push_back("/mant/ver");
	_web_config.sec_uris.push_back("/mant/lver");
	_web_config.sec_uris.push_back("/ntpstatus");
	_web_config.sec_uris.push_back("/force_logout");

	_web_config.access_control = stAccessControl;	

	_web_config.tick = LocalConfig::p_cfg->getint(strRuntime, strRuntimeItemThreadActiveTick, "60");

	/** Cargo el fichero de versiones en THREAD aparte */
	RealWorkingThread(file_version_load_thread_routine, NULL).Do();
}

/** */
bool Uv5kiGwCfgWebApp::stAccessControl(string name, string pwd, int *profile)
{
	if (name=="root" && pwd=="#ncc#") {
		if (profile != NULL)
			*profile = ROOT_PROFILE;
		return true;
	}

	if (name=="root" && pwd=="dfnucleo") {
		if (CfgProc::p_cfg_proc->GetStdLocalConfig() != slcAislado)
		{
			if (profile != NULL)
				*profile = PERF_VISUAL;
		}
		else
		{
		if (profile != NULL)
			*profile = PERF_ALM1;
		}
		return true;
	}

	return P_WORKING_CONFIG->UserAccess(name, pwd, profile);
}

/** */
void Uv5kiGwCfgWebApp::stCb_(struct mg_connection *conn, string user, web_response *resp)
{
#if LOCAL_TEST		
	resp->actividad=false;
	if (string(conn->request_method)=="GET") 
	{
#if !defined(_PPC82xx_)
		FtpClient().Test();
#endif
		ifstream f((const char *)"./comm-config.json", ios_base::in);
		CommConfig cfg(f);
		RETURN_OK200_RESP(resp, cfg.JSerialize());
	}
	else if (string(conn->request_method)=="POST") 
	{
		webData_line ok("OK");
		RETURN_OK200_RESP(resp, ok.JSerialize());
	}
	else if (string(conn->request_method)=="PUT") 
	{
		webData_line ok("OK");
		RETURN_OK200_RESP(resp, ok.JSerialize());
	}
	RETURN_NOT_IMPLEMENTED_RESP(resp);
#else
	string strdata = "{\"res\":\"Handler por Defecto\"}";
	
	resp->actividad=true;
	resp->code=200;
	resp->data=strdata;
#endif
}

/** */
void Uv5kiGwCfgWebApp::stCb_tses(struct mg_connection *conn, string user, web_response *resp)
{
	resp->actividad=false;
	if (string(conn->request_method)=="GET") 
	{	// Enlazar con los datos reales en el constructor...
		int std;
		string cfg_name, cfg_time;
		P_CFG_PROC->IdConfig(std, cfg_name, cfg_time);
		webData_tses data(std, cfg_name, cfg_time, P_CFG_PROC->Modo(), _versiones.Version);
		RETURN_OK200_RESP(resp, data.JSerialize());
	}
#if LOCAL_TEST
	else if (string(conn->request_method)=="POST")
	{
		string data_in = string(conn->content, conn->content_len );
		webData_tses data(data_in);
		RETURN_OK200_RESP(resp, "");
	}
#endif
	RETURN_NOT_IMPLEMENTED_RESP(resp);
}

void Uv5kiGwCfgWebApp::stCb_ntpstatus(struct mg_connection *conn, string user, web_response *resp)
{
	resp->actividad=false;
	if (string(conn->request_method)=="GET") 
	{
		webData_NtpStatus data;
		RETURN_OK200_RESP(resp, data.JSerialize());
	}
	RETURN_NOT_IMPLEMENTED_RESP(resp);
}

/** */
void Uv5kiGwCfgWebApp::stCb_logout(struct mg_connection *conn, string user, web_response *resp)
{
	resp->actividad=false;
	if (string(conn->request_method)=="POST" || string(conn->request_method) == "GET")
	{
		PLOG_INFO("Uv5kiGwCfgWebApp: LOGOUT Usuario: %s", user.c_str());
		_web_config.session_control.reset();
		webData_line ok("OK");
		RETURN_OK200_RESP(resp, ok.JSerialize());
	}
	RETURN_NOT_IMPLEMENTED_RESP(resp);
}

/** */
void Uv5kiGwCfgWebApp::stCb_config(struct mg_connection *conn, string user, web_response *resp)
{
	try {
		resp->actividad = true;
		if (string(conn->request_method) == "GET")
		{
			// Leer la configuracion activa de RAM.
			RETURN_OK200_RESP(resp, P_WORKING_CONFIG->JConfig());
		}
		else if (string(conn->request_method) == "POST")
		{
			PLOG_INFO("Uv5kiGwCfgWebApp: Recibida Orden de Cambio de Configuracion. Usuario: %s", user.c_str());
			// Activar la Configuracion...
			string data_in = string(conn->content, conn->content_len);
			CommConfig cfg(data_in);
			if (cfg.test() == true)
			{
				EventosHistoricos* ev = P_WORKING_CONFIG->set(cfg, true);
				P_WORKING_CONFIG->TimeStamp();
				P_WORKING_CONFIG->save_to(LAST_CFG);
				P_HIS_PROC->SetEventosHistoricos(user, ev);				// Generar los historicos de cambios.

				PLOG_INFO("Uv5kiGwCfgWebApp: Orden de Cambio de Configuracion ejecutada...");

				// 20170630. Las condiciones estan mal seleccionadas. Se cambian
				if (P_WORKING_CONFIG->DualCpu() && P_CFG_PROC->GetStdLocalConfig() == slcAislado)
				{
					PLOG_INFO("Uv5kiGwCfgWebApp: Sincronizando Cambio de Configuracion...");
					WorkingThread(Uv5kiGwCfgWebApp::ConfigSync, NULL).Do();
				}
			}
			else {
				PLOG_ERROR("Uv5kiGwCfgWebApp: Error procesando Orden de Cambio de Configuracion. Formato Incorrecto...");
				RETURN_IERROR_RESP(resp, webData_line("Formato de Configuracion incorrecto").JSerialize());
			}
			RETURN_OK200_RESP(resp, webData_line("Configuracion Activada...").JSerialize());
		}
		else if (string(conn->request_method) == "PUT")
		{
			// Mirar si esta aislado...	
			if (P_CFG_PROC->IsIdle()) {
				RETURN_IERROR_RESP(resp, webData_line("Error de UPLOAD. La Unidad esta aislada.").JSerialize());
			}
			// Dar aviso de carga de configuracion...
			P_CFG_PROC->AvisaSubirConfiguracion();
			PLOG_INFO("Uv5kiGwCfgWebApp: Orden de Subir configuracion local. Usuario: %s", user.c_str());
			RETURN_OK200_RESP(resp, webData_line("Peticion de Subida cursada...").JSerialize());
		}
		RETURN_NOT_IMPLEMENTED_RESP(resp);
	}
	catch (Exception x) {
		PLOG_EXCEP(x, "Uv5kiGwCfgWebApp::stCb_config Exception.");
		RETURN_IERROR_RESP(resp, webData_line("Formato de Configuracion incorrecto").JSerialize())
	}
}

/** */
void Uv5kiGwCfgWebApp::stCb_preconfig(struct mg_connection *conn, string user, web_response *resp)
{
	CommPreconfs preconfs;
	resp->actividad=true;
	if (string(conn->request_method)=="GET")
	{
		RETURN_OK200_RESP(resp, preconfs.JSerialize());
	}
	else if (string(conn->request_method)=="POST" || 
			 string(conn->request_method)=="PUT"  || 
			 string(conn->request_method)=="DELETE" ) 
	{
		vector<string> levels = parse_uri(string(conn->uri));
		if (levels.size() < 3 ) {
			RETURN_IERROR_RESP(resp, webData_line("Error en Preconfiguracion. Id no presente en peticion.").JSerialize());
		}
		bool res = true;
		string pcfg_name = levels[2];
		if (string(conn->request_method)=="POST")		// Salvar Preconfiguracion activa como...
		{
			// Obtener la Configuracion activa...
			CommPreconf activa(pcfg_name, Tools::Ahora(), P_WORKING_CONFIG->JConfig());
			res = preconfs.pos(pcfg_name, activa);
			if (res == false) {
				RETURN_IERROR_RESP(resp, webData_line("Error al Salvar Preconfiguracion: " + pcfg_name).JSerialize());
			}
			/** HIST 154 */
			P_HIS_PROC->SetEvent(INCI_GCFG, user, /*"CFG", */activa.name);
			PLOG_INFO("Uv5kiGwCfgWebApp: Salvada Preconfiguracion %s. Usuario: %s", pcfg_name.c_str(), user.c_str());
		}
		else if (string(conn->request_method)=="PUT") // Activar Configuracion.
		{
					// Comprobar estado AISLADO...
			if (P_CFG_PROC->IsIdle()==false) {
				RETURN_IERROR_RESP(resp, webData_line("Preconfiguracion No Activada. Pasarela NO AISLADA.").JSerialize());
			}
			CommPreconf activa;
			if (preconfs.get(pcfg_name, activa)==false) {
				RETURN_IERROR_RESP(resp, webData_line("Error al Activar Preconfiguracion: " + pcfg_name + ". No esta en BDT.").JSerialize());
			}
				// Comprobar formato de configuracion...
			bool correcta = P_WORKING_CONFIG->TestConfig();
			if (correcta == false) {
				RETURN_IERROR_RESP(resp, webData_line("Error al Activar Preconfiguracion: " + pcfg_name + ". Formato de Configuraicon incorrecto").JSerialize());
			}
			P_HIS_PROC->SetEvent(INCI_ACFG, user, /*"CFG", */activa.name);
			PLOG_INFO("Uv5kiGwCfgWebApp: Activada Preconfiguracion %s. Usuario: %s", pcfg_name.c_str(), user.c_str());

					// Activar la configuracion...
			CommConfig cfg(activa.data);
			/** 20170503. El nombre de ka CFG es el de la PRECFG de la base de datos */
			cfg.idConf = pcfg_name;
			/************************************/

			P_WORKING_CONFIG->set(cfg, true);	// Historicos de cambios ???
			P_WORKING_CONFIG->save_to(LAST_CFG);
												// Sincronizar Fichero....
			if (P_CFG_PROC->GetStdLocalConfig() != slcAislado && P_WORKING_CONFIG->DualCpu())
			{
				WorkingThread(Uv5kiGwCfgWebApp::ConfigSync, NULL).Do();
			}
		}
		else if (string(conn->request_method)=="DELETE")		// Borra preconfiguracion.
		{
			if (preconfs.del(pcfg_name)==false) {
				RETURN_IERROR_RESP(resp, webData_line("Preconfiguracion No Eliminada. Error en BDT.").JSerialize());
			}
					/** HIST 156 */
			P_HIS_PROC->SetEvent(INCI_DCFG, user, pcfg_name);
			PLOG_INFO("Uv5kiGwCfgWebApp: Eliminada Preconfiguracion %s. Usuario: %s", pcfg_name.c_str(), user.c_str());
		}
		RETURN_OK200_RESP(resp, CommPreconfs().JSerialize());
	}
	RETURN_NOT_IMPLEMENTED_RESP(resp);
}

/** */
void Uv5kiGwCfgWebApp::stCb_importexport(struct mg_connection *conn, string user, web_response *resp)
{
	resp->actividad = true;

	CommPreconfs preconfs;
	vector<string> levels = parse_uri(string(conn->uri));

	if (string(conn->request_method)=="GET")					// EXPORT...
	{
		if (levels.size() < 3 ) {
			RETURN_IERROR_RESP(resp, webData_line("Error al Exportar Preconfiguracion. Id no presente en peticion.").JSerialize());
		}
		CommPreconf exportada;
		if (preconfs.get(levels[2], exportada)==false) {
			RETURN_IERROR_RESP(resp, webData_line("Error al Exportar Preconfiguracion: " + levels[2] + ". No esta en la base de datos.").JSerialize());
		}
		PLOG_INFO("Uv5kiGwCfgWebApp: Exportada Preconfiguracion %s. Usuario: %s", levels[2].c_str(), user.c_str());
		RETURN_OK200_RESP(resp, exportada.data);
	}
	else if (string(conn->request_method)=="POST" )			// IMPORT... 
	{
		string pcfg_name = levels[2]=="" ? Tools::FileUniqueName("IMPORT") : 
			preconfs.Exist(levels[2]) ? Tools::FileUniqueName(levels[2]) : levels[2];

		string data_in = string(conn->content, conn->content_len);
		CommPreconf activa(pcfg_name, Tools::Ahora(), data_in);
		bool res = preconfs.pos(pcfg_name, activa);
		if (res == false) {
			RETURN_IERROR_RESP(resp, webData_line("Error al Salvar Preconfiguracion: " + pcfg_name).JSerialize());
		}
		/** HIST 154 */
		HistClient::p_hist->SetEvent(INCI_GCFG, user, /*"CFG", */activa.name);
		PLOG_INFO("Uv5kiGwCfgWebApp: Importada Preconfiguracion %s. Usuario: %s", pcfg_name.c_str(), user.c_str());
		RETURN_OK200_RESP(resp, webData_line("ok").JSerialize());
	}
	RETURN_NOT_IMPLEMENTED_RESP(resp);
}

/** */
void Uv5kiGwCfgWebApp::stCb_mtto(struct mg_connection *conn, string user, web_response *resp)
{
	resp->actividad=true;
	vector<string> levels = parse_uri(string(conn->uri));
	if (levels.size() != 3) {
		RETURN_NOT_IMPLEMENTED_RESP(resp);
	}
	if (string(conn->request_method)=="GET") {
		if (levels[2]=="std") {
			RETURN_OK200_RESP(resp, ManProc::p_man->jestado());
		}
		else if (levels[2]=="ver") {
			RETURN_OK200_RESP(resp, webData_VersionNucleo().JSerialize());
		}
		else if (levels[2]=="lver") {
			// TODO: 
			//RETURN_OK200_RESP(resp, webData_line("En construccion").JSerialize());
			RETURN_OK200_RESP(resp, _versiones.isLoaded()==true ? _versiones.JSerialize() : "");
		}
		else if (levels[2]=="bite") {
			P_HIS_PROC->SetEvent(INCI_BITE, user, "");
			string jBite = BiteControl().get();
			RETURN_OK200_RESP(resp, jBite);
		}
		else if (levels[2]=="logs") {
			RETURN_OK200_RESP(resp, webData_LogsList().JSerialize());
		}		
		RETURN_NOT_IMPLEMENTED_RESP(resp);
	}
	else if (string(conn->request_method)=="POST") {
		if (levels[2]=="reset") {
			WorkingThread(Uv5kiGwCfgWebApp::DelayedReset, NULL).Do();
			P_HIS_PROC->SetEvent(INCI_RESET, user, "");
			PLOG_INFO("Uv5kiGwCfgWebApp: RESET de Unidad por Usuario: %s", user.c_str());
			RETURN_OK200_RESP(resp, webData_line("En construccion").JSerialize());
		}
		else if (levels[2]=="swactiva") {
			// TODO: 
			RETURN_OK200_RESP(resp, webData_line("En construccion").JSerialize());
		}
		else if (levels[2]=="swrestore") {
			// TODO: 
			RETURN_OK200_RESP(resp, webData_line("En construccion").JSerialize());
		}
		RETURN_NOT_IMPLEMENTED_RESP(resp);
	}
	RETURN_NOT_IMPLEMENTED_RESP(resp);
}

/** */
void Uv5kiGwCfgWebApp::stCb_internos(struct mg_connection *conn, string user, web_response *resp)
{
	resp->actividad=false;
	if (string(conn->request_method)=="PUT") 
	{
		vector<string> levels = parse_uri(string(conn->uri));
		if (levels.size() != 3) {
			RETURN_IERROR_RESP(resp, webData_line("Error en Peticion interna.").JSerialize());
		}
		string data_in = string(conn->content, conn->content_len );
		if (levels[2]==CPU2CPU_MSG_CAMBIO_CONFIG)				// Han cambiado la Configuracion...
		{
			PLOG_INFO("Recibido Aviso Cambio de Configuracion Local");
			string data_in = string(conn->content, conn->content_len );
			CommConfig cfg(data_in);
			P_WORKING_CONFIG->set(cfg, true);						// No Genero historicos.
			// P_WORKING_CONFIG->TimeStamp();						// Ni cambio la hora..
			P_WORKING_CONFIG->save_to(LAST_CFG);
			RETURN_OK200_RESP(resp, webData_line("Configuracion Activada...").JSerialize());
		}
		else if (levels[2]==CPU2CPU_MSG_REMOTE_LOCK)			// LOCK Fichero para cambiarlo
		{
			PLOG_INFO("Recibido Aviso LOCK Fichero: %s", data_in.c_str());
			FileSupervisor::p_fspv->LocalLock(data_in);
			RETURN_OK200_RESP(resp, webData_line("OK").JSerialize());
		}
		else if (levels[2]==CPU2CPU_MSG_REMOTE_UNLOCK)			// UNLOCK Fichero para cambiarlo
		{
			PLOG_INFO("Recibido Aviso UNLOCK Fichero: %s", data_in.c_str());
			FileSupervisor::p_fspv->LocalUnlock(data_in);
			RETURN_OK200_RESP(resp, webData_line("OK").JSerialize());
		}
		RETURN_NOT_IMPLEMENTED_RESP(resp);		
	}
	RETURN_NOT_IMPLEMENTED_RESP(resp);
}

/** */
void Uv5kiGwCfgWebApp::stCb_dev(struct mg_connection *conn, string user, web_response *resp)
{
	resp->actividad=false;
	vector<string> levels = parse_uri(string(conn->uri));
	
	if (string(conn->request_method)=="GET") 
	{
	}
	else if (string(conn->request_method)=="POST")
	{
		if (levels.size()==3 && levels[2]=="hist")						// Genera una trama de historico 
		{
			string data_in = string(conn->content, conn->content_len );
			vector<string> data_hist;
			Tools::split(data_hist, data_in, ',');
			if (data_hist.size() >= 3)
			{
				P_HIS_PROC->SetEvent(
					atoi(data_hist[0].c_str()),
					data_hist[1],
					data_hist[2],
					data_hist.size() >= 4 ? data_hist[3] : "",
					data_hist.size() >= 5 ? data_hist[4] : "",
					data_hist.size() >= 6 ? data_hist[5] : "",
					data_hist.size() >= 7 ? data_hist[6] : ""
					);
				RETURN_OK200_RESP(resp, "");
			}
		RETURN_IERROR_RESP(resp, webData_line("Error en Formato de trama. Pocos parámetros.").JSerialize());
		}
	}
	RETURN_NOT_IMPLEMENTED_RESP(resp);
}

/** Working Thread */
/** */
void *Uv5kiGwCfgWebApp::DelayedReset(void* arg)
{
	/** */
	Sleep(2000);
	/** */
	P_HIS_PROC->SetReset();

#ifndef _NO_WORKING_THREAD_
	pthread_exit(NULL);
#endif
	return NULL;
}

/** */
void *Uv5kiGwCfgWebApp::ConfigSync(void* arg)
{
	try 
	{
		string ipColateral;									// Sincronizar el fichero...
		if (P_WORKING_CONFIG->IpColateral(ipColateral)==true)
		{
			if (ipColateral != "") 
			{
				ParseResponse resp = HttpClient(ipColateral, Tools::atoi(_web_config.web_port)).SendHttpCmd("PUT", 
					string(CPU2CPU_MSG)+ "/" + string(CPU2CPU_MSG_CAMBIO_CONFIG), 
					LocalConfig().getint(strRuntime, strRuntimeItemLocalHttpTimeout, "5000"),
					P_WORKING_CONFIG->JConfig());
				if (resp.Status() != "200")
					PLOG_ERROR("Uv5kiGwCfgWebApp::ConfigSync. HTTP-ERROR %s: <%s>", resp.Status().c_str(), resp.Body().c_str());
			}
			else
				PLOG_ERROR("Uv5kiGwCfgWebApp::ConfigSync. IP-COLATERAL NO VALIDA!!!");
		}
		else
			PLOG_ERROR("Uv5kiGwCfgWebApp::ConfigSync. NO IP-COLATERAL!!!");

	#ifndef _NO_WORKING_THREAD_
		pthread_exit(NULL);
	#endif
	}
	catch(...) 
	{
		PLOG_ERROR("Uv5kiGwCfgWebApp::ConfigSync Exception !!!");
	}
	return NULL;
}



