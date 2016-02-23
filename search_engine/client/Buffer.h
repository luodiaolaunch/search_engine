#ifndef MY_BUFFER_H
#define MY_BUFFER_H

#include <queue>
#include <memory>
#include "Noncopyable.h"
#include "Condition.h"
#include "MutexLock.h"

namespace wd
{
class Task;
class Buffer : Noncopyable
{
	friend class ThreadPool;
public:
	Buffer(size_t size);

	void push(Task*);
	std::shared_ptr<Task> pop();

	bool full();
	bool empty();
private:
	MutexLock  		m_mutex;
	Condition  		m_notFull;
	Condition 		m_notEmpty;
	bool 			m_flag;
	size_t     		m_size;
	std::queue<std::shared_ptr<Task> > m_buf;

	
};
}//namespace wd

#endif
