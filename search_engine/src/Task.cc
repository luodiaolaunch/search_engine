#include <unordered_map>
#include <vector>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <functional>
#include <unistd.h>
#include <stdlib.h>
#include "Task.h"
#include "json/json.h"
#include "Config.h"
//#include <python2.7/Python.h>

namespace wd 
{

Task::Task(string queryData,
		   const TcpConnectionPtr conn,
		   acl::redis_client_pool& redisPool)
	:m_query(queryData),
	 m_conn(conn),
	 m_pRipepage(Singleton<Ripepage>::getInstance()),
	 m_redisClientPool(redisPool),
	 m_pRedisClientConn((acl::redis_client*)redisPool.peek()),
	 m_redisString(m_pRedisClientConn)
{	
	//检验Redis连接池返回值
	if(m_pRedisClientConn)
		logWarn("redisClientPool peek return null");

	if(m_query.back() == '\n')
		m_query.pop_back();
}

string Task::GetQueryData()const 
{
	return m_query;
}
const TcpConnectionPtr Task::GetTcpConnectionPtr() const
{
	return m_conn;
}


void Task::Run()
{
	if(!DoRedisQuery())
	{
		std::cout << "not found by redis\n";
		InitQueryData();
		DoQuery();
	}
	else
		std::cout << "found by redis\n";

	//使用完redis客户端连接应将其放回连接池中
	m_redisClientPool.put(m_pRedisClientConn, true);	
	GetResultAndSendToClient();
	//::sleep(10);
}

bool Task::DoRedisQuery()
{
//	int num = m_redisHash.hlen(REDIS_HASH_KEY.c_str());
//	std::cout << num << " data in redis\n";
	acl::string value;

	m_redisString.clear();
	if(m_redisString.get(m_query.c_str(), value) == false)
		return false;
	
	vector<acl::string> pageVec;
	pageVec = value.split2(REDIS_SEP_TAG.c_str());
	for(const auto & r : pageVec)
	{
		std::cout << r.c_str() << " : ";
		m_page.push_back(::atol(r.c_str()));
	}
	if(m_page.empty())
	{
		std::cout << "REDIS retrun null value, can't found in database\n";
		return true;
	}
	std::cout << std::endl;
	return true;
}

void Task::InitQueryData()
{
	vector<string> wordsVec;
	map<string, int> dict;

	//分词统计词频
	m_pRipepage->m_split.Cut(m_query, wordsVec);
	
	for(const auto & word : wordsVec)
	{
		if(word.empty())
			continue;
		
		auto iter = m_pRipepage->m_index.find(word);
		if(iter == m_pRipepage->m_index.end())
		{
			//没查到怎么处理？ 删除这个词
			logInfo("word not found in index.lib");
			continue;
		}

		//判断是否为空白字符词
		if(0 != std::isspace(word.front()))
		{
			auto iter = std::find_if(word.begin(),
					word.end(),
					[](char x){return !std::isspace(x);});
			if(iter == word.end())
				continue;
		}

		auto pairRet = dict.insert({word, 1});
		if(!pairRet.second)
			++(pairRet.first->second);
	}


	//计算权重并归一化
	double weight = 0.0, weightNormalized = 0.0;
	for(const auto pair : dict)
	{
		weight = pair.second * ::log(1 + 0.05);
		weightNormalized += weight * weight;
		m_weight.insert({pair.first, weight});
	}

	weightNormalized = ::sqrt(weightNormalized);


	std::cout << "InitQueryData : query weight map:\n";
	for(auto & pair : m_weight)
	{
		pair.second /= weightNormalized;
		std::cout << pair.first << " --> " << pair.second << std::endl;
	}

}

void Task::DoQuery()
{
	//此处注意pageMap和m_weight 的类型都为Map并且排序相同，以致2个容器中的词相对应
	map<string, map<unsigned long, double> > pageMap;
	map<unsigned long, double> tmpMap;
	vector<unsigned long> docVec;

	//将Query的词从索引中取出来
	for(const auto & pair : m_weight)
	{
		auto iter = m_pRipepage->m_index.find(pair.first);
		if(iter == m_pRipepage->m_index.end())
		{
			//没查到怎么处理？ 在InitQueryData中删除这个词，搜索其他的词
			logInfo("word not found in index.lib");
			continue;
		}
		for(auto indexValue : iter->second)
		{
			tmpMap.insert({indexValue.m_docid, indexValue.m_weight});
		}
		pageMap.insert({pair.first, tmpMap});
		tmpMap.clear();
	}

	//计算从索引中取得向量的公共元素-docid
	bool isFirst = true;
	
	for(const auto & pair : pageMap)
	{
#if 0
		std::cout << pair.first << "\n";
		for(const auto & pair_p : pair.second)
		{
			std::cout << pair_p.first << "\t";
		}
		std::cout << "\n\n";
#endif

		if(isFirst)
		{
			for(const auto &r : pair.second)
			{
				docVec.push_back(r.first);	
			}
			isFirst = false;
		}
		else
		{
			unsigned long indexi;

			//注意此处遍历的2个容器为不同类型容器
			for(indexi = 0; indexi != docVec.size(); ++indexi)
			{

				//这句内层for的初始化语句不能放在line135前面，每次外循环都要进行迭代器初始化
				auto iter_map = pair.second.begin();
				for(; iter_map != pair.second.end(); ++iter_map)
				{
					if(docVec[indexi] == iter_map->first)
						break;
				}

				//不是共有元素，需要删除indexi元素
				if(iter_map == pair.second.end())
				{
					unsigned long tmp = docVec[indexi];
					docVec[indexi] = docVec.back();
					docVec.back() = tmp;
					docVec.pop_back();
					--indexi;
				}
			}
			
		}//end if
	}//end for


	//客户端请求数据权重向量
	vector<double> weightVecQuery;
	for(const auto & pair : m_weight)
	{
		weightVecQuery.push_back(pair.second);
	}

	//文档数据权重向量
	vector<double> weightVec;

	multimap<double, unsigned long, std::greater<double> > orderedPage;

	//计算第docid篇文档中词的权重向量， 并且与m_weight做相似度比较

#if 0	
	for(const auto &docid : docVec)
	{
		std::cout << "docid = " << docid << std::endl;
	}
#endif

	for(const auto & docid : docVec)
	{
		//此处不能使用const auto 因为line 152 可能会改变数据
		for(auto & page : pageMap)
		{
			if(page.second.find(docid) == page.second.end())
			{
				logWarn("pageMap data error");
				continue;
			}
			weightVec.push_back(page.second[docid]);
		}	


		//2个容器都是遍历2个MAP所得，这2个MAP的词已经相对应（都使用Map默认排序）
		double value = CalcSimilarDegree(weightVecQuery, weightVec);
		orderedPage.insert({value, docid});

		//下一个for循环新的文档权重向量内存应清除
		weightVec.clear();
	}

	
	//print
	std::cout << "Query.cc line 130 :orderedPage map:\n";
	string redis_value;
	acl::string docid_str;
	int pageCnt = 0;
	for(const auto & pair : orderedPage)
	{
		pageCnt++;
		if(pageCnt > PAGE_NUM)
			break;
		docid_str.format("%ld:", pair.second);
		redis_value.append(docid_str.c_str());
		m_page.push_back(pair.second);
		//std::cout << pair.first << " --> " << pair.second << std::endl;
	}

	std::cout << "redis_value = " << redis_value << std::endl;

	//查询结果存入redis缓存
	m_redisString.clear();
	int ret, timeout = 300;
//	acl::string REDIS_HASH_KEY1, m_query1;
//	REDIS_HASH_KEY1.format("%s", REDIS_HASH_KEY.c_str());
//	m_query1.format("%s", m_query.c_str());
	if((ret = m_redisString.setex(m_query.c_str(),
								  redis_value.c_str(),
								  timeout)) < 0)
	{
		logError("redisHash set fail");
	}
	else if(ret == 1)
	{
		logInfo("redisHash add success");
	}
	else if(ret == 0)
	{
		logInfo("field exist in redisHash and update success");
	}
}

double Task::CalcSimilarDegree(const vector<double> &lhs, const vector<double> &rhs)
{
	if (lhs.size() != rhs.size())
	{
		logNotice("weight vector size error");
		return 0;
	}

	double numerator = 0.0, denominator = 0.0;
	double sum1 = 0.0, sum2 = 0.0;
	for(int idx = 0; idx != lhs.size(); ++idx)
	{
		numerator += lhs[idx] * rhs[idx];
		sum1 += lhs[idx] * lhs[idx];
		sum2 += rhs[idx] * rhs[idx];
	}

	denominator = ::sqrt(sum1 * sum2);
	return (::acos(numerator / denominator));
}

void Task::GetResultAndSendToClient()
{
	MyConf *pMyConf = 
		Singleton<MyConf>::getInstance();
	if(pMyConf == NULL)
	{
		logError("Myconf getInstance fail");
		return;
	}
	string file_name = 
		pMyConf->getConfigByName(RIPEPAGE_NAME);
	std::ifstream ifs(file_name);
	if(!ifs)
	{
		logError("open ripepage.lib fail");
		return;
	}
	SafeFile<std::ifstream> safe_ifs(ifs);

	vector<MyPage> pageVec;
	Json::Value root;
	for(const auto &docid : m_page)
	{
		pos_type offset = m_pRipepage->m_offset[docid].first;
		pos_type size = m_pRipepage->m_offset[docid].second;
		ifs.seekg(offset);
		char *buf = new char[size];
		if(buf == NULL)
		{
			logError(" new char[] fail");
			return;
		}

		::bzero(buf, size);
		ifs.readsome(buf, size);
		string page(buf);
		delete []buf;

		string str_tmp = DecomposePageData(page, DOCID_START, DOCID_END);
		unsigned long pageID = std::stoi(str_tmp);
		assert(pageID == docid);
		std::cout << "pageID = " << pageID << std::endl;

		string pageUrl = DecomposePageData(page, DOCURL_START, DOCURL_END);
		std::cout << "url = " << pageUrl << std::endl;

		string pageTitle = DecomposePageData(page, DOCTITLE_START, DOCTITLE_END);
		std::cout << "title = " << pageTitle << std::endl;

		string pageContent = DecomposePageData(page, DOCCONTENT_START, DOCCONTENT_END);
		//std::cout << "pageContent = " << pageContent << std::endl;

		///string pageAbstract = GetPageAbstract(pageContent);
		//std::cout << "pageAbstract = " << pageAbstract << std::endl;

		Json::Value jsonPage;
		jsonPage["title"] = pageTitle;
		jsonPage["content"] = pageContent;
	//	jsonPage["abstract"] = pageAbstract;
		root.append(jsonPage);
	}

	string result = root.toStyledString();
	size_t size = result.size();
	int sockfd = m_conn->getSocket().fd();

	//不能使用TcpConnect接口发送
	::write(sockfd, &size, sizeof(size));
	m_conn->send(result);
}


//文本摘要
string Task::GetPageAbstract(string pageText)
{
#if 0
	Py_Initialize();
	if(!Py_IsInitialized())
	{
		logError("python initialize fail");
		return string("");
	}
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./')");
	PyObject *pModule = NULL;
	PyObject *pFunc = NULL;
	pModule = PyImport_ImportModule("../textrank4zh/TextRank4Sentence.py");
	if(!pModule)
	{
		logError("can't open python file");
		return string("");
	}
	PyObject *pClassTextRank = PyObject_GetAttrString(pModule, "TextRank4Sentence");
	if(!pClassTextRank)
	{
		logError("can't find TextRank4Sentence class");
		return string("");
	}
	PyObject *pInstanceTextRank = PyInstance_New(pClassTextRank, NULL, NULL);
	if(!pInstanceTextRank)
	{
		logError("can't create TextRank4Sentence instance");
		return string("");
	}
	PyObject_CallMethod(pInstanceTextRank, "train", "s", pageText.c_str());
	return string("myAbstract");
#endif
}

string Task::DecomposePageData(string &pageData, const string &startTag, const string &endTag)
{
	auto idxStart = pageData.find(startTag);
	auto idxEnd = pageData.find(endTag);
	auto str_pos = idxStart + startTag.size();
	auto str_size = idxEnd - str_pos;
	return (pageData.substr(str_pos, str_size));
}

}//namespace wd 
