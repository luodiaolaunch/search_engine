#ifndef MY_NONCOPYABE_H
#define MY_NONCOPYABE_H

namespace wd
{


class Noncopyable
{
protected:
	Noncopyable() {}
	Noncopyable(const Noncopyable&) = delete;
	Noncopyable& operator=(const Noncopyable&) = delete;
};

}//namespace wd

#endif
