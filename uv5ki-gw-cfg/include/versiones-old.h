#ifndef __VERSIONES__
#define __VERSIONES__
#include <string>
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream#include <utility>
#include <fstream>
#include <algorithm>

#include "./base/code-base.h"
#include "./base/sistema.h"
#include "./base/sqlite3++.h"
#include "./tools/tools.h"
#include "./config/local-config.h"

using namespace std;

/** */
typedef string(*cbQuery)(CppSQLite3Query qry);

/** */
class Versiones : CodeBase
{
public:
	static Versiones swrep;
	static string snmpVersion;
	static string recVersion;

public:
	Versiones(void);
	~Versiones(void);

public:
	string jActualVerbose();
	string jActualAndPrev();
	bool TestAndPreRegistra(string idSistema, string idVersion, string path);
	void ActivaDesactivaPreRegistro(bool activa);
	bool RestoreLastVersion();
	void ActualizaVersiones();

protected:
	string ExecQuery(string qry, cbQuery cb);
	bool PreRegistra(string idSistema, string idVersion, string path, string verbose="");
	bool Registra();
	bool Unregistra();
	bool TestImagen(string path);
	void ActivarImagen();

protected:
	static string cbActualVerbose(CppSQLite3Query qry);
	static string cbActualAndPrev(CppSQLite3Query qry);
	static string cbExist(CppSQLite3Query qry);
	static string cbActualPath(CppSQLite3Query qry);
	static string cbCount(CppSQLite3Query qry);

public:
	static string json();

public:
};

#endif
