#include "../../include/config/comm-preconf.h"

#define _DBNAME (ON_FLASH("ug5k.db"))

/** */
void CommPreconfs::read()
{
	clear_array(preconfs);
	try
	{
		db.open(_DBNAME);
		CppSQLite3Query q = db.execQuery("select * from tbpc;");
		while (!q.eof())
		{
#if __POR_REFERENCIA__
			preconfs.push_back(CommPreconf(q.fieldValue("name"), q.fieldValue("fecha"), q.fieldValue("data")));
#else
			CommPreconf *p_preconf = new CommPreconf(q.fieldValue("name"), q.fieldValue("fecha"), q.fieldValue("data"));
			preconfs.push_back(p_preconf);
#endif

			q.nextRow();
		}
		q.finalize();
		db.close();
	}
	catch(CppSQLite3Exception x)
	{
		PLOG_ERROR("CppSQLite3Exception (configs): %s", x.errorCodeAsString(x.errorCode()));
	}
}

/** */
bool CommPreconfs::get(string name, CommPreconf &item)
{
#if __POR_REFERENCIA__
	vector<CommPreconf>::iterator it;
	for (it=preconfs.begin(); it!=preconfs.end(); it++)
	{
		if ((*it).name == name)
		{
			item = (*it);
			return true;
		}
	}
#else
	vector<CommPreconf *>::iterator it;
	for (it=preconfs.begin(); it!=preconfs.end(); it++)
	{
		if ((*it)->name == name)
		{
			item = *(*it);
			return true;
		}
	}
#endif
	return false;
}

/** */
bool CommPreconfs::put(string name, CommPreconf &cfg)
{
	ostringstream strqry;
	strqry << "UPDATE tbpc SET name='" << name;
	strqry << "', fecha='" << cfg.fecha;
	strqry << "', data='" << cfg.data;
	strqry << "';";
	try
	{
		db.open(_DBNAME);
		db.execQuery(strqry.str().c_str()).finalize();
		db.close();
	}
	catch(CppSQLite3Exception x)
	{
		PLOG_ERROR("CppSQLite3Exception (put): %s", x.errorCodeAsString(x.errorCode()));
		return false;
	}
	read();
	return true;
}

/** */
bool CommPreconfs::pos(string name, CommPreconf &cfg)
{
	ostringstream strqry;

	strqry.clear();
	strqry << "INSERT INTO tbpc (name, fecha, data) VALUES (";
	strqry << "'" << name << "', ";
	strqry << "'" << cfg.fecha << "', ";
	strqry << "'" << cfg.data << "'";
	strqry << ");";
	try
	{
		db.open(_DBNAME);
		db.execQuery(strqry.str().c_str()).finalize();
		db.close();
	}
	catch(CppSQLite3Exception x)
	{
		PLOG_ERROR("CppSQLite3Exception (pos): %s", x.errorCodeAsString(x.errorCode()));
		return false;
	}
	read();
	return true;
}

/** */
bool CommPreconfs::del(string name)
{		
	ostringstream strqry;
	strqry << "DELETE FROM tbpc where name='" << name << "';";
	try
	{
		db.open(_DBNAME);
		db.execQuery(strqry.str().c_str()).finalize();
		db.close();
	}
	catch(CppSQLite3Exception x)
	{
		PLOG_ERROR("CppSQLite3Exception (del): %s", x.errorCodeAsString(x.errorCode()));
		return false;
	}
	read();
	return true;
}

/** */
int CommPreconfs::Count() 
{
	int registros = 0;
	ostringstream strqry;
	strqry << "SELECT COUNT(*) FROM tbpc;";
	try
	{
		db.open(_DBNAME);
		registros = db.execScalar(strqry.str().c_str());
		db.close();
	}
	catch(CppSQLite3Exception x)
	{
		PLOG_ERROR("CppSQLite3Exception (del): %s", x.errorCodeAsString(x.errorCode()));
	}
	return registros;
}

/** */
bool CommPreconfs::Exist(string name) 
{
	CommPreconf cfg;
	return get(name, cfg);
}

