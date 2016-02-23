#include "WorkThread.h"
#include "ThreadPool.h"
#include <iostream>

namespace wd
{

WorkThread::WorkThread(ThreadPool &threadpool)
	:m_threadPool(threadpool)
{
}

void WorkThread::run()
{
	m_threadPool.threadFunc(shared_from_this());
}

#if 0
Cache& WorkThread::getWorkThreadCache()
{
	return m_cache;
}

void WorkThread::updateWorkThreadCache(Cache &cache)
{
	m_cache.setCacheMap(cache);
}
#endif


}//namespace wd
