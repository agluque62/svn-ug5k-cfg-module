#ifndef _WEB_APP_SERVER_H_
#define _WEB_APP_SERVER_H_

#include "../../include/base/thread.h"
#include "mongoose.h"

#define _PERFILES_EN_20170208_

#if defined(_PERFILES_EN_20170208_)
 #define PERF_VISUAL		(0x0001)
 #define PERF_CTRL			(0x0002)
 #define PERF_ALM1			(0x0008)
 #define PERF_ADMIN			(0x0040)
 #define PERF_PCONF			(0x1000)
 #define ROOT_PROFILE		(PERF_VISUAL | PERF_CTRL | PERF_ADMIN | PERF_PCONF)
#else
 #define PERF_VISUAL		(0x01)
 #define PERF_CTRL			(0x02)
 #define PERF_ALM2			(0x04)
 #define PERF_ALM1			(0x08)
 #define PERF_GEST			(0x10)
 #define PERF_ING			(0x20)
 #define PERF_ADMIN			(0x40)
 #define ROOT_PROFILE		(0x80)
#endif

typedef struct
{
	bool actividad;
	int code;
	string data;
} web_response;

typedef void (*webCallback)(struct mg_connection *conn, string user, web_response *resp);
typedef map<string, webCallback> restHandler;
typedef restHandler::iterator restHandler_iterator;
typedef bool (*webAccessRoutine)(string name, string pwd, int *profile);

/** */
class SessionControl
{
public:
	SessionControl()
	{
		reset();
	}
public:
	bool active(){return _set;}
	void active(string ssid, string name, int profile)
	{
		_ssid = ssid;
		_name = name;
		_profile = profile;
		_start = _last = time(NULL);
		_set = true;
	}
	bool check(string ssid) {
		return ssid == _ssid;
	}
	bool isroot() {
		return _name=="root" && _profile==ROOT_PROFILE;
	}
	void reset()
	{
		_set=false;
		_ssid = "";
		_start = _last = 0;
		_name = "";
		_profile = 0;
	}
	void tick()
	{
		_last = time(NULL);
	}
	bool noactive(int _cuanto)
	{
        time_t _now = time(NULL);
		return (_set==true && (_now-_last)>_cuanto) ? true : false;
	}
	time_t time_active()
	{
		return (_last-_start);
	}

private:
	bool _set;
	time_t _start;
	time_t _last;
	string _ssid;
	string _name;
	int _profile;
};

typedef struct
{
	string				web_port;
	string				document_root;
	string				default_page;
	string				login_uri;			// "/login.html";
	string				bad_user_uri;		//
	string				closed_session_uri;	//
	string				secret;				// Clave para encriptado...
	int					session_time;		// Duracion de las Sesiones.
	bool				enable_login;
	bool				enable_ssession;		
	vector<string>		sec_uris;			// Lista de URIS que no se ven afectadas por la seguridad. Dependen de la aplicacion.
	webAccessRoutine	access_control;		// Rutina que permite el acceso o no en función de los usuarios.
	SessionControl		session_control;
	int					tick;				// Para el control de presencia del Servior
} web_config;


/** */
class WebAppServer :
	public CThread
{
public:
	WebAppServer(void);
	~WebAppServer(void);

public:
	void Start();
	void Dispose();

protected:
	void Run();
	int  WebHandler(struct mg_connection *conn, enum mg_event ev);
	int ProcessRequest(struct mg_connection * conn);
	webCallback FindRest(string url);
	string current_user(struct mg_connection *conn);
	int check_login_form_submission_old(struct mg_connection *conn);
	int check_login_form_submission(struct mg_connection *conn);
	int check_auth(struct mg_connection *conn) ;
	void generate_ssid(const char *user_name, int profile, const char *expiration_date,
                          char *ssid, size_t ssid_size);
	string CookieTime(char *buffer, struct tm stime);
	bool Check4SecureUri(string uri);
	bool IsServerRequest(struct mg_connection* conn);

private:
	static int  stWebHandler(struct mg_connection *conn, enum mg_event ev);

private:
	virtual restHandler *handlers()=0;
	virtual web_config *config()=0;

private:
	struct mg_server *server;
};

#endif
