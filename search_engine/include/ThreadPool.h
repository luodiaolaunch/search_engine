#ifndef MY_THREADPOOL_H
#define MY_THREADPOOL_H

#include "Buffer.h"
//#include "CacheThread.h"
#include "Thread.h"
#include "Condition.h"
#include "MutexLock.h"
#include "Config.h"
#include "acl_cpp/lib_acl.hpp"
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
public:
	using UINT4 = unsigned int ;
	using VecThreads = std::vector<std::shared_ptr<Thread> >;
	using ThreadPtr = std::shared_ptr<Thread>;

	ThreadPool(size_t nThread, size_t nBuf);

	void addTask(Task *);
	void start();
	void stop();

	acl::redis_client_pool& getRedisClientPool();


protected:
	void threadpoolAdjust();
	void moveToBusyList(ThreadPtr pthread);
	void moveToIdleList(ThreadPtr pthread);

	void deleteFromIdleList(ThreadPtr pthread);
	void deleteFromBusyList(ThreadPtr pthread);

	void deleteIdleThread(int num);
	void createIdleThread(int num);

	void printStatus();
#if 0
	void setMaxNum(int maxnum = THREAD_MAX_NUM);
	void setAvailLowNum(int availlow = THREAD_AVAIL_LOW);
	void setAvailHighNum(int availhigh = THREAD_AVAIL_HIGH);
	void setInitNum(int initnum = THREAD_INIT_NUM);

	int getMaxNum() const;
	int getAvailLowNum() const;
	int getAvailHighNum() const;
	int getAvailNum() const;
	int getInitNum() const;
	int getAllNum() const;


protected:
	ThreadPtr getIdleThread();
	void appendToIdleList(ThreadPtr workthread);
	void moveToBusyList(ThreadPtr idlethread);
	void moveToIdleList(ThreadPtr busythread);
	void deleteIdleThread(int num);
	void createIdleThread(int num);
#endif
private:
	std::shared_ptr<Task> getTask();
	void threadFunc(ThreadPtr workthread);

private:
//	VecThreads 		 m_vecThreads;
	VecThreads       m_BusyList;
	VecThreads       m_IdleList;

//ThreadPtr		m_cacheThread;
	size_t 			 m_threadsNum;
	Buffer 			 m_buf;
	bool   			 m_isExit;

	MutexLock		 m_mutex;
	Condition		 m_maxNumCond;

private:
	UINT4 			 m_maxNum = THREADS_MAX;
	UINT4			 m_availLow = THREADS_LOW;
	UINT4 			 m_availHigh = THREADS_HIGH;
	UINT4			 m_availNum = THREADS_NUM;
	acl::redis_client_pool m_redisClientPool;
};


}//namespace wd

#endif
