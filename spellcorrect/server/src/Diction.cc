#include "Diction.h"
#include "Config.h"
#include <unistd.h>
#include <algorithm>
#include <iomanip>
#include <cctype>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <iterator>

namespace wd
{

Diction::Diction()
	:m_jieba(JIEBA_DICT.c_str(),
			 JIEBA_HMM.c_str(),
			 JIEBA_USR_DICT.c_str())
{
}

void Diction::writeDictFile_CN(const std::string &filename_CN)
{
	std::ofstream ofs_CN(filename_CN);
	if(!ofs_CN)
	{
		logError("writeDictFile_CN");
		::exit(EXIT_FAILURE);
	}	

	SafeFile<std::ofstream> safe_ofs_CN(ofs_CN);

	for(const auto & pair : m_mapDict_CN)
	{
		ofs_CN << std::left << std::setw(30) <<  pair.first << " " << pair.second << std::endl;
	}
}

void Diction::writeDictFile_EN(const std::string &filename_EN)
{
	std::ofstream ofs_EN(filename_EN);

	if(!ofs_EN)
	{
		logError("writeDictFile_EN");
		::exit(EXIT_FAILURE);
	}

	SafeFile<std::ofstream> safe_ofs_EN(ofs_EN);

	for(const auto & pair : m_mapDict_EN)
	{
		ofs_EN <<  std::left << std::setw(30) << pair.first << " " << pair.second << std::endl;
	}
}

void Diction::TraverseDir(const std::string &dirname, std::set<std::string>& set_filename)
{
	DIR *fdir;
	std::string dir_name = dirname;
	fdir = ::opendir(dir_name.c_str());
	if(NULL == fdir)
	{
		logError("TraverseDir");
		::closedir(fdir);
		return;
	}
	
	struct dirent *p;
	std::string path_name;

	while(NULL != (p = readdir(fdir)))
	{
		if(::strcmp(p->d_name, ".") &&
		   ::strcmp(p->d_name, ".."))
		{
			path_name = dir_name + "/" + std::string(p->d_name);	
			set_filename.insert(path_name);	
		}
	}
	::closedir(fdir);
}


void Diction::loadOriginDirectory_CN(std::string &dir)
{
	std::set<std::string> vec_file;
	std::set<std::string> vec_file_CN;


	//原始数据目录
	TraverseDir(dir, vec_file);

	for(const auto&  file : vec_file)
	{
		if(file == (dir + "/" + SRC_FILE_CN) )
		{
			//遍历中文源文件
			TraverseDir(file, vec_file_CN);
			for(const auto &file : vec_file_CN)
			{
				loadOriginFile_CN(file);
			}
		}
	}//end for
}


void Diction::loadOriginDirectory_EN(std::string &dir)
{
	std::set<std::string> vec_file;
	std::set<std::string> vec_file_EN;

	//原始数据目录
	TraverseDir(dir, vec_file);

	for(const auto&  file : vec_file)
	{
		if(file == (dir + "/" + SRC_FILE_EN) )
		{
			//遍历英文源文件
			TraverseDir(file, vec_file_EN);
			for(const auto &file : vec_file_EN)
			{
				loadOriginFile_EN(file);
			}
		}
	}//end for
}

void Diction::loadOriginFile_CN(const std::string& filename)
{

	std::ifstream ifs(filename);
	//std::cout << filename << std::endl;
	if(!ifs)
	{
		logError("loadOriginFile_CN");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs(ifs);

	std::istreambuf_iterator<char> beg(ifs), end;
	std::string file_data(beg, end);

	std::vector<std::string> words;
	words.reserve(5000);
	m_jieba.Cut(file_data, words, true);

	//将切词后的词放入map
	for(const auto & word : words)
	{
		if(word.empty())
			continue;

		auto pair = m_mapDict_CN.insert({word, 1});
		if(!pair.second)
			++(pair.first->second);
	}
	
}


void Diction::loadOriginFile_EN(const std::string& filename)
{
	std::ifstream ifs(filename);
	//std::cout << filename << std::endl;
	if(!ifs)
	{
		logError("loadOriginFile_EN");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs(ifs);

	std::string word;
	while(ifs >> word)
	{

		//删除含有特殊符号的词
		
		if(!std::isalpha(word.front()))
		{
			continue;
		}

		auto iter = std::find_if(word.begin(), 
								 word.end(),
			   					 [](char x){return !(std::isalpha(x));});
		if(iter < word.end() - 1)
			continue;
		else if(iter == word.end() - 1)
		{
			if(!std::ispunct(*iter))
				continue;
			else
				word.pop_back();
		}
				
		std::transform(word.begin(), word.end(),
					   word.begin(), ::tolower);

		auto pair = m_mapDict_EN.insert({word, 1});
		if(!pair.second)
			++(pair.first->second);
	}
}

#if 0
void Diction::loadOriginFile(const std::string &filename)
{
	std::ifstream ifs(filename);
	if(!ifs)
	{
		logError("loadIndexFile error");
		::exit(EXIT_FAILURE);
	} 
	SafeFile<std::ifstream> safe_ifs(ifs);
	
	std::string word, subword, word_get;
//	std::string alpha(ALPHA_SEQ);
	while(ifs >> word)
	{
		std::transform(word.begin(), word.end(), 
					   word.begin(), ::tolower);

		//分割英文字符串
		
		std::string::size_type pos = 0;
		do
		{
			pos = word.find_first_of(ALPHA_SEQ);
			if(pos == std::string::npos)
				break;

			subword = word.substr(pos);
			pos = subword.find_first_not_of(ALPHA_SEQ);
			word_get = subword.substr(0, pos);
			
			pushPairToVecDict(word_get);

			if(pos != std::string::npos)
				word = subword.substr(pos + 1);
		}while(pos != std::string::npos);
	}
}

void Diction::pushPairToVecDict(const std::string word)
{
	auto tmp_pair = std::make_pair(word, 1);
	auto it = m_vecDict.begin();
	for( ; it != m_vecDict.end(); ++it)
	{
		if(it->first == word)
			break;
	}

	//单词已存在
	if(it != m_vecDict.end())
	{
		++(it->second);
	}
	else
	{	
		m_vecDict.push_back(tmp_pair);
	}
}

#endif


}//namespace wd
