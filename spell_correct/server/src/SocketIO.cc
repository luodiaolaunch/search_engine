#include "SocketIO.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace wd
{

SocketIO::SocketIO(int sockfd)
	:m_sockfd(sockfd)
{
}

size_t SocketIO::readn(char *buf, size_t cnt)
{
	size_t read_sum = 0, read_ret;
	while(read_sum < cnt)
	{
		read_ret = ::read(m_sockfd, buf + read_sum, cnt);
		if(read_ret == -1)
		{
			if(errno == EINTR)
				continue;
			return read_ret;
		}
		read_sum += read_ret;
	}
}

size_t SocketIO::writen(const char *buf, size_t cnt)
{
	size_t write_sum = 0, write_ret;
	while(write_sum < cnt)
	{
		write_ret = ::write(m_sockfd, buf + write_sum, cnt);
		if(write_ret == -1)
		{
			if(errno == EINTR)
				continue;
			return write_ret;
		}
		write_sum += write_ret;
	}
}

size_t SocketIO::recv_peek(char *buf, size_t cnt)
{
	int nread;
	do
	{
		nread = ::recv(m_sockfd, buf, cnt, MSG_PEEK);
	}while(nread == -1 && errno == EINTR);
	return nread;
}

size_t SocketIO::readline(char *buf, size_t cnt)
{
	size_t read_sum = 0;
	while(read_sum < cnt -1)
	{
		size_t nread = recv_peek(buf + read_sum, cnt - 1);
		if(nread <= 0)
			return nread;

		for(size_t idx = 0; idx != nread; ++idx)
		{
			if(buf[idx] == '\n')
			{
				size_t nsize = idx + 1;
				if(nsize != readn(buf + read_sum, nsize))
					return EXIT_FAILURE;
				read_sum += nsize;
				buf[read_sum] = 0;
				return read_sum;
			}
		}

		if(nread != readn(buf + read_sum, nread))
			return EXIT_FAILURE;
		read_sum += nread;
	}
	buf[read_sum] = 0;
	return cnt - 1;
}


}//namespace wd


