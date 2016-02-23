#include "Index.h"
#include "Config.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cstdio>

namespace wd
{
#if 0
bool findChar(unsigned char *ch, const std::string str, int step)
{
	int idx = 0, cnt = 0, len = str.size()/step;
	while(idx < len)
	{
		std::string tmp_str = str.substr(idx*step, (idx+1)*step);
		while(cnt < step)
		{
			if(ch[cnt] != tmp_str[cnt])
			{
				break;
			}
			++cnt;
		}
		if(cnt == step)
			return true;

		++idx;
	}
	return false;
}

int unicode_to_utf8(unsigned long unic, unsigned char *output, int outSize)
{
	assert(output != NULL);
	assert(outSize >= 6);
	if(unic <= 0x0000007F)
	{
		*output = (unic & 0x7F);
		return 1;
	}
	else if(unic >= 0x00000080 && unic <= 0x000007FF)
	{
		*(output + 1) = (unic & 0x3F) | 0x80;
		*output 	  = ((unic >> 6) & 0x1F) | 0xC0;
		return 2;
	}
	else if(unic >= 0x00000800 && unic <= 0x0000FFFF)
	{
		*(output + 2) = (unic & 0x3F) | 0x80;
		*(output + 1) = ((unic >> 6) & 0x3F) | 0x80;
		*output 	  = ((unic >> 12) & 0x0F) | 0xE0;
		return 3;
	}// ...
	return 4;
}
#endif

//=================================================


std::ostream& operator<<(std::ostream &os, std::set<Index::set_size_type>& index_set)
{ 
	for(const auto &index : index_set)
	{
		os << index << "  ";
	}
	return os;
}

std::istream& operator>>(std::istream &is, std::set<Index::set_size_type>& index_set)
{
	int index;
	while( is >> index)
	{
		index_set.insert(index);
	}
	return is;
}

Index::Index()
{
	m_vecDict_CN.reserve(RESERVE_VALUE);
	m_vecDict_EN.reserve(RESERVE_VALUE);
}

void Index::loadDictVecFromDictFile_EN()
{
	MyConf* pMyConf = Singleton<MyConf>::getInstance();
	std::string filename_EN = pMyConf->getConfigByName(DICT_NAME_EN);
	

	std::ifstream ifs_EN(filename_EN);
	if(!ifs_EN)
	{
		logError("loadIndexVecFromDictFile_EN");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs_EN(ifs_EN);

	std::pair<std::string, int> tmp_pair;

	while(ifs_EN >> tmp_pair.first >> tmp_pair.second)
	{
		m_vecDict_EN.push_back(tmp_pair);
	}
}

void Index::loadDictVecFromDictFile_CN()
{
	MyConf* pMyConf = Singleton<MyConf>::getInstance();
	std::string filename_CN = pMyConf->getConfigByName(DICT_NAME_CN);
	

	std::ifstream ifs_CN(filename_CN);
	if(!ifs_CN)
	{
		logError("loadIndexVecFromDictFile_CN");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs_CN(ifs_CN);

	std::pair<std::string, int> tmp_pair;


	while(ifs_CN >> tmp_pair.first >> tmp_pair.second)
	{
		m_vecDict_CN.push_back(tmp_pair);
	}
}

void Index::establishIndexMap_CN()
{
	long value = 0;
	std::string tmp_str;
	size_t idx = 0;
	int nbytes = 3, cnt = 0, len = 0, step = 0;
	std::set<set_size_type> index_set;

	for(idx = 0; idx !=m_vecDict_CN.size(); ++idx)
	{
	//	tmp_str = m_vecDict_CN[idx].first;
		tmp_str = m_vecDict_CN[idx].first.c_str();
		len = tmp_str.size()/nbytes;
		cnt = 0;
		while(cnt < len)
		{
			step = cnt * nbytes;
			value = 0x10000*(tmp_str[step]) + 
				    0x100*(tmp_str[step+1]) +
				   	tmp_str[step+2];
			auto iter = m_mapIndex_CN.find(value);
			if(iter != m_mapIndex_CN.end())
			{
				iter->second.insert(idx);
			}
			else
			{
				index_set.insert(idx);
				m_mapIndex_CN.insert({value, index_set});
				index_set.clear();
			}
			++cnt;
		}
	}
}


void Index::establishIndexMap_EN()
{
	std::set<set_size_type> index_set;
	for(int idx = 0; idx != m_vecDict_EN.size(); ++idx)
	{
		for(const auto &ch : m_vecDict_EN[idx].first)
		{
			auto iter = m_mapIndex_EN.find(ch);
			if(iter != m_mapIndex_EN.end())
			{
				iter->second.insert(idx);
			}
			else
			{
				index_set.insert(idx);
				m_mapIndex_EN.insert({ch, index_set});
				index_set.clear();
			}
		}
	}//end for
}

void Index::writeIndexMapToIndexFile_EN()
{
	MyConf* pMyConf = Singleton<MyConf>::getInstance();
	std::string filename = pMyConf->getConfigByName(INDEX_NAME_EN);
	
	std::ofstream ofs(filename);
	if(!ofs)
	{
		logError("writeIndexMapToIndexFile_EN");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ofstream> safe_ofs(ofs);

	for(auto &pair : m_mapIndex_EN)
	{
		ofs << pair.first << "  " << pair.second << std::endl;
	}
}

void Index::writeIndexMapToIndexFile_CN()
{
	MyConf* pMyConf = Singleton<MyConf>::getInstance();
	std::string filename = pMyConf->getConfigByName(INDEX_NAME_CN);
	
	std::ofstream ofs(filename);
	if(!ofs)
	{
		logError("writeIndexMapToIndexFile_EN");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ofstream> safe_ofs(ofs);
	for(auto &pair : m_mapIndex_CN)
	{
		ofs << pair.first << "  " << pair.second << std::endl;
	}
}

void Index::loadIndexMapFromIndexFile_EN()
{
	MyConf* pMyConf = Singleton<MyConf>::getInstance();
	std::string filename = pMyConf->getConfigByName(INDEX_NAME_EN);

	std::ifstream ifs(filename);
	if(!ifs)
	{
		logError("loadIndexVecFromDictFile_EN");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs(ifs);

	std::string line;	
	std::set<set_size_type> index_set;
	long ch;
	while(std::getline(ifs, line))
	{
		std::istringstream record(line);
		record >> ch >> index_set;

		m_mapIndex_EN.insert({ch, index_set});
		index_set.clear();
	}
}

void Index::loadIndexMapFromIndexFile_CN()
{
	MyConf* pMyConf = Singleton<MyConf>::getInstance();
	std::string filename = pMyConf->getConfigByName(INDEX_NAME_CN);

	std::ifstream ifs(filename);
	if(!ifs)
	{
		logError("loadIndexVecFromDictFile_EN");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs(ifs);

	std::string line;	
	std::set<set_size_type> index_set;
	long ch;
	while(std::getline(ifs, line))
	{
		std::istringstream record(line);
		record >> ch >> index_set;

		m_mapIndex_CN.insert({ch, index_set});
		index_set.clear();
	}
}

Index::DictData& Index::getDictVec_EN()
{
	return m_vecDict_EN;
}

Index::DictData& Index::getDictVec_CN()
{
	return m_vecDict_CN;
}

Index::IndexData_EN& Index::getIndexMap_EN()
{
	return m_mapIndex_EN;
}

Index::IndexData_CN& Index::getIndexMap_CN()
{
	return m_mapIndex_CN;
}


}//namespace wd
