// Thread.h: interface for the CThread class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_THREAD_H__E04790E1_2ADA_11D3_84D1_00201856631F__INCLUDED_)
#define AFX_THREAD_H__E04790E1_2ADA_11D3_84D1_00201856631F__INCLUDED_

#ifdef _WIN32
	#include <Windows.h>
	#include <process.h>
#else
	#include <unistd.h>
	#define Sleep(m)	usleep(1000*m)
	#define _NO_WORKING_THREAD_
#endif
#include <stdio.h>

#include <string>
#include <queue>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "code-base.h"
#include "sistema.h"

#ifndef THREAD_PRIORITY_NORMAL
	#define THREAD_PRIORITY_NORMAL 1
#endif

/** */
class TimeMeasure 
{
public:
	TimeMeasure() {
		_last = sistema::time();
		_diff = 0;
	}
	bool elapsed_old(time_t _timeout) {
		time_t now = sistema::time();
		
		_diff = now >= _last ? now - _last : 
			sistema::time_max() - (_last - now);

		if (now > (_last + _timeout)) {
			_last = now;
			return true;
		}
		return false;
	}
	bool elapsed(time_t timeout_seg) {
		unsigned int jtimeout = timeout_seg * JIFFIES_PER_SECOND;
		unsigned int jlast = (unsigned int)(_last * JIFFIES_PER_SECOND);
		unsigned int jnow = sistema::jiffies();
		if (jnow >= jlast) {
			unsigned jdif = jnow - jlast;
			_diff = jdif / JIFFIES_PER_SECOND;
			if (jdif >= jtimeout) {
				_last = jnow / JIFFIES_PER_SECOND;
				return true;
			}
		}
		else {
			_diff = 0;
			_last = jnow / JIFFIES_PER_SECOND;
		}
		return false;
	}
	/** */
	time_t lastdiff() {return _diff;}

private:
	time_t _last;
	time_t _diff;

};

/**
*/
class CCritSec  
{
public:
	pthread_mutex_t m_critSec;

public:
	CCritSec()
    {
		int res = pthread_mutex_init(&m_critSec, 0);
	    assert( res == 0);
    }

	~CCritSec()
    {
		int res = pthread_mutex_destroy(&m_critSec);
		assert( res == 0);
    }
	operator pthread_mutex_t *()
    {
		return &m_critSec;
    }

private:

};

/**
*/
class CCSLock
{
public:
    CCSLock(CCritSec &cs) : m_cs(cs)
    {
		pthread_mutex_lock((pthread_mutex_t *)m_cs);
    }
    ~CCSLock()
    {
		pthread_mutex_unlock((pthread_mutex_t *)m_cs);
    }
private:
    CCritSec& m_cs;
};

/**
*/
class CDead
{
protected:
	bool m_bRunning;

	struct _event_t_
	{
		pthread_cond_t CVariable;
		pthread_mutex_t Mutex;
	} m_hDead;

public:
	CDead();
	virtual ~CDead();

	void Start();
	void Stop();
	void Signal();
	bool IsRunning(){ return m_bRunning;};

	void Wait()
	{
		pthread_cond_wait(&m_hDead.CVariable, &m_hDead.Mutex);
	}
};


/**
*/
class CThread : public CodeBase
{
private:
	std::string m_id;

public:
	CThread();
	virtual ~CThread();

	virtual void Start(/*int nPriority = THREAD_PRIORITY_NORMAL*/);
	virtual void Stop();
	bool IsLive();
	TimeMeasure Tick;
public:
	void SetId(std::string id) 
	{ 
		m_id=id; 
#if !defined(_WIN32) && !defined(__APPLE__) && !defined(_PPC82xx_)
		pthread_setname_np(m_hThread, id.c_str());
#endif

	}
	std::string GetId(void)  { return m_id; }
	/** */
	int pid() 
	{
#if defined(_WIN32)
		return (int )GetCurrentThreadId();
#else
		return (int )getpid();
#endif
	}


protected:
	virtual void Run() = 0;
	bool IsRunning()
	{
		return m_Dead.IsRunning();
	}
	CDead m_Dead;
	bool m_bLive;

	void _sleep(int msec)
	{
#ifdef _WIN32
		Sleep(msec);
#else
		usleep(msec*1000);
#endif
	}
	void StackInfo(const char *pname);
	void PrioritySet(int level = 1);

	pthread_t m_hThread;
	static void *sRun(void *arg);
public:
	static bool sleep(int msec);

};

///
class WorkingThread
{
private:
	void *(*_start)(void *);
	void *_arg;

public:
	WorkingThread(void *(*start_add)(void *), void *arg)
	{
		_start = start_add;
		_arg = arg;
	}
	~WorkingThread()
	{

	}
	/** */
	void Do()
	{
#ifdef _NO_WORKING_THREAD_
		_start(_arg);
#else
		pthread_t m_hThread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&m_hThread, &attr, _start, _arg);
#endif
	}
};

/** */
class RealWorkingThread
{
private:
	void *(*_start)(void *);
	void *_arg;

public:
	RealWorkingThread(void *(*start_add)(void *), void *arg)
	{
		_start = start_add;
		_arg = arg;
	}
	~RealWorkingThread()
	{
	}
	/** */
	void Do()
	{
		pthread_t m_hThread;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&m_hThread, &attr, _start, _arg);
	}
};


/** */
template <typename T> class ColaAventos
{
private:
    std::queue<T>    m_queue;
    pthread_mutex_t  m_mutex;
public:
    ColaAventos() 
	{
        pthread_mutex_init(&m_mutex, NULL);
    }
    ~ColaAventos() 
	{
        pthread_mutex_destroy(&m_mutex);
    }
public:
	bool get(T &data)
	{
		bool retorno = false;
		pthread_mutex_lock(&m_mutex);
		if (!m_queue.empty())
		{
			data = m_queue.front();
			m_queue.pop();
			retorno = true;
		}
        pthread_mutex_unlock(&m_mutex);
		return retorno;
	}
	void set(T &data)
	{
		pthread_mutex_lock(&m_mutex);
		m_queue.push(data);
		pthread_mutex_unlock(&m_mutex);
	}
	void clear()
	{
		pthread_mutex_lock(&m_mutex);
		while (!m_queue.empty())
			m_queue.pop();
		pthread_mutex_unlock(&m_mutex);
	}
};


#endif // !defined(AFX_THREAD_H__E04790E1_2ADA_11D3_84D1_00201856631F__INCLUDED_)
