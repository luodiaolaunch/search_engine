#ifndef WD_RAII_H
#define WD_RAII_H

#include "Noncopyable.h"
#include <utility>


namespace wd
{

template<typename T>
class RAII : private Noncopyable
{
public:
	explicit RAII(T *p);
	~RAII();

	void reset(T *p);
	T* get() const;
	T& operator*() const;
	void swap(RAII& other);

private:
	T* m_srcPtr;
};

template<typename T>
RAII<T>::RAII(T* p)
	:m_srcPtr(p)
{}

template<typename T>
RAII<T>::~RAII()
{
	delete m_srcPtr;
}

template<typename T>
void RAII<T>::reset(T *p)
{
	delete m_srcPtr;
	m_srcPtr = p;
}

template<typename T>
T* RAII<T>::get() const
{
	return m_srcPtr;
}

template<typename T>
T& RAII<T>::operator*() const
{
	return *m_srcPtr;
}

template<typename T>
void RAII<T>::swap(RAII& other)
{
	std::swap(m_srcPtr, other.m_srcPtr);
}

}//namespace wd


#endif
