#include "InetAddress.h"
#include "Socket.h"
#include "../include/json/json.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <errno.h>

using std::string;


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
	wd::InetAddress inetaddr(8889);
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
	Json::Value value;
	Json::Reader reader;
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
	//	return 0;

		char *pageBuf = new char[len];
		bzero(buf, sizeof(buf));
		int nread = readn(socket.fd(), pageBuf, len);

		if(reader.parse(pageBuf, value))
		{
			for(int i = 0; i != value.size(); ++i)
			{
				string title = value[i]["title"].asString();
				string content = value[i]["content"].asString();
				std::cout << "----------------------------------------------------\n";
				std::cout << "Title : " << title << std::endl;
				//std::cout << "Content : " << content << std::endl;
				std::cout << "----------------------------------------------------\n";
			}

		}
		::bzero(pageBuf, sizeof(pageBuf));
	}
}
