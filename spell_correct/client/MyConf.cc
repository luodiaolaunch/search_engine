#include "MyConf.h"
#include "Config.h"

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <algorithm>

namespace wd
{

#if 0
MyConf* MyConf::m_pMyConf = NULL;

MyConf* MyConf::getInstance()
{
	if(m_pMyConf == NULL)
		m_pMyConf = new MyConf();
	return m_pMyConf;
}

#endif

std::string MyConf::getConfigByName(const std::string &name)
{
	auto it = m_conf.find(name);
	if(it == m_conf.end())
	{
		return std::string(" ");
	}else{
		return m_conf[name];
	}
}

void MyConf::loadConfig(const std::string &path)
{
	std::ifstream ifs(path);
	if(!ifs)
	{
		logError("load Config fail");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs(ifs);

	std::string line, name, value;
	while(std::getline(ifs, line))
	{
		auto n = line.find('=');
		name = line.substr(0, n);
		value = line.substr(n+1);
		

		//删除等号两端空白字符
		name.erase(std::remove_if(name.begin(),
							name.end(),
							[](char x){ return std::isspace(x); }),
							name.end());

		value.erase(std::remove_if(value.begin(),
								 value.end(),
								 [](char x){ return std::isspace(x); }),
								 value.end());

		auto it_map = m_conf.insert(std::make_pair(name, value));
		if(!it_map.second)
		{
			logInfo("config file duplicated data");
		}
	}
}

}//namespace wd

