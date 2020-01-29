#include "../include/versiones.h"

#define _DBNAME			(ON_FLASH("ug5k.db"))
#define _VERTABLE		"swrep"

/** */
Versiones Versiones::swrep;
string Versiones::snmpVersion;
string Versiones::recVersion;

/** Zona de Callback */
/** Mira si existe resultado para una determinada QRY */
string Versiones::cbExist(CppSQLite3Query qry)
{
	return qry.eof() ? "no" : "si";
}

/** JSON para la Version Actual detallada */
string Versiones::cbActualVerbose(CppSQLite3Query qry)
{
	if (!qry.eof())
	{
		//string versiones = string(qry.fieldValue("strVersiones"));
		istringstream instr(string(qry.fieldValue("strVersiones")));
		stringstream  jtxt;
		string line;
		bool primera = true;

		jtxt << "{";
		jtxt << "\"fecha\":\"" << qry.fieldValue("fecha") << "\",\"version\": \"" << qry.fieldValue("idVersion") << "\",";
		jtxt << "\"lines\":[";

		while (std::getline(instr, line))
		{
			if (line=="") continue;
			if (line.find("Ultima Actualizacion") == 0) continue;
			// line.erase(std::remove_if(line.begin(), line.end(), &Tools::IsNoAscii), line.end());

			if (line.find(":") == line.length()-1) 
			{
				string line1;
				if (std::getline(instr, line1))
				{
					// line1.erase(std::remove_if(line1.begin(), line1.end(), &Tools::IsNoAscii), line1.end());
					line = line + " " + line1;
				}
			}                        
			if (!primera)
				jtxt << ",";
			jtxt << "{\"line\":\"" << line << "\"}";
			primera = false;
		}

		jtxt << "]}";
		string retorno = jtxt.str();
		retorno.erase(std::remove_if(retorno.begin(), retorno.end(), &Tools::IsNoAscii), retorno.end());
		return retorno;
		// return jtxt.str();
	}

	return "{\"fecha\": \"\", \"version\": \"\", \"lines\": []}";
}

/** JSON Version Actual y Anterior. La Actual debe venir en Primera Posicion */
string Versiones::cbActualAndPrev(CppSQLite3Query qry)
{
	stringstream  jtxt;
	jtxt << "[";

	/** Analizo las dos primeras si existen... */
	for (int v=0;v<2;v++)
	{
		if (!qry.eof())
		{
			jtxt << "{";
			jtxt << "\"fecha\": \"" << string(qry.fieldValue("fecha")) << "\", ";
			jtxt << "\"version\": \"" << string(qry.fieldValue("idVersion")) << "\",";
			jtxt << "\"std\": \"" << (v==0 ? "Actual" : "Anterior") << "\"}";
			jtxt << (v==0 ? ",": "");
		}
		else
		{
			jtxt << "{";
			jtxt << "\"fecha\": \"" << "---" << "\", ";
			jtxt << "\"version\": \"" << "---" << "\",";
			jtxt << "\"std\": \"" << (v==0 ? "Actual" : "Anterior") << "\"}";
			jtxt << (v==0 ? ",": "");
		}
		qry.nextRow();
	}

	jtxt<<"]";
	return jtxt.str();
}

/** */
string Versiones::cbActualPath(CppSQLite3Query qry)
{
	if (!qry.eof())
		return string(qry.fieldValue("path"));

	return "";
}

/** */
string Versiones::cbCount(CppSQLite3Query qry)
{
	if (!qry.eof())
	{
		int nreg = atoi(qry.fieldValue(0));
		return Tools::Int2String(nreg);
	}
	return "0";
}

/** */
Versiones::Versiones(void)
{
}

/** */
Versiones::~Versiones(void)
{
}

