#include "Buffer.h"
#include "Task.h"
#include <iostream>
#include <assert.h>

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

void Buffer::setFlag(bool flag)
{
	m_flag = flag;
}

std::shared_ptr<Task> Buffer::pop()
{
	MutexLockGuard guard(m_mutex);

#if 1
	while(empty() && m_flag)
	{
		m_notEmpty.wait();
	}
	if(!m_flag)
	{
		return std::shared_ptr<Task>(NULL);
	}
#endif

#if  0   //此处为进程池模式下buffer的修改
	if(empty())
		return std::shared_ptr<Task>(NULL);
#endif
	assert(m_buf.empty() == false);
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


