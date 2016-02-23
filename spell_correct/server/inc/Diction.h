#ifndef WD_DICTION_H
#define WD_DICTION_H

#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <map>
#include <set>
#include "jieba/src/Jieba.hpp"

namespace wd
{

class Diction
{
public:
	using DictDataVec = std::vector<std::pair<std::string, int > >;
	using DictDataMap = std::map<std::string, int> ;

	Diction();

	void loadOriginDirectory_CN(std::string &dir);
	void loadOriginDirectory_EN(std::string &dir);


	void writeDictFile_CN(const std::string &filename_CN);
	void writeDictFile_EN(const std::string &filename_EN);

	void TraverseDir(const std::string &dirname, std::set<std::string>& set_filename);
	
private:
	void loadOriginFile_EN(const std::string &filename);
	void loadOriginFile_CN(const std::string &filename);

private:
	DictDataMap m_mapDict_CN;
	DictDataMap m_mapDict_EN;
	cppjieba::Jieba m_jieba;
};


}//namespce wd


#endif
