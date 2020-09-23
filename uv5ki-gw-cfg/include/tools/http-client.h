#ifndef _HTTP_CLIENT_
#define _HTTP_CLIENT_
#include <string>
#include <stdio.h>
#include "../base/code-base.h"
#include "../base/thread.h"
#include "../base/socket.h"
#include "../config/local-config.h"
#include "./tools.h"
#include "./parse.h"

using namespace std;

/** */
class HttpClient : public CodeBase
{
public:
	HttpClient(string host="localhost", int port = 80) {
		server = host + ":" + Tools::itoa(port);
		char_timeout = LocalConfig::p_cfg->getint(strRuntime, strRuntimeItemHttpCharTimeout, "100");
		conn_timeout = LocalConfig::p_cfg->getint(strRuntime, strRuntimeItemTcpConnTimeout, "60");
	}
	~HttpClient(void){}

public:
#ifdef _POINTER_TO_RESPONSE_
	ParseResponse SendHttpCmd(string cmd, int ms_timeout);
	ParseResponse SendHttpCmd(string metodo, string cmd, int ms_timeout, string jdata="");
#else
	void SendHttpCmd(string cmd, ParseResponse *httpResp, int ms_timeout);
	void SendHttpCmd(string metodo, string cmd, ParseResponse* httpResp, int ms_timeout, string jdata = "");
#endif

public:
	static void ParseHost(string host, string &ip, int &port);

	/** Testing */
	void TestChunkResponse();

private:
	string server;
	int char_timeout;
	int conn_timeout;
};

#endif
