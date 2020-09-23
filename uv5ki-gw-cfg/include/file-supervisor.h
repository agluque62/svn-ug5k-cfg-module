#ifndef _FileSupervisor_
#define _FileSupervisor_

#include "./base/code-base.h"
#include "./base/thread.h"
#include "./base/socket.h"
#include "./base/sistema.h"
#include "./tools/tools.h"
#include "./tools/ftp-client.h"
#include "./tools/http-client.h"
#include "./config/local-config.h"

using namespace std;

/**
*/
class SupervisedFile : public CodeBase
{
public:
	SupervisedFile(string src_path, string dst_path="", string user="", string pwd="")
	{
		_srcpath=onfs(src_path);
		_dstpath=onfs(dst_path);
		_user=user;
		_pwd=pwd;
		_lock=0;
#ifdef _WIN32
		_stat( _srcpath.c_str(), &_lastMod );
#else
		lstat( _srcpath.c_str(), &_lastMod );
#endif
	}
	~SupervisedFile() {}

public:
	bool operator == (SupervisedFile other)
	{
		return _srcpath==other._srcpath;
	}

public:
	/** */
	bool HasChanged()
	{
		if (_TIMER_IS_RUNNING(_lock))
		{
			if (_TIMER_EXPIRED(_lock)==false)
				return false;

			LocalUnlock();
		}
#ifdef _WIN32
		_stat( _srcpath.c_str(), &_currentMod );
#else
		lstat( _srcpath.c_str(), &_currentMod );
#endif
		if (_lastMod.st_mtime != _currentMod.st_mtime)
		{
			_lastMod.st_mtime = _currentMod.st_mtime;
			return true;
		}
		return false;
	}
	/** */
	void LocalLock(int maxLockedTime=10)
	{
		_TIMER_SET(_lock, maxLockedTime);			
		PLOG_INFO("%s Locked", _srcpath.c_str());
	}
	/** */
	void LocalUnlock() 
	{
#ifdef _WIN32
		_stat( _srcpath.c_str(), &_lastMod );
#else
		lstat( _srcpath.c_str(), &_lastMod );
#endif
		_TIMER_CLR(_lock);
		PLOG_INFO("%s Unlocked", _srcpath.c_str());
	}
	/** */
	void SyncToRemote(string ipRemote);
	string srcPaht(){return _srcpath;}

protected:
	bool RemoteLock(string httpHost);
	bool SendToRemote(string ipRemote);
	bool RemoteUnlock(string httpHost);

private:
	string _srcpath,_dstpath,_user,_pwd;
#ifdef _WIN32
	struct _stat _lastMod;
	struct _stat _currentMod;
#else
	struct stat _lastMod;
	struct stat _currentMod;
#endif
	time_t _lock;
#ifndef _POINTER_TO_RESPONSE_
	ParseResponse httpResponse;
#endif
};

/**
*/
class FileSupervisor :
	public CThread
{
public:
	static FileSupervisor *p_fspv;

public:
	FileSupervisor(void);
	~FileSupervisor(void);

public:
	bool LocalLock(string pathoffile);
	bool LocalUnlock(string pathoffile);

protected:
	void Run();
	void Dispose();
	void Init();
	SupervisedFile *Find(string pathoffile);

private:
	CCritSec   _acceso;
	list<SupervisedFile > _files;
};

#endif
