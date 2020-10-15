// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////
#include "../../include/base/thread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void AssertEqual0(int val)
{
	assert(val==0);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//
CDead::CDead()
{
	int res = pthread_cond_init(&m_hDead.CVariable, 0);
	assert( res == 0);
	res = pthread_mutex_init(&m_hDead.Mutex, 0);
	assert(res == 0);
	m_bRunning = false;
};

//////////////////////////////////////////////////////////////////////
//
CDead::~CDead()
{
	int res = pthread_cond_destroy(&m_hDead.CVariable);
	assert( res == 0);
	pthread_mutex_destroy(&m_hDead.Mutex);
};

//////////////////////////////////////////////////////////////////////
//
void CDead::Start()
{
	m_bRunning = true;
};

//////////////////////////////////////////////////////////////////////
//
void CDead::Stop()
{
	if(m_bRunning)
	{
		m_bRunning = false;
		int res = pthread_cond_wait(&m_hDead.CVariable, &m_hDead.Mutex);
		assert(res==0);
	}
};

//////////////////////////////////////////////////////////////////////
//
void CDead::Signal()
{
	int res = pthread_cond_signal(&m_hDead.CVariable);
	assert( res== 0);
};


//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
//
CThread::CThread()
{
#ifdef _WIN32
	m_hThread.p = NULL;
#else
	m_hThread = 0;
#endif
	m_bLive = false;
	m_id = "cthread";
}

//////////////////////////////////////////////////////////////////////
//
CThread::~CThread()
{
#ifdef _WIN32
	m_hThread.p = NULL;
#else
	m_hThread = 0;
#endif
}

//////////////////////////////////////////////////////////////////////
//
void CThread::Start(/*int nPriority*/)
{
	int res = pthread_create(&m_hThread, NULL, sRun, this);
	assert( res == 0);
}

//////////////////////////////////////////////////////////////////////
//
void CThread::Stop()
{
	m_Dead.Stop();
}

void CThread::PrioritySet(int iPrio)
{
#ifdef _WIN32
#else
	struct sched_param sSchedDatos;
	int iAux;

	if (1 == iPrio || 2 == iPrio || 3 == iPrio)  //EDU 0000
	{
		// Me convierto en proceso de tiempo real con politica de planificacion SCHED_FIFO.
		// Esto solo funciona si somos root.

		sched_getparam(0, &sSchedDatos);

		sSchedDatos.sched_priority = sched_get_priority_min(SCHED_FIFO);
		sSchedDatos.sched_priority += iPrio - 1; //EDU 0000
		if (0 != (iAux = sched_setscheduler(0, SCHED_FIFO, &sSchedDatos)))
		{
			PLOG_ERROR("Error %d on sched_setscheduler. pid=%d", iAux, pthread_self());
			return;
		}
	}
	else
	{
		// Proceso con prioridad normalita.
		sched_getparam(0, &sSchedDatos);
		sSchedDatos.sched_priority = sched_get_priority_min(SCHED_OTHER);
		if (0 != (iAux = sched_setscheduler(0, SCHED_OTHER, &sSchedDatos)))
		{
			PLOG_ERROR("Error %d on sched_setscheduler. pid=%d", iAux, pthread_self());
			return;
		}
	}
#endif
}

//////////////////////////////////////////////////////////////////////
//
void *CThread::sRun(void *pParam)
{	
    // CThread *pTh = (CThread *)pParam;
	((CThread *)pParam)->m_Dead.Start();
	((CThread *)pParam)->m_bLive = true;
	try
	{
		((CThread *)pParam)->Run();
	}
	catch(...)
	{
        // PLOG_ERROR("Error Irrecuperable en THREAD %s\n", pTh->m_id.c_str());
		sleep(500);
		assert(0);
	}

	((CThread *)pParam)->m_bLive = false;
	((CThread *)pParam)->m_Dead.Signal();

	return 0;
}

/** */
bool CThread::IsLive()
{
	return m_bLive;
}

void CThread::StackInfo(const char* pname)
{
#ifdef _WIN32
#else
	int s;
	pthread_attr_t gattr;
	size_t v;
	void* stkaddr;

	/* pthread_getattr_np() is a non-standard GNU extension that
	   retrieves the attributes of the thread specified in its
	   first argument */

	s = pthread_getattr_np(pthread_self(), &gattr);
	if (s == 0) {
		s = pthread_attr_getstack(&gattr, &stkaddr, &v);
		if (s == 0) {
			PLOG_INFO("%s. Stack Info: Stack [%p - 0x%x] SP=0x%x", pname, stkaddr, v, &s);
		}
		else {
			PLOG_ERROR("%s. Error %d on pthread_getattr_np. pid=%d", pname, s, pthread_self());
		}
	}
	else {
		PLOG_ERROR("%s. Error %d on pthread_attr_getstack. pid=%d", pname, s, pthread_self());
	}
#endif
}

/** */
bool CThread::sleep(int msec)
{
#ifdef _WIN32
	Sleep(msec);
#else
	usleep(msec*1000);
#endif
	return true;
}
