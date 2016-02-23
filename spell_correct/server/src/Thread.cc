#include "Thread.h"
#include <iostream>
#include <pthread.h>

namespace  wd
{


Thread::Thread()
{}

void Thread::start()
{
	pthread_create(&m_pid, NULL, threadFunc, this);
	m_isRunning = true;
}

void* Thread::threadFunc(void* arg)
{
	Thread* pthread = (Thread*) arg;
	if(pthread != NULL)
	{
		pthread->run();
	}
	pthread->m_isRunning = false;
	return (void*) 0;
}

void Thread::join()
{
	pthread_join(m_pid, NULL);
}

Thread::~Thread()
{
	if(m_isRunning)
		pthread_detach(m_pid);
}

}//namespace wd
