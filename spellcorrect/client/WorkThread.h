#ifndef MY_WORKTHREAD_H
#define MY_WORKTHREAD_H

#include "Thread.h"
#include "Cache.h"

namespace wd
{
class ThreadPool;
class WorkThread : public Thread
{
public:

	WorkThread(ThreadPool &threadpool);

	void run();

	Cache& getWorkThreadCache();
	void updateWorkThreadCache(Cache &cache);



private:
	ThreadPool& m_threadPool;
	Cache		m_cache;
};

}//namespace wd

#endif