/** */
string Versiones::ExecQuery(string strqry, cbQuery cb)
{
	CppSQLite3DB db;
	CppSQLite3Query qry;
	string retorno = "";
	try
	{
		db.open(_DBNAME);
		qry = db.execQuery(strqry.c_str());
		if (cb != NULL)
			retorno = cb(qry);
		qry.finalize();
		db.close();
	}
	catch(CppSQLite3Exception x)
	{
		PLOG_ERROR("VERSIONES:ExecQuery. CppSQLite3Exception: %s.", x.errorMessage());
	}
	return retorno;
}


/** */
string Versiones::jActualVerbose()
{
	if (ExecQuery("SELECT * FROM swrep WHERE idPos=1;", cbExist)=="no")
	{
		/** Si no existe el primer elemento genero uno por defecto....*/
		ifstream infile(LocalConfig::cfg.PathToVersiones().c_str());
		stringstream buffer;
		buffer << infile.rdbuf();

		PreRegistra("UG5K-REDAN", "0.0.1", ON_SWREP("default"), buffer.str());
		Registra();
	}
	return ExecQuery("SELECT * FROM swrep WHERE idPos=1;", cbActualVerbose);
}

/** */
string Versiones::jActualAndPrev()
{
	return ExecQuery("SELECT * FROM swrep WHERE idPos=1 OR idPos=2 ORDER BY idPos;", cbActualAndPrev);
}

/** */
bool Versiones::TestAndPreRegistra(string idSistema, string idVersion, string path)
{
	if (TestImagen(path))
	{
		PreRegistra(idSistema,idVersion,path,"PreRegistro");
		return true;
	}
	return false;
}

/** */
void Versiones::ActivaDesactivaPreRegistro(bool activa)
{
	if (activa == true)
	{
		Registra();
		ActivarImagen();
	}
	else
	{
		/** Borrar el Preregistro... */
		string path = ExecQuery("SELECT * FROM swrep WHERE idPos=-1;", cbActualPath);
		if (path != "")
		{
			/** Borro el Fichero */
			sistema::ExecuteCommand((char *)sistema::GetDelCommand(path).c_str());
		}
		/** Borro el Registro */
		ExecQuery("DELETE FROM swrep WHERE idPos=-1;", NULL);
	}	
}

/** */
bool Versiones::RestoreLastVersion()
{
	if (Unregistra())
	{
		ActivarImagen();
		return true;
	}
	return false;
}

/** */
bool Versiones::PreRegistra(string idSistema, string idVersion, string path, string verbose)
{
	/** TODO: Controlar que no haya un Preregistro previo */

	stringstream  qry;
	qry << "INSERT INTO swrep (idPos,fecha,idSistema,idVersion,path,strVersiones) VALUES (-1,";
	qry << "'" << Tools::Ahora() << "',";
	qry << "'" << idSistema << "',";
	qry << "'" << idVersion << "',";
	qry << "'" << path << "',";
	qry << "'" << verbose << "');";
	ExecQuery(qry.str(),NULL);
	return true;
}

/** */
bool Versiones::Registra()
{
	/** No Hay un Preregistro */
	if (ExecQuery("SELECT * FROM swrep WHERE idPos=-1;", cbExist)!="si")
		return false;

	if (ExecQuery("SELECT * FROM swrep WHERE idPos=3;", cbExist)=="si")
	{
		/** Existe Tercer elemento lo borro */
		ExecQuery("DELETE FROM swrep WHERE idPos=3;", NULL);
	}
	if (ExecQuery("SELECT * FROM swrep WHERE idPos=2;", cbExist)=="si")
	{
		/** Existe Segundo Elemento. lo pongo como tercero */
		ExecQuery("UPDATE swrep SET idPos=3 WHERE idPos=2;", NULL);
	}
	if (ExecQuery("SELECT * FROM swrep WHERE idPos=1;", cbExist)=="si")
	{
		/** Existe Primer Elemento. lo pongo como segundo */
		ExecQuery("UPDATE swrep SET idPos=2 WHERE idPos=1;", NULL);
	}

	/** Registro como Primero */
	ExecQuery("UPDATE swrep SET idPos=1 WHERE idPos=-1;", NULL);
	return true;
}

