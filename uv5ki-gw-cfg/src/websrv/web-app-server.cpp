#include "../../include/websrv/web-app-server.h"
#include "../../include/config/local-config.h"
#include "../../include/tools/tools.h"

#define _UPLOAD_				0
#define _NO_EXPIRE_				1
#ifdef _WIN32
 #define TIEMPO_SESSION_DEFAULT	(10*60)
#else
 #define TIEMPO_SESSION_DEFAULT	(30*60)
#endif

/** */
WebAppServer::WebAppServer(void) 
{
#ifdef _WIN32
#else
#endif
}

/** */
WebAppServer::~WebAppServer(void)
{
}

/** */
void WebAppServer::Start()
{
	const char *ret;
#ifdef _WIN32
	// Solo escucha en la IP de Windows...
	string wport = LocalConfig::p_cfg->get(strWindowsTest, strItemWindowsWebPort, "8080");
	string wip = LocalConfig::p_cfg->get(strWindowsTest, strItemWindowsTestIp, "127.0.0.1");
	string AddToBind = wip + ":" + wport;
#else
	string AddToBind = config()->web_port;
#endif
	PLOG_INFO("WebAppServer. Starting (%s) (%s) (%s)...",
		/*config()->web_port*/AddToBind.c_str(),
		config()->document_root.c_str(),
		config()->default_page.c_str());

	if ((server = mg_create_server(this, stWebHandler))==NULL)
		throw Exception("Error al Crear Servidor HTTP");
	if ((ret=mg_set_option(server, "listening_port", /*config()->web_port*/AddToBind.c_str()))!=NULL)
		throw Exception("Error en mg_set_option: listening_port");
	if ((ret = mg_set_option(server, "document_root", config()->document_root.c_str()))!=NULL)
		throw Exception("Error en mg_set_option: document_root");
	if ((ret = mg_set_option(server, "index_files", config()->default_page.c_str()))!=NULL)
		throw Exception("Error en mg_set_option: index_files");

	CThread::Start();
	PLOG_INFO("WebAppServer. Started...");
}

/** */
void WebAppServer::Dispose() 
{
	Stop();
	mg_destroy_server(&server);
}

/** */
void WebAppServer::Run()
{
	time_t tick = config()->tick;

	SetId("WebAppServer");
	config()->session_control.reset();

	PLOG_INFO("WebAppServer (%d) running...", pid());
	while(IsRunning())
	{
		try
		{
			this->sleep(10);
			mg_poll_server(server, 1000);
			if (config()->enable_ssession==true)
			{
				int t_inact = config()->session_time==0 ? TIEMPO_SESSION_DEFAULT : config()->session_time;
				if (config()->session_control.noactive(t_inact)==true)      // Supervision de Tiempo de Inactividad.
					config()->session_control.reset();
			}

			if (Tick.elapsed(tick)==true) {
				PLOG_INFO("(%d)=>WebAppServer. TICK", pid());
			}

		}
		catch(...)
		{
			PLOG_ERROR("WebAppServer::Run Excepcion en Lazo... ");
		}
	}
	PLOG_INFO("WebAppServer leaving...");
}

/** */
int WebAppServer::stWebHandler(struct mg_connection *conn, enum mg_event ev)
{
	WebAppServer *server = (WebAppServer *)conn->server_param;
	return server->WebHandler(conn, ev);
}

/** */
int WebAppServer::WebHandler(struct mg_connection *conn, enum mg_event ev)
{
	try 
	{
		switch (ev)
		{
		case MG_AUTH:

			if (config()->enable_login==false)
				return MG_TRUE;
			return check_auth(conn);	  // return MG_TRUE;  

		case MG_RECV:
#if _UPLOAD_
			if (_hup.HandleRecv(conn, result))
				return result;
#endif
			return MG_FALSE;

		case MG_CLOSE:
#if _UPLOAD_
			if (_hup.HandleClose(conn, result))
				return result;
#endif
			return MG_FALSE;

		case MG_REQUEST:
#if _UPLOAD_
			if (_hup.HandleRequest(conn, result))
				return result;
#endif
			if (config()->enable_login==true)
			{
				if (Check4SecureUri(conn->uri) == true && 
					(strcmp(conn->request_method, "POST") == 0 /*|| strcmp(conn->request_method, "PUT") == 0*/) ) 		
				{		
					return check_login_form_submission(conn);		
				}	  	
			}
			return ProcessRequest(conn);

		default:
			return MG_FALSE;
		}
	}
	catch(...)
	{
		PLOG_ERROR("WebAppServer::WebHandler Exception.");
	}
	return MG_FALSE;
}

