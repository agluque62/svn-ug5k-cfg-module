#ifndef __COMM_MSDC91_CONFIG_H__
#define __COMM_MSDC91_CONFIG_H__

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream#include <utility>
#include <queue>
#include <algorithm>

#include "../base/code-base.h"
#include "../websrv/uv5kigwcfg-web-app-data.h"
#include "../tools/tools.h"
#include "../config/local-config.h"
#include "../base/sistema.h"


enum respuestasATSR2 {respFREE=6, respBUSY=3, respOUT=5, respCONGESTION=8, respLIBERACION=10, respBLOCKING=11};

/** */
class CommRespSIPtoATSR2 : public jData
{
public:
	CommRespSIPtoATSR2() {
		respSIP = 422;
		respATSR2 = 0;
	}
	CommRespSIPtoATSR2(int codSIP, int respuestaATSR2) {
		respSIP = codSIP;
		respATSR2 = respuestaATSR2;
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "respSIP", respSIP);
		write_key(writer, "respATSR2", respATSR2);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "respSIP", respSIP);
		read_key(base, "respATSR2", respATSR2);
	}

public:
	int respSIP;
	int respATSR2;
};

/** */
class CommModoSDC91Config : public jData
{
public:
	CommModoSDC91Config() {
		this->respuestasSIPtoATSR2.push_back(CommRespSIPtoATSR2(404,respOUT));
		this->respuestasSIPtoATSR2.push_back(CommRespSIPtoATSR2(486,respBUSY));
		this->respuestasSIPtoATSR2.push_back(CommRespSIPtoATSR2(400,respBLOCKING));
	}
	//CommGenConfig(soap_config &sc);
	~CommModoSDC91Config() {
		clear_array(respuestasSIPtoATSR2);
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "respuestasSIPtoATSR2", respuestasSIPtoATSR2);

	}
	virtual void jread(Value &base)
	{
		read_key(base, "respuestasSIPtoATSR2", respuestasSIPtoATSR2);
	}

public:
#if __POR_REFERENCIA__
	vector<CommRespSIPtoATSR2> respuestasSIPtoATSR2;
#else
	vector<CommRespSIPtoATS *> respuestasSIPtoATSR2;
#endif
};

#endif
