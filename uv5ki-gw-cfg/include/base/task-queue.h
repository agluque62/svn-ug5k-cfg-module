#ifndef _EVENTQUEUE_
#define _EVENTQUEUE_
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

typedef void (*ptTask)(void);

class TaskItem
{
public:
	TaskItem() {
		Id = "";
		Task = NULL;
		Valid = false;
	}
	TaskItem(string id, ptTask task) {
		Id = id;
		Task = task;
		Valid = true;
	}
public:
	string Id;
	ptTask Task;
	bool Valid;
};

class TaskQueue : public CodeBase
{
public:
	static TaskQueue Tasks;
public:
	TaskQueue() {
		stopped = true;
		signal2Stop = false;
	}
public:
	void Start();
	void Stop(bool fromWithin = false);
	void Enqueue(string id, ptTask task);
private:
	static void *stDo(void* arg) {
		((TaskQueue *)arg)->DoTask();
		return 0;
	}
	void DoTask();
	void Clear()
	{
		std::queue<TaskItem> empty;
		std::swap(taskQueue, empty);
	}
	bool taskGet(TaskItem* p_task) {
		util::MutexLock lock(qMutex);
		if (!taskQueue.empty())
		{
			*p_task = taskQueue.front();
			taskQueue.pop();
			return true;
		}
		return false;
	}
private:
	bool stopped;
	bool signal2Stop;
	queue<TaskItem> taskQueue;
	pthread_t mainThreadId;
	util::Mutex qMutex;
};

#endif
