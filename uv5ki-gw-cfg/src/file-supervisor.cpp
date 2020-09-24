#include "../include/file-supervisor.h"
#include "../include/cfg-proc.h"

#define P_CFG_PROC								(CfgProc::p_cfg_proc)
#define P_WORKING_CONFIG						(P_CFG_PROC->p_working_config)

/**
*/
FileSupervisor *FileSupervisor::p_fspv;

/**
*/
FileSupervisor::FileSupervisor(void)
{
}

/**
*/
FileSupervisor::~FileSupervisor(void)
{
}

/**
*/
bool FileSupervisor::LocalLock(string pathoffile)
{
	CCSLock lock(_acceso, "FileSupervisor");
	SupervisedFile *file = Find(pathoffile);
	if (file==NULL)
		return false;

	PLOG_INFO("Locking %s", pathoffile.c_str());
	file->LocalLock(atoi(LocalConfig::p_cfg->get(strFilesSupervidor, strSupervisedFileUnlockTime, "10").c_str())/*SupervisedFileUnlocktime()*/);

	return true;
}

/**
*/
bool FileSupervisor::LocalUnlock(string pathoffile)
{
	CCSLock lock(_acceso, "FileSupervisor");
	SupervisedFile *file = Find(pathoffile);
	if (file==NULL)
		return false;

	PLOG_INFO("Unlocking %s", pathoffile.c_str());
	file->LocalUnlock();

	return true;
}

/**
*/
void FileSupervisor::Run()
{
	time_t tick = LocalConfig::p_cfg->getint(strRuntime, strRuntimeItemThreadActiveTick, "60");
	/** */
	SetId("FileSupervisor");
	PLOG_INFO("FileSupervisor (%d) running...", pid());

	Init();

	while (IsRunning())
	{
		this->sleep(500);
		try 
		{
			CCSLock lock(_acceso, "FileSupervisor");
			for (list<SupervisedFile >::iterator file = _files.begin(); file != _files.end(); file++)
			{
				if ((*file).HasChanged()==true)
				{
					PLOG_INFO("FileSupervisor. Detectado Cambio en Fichero: %s", (*file).srcPaht().c_str());
					try 
					{
						if (P_WORKING_CONFIG->DualCpu() == true)
						{
							string ipColateral;
							if (P_WORKING_CONFIG->IpColateral(ipColateral)==true)
							{
								(*file).SyncToRemote(ipColateral);
								PLOG_INFO("FileSupervisor. Fichero: %s Sincronizado...", (*file).srcPaht().c_str());
							}
							else
							{
								PLOG_ERROR("Error en FileSupervisor. Fichero: %s. Ip Colateral no Valida.", (*file).srcPaht().c_str());
							}
						}
					}
					catch(FtpClientException e)
					{
						PLOG_ERROR("FtpClientException en FileSupervisor. Fichero: %s, Excepcion: %s", (*file).srcPaht().c_str(), e.what().c_str());
					}
					catch(Exception e)
					{
						PLOG_ERROR("HttpClientException en FileSupervisor. Fichero: %s, Excepcion: %s", (*file).srcPaht().c_str(), e.what().c_str());
					}
					catch(...)
					{
						PLOG_ERROR("Exception no Esperada en FileSupervisor. Fichero: %s", (*file).srcPaht().c_str());
					}
				}
			}
		}
		catch(...) 
		{
			PLOG_ERROR("Exception no Esperada en FileSupervisor.");
		}
		if (Tick.elapsed(tick)==true) {		
			PLOG_INFO("(%d)=>FileSupervisor. TICK", pid());
		}
	}

	Dispose();
	PLOG_INFO("FileSupervisor leaving...");
}

/**
*/
void FileSupervisor::Dispose()
{
	try 
	{
		while (_files.empty()==false)
		{
			_files.pop_front();
		}
	}
	catch(...)
	{
		PLOG_ERROR("Exception no Esperada en FileSupervisor::Dispose.");
	}
}

