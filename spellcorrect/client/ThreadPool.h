#ifndef MY_THREADPOOL_H
#define MY_THREADPOOL_H

#include "Buffer.h"
#include "CacheThread.h"
#include "Thread.h"
#include <vector>
#include <memory>
namespace wd
{

class Thread;
class WorkThread;
class Cache;

class ThreadPool
{
	friend class WorkThread;
	friend class TcpServer;
public:
	using VecThreads = std::vector<std::shared_ptr<Thread> >;
	using ThreadPtr = std::shared_ptr<Thread>;

	ThreadPool(size_t nThread, size_t nBuf);

	void addTask(Task *);
	void start();
	void stop();


private:
	std::shared_ptr<Task> getTask();
	void threadFunc(Thread* workthread);

private:
	VecThreads 		m_vecThreads;
	ThreadPtr		m_cacheThread;
	size_t 			m_threadsNum;
	Buffer 			m_buf;
	bool   			m_isExit;
};


}//namespace wd

#endif
