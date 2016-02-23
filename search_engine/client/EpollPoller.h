#ifndef WD_EPOLLPOLLER_H
#define WD_EPOLLPOLLER_H

#include "Noncopyable.h"
#include "TcpConnection.h"
#include <sys/epoll.h>
#include <vector>
#include <map>

namespace wd
{

class EpollPoller : private Noncopyable
{
public:
	using EpollCallback = TcpConnection::TcpConnectionCallback;

	EpollPoller(int listenfd);
	~EpollPoller();

	void loop();
	void unloop();

	void setConnectionCallback(EpollCallback cb);
	void setMessageCallback(EpollCallback cb);
	void setCloseCallback(EpollCallback cb);

private:
	void waitEpollfd();
	void handleConnection();
	void handleMessage(int peerfd);

private:
	int m_epollfd;
	int	m_listenfd;
	bool m_isLooping;

	using EventList = std::vector<struct epoll_event>;
	EventList m_eventsList;

	using ConnectionMap = std::map<int, TcpConnectionPtr>;
	ConnectionMap m_connMap;

	EpollCallback m_onConnectionCb;
	EpollCallback m_onMessageCb;
	EpollCallback m_onCloseCb;
};
}//namespace wd

#endif
