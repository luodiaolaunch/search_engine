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
	virtual ~Thread();

	void start();
	void join();
	virtual void run() = 0 ;
	void setStatus(bool status);
	bool isRunning()const;

	static void *threadFunc(void*);
protected:
	pthread_t m_pid;
	bool m_isRunning;
};

}//namespace wd


#endif

