#include "TcpServer.h"
#include "ThreadPool.h"
#include <functional>

namespace wd
{

TcpServer::TcpServer(unsigned short port, ThreadPool& threadPool)
	:m_inetAddr(port),
	 m_socket(),
	 m_epoller(m_socket.fd()),
	 m_threadPool(threadPool)
{
	m_socket.ready(m_inetAddr);
}

TcpServer::TcpServer(const char *pIp, unsigned short port, ThreadPool& threadPool)
	:m_inetAddr(pIp, port),
	 m_socket(),
	 m_epoller(m_socket.fd()),
	 m_threadPool(threadPool)
{
	m_socket.ready(m_inetAddr);
}

void TcpServer::start()
{
	m_threadPool.start();
	m_epoller.loop();
}

void TcpServer::stop()
{
	m_epoller.unloop();
	m_threadPool.stop();
}

void TcpServer::setConnectionCallback(TcpServerCallback cb)
{
	m_epoller.setConnectionCallback(cb);
}

void TcpServer::setMessageCallback(TcpServerMessageCallback cb)
{
	m_epoller.setMessageCallback(std::bind(cb, std::placeholders::_1, std::ref(m_threadPool)));
}

void TcpServer::setCloseCallback(TcpServerCallback cb)
{
	m_epoller.setCloseCallback(cb);
}

}
//namespace wd