/** */
int WebAppServer::ProcessRequest(struct mg_connection * conn)
{
	try 
	{
		web_response response;

		string user = config()->enable_login==true ? current_user(conn) : "";
		webCallback wb = FindRest(conn->uri);
		if (wb != NULL) 
		{
			wb(conn, user, &response);

			if (config()->enable_ssession==true && response.actividad==true)
				config()->session_control.tick();

			mg_send_status(conn, response.code);
			mg_send_header(conn, "Content-type", "application/json");
			mg_send_data(conn, response.data.c_str(), response.data.length());
			return MG_TRUE;
		}
		return MG_FALSE;
	}
	catch(Exception x) 
	{
		PLOG_EXCEP(x, "WebAppServer::ProcessRequest Exception.");
	}
	return MG_FALSE;
}

/** */
webCallback WebAppServer::FindRest(string url)
{
	for (restHandler_iterator hit=handlers()->begin(); hit != handlers()->end(); ++hit)
	{
		if (url.find(hit->first) != string::npos)
			return hit->second;
	}
	return NULL;
}

/** */
int WebAppServer::check_auth(struct mg_connection *conn) 
{
	char ssid[100];
	//char calculated_ssid[100], name[100], expire[100];
	//char str_profile[16];

	// REDAN V2. Aceptar las Peticiones del Servidor.
	if (IsServerRequest(conn) == true)
		return MG_TRUE;

	// Always authenticate requests
	if (Check4SecureUri(conn->uri)==true)
		return MG_TRUE;

	if (config()->session_control.active()==true)
	{
		// Look for session ID in the Cookie.
		// That session ID can be validated against the database that stores current active sessions.
		mg_parse_header(mg_get_header(conn, "Cookie"), "ssid", ssid, sizeof(ssid));

//#if _NO_EXPIRE_
//#ifdef _WIN32
//		if (sscanf_s(ssid, "%[^|]|%[^|]|", name, sizeof(name), str_profile, sizeof(str_profile)) == 2) 
//#else
//		if (sscanf(ssid, "%[^|]|%[^|]|", name, str_profile) == 2) 
//#endif
//		{
//			generate_ssid(name, atoi(str_profile), expire, calculated_ssid, sizeof(calculated_ssid));
//			if (strcmp(ssid, calculated_ssid) == 0) 
//			{
//				// _session.RegActividad();			// No se puede chequear aqui por los Polling automaticos de la pagina.
//				return MG_TRUE;  // Authenticate
//			}
//		}
//#else
//#ifdef _WIN32
//		if (sscanf_s(ssid, "%[^|]|%[^|]|%[^|]|", name, sizeof(name), str_profile, sizeof(str_profile), expire, sizeof(expire)) == 3) 
//#else
//		if (sscanf(ssid, "%[^|]|%[^|]|%[^|]|", name, str_profile, expire) == 3) 
//#endif
//		{
//			generate_ssid(name, atoi(str_profile), expire, calculated_ssid, sizeof(calculated_ssid));
//			if (strcmp(ssid, calculated_ssid) == 0) 
//			{
//				config()->session_control.RegActividad();
//				return MG_TRUE;  // Authenticate
//			}
//		}
//#endif
		if (config()->session_control.check(ssid))
			return MG_TRUE;
	}

	// Auth failed, do NOT authenticate, redirect to login page
	mg_printf(conn, "HTTP/1.1 302 Moved\r\nLocation: %s\r\n\r\n",config()->login_uri.c_str());
	return MG_FALSE;
}

