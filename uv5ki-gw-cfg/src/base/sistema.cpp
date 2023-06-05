#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>

#include "../../include/base/sistema.h"
#include "../../include/config/local-config.h"

#define SIGU5K01        40
#if defined(_PPC82xx_)
	#define ESD_DEV "/dev/esdiscretas"
	#define ESDI_LEE_LADO_LR	_IOR('E',16, unsigned int)
	#define ESDI_DIME_JIFFIES   _IOR('E', 7, unsigned int)  
#endif
/*******************************************************/
class MonthNameConv
{
public:
	MonthNameConv() {
		months["Dec"]="12";
		months["Nov"]="11";
		months["Oct"]="10";
		months["Sep"]="09";
		months["Aug"]="08";
		months["Jul"]="07";
		months["Jun"]="06";
		months["May"]="05";
		months["Apr"]="04";
		months["Mar"]="03";
		months["Feb"]="02";
		months["Jan"]="01";
	}
public:
	string translate(string name) {
		map<string,string>::iterator it = months.find(name);
		if (it != months.end())
			return it->second;
		return name;
	}
private:
	map<string, string> months;
};
/*******************************************************/


/**
*/
sistema::sistema(void)
{
}

/** */
sistema::~sistema(void)
{
}

/** */
void sistema::ExecuteCommand(char *cmd)
{
#ifdef _WIN32
	system(cmd);
#else
	system(cmd);
#endif
}

/** */
string sistema::ResultExecuteCommand(char* cmd, string paraTest)
{
#ifdef _WIN32
	#define POPEN	_popen
	#define PCLOSE	_pclose
#else
	#define POPEN	popen
	#define PCLOSE	pclose
#endif

#if defined(_PPC82xx_)
    char buffer[128];
    std::string result = "";
    FILE* pipe = POPEN(cmd, "r");
    if (!pipe) /*throw std::runtime_error("popen() failed!");*/{
        return "Failed to run command";
    }
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        PCLOSE(pipe);
        throw;
    }
    PCLOSE(pipe);
	result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return result;   
#else
	return paraTest;
#endif
}

/** */
bool sistema::GetIpAddress(/*char *szIf, */string &ip)
{
	char *szIf = (char *)LocalConfig::p_cfg->get(strSection, strItemNetworkInterfaceActiva, "eth0")/*.NetworkInterfaceActiva()*/.c_str();
#ifdef _WIN32
	ip = LocalConfig::p_cfg->get(strWindowsTest, strItemWindowsTestIp)/*.ipWindows()*/;
	return true;
#else
    int iSocket;
    struct ifreq sIfr;

    if ( 0 <= ( iSocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) )
    {
        strcpy(sIfr.ifr_name, szIf );
        sIfr.ifr_addr.sa_family = AF_INET;
        if ( -1 != ioctl( iSocket, SIOCGIFADDR, &sIfr ) )
        {
			struct sockaddr_in* ipaddr = (struct sockaddr_in*)&sIfr.ifr_addr;
			ip = string(inet_ntoa(ipaddr->sin_addr));
            // return ( *(unsigned int*)(((char*)(&sIfr.ifr_ifru.ifru_addr.sa_data))+2) );
			close(iSocket);
			return true;
        }
		else {
			PLOG_ERROR("sistema::GetIpAddress. Error en IOCTL: %s", strerror(errno));
		}
		close(iSocket);
    }
	else {
		PLOG_ERROR("sistema::GetIpAddress. Error en Apertura de socket: %d", iSocket);
	}
    return false;    
#endif
}

/** */
void sistema::GetMacAddress(char *lan, char *mac)
{
#if defined _WIN32
	strcpy_s(mac,16,"11:11:11:11:11");
#elif defined __APPLE__
	strcpy(mac, "11:11:11:11:11");
#else
    int iSocket;
    struct ifreq sIfr;

    memset( mac, 0, 8);
    if ( 0 <= ( iSocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) )
        {
        strcpy(sIfr.ifr_name, lan );
        if ( -1 != ioctl( iSocket, SIOCGIFHWADDR, &sIfr ) )
           memcpy( mac, sIfr.ifr_hwaddr.sa_data, 8);
        }    
#endif
}

/** */
void sistema::GetWorkingIpAddressAndName(string &ip, string &ipserv, string &name)
{
	DatosLocales datos_locales;
	GetIpAddress(ip);
	ipserv = datos_locales.ipServ;
	name = datos_locales.idGw;							// TODO: 
}

/** */
void sistema::SignalNuevaConfiguracion(string npid)
{
#ifdef _WIN32
#else
	pid_t pid = pidof(npid);
    if (pid != 0)
		kill(pid, SIGU5K01);
#endif
}

