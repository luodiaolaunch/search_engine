#include "Diction.h"
#include "Index.h"
#include "Task.h"
#include "Config.h"
#include "Cache.h"
#include "ThreadPool.h"
#include "TcpServer.h"

#include <iostream>
#include <algorithm>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <utility>



int main(void)
{
	//启动配置文件加载数据
	wd::MyConf* pMyConf = wd::Singleton<wd::MyConf>::getInstance();
	pMyConf->loadConfig(wd::CONFIG_FILE);
	//pMyConf->loadConfig(argv[1]);
	std::string origfile = pMyConf->getConfigByName(wd::ORIGIN_NAME);
	std::string indexfile_EN = pMyConf->getConfigByName(wd::INDEX_NAME_EN);
	std::string indexfile_CN = pMyConf->getConfigByName(wd::INDEX_NAME_CN);
	std::string dictfile_CN = pMyConf->getConfigByName(wd::DICT_NAME_CN);
	std::string dictfile_EN = pMyConf->getConfigByName(wd::DICT_NAME_EN);
	std::string datadir = pMyConf->getConfigByName(wd::DATA_DIR);
	std::string port_str = pMyConf->getConfigByName(wd::PORT_NAME);
	std::string ip_str = pMyConf->getConfigByName(wd::IP_NAME);
	unsigned int port = ::atoi(port_str.c_str());

	wd::Diction* pdict = wd::Singleton<wd::Diction>::getInstance();
	wd::Index* pIndex = wd::Singleton<wd::Index>::getInstance();

	//遍历数据目录
	std::set<std::string> set_file;
	pdict->TraverseDir(datadir, set_file);

	//缺少中/英文索引或侯词文件将重新载入源文件并计算索引
	if(set_file.find(dictfile_CN) == set_file.end() ||
	   set_file.find(indexfile_CN) == set_file.end())
	{
		logInfo("load source_CN file");
		pdict->loadOriginDirectory_CN(origfile);
		pdict->writeDictFile_CN(dictfile_CN);

		pIndex->loadDictVecFromDictFile_CN();
		pIndex->establishIndexMap_CN();
		pIndex->writeIndexMapToIndexFile_CN();
	}
	else
	{		
		logInfo("load index_CN file");
		pIndex->loadDictVecFromDictFile_CN();
		pIndex->loadIndexMapFromIndexFile_CN();
	}

	if(set_file.find(dictfile_EN) == set_file.end() ||
	   set_file.find(indexfile_EN) == set_file.end())
	{
		logInfo("load source_EN file");
		pdict->loadOriginDirectory_EN(origfile);
		pdict->writeDictFile_EN(dictfile_EN);

		pIndex->loadDictVecFromDictFile_EN();
		pIndex->establishIndexMap_EN();
		pIndex->writeIndexMapToIndexFile_EN();
	}
	else
	{
		logInfo("load index_EN file");
		pIndex->loadDictVecFromDictFile_EN();
		pIndex->loadIndexMapFromIndexFile_EN();
	}

	//启动线程池
	wd::ThreadPool threadpool(wd::THREAD_NUM, wd::TASK_NUM);

	wd::TcpServer tcpServer(threadpool,  port);
	tcpServer.setConnectionCallback(wd::onConnection);
	tcpServer.setMessageCallback(wd::onMessage);
	tcpServer.setCloseCallback(wd::onClose);
	tcpServer.start();
	return 0;  
}


namespace wd 
{
	
void onConnection(const wd::TcpConnectionPtr &conn)
{
	logInfo(conn->toString().c_str());
	conn->send("Hello, welcome to wd's server\n");
}

void onMessage(const wd::TcpConnectionPtr &conn, wd::ThreadPool &threadpool)
{
	std::string msg(conn->receive());
	threadpool.addTask(new wd::Task(msg, conn));
}

void onClose(const wd::TcpConnectionPtr &conn)
{
	char buf[BUF_MAX_SIZE] = {0};
	::snprintf(buf, sizeof(buf), "%s close", conn->toString().c_str());
	logInfo(buf);
}


}//namespace wd 