/**
*/
void FileSupervisor::Init()
{
	try
	{
		INISection SupervisedFiles = LocalConfig::p_cfg->get(strFilesSupervidor);
		string keyStart = strKeySupervisedFile;
		for (INISection::iterator item=SupervisedFiles.begin(); item != SupervisedFiles.end(); item++)
		{
			if (item->first.compare(0, keyStart.size(), keyStart)==0)
			{
				vector<string> partes;
				Tools::split(partes, item->second, ','); 
				if (partes.size()==4)
				{
					_files.push_back(SupervisedFile(partes[0],partes[1],partes[2],partes[3]));
					//_files.push_back(SupervisedFile(item->second));
				}
			}
		}
	}
	catch(...)
	{
		PLOG_ERROR("Exception no Esperada en FileSupervisor::Init.");
	}
}

/**
*/
SupervisedFile *FileSupervisor::Find(string pathoffile)
{
	for (list<SupervisedFile >::iterator file = _files.begin(); file != _files.end(); file++)
	{
		if ((*file)==SupervisedFile(pathoffile))
			return &(*file);
	}
	return NULL;
}

/*****************************************************************************************/
/**
*/
void SupervisedFile::SyncToRemote(string ipRemote)
{
	string httpHost = ipRemote + ":" + LocalConfig::p_cfg->get(strSection, strItemWebPort)/*.PuertoEscucha()*/;

	if (RemoteLock(httpHost)==true)				// Los Errores Saldran por excepcion...
	{
		if (SendToRemote(ipRemote)==true)
		{
			if (RemoteUnlock(httpHost)==true)
				return;
		}
	}
}

/**
*/
bool SupervisedFile::RemoteLock(string httpHost)
{
	
	/** Generar el comando y Espera Respuesta... */
	string request = "PUT /" + string(CPU2CPU_MSG) + "/" + string(CPU2CPU_MSG_REMOTE_LOCK) + " HTTP/1.1\r\n" + 
		"Host: " + httpHost + 
		"\r\n\r\n" + 
		_dstpath;
#ifdef _POINTER_TO_RESPONSE_
	ParseResponse httpResponse = HttpClient(httpHost).SendHttpCmd(request, LocalConfig().getint(strRuntime, strRuntimeItemLocalHttpTimeout, "5000"));
#else
	HttpClient(httpHost).SendHttpCmd(request, &httpResponse, LocalConfig().getint(strRuntime, strRuntimeItemLocalHttpTimeout, "5000"));
#endif
	if (httpResponse.Status() != "200")
	{
		throw Exception("REQUEST ERROR: PUT /" + string(CPU2CPU_MSG) + "/" + string(CPU2CPU_MSG_REMOTE_LOCK) + " Host: " + httpHost +  ". " + httpResponse.Status());
	}
	return true;
}

/**
*/
bool SupervisedFile::RemoteUnlock(string httpHost)
{
	/** Generar el comando y Espera Respuesta... */
	string request = "PUT /" + string(CPU2CPU_MSG) + "/" + string(CPU2CPU_MSG_REMOTE_UNLOCK) + " HTTP/1.1\r\n" + 
		"Host: " + httpHost + 
		"\r\n\r\n" +
		_dstpath;
#ifdef _POINTER_TO_RESPONSE_
	ParseResponse httpResponse = HttpClient(httpHost).SendHttpCmd(request, LocalConfig().getint(strRuntime, strRuntimeItemLocalHttpTimeout, "5000"));
#else
	HttpClient(httpHost).SendHttpCmd(request, &httpResponse, LocalConfig().getint(strRuntime, strRuntimeItemLocalHttpTimeout, "5000"));
#endif
	if (httpResponse.Status() != "200")
	{
		throw Exception("REQUEST ERROR: PUT /" + string(CPU2CPU_MSG) + "/" + string(CPU2CPU_MSG_REMOTE_LOCK) + " Host: " + httpHost +  ". " + httpResponse.Status());
	}
	return true;
}

/**
*/
bool SupervisedFile::SendToRemote(string ipRemote)
{
	FtpClient ftp(ipRemote, _user, _pwd);	// Los errores saldran por excepciones.

	ftp.Login();
	PLOG_INFO("SupervisedFile (%s). FTP Log In (%s,%s)", _srcpath.c_str(), _user.c_str(), _pwd.c_str());
	ftp.Upload(_srcpath, _dstpath);
	PLOG_INFO("SupervisedFile (%s). FTP SENT...", _srcpath.c_str());
	ftp.Close();
	return true;
}
