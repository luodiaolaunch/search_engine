#ifndef WD_TCPCONNECTION_H
#define WD_TCPCONNECTION_H

#include "Socket.h"
#include "SocketIO.h"
#include "Noncopyable.h"
#include "InetAddress.h"

#include <string>
#include <functional>
#include <memory>


namespace wd
{
using std::shared_ptr;
class ThreadPool;
class TcpConnection;
using TcpConnectionPtr = shared_ptr<TcpConnection>;

class TcpConnection : private Noncopyable,
	public std::enable_shared_from_this<TcpConnection>
{
public:
	using TcpConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
	using TcpConnectionMessageCallback = std::function<void(const TcpConnectionPtr&, ThreadPool&)>;

	TcpConnection(int sockfd);
	~TcpConnection();

	std::string receive();
	void send(const std::string );
	void shutdown();

	std::string toString();
	Socket& getSocket();

	void setConnectionCallback(TcpConnectionCallback cb);
	void setMessageCallback(TcpConnectionCallback cb);
	void setCloseCallback(TcpConnectionCallback cb);

	void handleConnectionCallback();
	void handleMessageCallback();
	void handleCloseCallback();

private:
	Socket 		m_socket;
	SocketIO 	m_socketIO;
	bool		m_isShutdownWrite;
	const InetAddress m_localAddr;
	const InetAddress m_peerAddr;

	TcpConnectionCallback m_onConnectionCb;
	TcpConnectionCallback m_onMessageCb;
	TcpConnectionCallback m_onCloseCb;
};

}//namespace wd


#endif

