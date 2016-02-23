#ifndef MY_MUTEXLOCK_H
#define MY_MUTEXLOCK_H

#include <pthread.h>
#include "Noncopyable.h"

namespace wd
{

class MutexLock : Noncopyable
{
public:
	MutexLock();
	~MutexLock();

	void lock();
	void unlock();
	pthread_mutex_t *getMutexLock();
private:
	pthread_mutex_t m_mutex;
	bool m_isLock;
};


class MutexLockGuard
{
public:
	MutexLockGuard(MutexLock &mutex);
	~MutexLockGuard();
private:
	MutexLock& m_mutex;
};


}//namespace wd

#endif
