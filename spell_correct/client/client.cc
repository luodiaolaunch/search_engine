#include "InetAddress.h"
#include "Socket.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <errno.h>

size_t readn(int sockfd, char *buf, size_t cnt)
{
	size_t read_sum = 0, read_ret;
	while(read_sum < cnt)
	{
		read_ret = ::read(sockfd, buf + read_sum, cnt);
		if(read_ret == -1)
		{
			if(errno == EINTR)
				continue;
			return read_ret;
		}
		read_sum += read_ret;
	}
}

int main(void)
{
	wd::InetAddress inetaddr("192.168.4.6", 8888);
	wd::Socket socket;
	if(-1 == ::connect(socket.fd(), 
					  (struct sockaddr*) inetaddr.getSockAddrPtr(),
					   sizeof(struct sockaddr)))
	{
		perror("connect error");
		::close(socket.fd());
		::exit(EXIT_FAILURE);	
	}
	printf("connect success\n");
	char buf[1024];
	bzero(buf, sizeof(buf));
	::read(socket.fd(), buf, 1024);
	printf("%s", buf);
	while(1)
	{
		::bzero(buf, sizeof(buf));
		::fgets(buf, sizeof(buf), stdin);
		int nwrite = ::write(socket.fd(), buf, strlen(buf));
		if(nwrite == -1)
		{
			if(errno == EINTR)
				continue;
			perror("write");
			::close(socket.fd());
			::exit(EXIT_FAILURE);
		}
		else if(nwrite == 0)
		{
			printf("server close!\n");
			::close(socket.fd());
			::exit(EXIT_FAILURE);
		}

		size_t len = 0;
		read(socket.fd(), &len, sizeof(len));

		bzero(buf, sizeof(buf));
		int nread = readn(socket.fd(), buf, len);
		printf("%s\n", buf);
	}
}
