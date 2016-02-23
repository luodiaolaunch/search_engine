#ifndef WD_SOCKETIO_H
#define WD_SOCKETIO_H

#include <stdio.h>

namespace wd
{
class SocketIO
{
public:
	SocketIO(int sockfd);

	size_t readn(char *buf, size_t cnt);
	size_t writen(const char *buf, size_t cnt);
	size_t readline(char *buf, size_t cnt);

private:
	size_t recv_peek(char *buf, size_t cnt);

private:
	int m_sockfd;
};

}//namespace wd

#endif
