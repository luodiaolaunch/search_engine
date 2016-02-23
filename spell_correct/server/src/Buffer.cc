#include "Buffer.h"
#include "Task.h"
#include <iostream>

namespace wd
{

Buffer::Buffer(size_t size)
	:	m_mutex(),
		m_notFull(m_mutex),
		m_notEmpty(m_mutex),
		m_size(size),
		m_flag(true)
	{

	}

void Buffer::push(Task* pTask)
{
	m_mutex.lock();
	while(full())
	{
		m_notFull.wait();
	}
	m_buf.push(std::shared_ptr<Task>(pTask));
	m_notEmpty.notify();
	m_mutex.unlock();
}


std::shared_ptr<Task> Buffer::pop()
{
	MutexLockGuard guard(m_mutex);
	while(empty() && m_flag)
	{
		m_notEmpty.wait();
	}
	if(!m_flag)
	{
		return std::shared_ptr<Task>(NULL);
		
	}
	std::shared_ptr<Task> tmp = m_buf.front();
	m_buf.pop();

	m_notFull.notify();
//	m_mutex.unlock();
	return tmp;
}

bool Buffer::full()
{

	return m_size == m_buf.size();
}

bool Buffer::empty()
{
	return 0 == m_buf.size();
}

}//namespace wd


