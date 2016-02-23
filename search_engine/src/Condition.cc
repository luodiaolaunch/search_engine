#include "Condition.h"
#include "MutexLock.h"  

namespace wd
{

Condition::Condition(MutexLock& mutex)
	:m_mutex(mutex)
{
	pthread_cond_init(&m_cond, NULL);
}

Condition::~Condition()
{
	pthread_cond_destroy(&m_cond);
}

void Condition::wait()
{
	pthread_cond_wait(&m_cond, m_mutex.getMutexLock());
}

void Condition::notify()
{
	pthread_cond_signal(&m_cond);
}

void Condition::notifyAll()
{
	pthread_cond_broadcast(&m_cond);
}

}//namespace wd
