#if !defined(MISOCKET_H__INCLUDED_)
#define MISOCKET_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#ifdef _WIN32
 #include <winsock.h>
 //#include <winsock2.h>
 //#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
	#define SD_RECEIVE      0x00
	#define SD_SEND         0x01
	#define SD_BOTH         0x02
#else
	#include <sys/types.h>     /*  Solaris 2.5.1 fix: u_short, required by sys/socket.h */
	#include <sys/socket.h>    /*  sockets */
    #include <sys/fcntl.h>
	#include <sys/select.h>
	#include <sys/time.h>      /*  timeval */
	#include <sys/ioctl.h>     /*  ioctl  */
	#include <string.h>        /*  bzero, for FD_SET */
	#include <strings.h>       /*  bzero, for FD_ZERO (AIX) */
	#include <netinet/in.h>    /*  INADDR_*, in_addr, sockaddr_in, htonl etc. */
	#include <netdb.h>         /*  getservbyname */
	#include <arpa/inet.h>     /*  inet_addr */
	#include <errno.h>         /*  socket error codes */

	#define SD_RECEIVE			SHUT_RD
	#define SD_SEND				SHUT_WR
	#define SD_BOTH				SHUT_RDWR

	#define INVALID_SOCKET		-1
	#define SOCKET_ERROR		-1
	#define WSAENOTSOCK			1
	#define WSAECONNRESET		2

#endif
#include "code-base.h"

/** */
class CIPAddress;
class CUDPSocket;

/** */
class CInitSocket
{
private:
    static long s_count;
    static CInitSocket s_is;

public:
	static char ipLocal[32];

public:
    CInitSocket();
    CInitSocket(const CInitSocket &is);
    ~CInitSocket();
};

//////////////////////////////
class socket_error : public Exception
{
protected:
    virtual const char *Prefix() const
    {
#ifdef _WIN32
        return "WSAE";
#else
        return "LSE";
#endif
    }

public:
    socket_error(const char *s="")
#ifdef _WIN32
        : Exception(s)
#else
		: Exception(s)	// obtener error.
#endif
    {
    }

    socket_error(int lastError, const char *s="")
		: Exception(s, lastError)
    {
    }

//    bool WouldBlock() const
//    {
//#ifdef _WIN32
//        return Code()==WSAEWOULDBLOCK;
//#else
//		return false;		// todo. obtener este valor en linux
//#endif
//    }
};

/** */
inline void TestSocket(int res, const char *txt)
{
#ifdef _WIN32
    if (res==SOCKET_ERROR)
        throw socket_error(txt);
#else
	if (res < 0)
	{
		throw socket_error(errno, txt);
	}
#endif
}

//////////////////////////////
class CIPAddress
{
public:
    static const char BROADCAST[], ANY[], LOOPBACK[], NONE[];

    CIPAddress(const sockaddr_in *sockaddr);
    CIPAddress(const char *name=NONE, int port=0);
    CIPAddress(const std::string &name, int port=0);
    CIPAddress(const char *name, const char *serv, const char *proto=NULL);
    CIPAddress(unsigned long dwIp, int port=0);

    operator const sockaddr*() const;
    operator sockaddr_in*();
    int size() const
    { return sizeof(m_addr); }

    bool IsAssigned() const;
    const std::string &GetHostName() const;
    const std::string &GetRealHostName() const;
    const in_addr &GetAddress() const;
    const std::string GetStringAddress() const;
    unsigned short GetPort() const;
	bool operator == (CIPAddress &in);	// {return TRUE;}

private:
    CInitSocket m_is;

    void Init(const char *name, int portNBO);
    sockaddr_in m_addr;

    mutable bool m_realHostName;  // mutable por GetRealHostName
    mutable std::string m_hostname; // mutable por GetRealHostName

};

template<class E, class T>
    std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T> &os, const CIPAddress &addr)
{
    return os << addr.GetHostName() << E(':') << addr.GetPort();
}

/////////////////////////////////////////////////////////////////////////////
// class CSocket

	class CSocket : public CodeBase
{
public:
    void NewSocket();
    void Bind(int port);
    void Bind(const CIPAddress &addr);
	virtual ~CSocket();

    bool Connect(const CIPAddress &addr);
    bool Connect(const CIPAddress &addr, int timeout);

    int SendTo(const void *buf, int sz, const CIPAddress &addr);
    int RecvFrom(void *buf, int sz, CIPAddress *addr); // si addr es NULL no devuelve la direcci�n