/** */
int sistema::pidof(string name)
{
#ifdef _WIN32
	return 0;
#else
 #define LEN 128
	char line[LEN];
	FILE *cmd = popen((string("pidof -s ")+name).c_str(), "r");

	fgets(line, LEN, cmd);
	pid_t pid = strtoul(line, NULL, 10);

	pclose(cmd);
	return pid;
#endif
}

/** */
string sistema::NormalizePath(string path)
{
#ifdef _WIN32
	char sepi='/',sepo='\\';
#else
	char sepi='\\',sepo='/';
#endif
	replace( path.begin(), path.end(), sepi, sepo );
	return path;
}

/** */
string sistema::GetDelCommand(string path)
{
#ifdef _WIN32
	return "del " + NormalizePath(path);
#else
	return "rm " + NormalizePath(path);
#endif
}

/** */
string sistema::GetCpCommand(string src, string dest)
{
#ifdef _WIN32
	return "copy " + NormalizePath(src) + " " + NormalizePath(dest);
#else
	return "cp " + NormalizePath(src) + " " + NormalizePath(dest);
#endif
}

/** */
string sistema::PathFileName(string path)
{
#ifdef _WIN32
	char sep='\\';
#else
	char sep='/';
#endif
	path = sistema::NormalizePath(path);
	size_t i = path.rfind(sep, path.length());
	if (i != string::npos) 
	{
		return(path.substr(i+1, path.length() - i));
	}
	return path;
}

/** */
void sistema::DataSaveAs(string data, string filename)
{
	ofstream ff(ON_FLASH(filename));
	ff.write(data.c_str(), data.length());
}

/** 
	return: true => Main, false => Standby
 */
bool sistema::MainOrStandby()
{
	string filepath = onfs(LocalConfig::p_cfg->get("M-S-CONTROL", "PATH", "/mnt/ramfs/cpumode"));
	string isMain = onfs(LocalConfig::p_cfg->get("M-S-CONTROL", "MAIN", "PRINCIPAL"));
	ifstream ff(filepath.c_str());

	if (ff.good())
	{
		string estado;

		ff >> estado;
		if (estado==isMain)
			return true;
		return false;
	}
	return true;
}

/** */
time_t sistema::_TIMER_CLK()
{
	time_t tval;
	::time(&tval);
	return tval;
}


/** */
string sistema::SnmpAgentVersion()
{
#if defined(_PPC82xx_)
    string file = onfs(LocalConfig::p_cfg->get(strModulos, strItemExeSnmp));
	string command = file + " /V";
	return ResultExecuteCommand((char *)command.c_str());
#else
	return "SnmpAgentVersion (WIN-LNX) 1.0.1";
#endif
}

/** */
string sistema::RecordServiceVersion() 
{
#if defined(_PPC82xx_)
	string file = onfs(LocalConfig::p_cfg->get(strModulos, strItemExeGrabador));
	string command = file + " /V";
	return ResultExecuteCommand((char *)command.c_str());
#else
	return "RecordServiceVersion (WIN-LNX) 1.0.1";
#endif
}

/** */
int sistema::ParImpar()
{
#if defined(_PPC82xx_)
	int iFdEsd = -1;
   	if ( (iFdEsd=open(ESD_DEV, O_RDWR)) < 0 )
	{
		PLOG_ERROR("sistema::ParImpar. Error en Apertura de Driver ED !!!");
   		return -1;
	}
	int iLado = ioctl( iFdEsd, ESDI_LEE_LADO_LR, 0 );
	close(iFdEsd);

	PLOG_INFO("sistema::ParImpar: lado %i", iLado);

	return iLado;
#else
	return 1;
#endif
}

