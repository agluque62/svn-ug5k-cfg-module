#include "../../include/tools/http-client.h"


/** */
void HttpClient::ParseHost(string host, string &ip, int &port)
{
	vector<string> partes;
	Tools::split(partes, host, ':');
	if (partes.size() == 1) {
		ip = host;
		port = 80;
	} else {
		ip = partes[0];
		port = atoi(partes[1].c_str());
	}
}

/** */
ParseResponse HttpClient::SendHttpCmd(string cmd, int ms_timeout)
{
	string ip;
	int port;
	ParseHost(server, ip, port);
	CIPAddress host(ip, port);
	CTCPSocket sck;

	Tools::Trace("HttpClient::SendHttpCmd CmdSize %d.", sizeof(cmd));

	try
	{
		if (!sck.Connect(host, conn_timeout))
			throw Exception("No puedo conectarme al HOST: " + server);
		if (sck.Send(cmd.c_str(), cmd.length()) != (int) cmd.length())
			throw Exception("Error al Enviar request: " + cmd);

		ParseResponse respuesta(sck, ms_timeout);
		Tools::Trace("HttpClient::SendHttpCmd ResponseSize %d.", sizeof(respuesta));
		sck.Close();

		return respuesta;

	} 
	catch (socket_error e) 
	{
		// throw Exception(e);
		throw Exception("Error al Conectarme al HOST " + server + ": " + e.Message(), e.Code());
	}
}

/** */
ParseResponse HttpClient::SendHttpCmd(string metodo, string cmd, int ms_timeout, string jdata)
{
	string line1 = metodo + " /" + cmd + " HTTP/1.1\r\n";
	string line2 = "Host: " + server + "\r\n";
	string line3 = "Content-Type: application/json\r\n";
	string line4 = "Content-Length: " + Tools::itoa(jdata.length()) + "\r\n";

	string request = line1 + line2 + line3 + line4 + "\r\n" + jdata + "\r\n";
	// string request = metodo + "/" + cmd + " HTTP/1.1\r\nHost: " + server + "\r\nContent-Type: application/json\r\n\r\n" + jdata + "\r\n";

	return SendHttpCmd(request, ms_timeout);
}

/** Testing */
void HttpClient::TestChunkResponse()
{
	string request = "GET /ntpstatus  HTTP/1.1\r\nHost: 192.168.0.53:8080\r\nContent-Type: application/json\r\n\r\n";
	ParseResponse response = SendHttpCmd(request, 5000);
}
