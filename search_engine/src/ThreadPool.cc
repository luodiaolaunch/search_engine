#include "ThreadPool.h"
#include "Thread.h"
#include "Mylog.h"
#include "WorkThread.h"
#include "Task.h"
#include "Config.h"
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <sys/time.h>

namespace wd
{


int timeval_substract(struct timeval* result, struct timeval* x, struct timeval *y)
{
	if(x->tv_sec > y->tv_sec)
		return -1;
	if(x->tv_sec == y->tv_sec &&
	   x->tv_usec > y->tv_usec)
		return -1;

	result->tv_usec = y->tv_usec - x->tv_usec;
	result->tv_sec = y->tv_sec - x->tv_sec;

	if(result->tv_usec < 0)
	{
		result->tv_sec--;
		result->tv_usec += 1000000;
	}
	return 0;
}


//==============================================================


ThreadPool::ThreadPool(size_t nThread, size_t nBuf)
	:m_threadsNum(nThread),
//	 m_cacheThread(std::shared_ptr<Thread>(new CacheThread(m_IdleList))),  
	 m_buf(nBuf),
	 m_isExit(false),
	 m_maxNumCond(m_mutex),
	 m_redisClientPool(REDIS_ADD.c_str(), m_maxNum)
{
	m_redisClientPool.set_timeout(REDIS_CONN_TIMEOUT, REDIS_RW_TIMEOUT);
}

void ThreadPool::addTask(Task* pTask)
{
	if(pTask != NULL)
		m_buf.push(pTask);
}

std::shared_ptr<Task> ThreadPool::getTask()
{
	return m_buf.pop();
}

acl::redis_client_pool& ThreadPool::getRedisClientPool()
{
	return m_redisClientPool;
}


void ThreadPool::threadFunc(ThreadPtr workthread)
{
	struct timeval start, stop, diff;

	while(!m_isExit && workthread->isRunning())
	{
		m_mutex.lock();

		while(m_BusyList.size() > m_maxNum)
		{
			std::cout << "busylist full\n";
			m_maxNumCond.wait();
		}

		m_mutex.unlock();


		//任务对象的销毁时机是在此处的作用域结束处
		auto pTask = getTask();

		if(pTask.get() != NULL)
		{

			::gettimeofday(&start, 0);
			m_mutex.lock();

			moveToBusyList(workthread);
			deleteFromIdleList(workthread);	

			std::cout << "111111111111\n";
			threadpoolAdjust();

			m_mutex.unlock();

//			pTask->setWordThreadPtr(dynamic_cast<WorkThread*>(workthread.get()));
			pTask->Run();//



			m_mutex.lock();

			moveToIdleList(workthread);
			deleteFromBusyList(workthread);


			std::cout << "22222222222222222\n";
			threadpoolAdjust();

			m_mutex.unlock();

			::gettimeofday(&stop, 0);
			timeval_substract(&diff, &start, &stop);
			std::cout << "time : " << diff.tv_sec * 1000 + diff.tv_usec/1000 << " ms\n";

			//调整线程池后再通知， 此处调整改变IdleList，不改变BusyList，所以notify在调整指令前后执行都可以
			//为了保证调整改变IdleList最后个元素为本线程，所以调整后再解锁
			
			m_maxNumCond.notify();

		}
		else
		{
			std::cout << "ptask = NULL\n";
		}
		std::cout << std::endl;
	}
	std::cout << "thread is not running\n";
}

void ThreadPool::moveToBusyList(ThreadPtr pthread)
{
	m_BusyList.push_back(pthread);
}

void ThreadPool::moveToIdleList(ThreadPtr pthread)
{
	m_IdleList.push_back(pthread);
}

void ThreadPool::deleteFromBusyList(ThreadPtr pthread)
{
	auto iter = std::find(m_BusyList.begin(), 
						  m_BusyList.end(), 
						  pthread);

	if(iter != m_BusyList.end())
		m_BusyList.erase(iter);
}

void ThreadPool::deleteFromIdleList(ThreadPtr pthread)
{
	auto iter = std::find(m_IdleList.begin(), 
						  m_IdleList.end(), 
						  pthread);

	if(iter != m_IdleList.end())
		m_IdleList.erase(iter);
}

void ThreadPool::threadpoolAdjust()
{
	int nthread = m_IdleList.size();
	int operand = 0;
	
	if(nthread > m_availHigh)
	{
		printStatus();
		std::cout << "      delete :" << nthread - m_availNum << std::endl;
		deleteIdleThread(nthread - m_availNum);
	}
	else if(nthread < m_availLow)
	{
		printStatus();
		std::cout << "                 create : " << m_availNum - nthread << std::endl;
		createIdleThread(m_availNum - nthread);
	}
	else
	{
		printStatus();
		std::cout << "  no operand\n";
	}
	std::cout << std::endl;
}

void ThreadPool::printStatus()
{
	std::cout << "busy num = " << m_BusyList.size() << std::endl;
	std::cout << "idle num = " << m_IdleList.size() << std::endl;
}
void ThreadPool::deleteIdleThread(int num)
{

	VecThreads vecTmp;
	ThreadPtr thread_tmp;

	//将本线程临时保存 ? 此处不需要临时保存，在Adjust前已经移入到工作线程队列中了
	thread_tmp = m_IdleList.back();
	m_IdleList.pop_back();

	for(int i = 0; i != num; ++i)
	{
		if(!m_IdleList.empty())
		{
			auto thread = m_IdleList.back();
			thread->setStatus(false);
			vecTmp.push_back(thread);
			m_IdleList.pop_back();
		}
		else
		{
			logError("deleteIdleThread IdleList empty");
			break;
		}
	}

	m_IdleList.push_back(thread_tmp);

	m_buf.m_flag = false;
	m_buf.m_notEmpty.notifyAll();

	//等到要删除的线程join至结束，才将buf flag重置为true；
	for(auto & thread : vecTmp)
	{
		thread->join();
	}

	m_buf.m_flag = true;
}

void ThreadPool::createIdleThread(int num)
{
	for(int i = 0; i != num; ++i)
	{
		auto workthread = std::shared_ptr<Thread>(new WorkThread(*this));
		m_IdleList.push_back(workthread);
		workthread->start();
	}
}

void ThreadPool::start()
{
	for(int index = 0; index != m_threadsNum; ++index)
		m_IdleList.push_back(std::shared_ptr<Thread>(new WorkThread(*this)));

	//开启缓存线程加载缓存文件，初始化缓存线程缓存，开启定时器
//	m_cacheThread->start();

	//开启REDIS服务器
	acl::acl_cpp_init();
	
	std::cout << "IdleList.size = " << m_IdleList.size() << std::endl;
	for(const auto &r: m_IdleList)
		r->start();
}

void ThreadPool::stop()
{
	if(!m_isExit)
	{
		m_isExit = true;
		m_buf.m_flag = false;
		m_buf.m_notEmpty.notifyAll();
	}
		
	for( auto &thread: m_BusyList)
	{
		thread->join();
	}
	
	for( auto &thread: m_IdleList)
	{
		thread->join();
	}
	m_IdleList.clear();
	m_BusyList.clear();
}





}//namespace wd