/** */
bool Versiones::Unregistra()
{
	int count = atoi(ExecQuery("SELECT COUNT(*) FROM swrep;", cbCount).c_str());
	if (count > 1)
	{
		if (ExecQuery("SELECT * FROM swrep WHERE idPos=1;", cbExist)=="si")
		{
			/** Existe Primer Elemento. lo borro */
			ExecQuery("DELETE FROM swrep WHERE idPos=1;", NULL);
		}
		if (ExecQuery("SELECT * FROM swrep WHERE idPos=2;", cbExist)=="si")
		{
			/** Existe Segundo Elemento. lo pongo como primero */
			ExecQuery("UPDATE swrep SET idPos=1 WHERE idPos=2;", NULL);
		}
		if (ExecQuery("SELECT * FROM swrep WHERE idPos=3;", cbExist)=="si")
		{
			/** Existe Tercer Elemento. lo pongo como segundo */
			ExecQuery("UPDATE swrep SET idPos=2 WHERE idPos=3;", NULL);
		}
		return true;
	}
	return false;
}

/* ... */
bool Versiones::TestImagen(string path)
{
	ifstream imgfile(path.c_str());
	string line;

	/** Valor de la Primera Linea del Fichero */
	if (!getline(imgfile, line) || (line != "#!/bin/bash"))
		return false;

	/** Busco el Limitador del autoextraible... */
	while (getline(imgfile, line))
	{
		if (line.find("#%#%#%#%#%#%#%#%#%#%#%#%#%#%#") !=std::string::npos)
			return true;
	}

	return false;
}

/* ... */
void Versiones::ActivarImagen()
{
	string path = ExecQuery("SELECT * FROM swrep WHERE idPos=1;", cbActualPath);
	if (path != "")
	{
		/** Copiar el fichero a /mnt/ramfs para que lo vea el carper */
		sistema::ExecuteCommand((char *)sistema::GetCpCommand(path, ON_RAM(sistema::PathFileName(path))).c_str());

		/** Personalizar el InstallCd40 con el nombre �nico del Fichero */
		ifstream infile(ON_FLASH("installcd40"));
		stringstream buffer;
		buffer << infile.rdbuf();
		string data = buffer.str();

#define TAG_FILE	"{{InstallationFile}}"

		if (data.find(TAG_FILE) != string::npos)
			data.replace(data.find(TAG_FILE), string(TAG_FILE).length(), sistema::PathFileName(path));
		else
			return;

		ofstream outfile(ON_RAM("installcd40"));
		outfile << data;

		///** Copiar Installcd40 en /mnt/ramfs para que lo vea el carper */
		//sistema::ExecuteCommand((char *)sistema::GetCpCommand(ON_FLASH("installcd40.tmp"), ON_RAM("installcd40")).c_str());
	}
}

/** 
Lleva el versiones.txt a la BDT Registro Ppal
Debe llamarse en el RESET...
*/
void Versiones::ActualizaVersiones()
{
	snmpVersion = sistema::SnmpAgentVersion();
	recVersion = sistema::RecordServiceVersion();

	if (ExecQuery("SELECT * FROM swrep WHERE idPos=1;", cbExist)=="si")
	{
		/** Leeo el Fichero Versiones.txt */
		ifstream infile(LocalConfig::cfg.PathToVersiones().c_str());
		stringstream buffer;
		buffer << infile.rdbuf();

		/** */
		stringstream  qry;
		qry << "UPDATE swrep SET strVersiones=";
		qry << "'" << buffer.str() << "'";
		qry << " WHERE idPos=1;";
		ExecQuery(qry.str(),NULL);
	}
}

/** */
string Versiones::json()
{
	return Versiones::swrep.jActualVerbose();
}
