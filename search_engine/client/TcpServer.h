#ifndef WD_TCPSERVER_H
#define WD_TCPSERVER_H

#include "InetAddress.h"
#include "Socket.h"
#include "EpollPoller.h"
#include "TcpConnection.h"

namespace wd
{
class ThreadPool;

class TcpServer
{
public:
	using TcpServerCallback = TcpConnection::TcpConnectionCallback;
	using TcpServerMessageCallback = TcpConnection::TcpConnectionMessageCallback;

	TcpServer(ThreadPool& threadpool, unsigned short port);
	TcpServer(ThreadPool& threadpool, const char* pIp, unsigned short port);

	void start();
	void stop();

	void setConnectionCallback(TcpServerCallback cb);
	void setMessageCallback(TcpServerMessageCallback cb);
	void setCloseCallback(TcpServerCallback cb);

private:
	ThreadPool& 	m_threadpool;
	InetAddress 	m_inetAddr;
	Socket 			m_socket;
	EpollPoller		m_epoller;
};

}//namespace wd


#endif