/** */
string WebAppServer::current_user(struct mg_connection *conn)
{
	char ssid[100], name[100], str_profile[16];
	const char *cookies = mg_get_header(conn, "Cookie");
	mg_parse_header(cookies, "ssid", ssid, sizeof(ssid));
#ifdef _WIN32
	if (sscanf_s(ssid, "%[^|]|%[^|]|", name, sizeof(name), str_profile, sizeof(str_profile)) == 2) 
#else
	if (sscanf(ssid, "%[^|]|%[^|]|", name, str_profile) == 2) 
#endif
	{
		return string(name);
	}
	return "Unknow";
}

/** */
int WebAppServer::check_login_form_submission_old(struct mg_connection *conn)
{
	char name[100], password[100], ssid[100], expire_epoch[100];
	int profile=64;
#if _NO_EXPIRE_
#else
	char expire[100];
#endif

	mg_get_var(conn, "name", name, sizeof(name));
	mg_get_var(conn, "password", password, sizeof(password));
	bool acceso = config()->enable_login==true ? config()->access_control(name, password, &profile) : true;
	bool session= config()->enable_ssession==true ? config()->session_control.active() : false;
	if (profile == ROOT_PROFILE || session == false)
	{
		if (acceso == true)
		{
			profile = profile==ROOT_PROFILE ? 64 : profile;
#if _NO_EXPIRE_
			// Tiempo Sesion infinito.
#ifdef _WIN32
			_snprintf_s(expire_epoch, sizeof(expire_epoch), "%lu", (unsigned long)0);
#else
			snprintf(expire_epoch, sizeof(expire_epoch), "%lu", (unsigned long)0);
#endif
			generate_ssid(name, profile, expire_epoch, ssid, sizeof(ssid));
			mg_printf(conn,
				"HTTP/1.1 302 Found\r\n"
				"Set-Cookie: ssid=%s \r\n"
				"Location: /\r\n\r\n\r\n",
				ssid);
#else // _NO_EXPIRE_
			int iexpire = config()->session_time;
			time_t t = time(NULL) + (iexpire==0 ? (24*3600) : iexpire);

#ifdef _WIN32
			tm t1;
			gmtime_s(&t1, &t);
			_snprintf_s(expire_epoch, sizeof(expire_epoch), "%lu", (unsigned long) t);
			strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", &t1);
#else
			snprintf(expire_epoch, sizeof(expire_epoch), "%lu", (unsigned long) t);
			strftime(expire, sizeof(expire), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
#endif
#ifdef _WIN32
			string expdate = CookieTime(expire, t1);
#else
			string expdate = CookieTime(expire, *(gmtime(&t)));
#endif
			generate_ssid(name, profile, expire_epoch, ssid, sizeof(ssid));

			// Set "session id" cookie, there could be some data encoded in it.
			mg_printf(conn, "HTTP/1.1 302 Found\r\n"
				"Set-Cookie: ssid=%s; expires=\"%s\"; \r\n"
				"Location: /\r\n\r\n\r\n",
				ssid, expire);

#endif // _NO_EXPIRE_
			config()->session_control.active(ssid, name, profile, "es");
			return MG_TRUE;
		}
		else
		{
			if (config()->enable_login==true)
				mg_printf(conn, "HTTP/1.1 302 Found\r\nLocation: %s\r\n\r\n\r\n", config()->bad_user_uri.c_str());
		}
	}
	else
	{
		if (config()->enable_login==true)
			mg_printf(conn, "HTTP/1.1 302 Found\r\nLocation: %s?flg=1\r\n\r\n\r\n", config()->closed_session_uri.c_str());
	}
	return MG_FALSE;
}

/** */
int WebAppServer::check_login_form_submission(struct mg_connection *conn)
{
	char name[100], password[100], ssid[100], expire_epoch[100];
	char lang[32];
	int profile=64;

	/** Datos */
	mg_get_var(conn, "name", name, sizeof(name));
	mg_get_var(conn, "password", password, sizeof(password));
	mg_get_var(conn, "lang", lang, sizeof(lang));
	bool acceso = config()->enable_login==true ? config()->access_control(name, password, &profile) : true;

	PLOG_INFO("login lang: %s n=%s pf=%x acc=%i", lang, name, profile, acceso? 1:0);
#ifdef _WIN32
	_snprintf_s(expire_epoch, sizeof(expire_epoch), "%lu", (unsigned long)0);
#else
	snprintf(expire_epoch, sizeof(expire_epoch), "%lu", (unsigned long)0);
#endif
	if (config()->session_control.active()==true)
	{
		if (profile == ROOT_PROFILE)
		{
			if (config()->session_control.isroot()==false)
			{
				generate_ssid(name, profile, expire_epoch, ssid, sizeof(ssid));
				mg_printf(conn,
						"HTTP/1.1 302 Found\r\n"
						"Set-Cookie: ssid=%s \r\n"
						"Location: /\r\n\r\n\r\n",
						ssid);
				config()->session_control.active(ssid,  name, profile, lang);
				return MG_TRUE;		
			}
		}
		if (config()->enable_login == true)
		{
			mg_printf(conn, "HTTP/1.1 302 Found\r\nLocation: %s?flg=%d\r\n\r\n\r\n", 
				config()->closed_session_uri.c_str(),
				string(name)=="root" && profile == ROOT_PROFILE && acceso==true ? 1 : 0);
		}
	}
	else
	{
		if (acceso == true)
		{
			generate_ssid(name, profile, expire_epoch, ssid, sizeof(ssid));
			mg_printf(conn,
				"HTTP/1.1 302 Found\r\n"
				"Set-Cookie: ssid=%s \r\n"
				"Location: /\r\n\r\n\r\n",
				ssid);
			config()->session_control.active(ssid, name, profile, lang);
			return MG_TRUE;		
		}
		else 
		{
			if (config()->enable_login==true)
			{
				mg_printf(conn, "HTTP/1.1 302 Found\r\nLocation: %s\r\n\r\n\r\n", config()->bad_user_uri.c_str());
			}
		}
	}
	return MG_FALSE;
}

/** */
string WebAppServer::CookieTime(char *buffer, struct tm stime)
{
	const std::string dayName[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	const std::string monName[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
#ifdef _WIN32
	sprintf_s(buffer, sizeof(buffer), "%s, %d %s %d %02d:%02d:%02d GMT", dayName[stime.tm_wday].c_str(), stime.tm_mday, monName[stime.tm_mon].c_str(),
		1900 + stime.tm_year, stime.tm_hour, stime.tm_min, stime.tm_sec);
#else
	sprintf(buffer,"%s, %d %s %d %02d:%02d:%02d GMT", dayName[stime.tm_wday].c_str(), stime.tm_mday, monName[stime.tm_mon].c_str(),
		1900 + stime.tm_year, stime.tm_hour, stime.tm_min, stime.tm_sec);
#endif

	return std::string(buffer);
}

/** */
void WebAppServer::generate_ssid(const char *user_name, int profile, const char *expiration_date, char *ssid, size_t ssid_size)
{
	char hash[33];  
#if _NO_EXPIRE_
	mg_md5(hash, user_name, config()->secret.c_str(), NULL);
#ifdef _WIN32  
	_snprintf_s(ssid, ssid_size, _TRUNCATE, "%s|%d|%s", user_name, profile, hash);
#else
	snprintf(ssid, ssid_size, "%s|%d|%s", user_name, profile, hash);
#endif

#else

	mg_md5(hash, user_name, ":", expiration_date, ":", config()->secret.c_str(), NULL);
#ifdef _WIN32  
	_snprintf_s(ssid, ssid_size,  _TRUNCATE, "%s|%d|%s|%s", user_name, profile, expiration_date, hash);
#else
	snprintf(ssid, ssid_size, "%s|%d|%s|%s", user_name, profile, expiration_date, hash);
#endif

#endif
}

/** */
bool WebAppServer::Check4SecureUri(string uri)
{
	for (vector<string>::iterator it=config()->sec_uris.begin(); it != config()->sec_uris.end(); it++)
	{
		if (uri.find(*it) == 0)
//		if (*it == uri)
			return true;
	}
	return false;
}
/** */
bool WebAppServer::IsServerRequest(struct mg_connection* conn) {
	for (int i = 0; i < conn->num_headers; i++) {
		mg_connection::mg_header header = conn->http_headers[i];
		if (strcmp(header.name,"RedanClient")==0 && strcmp(header.value,"SERVER")==0)
			return true;
	}
	return false;
}

