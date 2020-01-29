// MiSocket.cpp : implementation file
//
#include "../../include/base/socket.h"

#include <string>
#include <sstream>
#include <memory>


long CInitSocket::s_count = -1;
CInitSocket CInitSocket::s_is;			// para que la dll est� cargada todo el tiempo que dure la funci�n WinMain
char CInitSocket::ipLocal[32];

////////////////////////////////////////////////////////////////////////////////
// CInitSocket
CInitSocket::CInitSocket()
{
#ifdef _WIN32
	if (InterlockedIncrement(&s_count)==0)
	{
		WSADATA wsaData;
		int res = WSAStartup(MAKEWORD(2,0), &wsaData);
		// AGL
		char name[255];
		PHOSTENT hostinfo;
		if( gethostname ( name, sizeof(name)) == 0)
		{
			if((hostinfo = gethostbyname(name)) != NULL)
			{
				strncpy_s(ipLocal,inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list),sizeof(ipLocal));
			}
		}
		_ASSERT(res==0);
	}
#else
	// todo. Inicializacioin del sistema de sockets..
/*
	char name[255];
	PHOSTENT hostinfo;
	if( gethostname ( name, sizeof(name)) == 0)
	{
		if((hostinfo = gethostbyname(name)) != NULL)
		{
			strncpy(ipLocal,inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list),sizeof(ipLocal));
		}
	}
	*/
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
CInitSocket::CInitSocket(const CInitSocket &is)
{
#ifdef _WIN32
	InterlockedIncrement(&s_count); // no puede ser el primero, porque ya existe is
#else
	// todo
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
CInitSocket::~CInitSocket()
{
#ifdef _WIN32
	if (InterlockedDecrement(&s_count)<0)
	{                      
		int res = WSACleanup();
		_ASSERT(res==0);
	}
#else
	// todo
#endif
}

////////////////////////////////////////////////////////////////////////////////
// CIPAddress
////////////////////////////////////////////////////////////////////////////////
//
const char CIPAddress::BROADCAST[]  = {"<broadcast>"};
const char CIPAddress::ANY[]        = {"<any>"};
const char CIPAddress::LOOPBACK[]   = {"<localhost>"};
const char CIPAddress::NONE[]       = {"<none>"};

////////////////////////////////////////////////////////////////////////////////
//
CIPAddress::CIPAddress(const sockaddr_in *sockaddr)
{
#ifdef _WIN32
	_ASSERT(sockaddr->sin_family==AF_INET);
#else
	// todo
#endif

	m_addr = *sockaddr;
	m_realHostName = false;

		// broadcast y none tienen la misma direcci�n!!!
#ifdef _WIN32
	if (m_addr.sin_addr.S_un.S_addr==INADDR_BROADCAST)
		m_hostname = BROADCAST;
	else if (m_addr.sin_addr.S_un.S_addr==INADDR_ANY)
		m_hostname = ANY;
	else if (m_addr.sin_addr.S_un.S_addr==INADDR_LOOPBACK)
		m_hostname = LOOPBACK;
	else if (m_addr.sin_addr.S_un.S_addr==INADDR_NONE)
		m_hostname = NONE;
	else
		m_hostname = GetStringAddress();
#else
	if (m_addr.sin_addr.s_addr==INADDR_BROADCAST)
		m_hostname = BROADCAST;
	else if (m_addr.sin_addr.s_addr==INADDR_ANY)
		m_hostname = ANY;
	else if (m_addr.sin_addr.s_addr==INADDR_LOOPBACK)
		m_hostname = LOOPBACK;
	else if (m_addr.sin_addr.s_addr==INADDR_NONE)
		m_hostname = NONE;
	else
		m_hostname = GetStringAddress();

#endif


}

////////////////////////////////////////////////////////////////////////////////
//
CIPAddress::CIPAddress(const char *name/*=NONE*/, int port/*=0*/)
{
	Init(name, htons(port));
}

////////////////////////////////////////////////////////////////////////////////
//
CIPAddress::CIPAddress(const std::string &name, int port/*=0*/)
{
	Init(name.c_str(), htons(port));
}

////////////////////////////////////////////////////////////////////////////////
//
CIPAddress::CIPAddress(const char *name, const char *serv, const char *proto/*=NULL*/)
{
	servent *se = getservbyname(serv, proto);
	if (!se)
	   throw socket_error("getservbyname");

	Init(name, se->s_port);
}

////////////////////////////////////////////////////////////////////////////////
//
CIPAddress::CIPAddress(unsigned long dwIp, int port)
{
	memset(&m_addr, 0, sizeof(m_addr));

	m_addr.sin_family = AF_INET;
	m_addr.sin_port   = htons(port);
#ifdef _WIN32
	m_addr.sin_addr.S_un.S_addr = htonl(dwIp);
#else
	m_addr.sin_addr.s_addr = htonl(dwIp);
#endif

	m_realHostName = false;
	m_hostname = GetStringAddress();
}

////////////////////////////////////////////////////////////////////////////////
//
void CIPAddress::Init(const char *name, int portNBO)
{
	memset(&m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;

	if (portNBO)
		m_addr.sin_port = portNBO;

	if (!name)
		name = NONE;
	if (strcmp(name,"")==0)
	{
		name = NONE;
		throw socket_error("gethostbyname");
	}

	m_hostname = name;
	m_realHostName = false; //todav�a no se sabe el nombre real

#ifdef _WIN32
	if (m_hostname==BROADCAST)
		m_addr.sin_addr.S_un.S_addr = INADDR_BROADCAST;
	else if (m_hostname==ANY)
		m_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	else if (m_hostname==LOOPBACK)
		m_addr.sin_addr.S_un.S_addr = INADDR_LOOPBACK;
	else if (m_hostname==NONE)
		m_addr.sin_addr.S_un.S_addr = INADDR_NONE;
	else
	{
		m_addr.sin_addr.S_un.S_addr = inet_addr(name);
		if (m_addr.sin_addr.S_un.S_addr!=INADDR_NONE)
			;  // es una direccion a.b.c.d, no se conoce el nombre, y no se busca hasta que se pida
		else
		{    // es un nombre, as� que hay que resolver la direcci�n
			hostent *he = gethostbyname(name);
			if (!he)
				throw socket_error("gethostbyname");
			memcpy(&m_addr.sin_addr, he->h_addr_list[0], he->h_length);
			m_hostname = he->h_name;
			m_realHostName = true;
		}
	}
#else
	if (m_hostname==BROADCAST)
		m_addr.sin_addr.s_addr = INADDR_BROADCAST;
	else if (m_hostname==ANY)
		m_addr.sin_addr.s_addr = INADDR_ANY;
	else if (m_hostname==LOOPBACK)
		m_addr.sin_addr.s_addr = INADDR_LOOPBACK;
	else if (m_hostname==NONE)
		m_addr.sin_addr.s_addr = INADDR_NONE;
	else
	{
		m_addr.sin_addr.s_addr = inet_addr(name);
		if (m_addr.sin_addr.s_addr!=INADDR_NONE)
			;  // es una direccion a.b.c.d, no se conoce el nombre, y no se busca hasta que se pida
		else
		{    // es un nombre, as� que hay que resolver la direcci�n
			hostent *he = gethostbyname(name);
			if (!he)
				throw socket_error("gethostbyname");
			memcpy(&m_addr.sin_addr, he->h_addr_list[0], he->h_length);
			m_hostname = he->h_name;
			m_realHostName = true;
		}
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
CIPAddress::operator const sockaddr *() const
{
	return reinterpret_cast<const sockaddr*>(&m_addr);
}

////////////////////////////////////////////////////////////////////////////////
//
CIPAddress::operator sockaddr_in *() 
{
	return (&m_addr);
}

////////////////////////////////////////////////////////////////////////////////
//
bool CIPAddress::operator == (CIPAddress &o)
{
	if (GetPort() != o.GetPort())
		return false;

	return m_hostname == o.GetHostName();
}

////////////////////////////////////////////////////////////////////////////////
//
const std::string &CIPAddress::GetRealHostName() const
{
	if (!m_realHostName)
	{
		hostent *he = gethostbyaddr(reinterpret_cast<const char*>(&m_addr.sin_addr), sizeof(m_addr.sin_addr), AF_INET);
		if (!he)
			throw socket_error("gethostbyaddr");
		m_hostname = he->h_name;
		m_realHostName = true;
	}

	return m_hostname;
};

////////////////////////////////////////////////////////////////////////////////
//
bool CIPAddress::IsAssigned() const
{
#ifdef _WIN32
	return m_addr.sin_addr.S_un.S_addr!=INADDR_NONE;
#else
	return m_addr.sin_addr.s_addr!=INADDR_NONE;
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
const std::string &CIPAddress::GetHostName() const
{
	return m_hostname;
}

////////////////////////////////////////////////////////////////////////////////
//
const in_addr &CIPAddress::GetAddress() const
{
	return m_addr.sin_addr;
}

////////////////////////////////////////////////////////////////////////////////
//
const std::string CIPAddress::GetStringAddress() const
{
	std::ostringstream os;
	unsigned char *byte = (unsigned char *)(&m_addr.sin_addr);
#ifdef _WIN32
	os << (int)(unsigned char)m_addr.sin_addr.S_un.S_un_b.s_b1 << '.' << (int)(unsigned char)m_addr.sin_addr.S_un.S_un_b.s_b2 << '.'
	   << (int)(unsigned char)m_addr.sin_addr.S_un.S_un_b.s_b3 << '.' << (int)(unsigned char)m_addr.sin_addr.S_un.S_un_b.s_b4;
#else
	// todo..
	os << (int)byte[0] << '.' << (int)byte[1] << '.' << (int)byte[2] << '.' << (int)byte[3];

#endif
	return os.str();
}

////////////////////////////////////////////////////////////////////////////////
//
unsigned short CIPAddress::GetPort() const
{
	return ntohs(m_addr.sin_port);
}

/////////////////////////////////////////////////////////////////////////////
// CSocket
////////////////////////////////////////////////////////////////////////////////
//
CSocket::CSocket()
	: m_socket(-1)
{
}

////////////////////////////////////////////////////////////////////////////////
//
void CSocket::NewSocket()
{
	Close();
	Init();
}

////////////////////////////////////////////////////////////////////////////////
//
void CSocket::Bind(int port)
{
	CIPAddress addr(CIPAddress::ANY, port);
	Bind(addr);
}

////////////////////////////////////////////////////////////////////////////////
//
void CSocket::Bind(const CIPAddress &addr)
{
	int res = bind(m_socket, addr, addr.size());
	TestSocket(res, "bind");
}

////////////////////////////////////////////////////////////////////////////////
//
CSocket::~CSocket()
{
	if (m_socket != INVALID_SOCKET)
	{
#ifdef _WIN32
		TestSocket(closesocket(m_socket), "close");
#else
		TestSocket(close(m_socket), "close");
#endif
	}
}

////////////////////////////////////////////////////////////////////////////////
//
int CSocket::SendTo(const void *buf, int sz, const CIPAddress &addr)
{
	if (m_socket==INVALID_SOCKET)
		throw socket_error(WSAENOTSOCK, "SendTo");

	int num = sendto(m_socket, static_cast<const char*>(buf), sz, 0, addr, addr.size());
	TestSocket(num, "sendto");
	return num;
}

////////////////////////////////////////////////////////////////////////////////
//
int CSocket::RecvFrom(void *buf, int sz, CIPAddress *addr)
{
	if (m_socket==INVALID_SOCKET)
		throw socket_error(WSAENOTSOCK, "RecvFrom");

	sockaddr_in fromaddr;
	int len = sizeof(fromaddr);
	int num;
	char *pBuf = static_cast<char*>(buf);
	// int err = 0;
#ifdef _WIN32
	num = recvfrom(m_socket, pBuf, sz, 0, reinterpret_cast<sockaddr*>(&fromaddr), &len);
#else
	num = recvfrom(m_socket, pBuf, sz, 0, reinterpret_cast<sockaddr*>(&fromaddr), (socklen_t *)&len);
#endif
	TestSocket(num, "recvfrom");

	if (addr)
		*addr = CIPAddress(&fromaddr);

	return num;
}

////////////////////////////////////////////////////////////////////////////////
//
bool CSocket::Connect(const CIPAddress &addr)
{
	if (m_socket==INVALID_SOCKET)
		throw socket_error(WSAENOTSOCK, "Connect");

	int res = connect(m_socket, addr, addr.size());
	
	// TestSocket(res, "connect");
	return res==0 ? true : false;
}

////////////////////////////////////////////////////////////////////////////////
//
bool CSocket::Connect(const CIPAddress &addr, int timeout)
{
	if (m_socket==INVALID_SOCKET)
		throw socket_error(WSAENOTSOCK, "Connect");

#ifdef _WIN32
	/*
    TIMEVAL Timeout;
    Timeout.tv_sec = timeout;
    Timeout.tv_usec = 0;

	//set the socket in non-blocking
    unsigned long iMode = 1;
    int iResult = ioctlsocket(m_socket, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
        throw socket_error(iResult, "ioctlsocket failed with error: ");

	int res = connect(m_socket, addr, addr.size());
	if (res != 0)
		return false;
	if (connect(m_socket, addr, addr.size()) != 0)
		return false;

   // restart the socket mode
    iMode = 0;
	iResult = ioctlsocket(m_socket, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
        throw socket_error(iResult, "ioctlsocket failed with error: ");
 
    fd_set Write, Err;
    FD_ZERO(&Write);
    FD_ZERO(&Err);
	FD_SET(m_socket, &Write);
	FD_SET(m_socket, &Err);
 
    // check if the socket is ready
    select(0,NULL,&Write,&Err,&Timeout);			
	if(FD_ISSET(m_socket, &Write)) 
        return true;
 
    return false;
//
*/
	return Connect(addr);
#else
	int res, valopt; 
	long arg; 
	fd_set myset; 
	struct timeval tv; 
	socklen_t lon; 

	// Set non-blocking 
	arg = fcntl(m_socket, F_GETFL, NULL); 
	arg |= O_NONBLOCK; 
	fcntl(m_socket, F_SETFL, arg); 

	// Trying to connect with timeout 
	res = connect(m_socket, addr, addr.size());

	if (res < 0) 
	{ 
		if (errno == EINPROGRESS) 
		{ 
			tv.tv_sec = timeout; 
			tv.tv_usec = 0; 
			FD_ZERO(&myset); 
			FD_SET(m_socket, &myset); 
			if (select(m_socket+1, NULL, &myset, NULL, &tv) > 0) 
			{ 
				lon = sizeof(int); 
				getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon); 
				if (valopt) {
					errno = valopt;
					throw socket_error("Error in connect");
				}
			} 
			else {
				errno = ETIMEDOUT;
				throw socket_error("Timeout on connect");
			}
		} 
		else 
			throw socket_error(errno, "Error connecting");
	} 
	// Set to blocking mode again... 
	arg = fcntl(m_socket, F_GETFL, NULL); 
	arg &= (~O_NONBLOCK); 
	fcntl(m_socket, F_SETFL, arg); 
	// I hope that is all 
	return true;

#endif
  
	//// TestSocket(res, "connect");
	//return res==0 ? true : false;
}

////////////////////////////////////////////////////////////////////////////////
//
int CSocket::Send(const void *buf, int sz)
{
	if (m_socket==INVALID_SOCKET)
		throw socket_error(WSAENOTSOCK, "Send");

	int num = send(m_socket, static_cast<const char*>(buf), sz, 0);
	// TestSocket(num, "send");
	return num;
}

////////////////////////////////////////////////////////////////////////////////
//
int CSocket::Recv(void *buf, int sz, int _flags)
{ // Ojo: puede dar excepciones en UDP de vez en cuando, WSAECONNRESET
	if (m_socket==INVALID_SOCKET)
		throw socket_error(WSAENOTSOCK, "Recv");

	int num;
	char *pBuf = static_cast<char*>(buf);
// 	int err = 0;
	    
	num = recv(m_socket, pBuf, sz, _flags);
	// TestSocket(num, "recv");
	    
	return num;
}


////////////////////////////////////////////////////////////////////////////////
//
const CIPAddress &CSocket::GetLocalAddress() const
{
	if (!m_localAddr.IsAssigned())
	{
		sockaddr_in name;
		int len = sizeof(name);
#ifdef _WIN32
		if (getsockname(m_socket, reinterpret_cast<sockaddr*>(&name), &len)!=SOCKET_ERROR)
			m_localAddr = CIPAddress(&name);
#else
		if (getsockname(m_socket, reinterpret_cast<sockaddr*>(&name), (socklen_t *)&len)!=SOCKET_ERROR)
			m_localAddr = CIPAddress(&name);
#endif

	}
	return m_localAddr;
}

////////////////////////////////////////////////////////////////////////////////
//
const CIPAddress CSocket::GetRemoteAddress() const
{
	if (!m_remoteAddr.IsAssigned())
	{
		sockaddr_in name;
		int len = sizeof(name);
#ifdef _WIN32
		if (getpeername(m_socket, reinterpret_cast<sockaddr*>(&name), &len)!=SOCKET_ERROR)
			m_remoteAddr = CIPAddress(&name);
#else
		if (getpeername(m_socket, reinterpret_cast<sockaddr*>(&name), (socklen_t *)&len)!=SOCKET_ERROR)
			m_remoteAddr = CIPAddress(&name);
#endif
	}
	return m_remoteAddr;
}

////////////////////////////////////////////////////////////////////////////////
//
void CSocket::Close()
{
	if (m_socket==INVALID_SOCKET)
		return;

#ifdef _WIN32
	TestSocket(closesocket(m_socket),"close");
#else
	TestSocket(close(m_socket),"close");
#endif

	m_localAddr = m_remoteAddr = CIPAddress();
	m_socket = INVALID_SOCKET;
}

////////////////////////////////////////////////////////////////////////////////
//
void CSocket::Shutdown(int how/*=SD_SEND*/)
{
	if (m_socket==INVALID_SOCKET)
		return;
	TestSocket(shutdown(m_socket, how),"shutdown");
}

////////////////////////////////////////////////////////////////////////////////
//
void CSocket::SetSockOpt(int level, int optname, const void *optval, int optlen)
{
	if (m_socket==INVALID_SOCKET)
		throw socket_error(WSAENOTSOCK, "SetSockOpt");

	int res = setsockopt(m_socket, level, optname, static_cast<const char*>(optval), optlen);
	TestSocket(res, "setsockopt");
}

////////////////////////////////////////////////////////////////////////////////
//
void CSocket::GetSockOpt(int level, int optname, void *optval, int *optlen)
{
	if (m_socket==INVALID_SOCKET)
		throw socket_error(WSAENOTSOCK, "GetSockOpt");
#ifdef _WIN32
	int res = getsockopt(m_socket, level, optname, static_cast<char*>(optval), optlen);
#else
	int res = getsockopt(m_socket, level, optname, static_cast<char*>(optval), (socklen_t *)optlen);
#endif
	TestSocket(res, "getsockopt");
}

////////////////////////////////////////////////////////////////////////////////
//
// int CSocket::Select(FD_SET *fr, FD_SET *fw, FD_SET *fe, int timeout/*=0*/) const
int CSocket::Select(fd_set *fr, fd_set *fw, fd_set *fe, int timeout/*=0*/) const
{
	if (m_socket==INVALID_SOCKET)
		throw socket_error(WSAENOTSOCK, "Select");

	timeval *pt=NULL, t;
	if (timeout>=0)
	{
		t.tv_sec  = timeout / 1000;
		t.tv_usec = (timeout % 1000) * 1000;
		pt = &t;
	}

	int res = select(m_socket+1, fr, fw, fe, pt);
	TestSocket(res, "select");
	return res;
}

////////////////////////////////////////////////////////////////////////////////
//
bool CSocket::IsReadable(int timeout/*=0*/) const
{
	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(m_socket, &fs);

	return Select(&fs, NULL, NULL, timeout)!=0; 
}

////////////////////////////////////////////////////////////////////////////////
//
bool CSocket::IsWritable(int timeout/*=0*/) const
{
	fd_set fw;
	FD_ZERO(&fw);
	FD_SET(m_socket, &fw);

	return Select(NULL, &fw, NULL, timeout)!=0;
}

////////////////////////////////////////////////////////////////////////////////
//
bool CSocket::IsException(int timeout/*=0*/) const
{
	fd_set fs;
	FD_ZERO(&fs);
	FD_SET(m_socket, &fs);

	return Select(NULL, NULL, &fs, timeout)!=0;
}

////////////////////////////////////////////////////////////////////////////////
//
void CSocket::GetSockOpt(int level, int optname, void *optval, int optlen) // sin puntero en optlen
{
	GetSockOpt(level, optname, optval, &optlen);
}

void CSocket::SetRecvTimeout(int sec)
{
#ifdef _WIN32
	DWORD tv = sec*1000;
#else
	struct timeval tv;

	tv.tv_sec = sec;
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors
#endif

	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
}

////////////////////////////////////////////////////////////////////////////////
//
void CSocket::SetReusable()
{
	int yes = 1;
	SetSockOpt(SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
}

////////////////////////////////////////////////////////////////////////////////
//
void CSocket::JoinMulticastGroup(CIPAddress grupo, CIPAddress itf)
{
	ip_mreq mreq;
#ifdef _WIN32
	mreq.imr_multiaddr.S_un.S_addr = ((sockaddr_in *)grupo)->sin_addr.S_un.S_addr;
	mreq.imr_interface.S_un.S_addr = ((sockaddr_in *)itf)->sin_addr.S_un.S_addr;
#else
	mreq.imr_multiaddr.s_addr = ((sockaddr_in *)grupo)->sin_addr.s_addr;
	mreq.imr_interface.s_addr = ((sockaddr_in *)itf)->sin_addr.s_addr;
#endif
	SetSockOpt(IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
}

////////////////////////////////////////////////////////////////////////////////
//
CIPAddress CSocket::GetHostAddress(int nPort)
{
	CIPAddress ip(m_is.ipLocal, nPort);
	return ip;
}

/////////////////////////////////////////////
// class CUDPSocket
////////////////////////////////////////////////////////////////////////////////
//
CUDPSocket::CUDPSocket(bool broadcast/*=false*/)
		: m_broadcast(broadcast)
{
	Init();
}

////////////////////////////////////////////////////////////////////////////////
//
CUDPSocket::CUDPSocket(int localport, bool broadcast/*=false*/)
		: m_broadcast(broadcast)
{
	Init();

	Bind(localport);
}

////////////////////////////////////////////////////////////////////////////////
//
CUDPSocket::CUDPSocket(const CIPAddress &local, bool broadcast/*=false*/)
		: m_broadcast(broadcast)
{
	Init();

	Bind(local);
}

////////////////////////////////////////////////////////////////////////////////
//
void CUDPSocket::Init()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (m_socket==INVALID_SOCKET)
		throw socket_error("socket");

	if (m_broadcast)
	{
		bool bBC = true;
		SetSockOpt(SOL_SOCKET, SO_BROADCAST, &bBC, sizeof(bBC));
	}
}

////////////////////////////////////////////////////////////////////////////////
//
void CUDPSocket::SetBroadcast(void )
{
	bool bBC = true;
	m_broadcast=true;
	SetSockOpt(SOL_SOCKET, SO_BROADCAST, &bBC, sizeof(bBC));
}

////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
// class CTCPSocket

CTCPSocket::CTCPSocket()
{
	Init();
}

////////////////////////////////////////////////////////////////////////////////
//
CTCPSocket::CTCPSocket(const CIPAddress &remote)
{
	Init();
	Connect(remote);
}

////////////////////////////////////////////////////////////////////////////////
//
CTCPSocket::CTCPSocket(const CIPAddress &local, const CIPAddress &remote)
{
	Init();
	Bind(local);
	Connect(remote);
}

////////////////////////////////////////////////////////////////////////////////
//
CTCPSocket::CTCPSocket(int sck)
{
	m_socket = sck;
}

////////////////////////////////////////////////////////////////////////////////
//
void CTCPSocket::Init()
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (m_socket==INVALID_SOCKET)
		throw socket_error("socket");
}

////////////////////////////////////////////////////////////////////////////////
//
void CTCPSocket::Write(const void *buf, int sz)
{
	Send(buf, sz);
}

////////////////////////////////////////////////////////////////////////////////
//
void CTCPSocket::Read(void *buf, int sz)
{
	char *cbuf = reinterpret_cast<char*>(buf);

	while (sz>0)
	{
		int r = Recv(cbuf, sz);
		if (r==0)
			throw socket_error(WSAECONNRESET, "Read");
		cbuf += r;
		sz  -= r;
	}
}

int CTCPSocket::ReadLine(string &line, int timeout)
{
	line = "";
	if (this->IsReadable(timeout))
	{
		char leido;
		do 
		{
			int err = this->Recv(&leido, 1);

			if (err != 1)
				return err < 0 ? err : -1;

			if (leido == '\n')
				break;

			if (leido == '\r')
				continue;

			line.push_back(leido);
		} while (this->IsReadable(timeout));
	}
	return line.length();
}

////////////////////////////////////////////////////////////////////////////////
//
int CTCPSocket::Recv_text(string &text, int len, int timeout/*=0*/)
{
	text = "";
	if (this->IsReadable(timeout))
	{
		char leido;
		do 
		{
			int err = this->Recv(&leido, 1);
			if (err != 1)
				return err < 0 ? err : -1;

			text.push_back(leido);

		} while (--len > 0 && this->IsReadable(timeout));
	}
	return text.length();
}

int CTCPSocket::Recv_http(string &text, int timeout)
{
	text = "";
	if (this->IsReadable(timeout))
	{
		char localbuf[10000];
		int leidos = sizeof(localbuf);

		do 
		{
			leidos = this->Recv(localbuf, sizeof(localbuf));		
			for (int leido=0; leido<leidos; leido++)
				text.push_back(localbuf[leido]);
		} while(leidos==sizeof(localbuf) && this->IsReadable(50));
	}

	return text.length();
}


////////////////////////////////////////////////////////////////////////////////
//
int CTCPSocket::Recv_bin(vector<byte> &buff, int timeout/*=0*/, int char_timeout/*=10*/)
{
	buff.clear();
	if (this->IsReadable(timeout))
	{
		byte leido;
		do 
		{
			this->Recv(&leido, 1);
			buff.push_back(leido);
		} while (this->IsReadable(char_timeout));
	}
	return buff.size();
}

////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
// class CTCPServerSocket
CTCPServerSocket::CTCPServerSocket()
		: m_listenBacklog(0)
{
}

////////////////////////////////////////////////////////////////////////////////
//
CTCPServerSocket::CTCPServerSocket(int localport)
		: m_listenBacklog(0)
{
	Bind(localport);
}

////////////////////////////////////////////////////////////////////////////////
//
CTCPServerSocket::CTCPServerSocket(const CIPAddress &local)
		: m_listenBacklog(0)
{
	Bind(local);
}

////////////////////////////////////////////////////////////////////////////////
//
void CTCPServerSocket::Init()
{
	m_listenBacklog = 0;
	CTCPSocket::Init();
}

////////////////////////////////////////////////////////////////////////////////
//
void CTCPServerSocket::Listen(int backlog/*=SOMAXCONN*/)
{
	if (m_listenBacklog!=backlog)
	{
		m_listenBacklog = backlog;
		int res = listen(m_socket, m_listenBacklog);
		TestSocket(res, "listen");
	}
}

////////////////////////////////////////////////////////////////////////////////
//
CTCPSocket *CTCPServerSocket::Accept(int backlog/*=SOMAXCONN*/)
{
	int socket = AcceptPriv(backlog);

	CTCPSocket * pSck = new CTCPSocket(socket);

	return pSck;
}

////////////////////////////////////////////////////////////////////////////////
//
void CTCPServerSocket::Accept(CTCPSocket &sck, int backlog/*=SOMAXCONN*/)
{
	sck.Close();
	sck.m_socket = AcceptPriv(backlog);
}

////////////////////////////////////////////////////////////////////////////////
//
int CTCPServerSocket::AcceptPriv(int backlog)
{
	Listen(backlog);

	int socket = accept(m_socket, NULL, NULL);
	if (socket==INVALID_SOCKET)
		throw socket_error("accept");

	return socket;
}

// }  //namespace sck
