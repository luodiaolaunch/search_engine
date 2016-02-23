#include "ThreadPool.h"
#include "Thread.h"
#include "WorkThread.h"
#include "Task.h"
#include <iostream>

namespace wd
{

ThreadPool::ThreadPool(size_t nThread, size_t nBuf)
	:m_threadsNum(nThread),
	 m_cacheThread(std::shared_ptr<Thread>(new CacheThread(m_vecThreads))),  
	 m_buf(nBuf),
	 m_isExit(false)
{
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

void ThreadPool::threadFunc(Thread* workthread)
{
	while(!m_isExit)
	{
		auto pTask = getTask();
		if(pTask.get() != NULL)
		{
			pTask->setWordThreadPtr(dynamic_cast<WorkThread*>(workthread));
			pTask->process();//
		}
	}
}

void ThreadPool::start()
{
	for(int index = 0; index != m_threadsNum; ++index)
		m_vecThreads.push_back(std::shared_ptr<Thread>(new WorkThread(*this)));

	//开启缓存线程加载缓存文件，初始化缓存线程缓存，开启定时器
	m_cacheThread->start();

	//开启线程池并初始化缓存数据
	for(const auto &r: m_vecThreads)
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
	for( auto &thread: m_vecThreads)
	{
		thread->join();
	}
	m_vecThreads.clear();
}





}//namespace wd
