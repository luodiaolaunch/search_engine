#ifndef WD_CACHETHREAD_H
#define WD_CACHETHREAD_H

#include "Thread.h"
#include "Cache.h"
#include "ThreadPool.h"
#include "Timer.h"

namespace wd
{

class CacheThread : public Thread
{
public:
	using VecThreads = std::vector<std::shared_ptr<Thread> >;

	CacheThread(VecThreads&);

	void run();
	void updateFunc();

private:
	void initWorkThreadCache();
	void updateCache();
	void SaveCacheData();

private:
	VecThreads&		m_vecThreads;
	Cache			m_cache;
	Timer			m_timer;
	bool			m_isSaveCache;
};
}//namespace wd

#endif

