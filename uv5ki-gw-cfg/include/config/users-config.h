#ifndef __USERS_CONFIG_H__
#define __USERS_CONFIG_H__

#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream#include <utility>
#include <queue>
#include <algorithm>

#include "../base/code-base.h"
#include "../base/thread.h"
#include "../base/socket.h"
#include "../base/sistema.h"
#include "../websrv/uv5kigwcfg-web-app-data.h"
#include "../tools/tools.h"
#include "../config/local-config.h"

/** */
class UserData : public jData
{
public:
	UserData() {
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer)
	{
		write_key(writer, "name", name);
		write_key(writer, "clave", clave);
		write_key(writer, "perfil", perfil);
	}
	virtual void jread(Value &base)
	{
		read_key(base, "perfil", perfil);
		read_key(base, "clave", clave);
		read_key(base, "name", name);
	}

public:
	string name;
	string clave;
	int perfil;
};

#endif
