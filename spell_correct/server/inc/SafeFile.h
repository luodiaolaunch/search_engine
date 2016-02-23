#ifndef WD_SAFEFILE_H
#define WD_SAFEFILE_H

#include "Noncopyable.h"
#include <fstream>
#include <iostream>

namespace wd
{

template<typename T>
class SafeFile : private Noncopyable
{
public:
	explicit SafeFile(T& src);
	~SafeFile();

	T& get();

private:
	T& m_src;
};

template<typename T>
SafeFile<T>::SafeFile(T&  src)
	:m_src(src)
{
	if(!m_src)
	{
		logError("SafeFile");

	}
}

template<typename T>
SafeFile<T>::~SafeFile()
{
	m_src.close();
}

template<typename T>
T& SafeFile<T>::get()
{
	return m_src;
}

}//namespace wd

#endif