/** */
void sistema::fileattr(string path, int modo, string &date, string &size)
{
	struct stat stat_buf;
    int rc = stat(path.c_str(), &stat_buf);
	if (rc==0) {
		size = Tools::itoa(rc==0 ? stat_buf.st_size : 0);
		struct tm * timeinfo = localtime(&stat_buf.st_ctime); 
		Tools::tm2String(timeinfo, "%d/%m/%Y", date);

		/** Rectificamos la fecha en funcion del tipo de fichero */
		string strFound;
		size_t pos;
		switch(modo) 
		{
			case 0:		// Ficheros 'Comerciales'.
				date = "";
				break;
			case 1:		// '<file> /V'.				Buscar NUCLEODF YYYY-MM-DD
				strFound = ResultExecuteCommand((char *)(path + " /V").c_str(), "Recurso RAD V0.0.1 (NUCLEODF 2017-01-26 16:35:50)");
				pos = strFound.find("NUCLEODF ");
				strFound = strFound.substr(pos+9, strFound.size()-(pos+9+1));
				date = strFound.substr(8,2)+"/"+strFound.substr(5,2) + "/" + strFound.substr(0,4);
				break;
			case 2:		// 'grep NUCLEODF <file>'.	Buscar NUCLEODF YYYY-MM-DD
				strFound = ResultExecuteCommand((char *)("grep NUCLEODF " + path).c_str(), "NUCLEODF 2017-01-26 16:35:50");
				pos = strFound.find("NUCLEODF ");
				strFound = strFound.substr(pos+9, strFound.size()-(pos+9));
				date = strFound.substr(8,2)+"/"+strFound.substr(5,2) + "/" + strFound.substr(0,4);
				break;
			case 3:		// 'grep Revision <file>'.	Buscar (mmm DD YYYY. mmm Abreviaturas del mes en Ingles...
				strFound = ResultExecuteCommand((char *)("grep Revision " + path).c_str(), "Driver PCM 82xx MCC. $Revision: 1.3 $ (Dec 20 2016 18:13:11)");
				pos = strFound.find("$ (");
				strFound = strFound.substr(pos+3, strFound.size()-(pos+3+1));
				date = strFound.substr(4,2)+"/"+ MonthNameConv().translate(strFound.substr(0,3)) + "/" + strFound.substr(7,4);
				break;
			case 4:		// 'grep 201 <file>'.		Buscar DD-MM-YYYY
				strFound = ResultExecuteCommand((char *)("grep 202 " + path).c_str(), "05-01-2017, 15:39 <291328>");
				pos = strFound.find(", ");
				strFound = strFound.substr(0,pos);
				date = strFound.substr(0,2)+"/"+strFound.substr(3,2) + "/" + strFound.substr(6,4);
				break;
		}
	}

	else {
		size = "0";
		date = "???";
	}
}

/** */
int sistema::DualAlive()
{
	string filepath = onfs("/mnt/ramfs/dualalive");
	ofstream ff(filepath.c_str());

	if (ff.good())
	{
		ff << "dual alive";
		return 1;
	}
	return 0;
}


/** */
string sistema::ipColateral() 
{
	string filepath = onfs(LocalConfig::p_cfg->get(strRuntime, strRuntimeItemColateralIpInfoPath, "/mnt/ramfs/cpupair"));
	ifstream ff(filepath.c_str());

	if (ff.good())
	{
		string ip;
		ff >> ip;
		return ip;
	}
	return "";
}

/** 20180503. Obtener los tick del sistema */
int sistema::TickCount()
{
#if defined(_PPC82xx_)

	int iFdEsd = -1;
   	if ( (iFdEsd=open(ESD_DEV, O_RDWR)) < 0 )
	{
		PLOG_ERROR("sistema::TickCount. Error en Apertura de Driver ED !!!");
   		return -1;
	}
	int ticks = ioctl( iFdEsd, ESDI_DIME_JIFFIES, 0 );
	close(iFdEsd);

	return ticks;
#else
	return (int )::time(NULL);
#endif
}
/** */
time_t sistema::time()
{
#if defined(_PPC82xx_)
	return (time_t)((TickCount() * 10) / 1000);
#else
	return (time_t)TickCount();	// ::time(NULL);
#endif
}

time_t sistema::time_max()
{
#if defined(_PPC82xx_)
	return (time_t)((INT_MAX * 10) / 1000);
#else
	return INT_MAX;
#endif
}

/** 20210827. Revision de la gesti�n JIFFIES */
unsigned int sistema::jiffies() {
#if defined(_PPC82xx_)

	int iFdEsd = -1;
	if ((iFdEsd = open(ESD_DEV, O_RDWR)) < 0)
	{
		PLOG_ERROR("sistema::Jiffies. Error en Apertura de Driver ED !!!");
		return 0;
	}
	int ticks = ioctl(iFdEsd, ESDI_DIME_JIFFIES, 0);
	close(iFdEsd);

	return (unsigned int)ticks;
#else
	return (unsigned int)(JIFFIES_PER_SECOND * ::time(NULL));
#endif

}