    int Send(const void *buf, int sz);
    int Recv(void *buf, int sz, int _flags=0);

    const CIPAddress &GetLocalAddress() const;
    const CIPAddress GetRemoteAddress() const;
    void Close();
    void Shutdown(int how=SD_SEND);  // SD_SEND SD_RECEIVE SD_BOTH

    void SetSockOpt(int level, int optname, const void *optval, int optlen);
    void GetSockOpt(int level, int optname, void *optval, int *optlen);
    void GetSockOpt(int level, int optname, void *optval, int optlen); // sin puntero en optlen

	void SetRecvTimeout(int sec);

    // Funciones de 'select'. timeout en milisegundos: si es 0 -> no bloqueante, si es -1 -> espera infinita
    // int Select(FD_SET *fr, FD_SET *fw, FD_SET *fe, int timeout=0) const;
    int Select(fd_set *fr, fd_set *fw, fd_set *fe, int timeout=0) const;
    bool IsReadable(int timeout=0) const; 
    bool IsWritable(int timeout=0) const;
    bool IsException(int timeout=0) const;
	// AGL.
	void SetReusable();
	void JoinMulticastGroup(CIPAddress grupo, CIPAddress itf=CIPAddress::ANY);

    int GetHandle()
    { return m_socket; }

	/*
    bool IsValid() const
    { 
		return m_socket!=INVALID_SOCKET; 
	}
	*/
	CIPAddress GetHostAddress(int nPort = 0);

protected:
    CSocket();
    virtual void Init() =0;
	int m_socket;

private:
    CInitSocket m_is;
    mutable CIPAddress m_localAddr, m_remoteAddr;  // mutable por GetLocalName, GetRemoteName
    CSocket(const CSocket &sck); // los sockets no se copian
    CSocket &operator=(const CSocket &sck); // los sockets no se asignan
};

template<class E, class T>
    std::basic_ostream<E, T>& operator<<(std::basic_ostream<E, T> &os, const CSocket &sck)
{
    os << sck.GetLocalAddress() << E('-') << E('>') << sck.GetRemoteAddress();
    return os;
}

/////////////////////////////////////////////////////////////////////////////
// class CUDPSocket

class CUDPSocket : public CSocket
{
public:
	explicit CUDPSocket(bool broadcast=false);      // no hace bind
	explicit CUDPSocket(int localport, bool broadcast=false);  // localport=0 es un puerto cualquiera
	explicit CUDPSocket(const CIPAddress &local, bool broadcast=false);
public:
	void SetBroadcast(void );

protected:
    virtual void Init();
private:
    bool m_broadcast;
};

/////////////////////////////////////////////////////////////////////////////
// class CTCPSocket
class CTCPSocket : public CSocket
{
public:
	CTCPSocket();
	explicit CTCPSocket(const CIPAddress &remote);
	CTCPSocket(const CIPAddress &local, const CIPAddress &remote);

    void Write(const void *buf, int sz);  // escribe exactamente sz bytes (lo mismo que Send)
    void Read(void *buf, int sz);         // lee exactamente sz bytes: si hay menos, espera; si se cierra la conexi�n lanza el error WSAECONNRESET)
	// AGL.
	int ReadLine(string &line, int timeout=0);
	int Recv_text(string &text, int len, int timeout=0);
	int Recv_http(string &text, int timeout=5000);
	int Recv_bin(vector<byte> &buffer, int timeout=0, int char_timeout=10);
protected:
    virtual void Init();
private:
    friend class CTCPServerSocket;
	CTCPSocket(int sck);  // para los sockets del accept
};

/////////////////////////////////////////////////////////////////////////////
// class CTCPServerSocket
class CTCPServerSocket : public CTCPSocket
{
public:
    CTCPServerSocket();
	explicit CTCPServerSocket(int localport);
	explicit CTCPServerSocket(const CIPAddress &local);

    void Listen(int backlog=SOMAXCONN);
    CTCPSocket *Accept(int backlog=SOMAXCONN);
    void Accept(CTCPSocket &sck, int backlog=SOMAXCONN);
protected:
    virtual void Init();
private:
    int m_listenBacklog;

    int AcceptPriv(int backlog);
};



// }  //namespace sck

#endif // !defined(AFX_MISOCKET_H__7062F444_6603_487E_993C_747B875FBDE0__INCLUDED_)
