#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>
#include <string.h>
#include <iterator>
#include <errno.h>
#include "ThreadPool.h"
#include "Ripepage.h"
#include "MyConf.h"
#include "Config.h"
#include "TcpServer.h"
#include "Task.h"
#include "json/json.h"

//void TraverseDirNonrecursive(const std::string &dirname, std::set<std::string> &set_filename);

static struct cmsghdr *cmptr = NULL;

int main(void)
{

	//加载配置文件
	wd::MyConf* pMyConf = 
		wd::Singleton<wd::MyConf>::getInstance();
	pMyConf->loadConfig(wd::CONFIG_NAME);

	std::string indexFile = pMyConf->getConfigByName(wd::INDEX_NAME);
	std::string offsetFile = pMyConf->getConfigByName(wd::OFFSET_NAME);
	std::string ripepageFile = pMyConf->getConfigByName(wd::RIPEPAGE_NAME);
	std::string dataDir = pMyConf->getConfigByName(wd::DATA_DIR);
	std::string ip_str = pMyConf->getConfigByName(wd::IP_NAME);
	std::string port_str = pMyConf->getConfigByName(wd::PORT_NAME);
	unsigned int port = ::atoi(port_str.c_str());

	//遍历数据目录，生成或加载索引文件
	std::set<std::string> set_file;
	wd::TraverseDirNonrecursive(dataDir, set_file);

	wd::Ripepage* pRipe = 
		wd::Singleton<wd::Ripepage>::getInstance();

	if(set_file.find(indexFile) == set_file.end() ||
	   set_file.find(offsetFile) == set_file.end() ||
	   set_file.find(ripepageFile) == set_file.end())
	{
		std::cout << "create lib...\n";
		pRipe->LoadSourceFile();
		pRipe->CreateRipepageAndOffsetLib();
		pRipe->CreateIndexLib();
		pRipe->DeletePageVector();
	}
	else
	{
		std::cout << "load lib...\n";
		pRipe->LoadRipepageLib();
		pRipe->LoadOffsetLib();
 		pRipe->LoadIndexLib();
		std::cout << "load lib success!\n";
	}

	wd::ThreadPool threadPool(wd::THREADS_NUM, wd::BUF_NUM);


	//启动线程池，开始服务器监听
	wd::TcpServer tcpServer(port, threadPool);
	tcpServer.setConnectionCallback(wd::onConnection);
	tcpServer.setMessageCallback(wd::onMessage);
	tcpServer.setCloseCallback(wd::onClose);

	//注册信号
//	wd::RegisterSignal();

	tcpServer.start();
}

namespace wd 
{

void RegisterSignal()
{
	struct sigaction act_chld, act_usr1;
	act_chld.sa_handler = sig_chld;
	sigemptyset(&act_chld.sa_mask);
	act_chld.sa_flags = 0;
	sigaction(SIGCHLD, &act_chld, NULL);

	act_usr1.sa_sigaction = sig_usr1;
	sigemptyset(&act_usr1.sa_mask);
	act_usr1.sa_flags = 0;
	act_usr1.sa_flags |= SA_SIGINFO;
	sigaction(SIGUSR1, &act_usr1, NULL);

}

void TraverseDirNonrecursive(const string &dirname, set<string> &set_filename)
{
	DIR *fdir;
	std::string dir_name = dirname;
	fdir = ::opendir(dir_name.c_str());
	if(fdir == NULL)
	{
		logError("TraverseDirNonrecursive fail");
		::closedir(fdir);
		::exit(EXIT_FAILURE);
	}

	struct dirent *p;
	std::string path_name;

	while(NULL != (p = readdir(fdir)))
	{
		if(::strcmp(p->d_name, ".") != 0 &&
				::strcmp(p->d_name, "..") != 0)
		{
			path_name = dir_name + "/" + std::string(p->d_name);
			set_filename.insert(path_name);
		}
	}
	::closedir(fdir);
}

void onConnection(const wd::TcpConnectionPtr &conn)
{
	logInfo(conn->toString().c_str());
	conn->send("Hello, welcome to WD Search\n");
}

void onMessage(const wd::TcpConnectionPtr &conn, wd::ThreadPool &threadPool)
{
	std::string msg(conn->receive());
	threadPool.addTask(new Task(msg, conn, threadPool.getRedisClientPool()));
	return;
}

void onClose(const wd::TcpConnectionPtr &conn)
{
	char buf[BUF_SIZE] = {0};
	::snprintf(buf, sizeof(buf), "%s close\n", conn->toString().c_str());
	logInfo(buf);
}


void sig_chld(int signo)
{
#if 0
	pid_t pid;
	int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		logInfo("child process terminnated\n");
	return;
#endif
}

void sig_usr1(int signo, siginfo_t* siginfo, void * context)
{
#if 0
	//设置相应进程为空闲状态
	pid_t child_pid = siginfo->si_pid;
	auto pProcessPool = ProcessPool::getInstance(PROCESS_NUM, BUF_NUM);
	auto vecProcess = pProcessPool->getProcessVec();
	for(auto & process : vecProcess)
	{
		process->setStatus(false);
	}
#endif
}

int send_fd(int fd, int fd_to_send)
{
	struct iovec iov[1];
	struct msghdr msg;
	char buf[2];

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	if(fd_to_send < 0)
	{
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		buf[1] = -fd_to_send;
		if(buf[1] == 0)
			buf[1] = -1;
	}
	else
	{
		if(cmptr == NULL && (cmptr = (struct cmsghdr*)::malloc(CONTROLLEN)) == NULL)
			return -1;
		cmptr->cmsg_level = SOL_SOCKET;
		cmptr->cmsg_type = SCM_RIGHTS;
		cmptr->cmsg_len = CONTROLLEN;
		msg.msg_control = cmptr;
		msg.msg_controllen = CONTROLLEN;
		*(int *)CMSG_DATA(cmptr) = fd_to_send;
		buf[1] = 0;
	}
	buf[0] = 0;
	if(sendmsg(fd, &msg, 0) != sizeof(buf))
		return -1;
	return 0;
}

int recv_fd(int fd)
{
	int newfd, nret, status;
	char *ptr;
	char buf[BUF_SIZE];
	struct iovec iov[1];
	struct msghdr msg;

	status = -1;
	while(1)
	{
		iov[0].iov_base = buf;
		iov[0].iov_len = sizeof(buf);
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;
		msg.msg_name = NULL;
		msg.msg_namelen = 0;
		if(cmptr == NULL && (cmptr = (struct cmsghdr*)::malloc(CONTROLLEN)) == NULL)
			return -1;
		msg.msg_control = cmptr;
		msg.msg_controllen = CONTROLLEN;
		if((nret = recvmsg(fd, &msg, 0)) < 0)
		{
			logError("recvmsg error");
			return -1;
		}
		else if(nret == 0)
		{
			logError("main process exit");
			return -1;
		}

		int cnt = 0;
		for(ptr = buf; ptr < &buf[nret];)
		{
			if(*ptr++ == 0)
			{
				if(ptr != &buf[nret - 1])
				{
					logError("message format error");
					return -1;
				}
				status = *ptr & 0XFF;
				if(status == 0)
				{
					if(msg.msg_controllen < CONTROLLEN)
					{
						logError("status = 0 but no fd");
						return -1;
					}
					newfd = *(int*)CMSG_DATA(cmptr);
				}
				else
					newfd = -status;
			}
			nret -= 2; //管理进程中验证buf为2个字节
		}
		if(status >= 0)
			return newfd;
	}
}
}//namespace wd
