#ifndef MY_THREAD_H
#define MY_THREAD_H

#include "Noncopyable.h"
#include <pthread.h>

namespace wd
{

class Thread // private Noncopyable
{
public:
	Thread();
	~Thread();

	void start();
	void join();
	virtual void run() = 0 ;

	static void *threadFunc(void*);
protected:
	pthread_t m_pid;
	bool m_isRunning;
};

}//namespace wd


#endif

