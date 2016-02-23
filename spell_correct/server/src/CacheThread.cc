#include "CacheThread.h"
#include "Config.h"
#include "WorkThread.h"
#include <functional>

namespace  wd
{
CacheThread::CacheThread(VecThreads& vecThreads)
	:m_vecThreads(vecThreads),
	 m_isSaveCache(true),
	 m_timer(CACHE_VAL, CACHE_INTERVAL, 
			 std::bind(&CacheThread::updateFunc, this))
{	
}

void CacheThread::updateFunc()
{
	updateCache();

	if(m_isSaveCache)
	{
		SaveCacheData();
	}
}

//线程开启自动执行
void CacheThread::run()
{
	m_cache.loadCacheFile();
	initWorkThreadCache();
	m_timer.start();

}

void CacheThread::initWorkThreadCache()
{
	for(auto & pthread : m_vecThreads)
	{
		dynamic_cast<WorkThread*>(pthread.get())->updateWorkThreadCache(m_cache);
	}
}

void CacheThread::updateCache()
{

	//更新总缓存
	
	auto size_old = m_cache.size();


	for(auto pthread : m_vecThreads)
	{

		//使用引用 因为其成员mutexLock为对象语义
		Cache& th_cache = dynamic_cast<WorkThread*>(pthread.get())->getWorkThreadCache();
		auto th_cache_map = th_cache.getCacheMap();

		for(auto & pair : th_cache_map)
		{
			m_cache.addQueryToCache(pair);//(pair.first, pair.second);
		}
	}

	auto size_new = m_cache.size();

	//总缓存没有新数据，不再重写文件
	if(size_old != size_new)
	{
		m_isSaveCache = true;
	}else
	{
		m_isSaveCache = false;
	}

	//更新线程缓存
	initWorkThreadCache();

}


void CacheThread::SaveCacheData()
{
	m_cache.writeCacheFile();
}



}//namespace wd
