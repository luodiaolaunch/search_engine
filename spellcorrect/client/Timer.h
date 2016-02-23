#ifndef WD_TIMER_H
#define WD_TIMER_H

#include "Noncopyable.h"
#include <functional>

namespace wd
{

class Timer : private Noncopyable
{
public:
	typedef std::function<void()> CallBack;

	Timer(int value, int interval, CallBack cb);
	~Timer();

	void start();
	void stop();

private:
	int createTimerfd();
	int setTimerfd(int timerfd, int value, int interval);
	void stopTimerfd();
	ssize_t readTimerfd();

private:
	int 	 m_timerfd;
	int 	 m_value;
	int 	 m_interval;
	bool 	 m_isStarted;
	CallBack m_cb;
};

}// namespace wd

#endif
