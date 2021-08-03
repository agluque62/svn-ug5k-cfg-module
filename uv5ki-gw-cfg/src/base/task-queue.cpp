#include "../../include/base/task-queue.h"

TaskQueue TaskQueue::Tasks;

void TaskQueue::Start() {
	assert(stopped == true);
	int res = pthread_create(&mainThreadId, NULL, stDo, this);
	assert(res == 0);
	stopped = false;
	PLOG_DEBUG("TaskQueue Started...");
}

void TaskQueue::Stop(bool fromWithin) {
	if (fromWithin == false) {
		util::MutexLock lock(qMutex);

		if (stopped == true)
			return;
	}
	if (fromWithin == false) {
		/** Parar el proceso */
		signal2Stop = true;
		pthread_join(mainThreadId, NULL);
	}
	Clear();
	stopped = true;
	PLOG_DEBUG("TaskQueue Stopped...");
}

void TaskQueue::Enqueue(string id, ptTask task) {
	util::MutexLock lock(qMutex);
	if (!stopped) {
		TaskItem task(id, task);
		taskQueue.push(task);
	}
}

void TaskQueue::DoTask() {
	while (signal2Stop == false) {
		TaskItem item;
		if (taskGet(&item) == true) {
			try {
				PLOG_DEBUG("Executing %s", item.Id.c_str());
				item.Task();
				PLOG_DEBUG("%s Executed", item.Id.c_str());
			}
			catch (...) {
				PLOG_ERROR("Error Exception executing %s", item.Id.c_str());
			}
		}
		Sleep(10);
	}
}