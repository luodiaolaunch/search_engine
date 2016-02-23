#include "MutexLock.h"


namespace wd
{


MutexLock::MutexLock()
	:m_isLock(false)
{
	pthread_mutex_init(&m_mutex, NULL);
}

MutexLock::~MutexLock()
{
	if(!m_isLock)
		pthread_mutex_destroy(&m_mutex);
}

void MutexLock::lock()
{
	pthread_mutex_lock(&m_mutex);
	m_isLock = true;
}	

void MutexLock::unlock()
{
	pthread_mutex_unlock(&m_mutex);
	m_isLock = false;
}

pthread_mutex_t* MutexLock::getMutexLock()
{
	return &m_mutex;
}

MutexLockGuard::MutexLockGuard(MutexLock &mutex)
	:m_mutex(mutex){}

MutexLockGuard::~MutexLockGuard()
{
	m_mutex.unlock();	
}


}//namespace wd
