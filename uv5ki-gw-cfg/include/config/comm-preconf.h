#ifndef _COMM_PRECONF_H
#define _COMM_PRECONF_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream> 

#include "../base/code-base.h"
#include "../base/sistema.h"
#include "../base/sqlite3++.h"
#include "../websrv/uv5kigwcfg-web-app-data.h"
#include "../config/local-config.h"

using namespace std;
#define _PRECONF_VACIA_		"vacia"

/**
*/
class CommPreconf : public jData
{
public:
	CommPreconf() {
		name = _PRECONF_VACIA_;
		fecha = data = "";
	}
	CommPreconf(string &jdata) {
		JDeserialize(jdata);
	}
	CommPreconf(string nameIn, string fechaIn, string dataIn="") {
		name = nameIn;
		fecha=fechaIn;
		data=dataIn;
	}
	~CommPreconf(void){
	}
public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "name", name);
		write_key(writer, "fecha", fecha);
	}
	virtual void jread(Value &base) {
		read_key(base, "name", name);
		read_key(base, "fecha", fecha);
	}
public:
	string name;
	string fecha;
public:
	string data;
};

/**
*/
class CommPreconfs : public jData
{
public:
	CommPreconfs() {
		read();
	}
	~CommPreconfs() {
		clear_array(preconfs);
	}

public:
	virtual void jwrite(Writer<StringBuffer> &writer) {
		write_key(writer, "preconfs", preconfs);
	}
	virtual void jread(Value &base) {}

public:
	bool get(string name, CommPreconf &item);
	bool put(string name, CommPreconf &val);
	bool pos(string name, CommPreconf &val);
	bool del(string name);
	int Count();
	bool Exist(string name);

protected:
	CppSQLite3DB db;
	void read();

public:
#if __POR_REFERENCIA__
	vector<CommPreconf> preconfs;
#else
	vector<CommPreconf *> preconfs;
#endif
};

#endif
