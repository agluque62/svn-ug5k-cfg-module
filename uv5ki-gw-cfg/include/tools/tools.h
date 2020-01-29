#ifndef _TOOLS_
#define _TOOLS_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <string>
#include <list>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>

#ifdef _WIN32
	#include <Windows.h>
	#pragma warning (disable:4996) 
	#include "../base/dirent.h"
#else
	#include <dirent.h>
#endif

using namespace std;

#include "../base/code-base.h"

/** */
class url 
{
public:
	  // omitted copy, ==, accessors, ...
    url(const string& url_s) {
		parse(url_s);
	}
private:
    void parse(const string& url_s);

public:
    string protocol_, host_, path_, query_;
};

/** */
class Tools : public CodeBase
{
public:
	Tools(void){}
	~Tools(void){}
public:
	template< typename T > static std::string hexify(T i, bool prefijo);
public:
	static bool EsIgual(const char * str1, const char * str2);
	static bool FileExist(const char *filename);
	static char * Hoy(void);
	static bool HttpQuerySplit(const char *qry, string & prop, string & value);
	static string & replace(string & subj, string old, string neu);
	static string DateCookie(int seconds);
	static string HttpDecodeSpecialChars(string &src);
	static bool ValidateIpAddress(const string &ipAddress);
	static string itoa(int i);
	static int atoi(string str);
	static float atof(string str);
	static void Dir(vector<string> &lista, string path, string msc);
	static void DelFile(string path, string file);

public:
	static void split(std::vector<std::string> &tokens, const std::string &text, char sep);
	static bool StrContains(std::string text, std::string valor);
	static std::string StrFormat ( const char *fmt, ... );
	static std::string Ahora();
	static std::string Ahora_Servidor();
	static std::string Buff2String(int len, const char *data);
	static std::string Int2String(int val);
	static bool DateString2time(std::string str, struct tm &stime);
	static std::string ProgramUniqueId();
	static std::string FileUniqueName(std::string fname);
	static std::string CookieTime(char *buffer, struct tm stime);
	static bool IsNoAscii(char c);
	static string read_txt_file(string name);
	static void stringofint_to_vectorint(string inputstring, char sep, vector<int> &output);

	static bool ip_format_test(string ip);
	static void tm2String(struct tm* timeptr, string format, string &date);

	static void append2file(string name, string msg);

	static void fatalerror(string msg);

};

#endif
