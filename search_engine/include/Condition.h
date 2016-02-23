#ifndef MY_CONDITION_H
#define MY_CONDITION_H

#include <pthread.h>
#include "Noncopyable.h"

namespace wd
{
class MutexLock;
class Condition : Noncopyable	
{
public:
	Condition(MutexLock&);
	~Condition();

	void wait();
	void notify();
	void notifyAll();
	//void wakeup_empty();
private:
	pthread_cond_t m_cond;
	MutexLock &m_mutex;
};


}//namespace wd

#endif
