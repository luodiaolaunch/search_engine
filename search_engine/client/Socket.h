#ifndef WD_SOCKET_H
#define WD_SOCKET_H

#include "InetAddress.h"
#include "Noncopyable.h"

namespace wd
{


class Socket : private Noncopyable
{
public:
	const int MAX_LISTEN_NUM = 10;
	Socket();
	Socket(int sockfd);

	void ready(const InetAddress &addr);
	int  accept(InetAddress &addr);
	int  accept();
	void shutdownWrite();
	int fd() const;
	static InetAddress getLocalAddress(int);
	static InetAddress getPeerAddress(int);

private:
	void listen();
	void bind(const InetAddress &addr);
	void setAddrReuse(bool flag);
	void setPortReuse(bool flag);

private:
	int m_sockfd;
};


}//namespace wd


#endif
