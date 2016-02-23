#include "Timer.h"
#include "Config.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <errno.h>


namespace wd
{

Timer::Timer(int value, int interval, CallBack cb)
	:m_value(value),
	 m_interval(interval),
	 m_cb(cb),
	 m_isStarted(false),
	 m_timerfd(createTimerfd())
{
}


int Timer::createTimerfd()
{
 	int timerfd = ::timerfd_create(CLOCK_REALTIME, 0);
 	if(-1 == timerfd)
	{
		logError("create timerfd");
	}
	return timerfd;
}

int Timer::setTimerfd(int timerfd, int value, int interval)
{
	struct itimerspec val;
	val.it_value.tv_sec = value;
	val.it_value.tv_nsec = 0;
	val.it_interval.tv_sec = interval;
	val.it_interval.tv_nsec = 0;

	int ret = ::timerfd_settime(timerfd, 0, &val, NULL);
	if(-1 == ret)
	{
		
		logError("set timerfd");
	}
	return ret;
}

void Timer::stopTimerfd()
{
	setTimerfd(m_timerfd, 0, 0);
}

ssize_t Timer::readTimerfd()
{
	//ssize_t howmany;
	uint64_t howmany;
	ssize_t ret = read(m_timerfd, &howmany, sizeof(howmany));
	if(ret != sizeof(howmany))
	{
		logError("read tiemrfd error");
	}
	return howmany;
}

void Timer::start()
{
	setTimerfd(m_timerfd, m_value, m_interval);

	struct pollfd pfd;
	pfd.fd = m_timerfd;
	pfd.events = POLLIN;
	m_isStarted = true;
	while(m_isStarted)
	{
		int ret = poll(&pfd, 1, POLL_WAIT);
		if(-1 == ret)
		{
			if(errno == EINTR)
				continue;
			logError("poll call error");
			exit(EXIT_FAILURE);
		}
		else if(0 == ret)
		{
			logInfo("poll timeout");
		}
		else
		{
			if(pfd.revents == POLLIN)
			{
				ssize_t howmany = readTimerfd();
				m_cb();
			}
		}
	}//end while
}

void Timer::stop()
{
	if(m_isStarted)
	{
		m_isStarted = false;
		stopTimerfd();
	}
}

Timer::~Timer()
{
	if(m_isStarted)
	{
		stop();
	}
}


}//namespace wd


