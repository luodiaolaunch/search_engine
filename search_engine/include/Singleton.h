#ifndef WD_SINGLETON_H
#define WD_SINGLETON_H

#include "Mylog.h"
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>


namespace wd
{

template <typename T>
class Singleton
{
public:
	static T* getInstance();

private:
	Singleton(){}
	static void init_routine(void);

	static void destroy();

private:
	static T *pSingleton;
	static pthread_once_t once_control;
};


template <typename T>
T* Singleton<T>::pSingleton = NULL;

template<typename T>
pthread_once_t Singleton<T>::once_control = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::getInstance()
{
	if(0 != ::pthread_once(&once_control, init_routine))
	{
		logError("pthread_once error");
		::exit(EXIT_FAILURE);
	}
	return pSingleton;
}

template<typename T>
void Singleton<T>::init_routine(void)
{
	if(pSingleton == NULL)
	{
		::atexit(destroy);
		pSingleton = new T;
	}
}

template<typename T>
void Singleton<T>::destroy()
{
	if(pSingleton)
	{
		delete pSingleton;
	}
}

}//namespace wd


#endif  



