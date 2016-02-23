#include "TcpServer.h"
#include "ThreadPool.h"
#include <functional>

namespace wd
{

TcpServer::TcpServer(ThreadPool& threadpool, unsigned short port)
	:m_threadpool(threadpool),
	 m_inetAddr(port),
	 m_socket(),
	 m_epoller(m_socket.fd())
{
	m_socket.ready(m_inetAddr);
}

TcpServer::TcpServer(ThreadPool& threadpool, const char *pIp, unsigned short port)
	:m_threadpool(threadpool),
	 m_inetAddr(pIp, port),
	 m_socket(),
	 m_epoller(m_socket.fd())
{
	m_socket.ready(m_inetAddr);
}

void TcpServer::start()
{
	m_threadpool.start();
	m_epoller.loop();
}

void TcpServer::stop()
{
	m_epoller.unloop();
}

void TcpServer::setConnectionCallback(TcpServerCallback cb)
{
	m_epoller.setConnectionCallback(cb);
}

void TcpServer::setMessageCallback(TcpServerMessageCallback cb)
{
	m_epoller.setMessageCallback(std::bind(cb, std::placeholders::_1, std::ref(m_threadpool)));
}

void TcpServer::setCloseCallback(TcpServerCallback cb)
{
	m_epoller.setCloseCallback(cb);
}

}
//namespace wd