/** 20180322. Semaforo Global */
global_semaphore::global_semaphore(int id, int count)
{
#ifdef _WIN32
	Id = id;
#else
	key_t Clave;
	if ((Clave = ftok ("/bin/ls", id)) < 0)
	{
		PLOG_ERROR("global_semaphore ftok error %i", errno);
		Id = -1;
	}
	else 
	{
		if ((Id = semget (Clave, 1, 0600 | IPC_CREAT | IPC_EXCL)) < 0)
		{
			PLOG_ERROR("global_semaphore semget error %i", errno);
			//si da error suponemos que es porque esta creado
			if ((Id = semget (Clave, 1, 0600 | IPC_CREAT)) < 0)
			{
				PLOG_ERROR("global_semaphore semget error %i", errno);
				Id = -1;
			}
			else
			{
				PLOG_DEBUG("global_semaphore sem ya creado id %i\n", Id);
			}
		}
		else
		{	
			//el semaforo no estaba creado. Lo inicializamos
			union semun arg;
			PLOG_DEBUG("global_semaphore sem creado id %i. Inisem\n", Id);
			arg.val = count;
			if  ((semctl (Id, 0, SETVAL, arg)) < 0)
			{
				PLOG_ERROR("global_semaphore semclt to %i error %i", arg.val, errno);
				Id = -1;
			}
		}
	}
#endif
	procid = -1;
	initial_count = count;
}

global_semaphore::~global_semaphore()
{
#ifdef _WIN32
	Id = -1;
#else
	
	if (Id != -1 && procid != -1) 
	{
		release();
	}

#endif
}

int global_semaphore::value()
{
#ifdef _WIN32
	return 1;
#else
	if (Id != -1) 
	{
		int i;
		if ((i=semctl (Id, 0, GETVAL, 0)) <0)
		{
			PLOG_ERROR("global_semaphore semctl (GETVAL) error %i", errno);
			return -1;
		}
		return i;
	}
	PLOG_ERROR("global_semaphore (value). Semaforo no creado");
	return -1;
#endif
}

bool global_semaphore::acquire(int seconds_timeout)
{
#ifdef _WIN32
	return true;
#else
	PLOG_DEBUG("global_semaphore sem id %i. acquiring, TIMEOUT=%i..", Id, seconds_timeout);
	if (Id != -1) 
	{
#if _NOT_ACQUIRE_TIMEOUT_
		struct sembuf Operacion;
		Operacion.sem_num = 0;
		Operacion.sem_op = -1;
		Operacion.sem_flg = 0;

		if ((semop(Id, &Operacion, 1)) < 0)
		{
			PLOG_ERROR("global_semaphore semop (acquire) error %i", errno);
			return false;
		}
		procid = 1;
		PLOG_DEBUG("global_semaphore sem id %i. acquired...", Id);
		return true;
#else
		std::time_t when = std::time(0) + seconds_timeout;   // get time when exit...

		do {
			int current_val = value();
			if (current_val < 0) {
				PLOG_ERROR("global_semaphore semop (acquire) value error.");
				return false;
			}
			if (seconds_timeout <= 0 || current_val > 0 ) {
				struct sembuf Operacion;
				Operacion.sem_num = 0;
				Operacion.sem_op = -1;
				Operacion.sem_flg = 0;
				if ((semop(Id, &Operacion, 1)) < 0)
				{
					PLOG_ERROR("global_semaphore semop (acquire) error %i", errno);
					return false;
				}
				procid = 1;
				PLOG_DEBUG("global_semaphore sem id %i. acquired...", Id);
				return true;
			}
			else {
				Sleep(10);
			}
		} while (std::time(0) < when);

		PLOG_ERROR("global_semaphore semop (acquire). TIMEOUT");	// Para recuperar el semaforo en TIMEOUT retorno true;
		return true;
#endif
	}
	PLOG_ERROR("global_semaphore semop (acquire). Semaforo no creado");
	return false;

#endif
}

bool global_semaphore::release()
{
#ifdef _WIN32
	return true;
#else
	PLOG_DEBUG("global_semaphore sem id %i. releasing...", Id);
	if (Id != -1)
	{
		int val = value();
		if (val != -1 && val < initial_count)
		{
			struct sembuf Operacion;
			Operacion.sem_num = 0;
			Operacion.sem_op = 1;
			Operacion.sem_flg = 0;
			if ((semop (Id, &Operacion, 1)) < 0)
			{
				PLOG_ERROR("global_semaphore semop (release) error %i", errno);
				return false;
			}
			procid = -1;
			PLOG_DEBUG("global_semaphore sem id %i. released...", Id);
			return true;
		}
		else 
		{
			PLOG_ERROR("global_semaphore semop (release). Valor no esperado %i", val);
			return false;
		}
	}
	else 
	{
		PLOG_ERROR("global_semaphore semop (release). Semaforo no creado");
		return false;
	}
#endif
}

void global_semaphore::Test() {

	global_semaphore stest(1,1);

	/** */
	PLOG_INFO("Comportamiento NORMAL");
	stest.acquire(3);
	stest.release();

	PLOG_INFO("Recuperacion tras DOS acquire...");
	stest.acquire(3);
	stest.acquire(3);
	stest.release();

	PLOG_INFO("Recuperacion tras Salida con acquire...");
	stest.acquire();
	Sleep(1000);
}

/*****/
