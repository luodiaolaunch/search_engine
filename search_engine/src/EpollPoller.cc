#include "EpollPoller.h"
#include "Config.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <iostream>

namespace wd
{

//包裹函数
int createEpollerFd()
{
	int efd = ::epoll_create1(0);
	if(-1 == efd)
	{
		logError("epoll_create1");
		exit(EXIT_FAILURE);
	}
}

void addEpollerFdRead(int efd, int fd)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	int ret = ::epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev);
	if(-1 == ret)
	{
		logError("epoll_ctl add");
		exit(EXIT_FAILURE);
	}
}

void deleteEpollerFdRead(int efd, int fd)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	int ret = epoll_ctl(efd, EPOLL_CTL_DEL, fd, &ev);
	if(ret == -1)
	{
		logError("epoll_ctl del");
		exit(EXIT_FAILURE);
	}
}

int acceptConnFd(int listenfd)
{
	int peerfd = 0;
	while((peerfd = ::accept(listenfd, NULL, NULL)) < 0)
	{
		//accept为低速系统调用，能被系统中断
		if(errno == EINTR)
			continue;
		else
			logError("accept");
	}
	return peerfd;
}

size_t recvPeek(int sockfd, void *buf, size_t len)
{
	int nread;
	do
	{
		nread = ::recv(sockfd, buf, len, MSG_PEEK);
	}while(nread == -1 && errno == EINTR);
	return nread;
}

bool isConnectionClosed(int sockfd)
{
	char buf[1014];
	int nread = recvPeek(sockfd, buf, sizeof(buf));
	if(-1 == nread)
	{
		logInfo("recvPeek");
	}	
	return (0 == nread);
}

//===============================================

	EpollPoller::EpollPoller(int listenfd)
:m_epollfd(createEpollerFd()),
	m_listenfd(listenfd),
	m_isLooping(false),
	m_eventsList(1024)
{
	addEpollerFdRead(m_epollfd, listenfd);
}

EpollPoller::~EpollPoller()
{
	::close(m_epollfd);
}

void EpollPoller::loop()
{
	m_isLooping = true;
	while(m_isLooping)
	{
		waitEpollfd();
	}
}

void EpollPoller::unloop()
{
	if(m_isLooping)
		m_isLooping = false;
}

void EpollPoller::setConnectionCallback(EpollPoller::EpollCallback cb)
{
	m_onConnectionCb = cb;
}

void EpollPoller::setMessageCallback(EpollPoller::EpollCallback cb)
{
	m_onMessageCb = cb;
}

void EpollPoller::setCloseCallback(EpollPoller::EpollCallback cb)
{
	m_onCloseCb = cb;
}

void EpollPoller::waitEpollfd()
{
	int ret;
	do
	{
		ret = ::epoll_wait(m_epollfd, 
						   &(*m_eventsList.begin()),
						   m_eventsList.size(),
						   EPOLL_WAIT);
	}while(-1 == ret && errno == EINTR);

	if(-1 == ret)
	{
		logError("epoll_wait");
		exit(EXIT_FAILURE);
	}
	else if(0 == ret)
	{
		logInfo("epoll wait timeout\n");
	}
	else
	{
		if(ret == static_cast<int>(m_eventsList.size()))
		{
			m_eventsList.resize(2 * ret);
		}
	}

	for(int idx = 0; idx != ret; ++idx)
	{
		if (m_eventsList[idx].data.fd == m_listenfd)
		{
			if(m_eventsList[idx].events & EPOLLIN)
			{
				handleConnection();
			}
		}
		else 
		{
			if(m_eventsList[idx].events & EPOLLIN)
			{
				handleMessage(m_eventsList[idx].data.fd);
			}
		}
	}//end for
}

void EpollPoller::handleConnection()
{
	int peerfd = acceptConnFd(m_listenfd);
	addEpollerFdRead(m_epollfd, peerfd);

	TcpConnectionPtr conn(new TcpConnection(peerfd));

	conn->setConnectionCallback(m_onConnectionCb);
	conn->setMessageCallback(m_onMessageCb);
	conn->setCloseCallback(m_onCloseCb);
	
	std::pair<ConnectionMap::iterator, bool> ret_pair;
	ret_pair = m_connMap.insert(std::make_pair(peerfd, conn));
	assert(ret_pair.second);
	//void(ret_pair);

	conn->handleConnectionCallback();
}

void EpollPoller::handleMessage(int peerfd)
{
	bool isClosed = isConnectionClosed(peerfd);
	auto it = m_connMap.find(peerfd);
	assert(it != m_connMap.end());

	if(isClosed)
	{
		it->second->handleCloseCallback();
		deleteEpollerFdRead(m_epollfd, peerfd);
		m_connMap.erase(it);
	}
	else
	{
		it->second->handleMessageCallback();
	}
}

}//namespace 
