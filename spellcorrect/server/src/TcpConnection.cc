#include "TcpConnection.h"
#include <string.h>
#include <string>
#include <stdio.h>

namespace wd
{

TcpConnection::TcpConnection(int sockfd)
	:m_socket(sockfd),
	 m_socketIO(sockfd),
	 m_localAddr(wd::Socket::getLocalAddress(sockfd)),
	 m_peerAddr(wd::Socket::getPeerAddress(sockfd)),
	 m_isShutdownWrite(false)
{
}


std::string TcpConnection::receive()
{
	char buf[1024];
	bzero(buf, sizeof(buf));
	size_t ret = m_socketIO.readline(buf, sizeof(buf));
	if(ret == 0)
	{
		return std::string();
	}
	else
	{
		return std::string(buf);
	}

}

void TcpConnection::send(const std::string msg)
{
	m_socketIO.writen(msg.c_str(), msg.size());
}

void TcpConnection::shutdown()
{
	m_socket.shutdownWrite();
	m_isShutdownWrite = true;
}

TcpConnection::~TcpConnection()
{
	if(!m_isShutdownWrite)
		shutdown();
}

std::string TcpConnection::toString()
{
	char str[100];
	snprintf(str, sizeof(str), "%s:%d -> %s:%d",
			 m_localAddr.ip().c_str(),
			 m_localAddr.port(),
			 m_peerAddr.ip().c_str(),
			 m_peerAddr.port());
	return std::string(str);
}

void TcpConnection::setConnectionCallback(TcpConnectionCallback cb)
{
	m_onConnectionCb = cb;
}

void TcpConnection::setMessageCallback(TcpConnectionCallback cb)
{
	m_onMessageCb = cb;
}

void TcpConnection::setCloseCallback(TcpConnectionCallback cb)
{
	m_onCloseCb = cb;
}

void TcpConnection::handleConnectionCallback()
{
	if(m_onConnectionCb)
		m_onConnectionCb(shared_from_this());
}

void TcpConnection::handleMessageCallback()
{
	if(m_onMessageCb)
		m_onMessageCb(shared_from_this());
}


void TcpConnection::handleCloseCallback()
{
	if(m_onCloseCb)
		m_onCloseCb(shared_from_this());
}

Socket& TcpConnection::getSocket()
{
	return m_socket;
}

}//namespace wd

