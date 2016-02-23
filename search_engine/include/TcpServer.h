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

	TcpServer(unsigned short port, ThreadPool &threadPool);
	TcpServer(const char* pIp, unsigned short port, ThreadPool &threadPool);

	void start();
	void stop();

	void setConnectionCallback(TcpServerCallback cb);
	void setMessageCallback(TcpServerMessageCallback cb);
	void setCloseCallback(TcpServerCallback cb);

private:
	InetAddress 	m_inetAddr;
	Socket 			m_socket;
	EpollPoller		m_epoller;
	ThreadPool&		m_threadPool;

};



}//namespace wd


#endif
