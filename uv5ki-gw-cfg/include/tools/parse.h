/*class Parse parsing http query */
#ifndef _PARSE_
#define _PARSE_

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <string.h>
#include <algorithm>

#define POST 'P'
#define GET 'G'

#define FAIL_MESSAGE "Fail"
#define FAIL_SPLIT "Error in split function : "

#include "../base/socket.h"
#include "../tools/tools.h"

using namespace std;

class Parse {
public:
  Parse();
  Parse (string s);
  
public:
  void parse();
  void parse(string request);

public:
  string ValueByKey (string) const;
  map<string,string> &Var(void ) {return _variables;}

private:
  char whatIsIt () const; //post or get or some else
  void split (vector<string>&,string,char);
  void setMap (map<string,string>&,vector<string>);

  void parseGet();
  void parsePost();
  void clear();

private:
  vector<string> _query;

  map<string,string> _head; //head in query, example (GET /index.php?x=y&x1=y1 HTTP1.1)
  map<string,string> _variables; //variables in query (x=y&x1=y1...)
  map<string,string> _attributes; //attributes in query (Host : ...\nUser-Agent : ...\n)
};

/** */
class ParseResponse 
{
public:
	ParseResponse();
	ParseResponse(string response);
	ParseResponse(CTCPSocket &sck, int timeout=0);

public:
	string Status(){return _status;};	 
	string Header (string) const;
	string Body(){return _body;}
	string StatusText(){return _status_text;}

	bool Error(){return _error;}

private:
	bool Parse(string response);
	vector<string> Split(string entrada, string sep);
	bool ParseStatus(string line);
	bool ParseHeader(string line);
	string ParseCkunked(vector<string> &data, vector<string>::iterator &it);
	bool Parse(CTCPSocket &sck, int timeout);
	bool ParseCkunked(CTCPSocket &sck, int timeout);
	void SetError(){_error=true;}

private:
	string _status,_httpv, _status_text, _body;
	map<string, string> _headers;
	bool _error;

};

#endif
