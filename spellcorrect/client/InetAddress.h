#ifndef WD_INETADRESS_H
#define WD_INETADRESS_H

#include <netinet/in.h>
#include <string>

namespace wd
{

class InetAddress
{
public:
	InetAddress(short port);
	InetAddress(const char *pIp, short port);
	InetAddress(const struct sockaddr_in &addr);

	const struct sockaddr_in* getSockAddrPtr() const;
	std::string ip()const;
	unsigned short port()const;

private:
	struct sockaddr_in m_addr;
};

}//namespace wd


#endif
