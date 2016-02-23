#ifndef MY_WORKTHREAD_H
#define MY_WORKTHREAD_H

#include "Thread.h"
//#include "Cache.h"
#include <memory>

namespace wd
{
class ThreadPool;
class WorkThread : public Thread,
				   public std::enable_shared_from_this<WorkThread>

{
public:

	WorkThread(ThreadPool &threadpool);

	void run();
#if 0
	Cache& getWorkThreadCache();
	void updateWorkThreadCache(Cache &cache);
#endif

private:
	ThreadPool& 	 m_threadPool;
//	Cache		m_cache;
};

}//namespace wd

#endif
