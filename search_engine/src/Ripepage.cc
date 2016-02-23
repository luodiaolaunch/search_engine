#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <math.h>
#include <stdio.h>

#include "Ripepage.h"
#include "Config.h"

#include <iostream>

namespace wd 
{

Ripepage::Ripepage()
	:m_split(JIEBA_DICT.c_str(),
			 JIEBA_HMM.c_str(),
			 JIEBA_USR_DICT.c_str())
{
	m_pageVec.reserve(RIPE_RESERVE_VAL);
}

void Ripepage::LoadSourceFilePath(vector<string> &pathVec)
{
	MyConf *pMyConf = 
		Singleton<MyConf>::getInstance();
	if(pMyConf == NULL)
	{
		logError("Myconf getInstance fail");
		::exit(EXIT_FAILURE);
	}

	string source_dir = 
		pMyConf->getConfigByName(SOURCE_NAME);

	TraverseDir(source_dir, pathVec);
}

void Ripepage::LoadSourceFile()
{
	vector<string> pathVec;
	pathVec.reserve(RIPE_RESERVE_VAL);

	LoadStopWordsFile();
	LoadSourceFilePath(pathVec);

	string title, content;
	unsigned long docid = 0;

	int cnt = 0;
	for(const auto & file : pathVec)
	{
		std::cout << cnt++ << "\t";
		title.clear();
		content.clear();
		++docid;

		ProcessRipeFile(file, title, content);

		m_pageVec.push_back(MyPage(docid, title, file, content));
	}

}

void Ripepage::DeletePageVector()
{
	m_pageVec.clear();
}

void Ripepage::LoadStopWordsFile()
{
	std::ifstream ifs(JIEBA_STOP_WORDS);
	if(!ifs)
	{
		logError("open jieba stop_wors.utf8 fail");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs(ifs);

	string line;
	vector<string> stopWordsVec;
	while(std::getline(ifs, line))
	{
		stopWordsVec.push_back(line);
	}

	set<string> tmpSet(stopWordsVec.begin(), stopWordsVec.end());
	m_stopWords = tmpSet;
}

void Ripepage::CreateIndexLib()
{
	WEIGHT_ROOT_MAP weightRootMap;
//	double weightRoot = 0.0;
	set<IndexValue> indexValSet;
	WEIGHT_MAP weightMap;

	CalcWeightMap(weightMap, weightRootMap);

	std::cout << "calc page index\n page cnt:\n";
	int cnt = 0;
	for(const auto &page : m_pageVec)
	{
		printf("\t%d", ++cnt);
		for(const auto &dict_pair : page.m_dict)
		{
#if 0
			auto iter_map = weightMap.find({dict_pair.first, page.m_docid});
			if(iter_map == weightMap.end())
			{
				logWarn("weightMap data find fail");
				continue;
			}
#endif
			double weight = weightMap[{dict_pair.first, page.m_docid}] / weightRootMap[page.m_docid];

#if 1
			auto iter_dict = m_index.find(dict_pair.first);
			if(iter_dict == m_index.end())
			{
				indexValSet.clear();
				indexValSet.insert(IndexValue(page.m_docid,
											  dict_pair.second,
											  weight));
				m_index.insert({dict_pair.first,
								indexValSet});

			}
			else
			{
				m_index[dict_pair.first].insert(IndexValue(page.m_docid,
											 dict_pair.second,
											 weight));
			}
#endif
		}
	}

	WriteIndexLib();
}

//计算每篇文章中词对应的特征权重（未进行归一化），及词在所有文章中的权重根值
void Ripepage::CalcWeightMap(WEIGHT_MAP &weightMap, WEIGHT_ROOT_MAP &weightRootMap)
{
	unordered_map<string, int> wordAllPageFre;

	//计算逆文档频率
	std::cout << "cal reverse docRate\n";

	int cnt = 0;
	for(const auto &page : m_pageVec)
	{
		std::cout << cnt++ << "\t";
		for(const auto &dict_pair : page.m_dict)
		{
			auto iter = wordAllPageFre.find(dict_pair.first);
			if(iter == wordAllPageFre.end())
			{
				wordAllPageFre.insert({dict_pair.first, 
									   1});
			}
			else
			{
				++iter->second;
			}
		}
	}

	std::cout << "calc word weight\n";
	//计算词的特征权重
	cnt = 0;

	double weightRoot = 0.0;
	for(const auto &page : m_pageVec)
	{
		std::cout << cnt++ << "\t";
		weightRoot = 0.0;
		for(const auto &dict_pair : page.m_dict)
		{
			double weight = dict_pair.second * 
							::log(m_pageVec.size() / 
							wordAllPageFre[dict_pair.first] + 0.05);
	
			weightMap.insert({{dict_pair.first, page.m_docid},
							   weight});

#if 0   //修改前的归一划权重计算方式
			auto iter = weightRootMap.find(dict_pair.first);
			if(iter == weightRootMap.end())
			{
				weightRootMap.insert({dict_pair.first, 
									  weight * weight});
			}
			else
			{
				iter->second += weight * weight;
			}
#endif 

		//修改后
		weightRoot += weight * weight;
		}
		weightRootMap.insert({page.m_docid, weightRoot});

	}//end for

	//归一化权重值开平方
	for(auto & pair : weightRootMap)
	{
		pair.second = ::sqrt(pair.second);
	}
}

void Ripepage::WriteIndexLib() const
{
	MyConf *pMyConf = Singleton<MyConf>::getInstance();
	if(pMyConf == NULL)
	{
		logError("Myconf getInstance fail");
		::exit(EXIT_FAILURE);
	}

	string index_file =
		pMyConf->getConfigByName(INDEX_NAME);
	std::ofstream ofs(index_file);
	if(!ofs)
	{
		logError("open index.lib fail");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ofstream> safe_ofs(ofs);

	std::cout << "write index lib ,index.size = " << m_index.size() << std::endl;
	for(const auto &pair : m_index)
	{
		ofs << std::left << std::setw(30) << pair.first << '\t';
		for(const auto &indexVal : pair.second)
		{
			ofs << indexVal << '\t';
		}
		ofs << std::endl;
	}
	std::cout <<"write over\n";
}

void Ripepage::CreateRipepageAndOffsetLib()
{
	CutRipepageAndCalcTopK();
	PagedupRemove();
	WriteRipepageAndOffsetLib();
}


void Ripepage::CutRipepageAndCalcTopK()
{
	std::cout << "\nCutRipepage()" << std::endl;
	vector<string> wordVec;
	for(auto &page : m_pageVec)
	{
		m_split.Cut(page.m_content, wordVec);

		for(const auto & word : wordVec)
		{
			if(word.empty())
				continue;

			//判断是否为空白字符词
			if(0 != std::isspace(word.front()))
			{
				auto iter = std::find_if(word.begin(),
										 word.end(),
										 [](char x){return !std::isspace(x);});
				if(iter == word.end())
					continue;
			}

			auto pair = page.m_dict.insert({word, 1});
			if(!pair.second)
				++(pair.first->second);
		}
		
		page.CalcTopK(m_stopWords);
	//	page.printTopK();
	}


}

void Ripepage::PagedupRemove()
{
	vector<MyPage>::size_type indexi, indexj;

	//for语句中的条件语句 每次循环都会执行？
	std::cout << "remove before " << m_pageVec.size() << std::endl;

	for(indexi = 0; indexi != m_pageVec.size() - 1; ++indexi)
	{
		std::cout << indexi << std::endl;
		for(indexj = indexi + 1; indexj != m_pageVec.size();  ++indexj)
		{
			if(PageCompare(m_pageVec[indexi], m_pageVec[indexj]))
			{
			//	std::cout << "page" << indexi+1 << " and page " << indexj+1 << " are same\n";

				auto tmp = m_pageVec[indexj];
				m_pageVec[indexj] = m_pageVec.back();
				m_pageVec.back() = tmp;
				m_pageVec.pop_back();

				--indexj;
			}
		}
	}
	std::cout << "remove :" << m_pageVec.size() << std::endl;
}

void Ripepage::WriteRipepageAndOffsetLib()
{
	MyConf *pMyConf = 
		Singleton<MyConf>::getInstance();
	if(pMyConf == NULL)
	{
		logError("MyConf getInstance fail");
		::exit(EXIT_FAILURE);
	}

	string ripepage_file = 
		pMyConf->getConfigByName(RIPEPAGE_NAME);
	std::ofstream ofs_ripepage(ripepage_file);
	if(!ofs_ripepage)
	{
		logError("open ripepage.lib fail");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ofstream> safe_ofs_ripepage(ofs_ripepage);

	string offset_file = pMyConf->getConfigByName(OFFSET_NAME);
	std::ofstream ofs_offset(offset_file);
	if(!ofs_offset)
	{
		logError("open offset.lib fail");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ofstream> safe_ofs_offset(ofs_offset);


	auto pos_old = ofs_ripepage.tellp();
	auto pos_new = ofs_ripepage.tellp();

	for(const auto & page : m_pageVec)
	{
		pos_old = ofs_ripepage.tellp();

		ofs_ripepage <<	DOC_START
					 << DOCID_START << page.m_docid << DOCID_END
					 << DOCURL_START << page.m_url << DOCURL_END
					 << DOCTITLE_START << page.m_title << DOCTITLE_END
					 << DOCCONTENT_START << page.m_content << DOCCONTENT_END
					<< DOC_END;

		pos_new = ofs_ripepage.tellp();

		m_offset.insert({page.m_docid, {pos_old, pos_new - pos_old}});

		ofs_offset << std::left << std::setw(6) << page.m_docid << std::setw(10) << pos_old << std::setw(10) << pos_new - pos_old << std::endl;
	}
}

void Ripepage::ProcessRipeFile(const string ripefile, string &title, string &content)
{
	std::ifstream ifs(ripefile);
	if(!ifs)
	{
		logError("open ripe file fail");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs(ifs);
	
	std::istreambuf_iterator<char> beg(ifs), end;
	string content_tmp(beg, end);

	content = content_tmp;

	ifs.seekg(0); //rewind
	string line;
	int cnt_line = 0;

	while(std::getline(ifs, line))
	{
		if(line == TEXT_TAG || cnt_line > PAGE_LINE_MAX)
			break;


		auto pos = line.find(TITLE_TAG1);
		if(pos != string::npos)
		{
			title = line.substr(pos + TITLE_TAG1.size());
			return;
		}	
		
		pos = line.find(TITLE_TAG2);
		if(pos != string::npos)
		{
			title = line.substr(pos + TITLE_TAG2.size());
			return;
		}		
		
		pos = line.find(TITLE_TAG3);
		if(pos != string::npos)
		{
			title = line.substr(pos + TITLE_TAG3.size());
			return;
		}	
	
		pos = line.find(TITLE_TAG4);
		if(pos != string::npos)
		{
			title = line.substr(pos + TITLE_TAG4.size());
			return;
		}	
		cnt_line++;
	}

	std::getline(ifs, line);
	title = line;
}


void Ripepage::TraverseDir(const string dir, vector<string> &pathVec)
{
	const char *dirname = dir.c_str();
	DIR *fdir;
	fdir = ::opendir(dirname);
	if(NULL == fdir)
	{
		logError("opendir fail");
		::exit(EXIT_FAILURE);
	}

	struct dirent *p;
	struct stat buf;
	std::string pathdir;
	int ret;
	
	while((p = readdir(fdir))!= NULL)
	{
		pathdir.clear();
		pathdir = dir + "/" + string(p->d_name);
		ret = ::stat(pathdir.c_str(), &buf);
		if(ret != 0)
		{
			logError("stat fail");
			::exit(EXIT_FAILURE);
		}
		
		if(!S_ISDIR(buf.st_mode))
		{
			pathVec.push_back(pathdir);
		}
		else
		{
			if(!::strcmp(p->d_name, ".") ||
			   !::strcmp(p->d_name, ".."))
			{
			}
			else
			{
				TraverseDir(pathdir, pathVec);
			}
		}
	}//end while
}

void Ripepage::LoadRipepageLib()
{
	string line;

}

void Ripepage::LoadOffsetLib()
{
	MyConf *pMyConf = 
		Singleton<MyConf>::getInstance();
	string filename = pMyConf->getConfigByName(OFFSET_NAME);
	std::ifstream ifs(filename);
	if(!ifs)
	{
		logError("load offset.lib");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs(ifs);

	unsigned long docid, offset, size;
	string line;
	while(std::getline(ifs, line))
	{
		std::istringstream record(line);
		record >> docid >> offset >> size;
		m_offset.insert({docid, {offset, size}});
	}
}

void Ripepage::LoadIndexLib()
{
	MyConf *pMyConf = 
		Singleton<MyConf>::getInstance();
	string filename = pMyConf->getConfigByName(INDEX_NAME);
	std::ifstream ifs(filename);
	if(!ifs)
	{
		logError("load index.lib");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs(ifs);

	set<IndexValue> indexValueSet;
	IndexValue indexValue;

	string line, word;
	while(getline(ifs, line))
	{
		std::istringstream iss(line);
		iss >> word;
		while(iss >> indexValue)
		{
			indexValueSet.insert(indexValue);
		}
		m_index.insert({word, indexValueSet});
		indexValueSet.clear();
	}
}

}
//namespace wd 
