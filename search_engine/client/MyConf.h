#ifndef WD_MYCONF_H
#define WD_MYCONF_H

#include <map>
#include <string>

namespace wd
{
 
class MyConf
{
public:
//	static MyConf* getInstance();
	
	std::string getConfigByName(const std::string &name);
	void loadConfig(const std::string &path);

	MyConf() {}

private:
//	static MyConf* m_pMyConf;
	std::map<std::string, std::string> m_conf;

};

}//namespace wd

#endif
