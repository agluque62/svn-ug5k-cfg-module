#ifndef _UV5KIGWCFG_WEB_APP_H_
#define _UV5KIGWCFG_WEB_APP_H_

#include "web-app-server.h"
#include "uv5kigwcfg-web-app-data.h"
#include "../tools/tools.h"
#include "../tools/ftp-client.h"
#include "../../include/config/comm-config.h"
#include "../../include/config/comm-preconf.h"
#include "../../include/cfg-proc.h"
#include "../../include/man-proc.h"
#include "../../include/his-proc.h"
#include "../../include/file-supervisor.h"

#define RETURN_NOT_IMPLEMENTED_RESP(r)			{r->code=404; r->data="{\"res\":\"Operacion no Implementada\"}";return;}
#define RETURN_OK200_RESP(r, d)					{r->code=200; r->data=d;return;}
#define RETURN_IERROR_RESP(r, d)				{r->code=500; r->data=d;PLOG_ERROR(d.c_str());return;}

#define CPU2CPU_MSG								((const char *)"cpu2cpu")
#define CPU2CPU_MSG_CAMBIO_CONFIG				((const char *)"1")
#define CPU2CPU_MSG_REMOTE_LOCK					((const char *)"2")
#define CPU2CPU_MSG_REMOTE_UNLOCK				((const char *)"3")
#define CPU2CPU_MSG_CHECK_CFG					((const char *)"checkcfg")
#define CPU2CPU_MSG_GET_CFG						((const char *)"getcfg")

/** */
class BiteControl : public CodeBase
{
public:
	BiteControl() {
		This = this;
		P_HIS_PROC->SetBite(BiteControl::st_bite_rsp);
	}
	string get() {
		wait.Wait();
		return jdata;
	}
protected:
	static void st_bite_rsp(bool res, int len, void *data) {
		This->bite_rsp(res, len, data);
	}
	void bite_rsp(bool res, int len, void *data) {
		PLOG_DEBUG("Recibida Respuesta a comando BITE...");
		if (res==true)
		{
			P_MAN_PROC->setBite(len, data);
			jdata = P_MAN_PROC->jbite();
		}
		else
		{
			jdata="{\"res\": \"Error en Ejecucion comando BITE\"}";
		}
		wait.Signal();
	}
private:
	CDead wait;
	string jdata;
	static BiteControl *This;
};

/** */
class Uv5kiGwCfgWebApp :
	public WebAppServer
{
public:
	Uv5kiGwCfgWebApp(void *pcfg);
	~Uv5kiGwCfgWebApp(void);

protected:
	static void stCb_(struct mg_connection *conn, string user, web_response *resp);
	static void stCb_tses(struct mg_connection *conn, string user, web_response *resp);
	static void stCb_ntpstatus(struct mg_connection *conn, string user, web_response *resp);
	static void stCb_logout(struct mg_connection *conn, string user, web_response *resp);
	static void stCb_config(struct mg_connection *conn, string user, web_response *resp);
	static void stCb_preconfig(struct mg_connection *conn, string user, web_response *resp);
	static void stCb_importexport(struct mg_connection *conn, string user, web_response *resp);
	static void stCb_mtto(struct mg_connection *conn, string user, web_response *resp);
	static void stCb_internos(struct mg_connection *conn, string user, web_response *resp);
	static void stCb_dev(struct mg_connection *conn, string user, web_response *resp);

	static bool stAccessControl(string name, string pwd, int *profile);

	void GetHandlers();
	void GetConfig(void *pvcfg);

protected:
	static void *DelayedReset(void* arg);
	static void *ConfigSync(void* arg);
	static void SynchronizeConfigIfApplicable();

protected:
	static vector<string> parse_uri(string uri)
	{
		vector<string> _levels;	
		Tools::split(_levels, uri, '/');
		return _levels;
	}
private:
	virtual restHandler *handlers(){return &_handlers_list;}
	virtual web_config *config(){return &_web_config;}

private:
	static restHandler _handlers_list;
	static web_config  _web_config;
	static webData_VersionNucleoNew _versiones;
	static void *file_version_load_thread_routine(void *arg);
};

#endif
