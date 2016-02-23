#include "InetAddress.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


namespace wd
{

InetAddress::InetAddress(short port)
{
	::bzero(&m_addr, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);
	m_addr.sin_addr.s_addr = INADDR_ANY;
}

InetAddress::InetAddress(const char *pIp, short port)
	:InetAddress(port)
{
	m_addr.sin_addr.s_addr = inet_addr(pIp);
}

InetAddress::InetAddress(const struct sockaddr_in &addr)
	:m_addr(addr)
{
	
}

const struct sockaddr_in* InetAddress::getSockAddrPtr() const
{
	return &m_addr;
}

std::string InetAddress::ip() const
{
	return std::string(inet_ntoa(m_addr.sin_addr));
}

unsigned short InetAddress::port() const 
{
	return ntohs(m_addr.sin_port);
}


}//namespace wd

