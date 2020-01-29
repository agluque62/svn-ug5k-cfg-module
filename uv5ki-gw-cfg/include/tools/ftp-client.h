#ifndef _FTP_CLIENT_
#define _FTP_CLIENT_
#include <string>
#include <stdio.h>
#include "../base/code-base.h"
#include "../base/thread.h"
#include "../base/socket.h"
#include "../config/local-config.h"

using namespace std;

/** Excepcion especifica para FTP... */
class FtpClientException : public Exception
{
public:
	FtpClientException(string msg, string data="")		
	{
		char buff[256];
#ifdef _WIN32
		_snprintf_s(buff, sizeof(buff), "%s%s (Network Error: %d)", msg.c_str(), data.c_str(), WSAGetLastError());
#else
		snprintf(buff, sizeof(buff), "%s%s (Network Error: %d)", msg.c_str(), data.c_str(), errno);
#endif	
		_msg=string(buff);
	}
	FtpClientException(socket_error error)
	{
		_msg = "Socket Error: " + string(error.what());
	}
        ~FtpClientException(void)throw(){}
        
private:
	string _msg;
public:
#if defined _WIN32
	virtual const string what() {return _msg.c_str();}
#elif defined __APPLE__
	virtual const string what() const _NOEXCEPT {return _msg.c_str();}
#elif defined _PPC82xx_
	virtual const string what() {return _msg.c_str();}
#else
	virtual const string what() {return _msg.c_str();}
#endif

};

/** */
class FtpClient : public CodeBase
{
public:
	FtpClient(string host="localhost", string user="anonymous", string pwd="anonymous@anonymous.net", int port = 21);
	~FtpClient(void);

public:
	void Login();
	void Close();
	void Donwload(string rFile, string lFile);
	void Upload(string lFile, string rFile);
	void Delete(string rFile);
	void ChangeDir(string rDir);
#if !defined(_PPC82xx_)
	void Test();
	static void *ftpTest(void *arg);
#endif

protected:
	int  readResponse(string &data, int timeout=1000);
	void readLine(string &line, int timeout);
	bool sendCommand(string cmd, int &rcode, string &response);
	bool createDataSocket();
	void cleanup();
	void sendFile(string filename);

private:
	string server;
	int port;
	string username;
	string password;

private:
	CTCPSocket *pCtrl_Sck;
	CTCPServerSocket *pData_Sck;
	bool loggedin;

private:
	string respuesta;
	int retcode;
};

#endif
