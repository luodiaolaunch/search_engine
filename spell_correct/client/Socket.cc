#include "Socket.h"
#include "InetAddress.h"
#include "Mylog.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

namespace wd
{

Socket::Socket()
{
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == m_sockfd)
	{
		logError("socket error");
		::exit(EXIT_FAILURE);
	}
}

Socket::Socket(int sockfd)
	:m_sockfd(sockfd) {}

void Socket::ready(const InetAddress &addr)
{
	setAddrReuse(true);
	setPortReuse(true);
	bind(addr);
	listen();	
}

void Socket::bind(const InetAddress &addr)
{
	if(-1 == ::bind(m_sockfd, (struct sockaddr*) addr.getSockAddrPtr(), sizeof(addr)))
	{
		logError("bind address error");
		::close(m_sockfd);
		::exit(EXIT_FAILURE);
	}
}

void Socket::listen()
{
	if(-1 == ::listen(m_sockfd, MAX_LISTEN_NUM))
	{
		logError("listen error");
		::close(m_sockfd);
		::exit(EXIT_FAILURE);
	}		
}

void Socket::setAddrReuse(bool flag)
{
	int on = flag ? 1 : 0;
	if(-1 == ::setsockopt(m_sockfd, SOL_SOCKET, 
					    SO_REUSEADDR, &on, static_cast<socklen_t>(sizeof(on))))
	{
		logError("setsockopt reuseaddr error");
	}
}

void Socket::setPortReuse(bool flag)
{
#ifdef SO_REUSEPORT
	int on = flag ? 1 : 0;
	if(-1 == ::setsockopt(m_sockfd, SOL_SOCKET, 
				SO_REUSEPORT, &on, static_cast<socklen_t>(sizeof(on))))
	{
		logError("setsockopt reuseport error");
	}
#else
	if(flag)
	{
		logInfo("SO_REUSEPROT not defined");
	}
#endif
}

int Socket::accept()
{
	int peerfd = ::accept(m_sockfd, NULL, 0);
	if(peerfd == -1)
	{
		logError("accept error");
	}
	return peerfd;
}

int Socket::accept(InetAddress &addr)
{
	auto pAddr = addr.getSockAddrPtr();
	socklen_t len = static_cast<socklen_t>(sizeof(decltype(addr.getSockAddrPtr())));
	int peerfd = ::accept(m_sockfd, (struct sockaddr*) pAddr, &len);
	if(peerfd == -1)
	{
		logError("accept(InetAddress&) error");
	}
	return peerfd;
}

void Socket::shutdownWrite()
{
	if(-1 == ::shutdown(m_sockfd, SHUT_WR))
	{
		logError("shutdown error");
	}	
}

int Socket::fd() const
{
	return m_sockfd;
}

InetAddress Socket::getLocalAddress(int sockfd)
{
	struct sockaddr_in addr;
	socklen_t len = static_cast<socklen_t>(sizeof(sockaddr_in));
	if(-1 == ::getsockname(sockfd, (struct sockaddr*) &addr, &len))
	{
		logError("getsockname error");
	}
	return InetAddress(addr);
}

InetAddress Socket::getPeerAddress(int sockfd)
{
	struct sockaddr_in addr;
	socklen_t len = static_cast<socklen_t>(sizeof(sockaddr_in));
	if(-1 == ::getpeername(sockfd, (struct sockaddr*) &addr, &len))
	{
		logError("getpeername error");
	}
	return InetAddress(addr);
}
}//namespace wd
