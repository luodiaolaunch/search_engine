#include "Task.h"
#include "WorkThread.h"
#include "Index.h"
#include "Config.h"
#include "Cache.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/time.h>
#include <string.h>


extern wd::Cache myCache;
namespace wd
{


int Min(int a, int b)
{
	return a > b ? b : a;
}


#if 0
//获取字符个数 判断每个字符
size_t getLength(const std::string &str)
{
	int n = 0;
	size_t len = 0, idx = 0, sz = str.size();
	char ch, tmp = 0;
	size_t num = 0;

	while(idx < sz)
	{
		tmp = str[idx];

		//一个字节对应一个符号
		if(tmp > 0)
		{
			++num;
			++idx;
		}
		//len个字节对应一个符号
		else
		{
			n = 0;
			len = 0;
			while(n < 8)
			{
				ch = tmp << n;
				if(ch < 0)
					++len;
				else
					break;
				++n;
			}
			++num;
			idx += len;		
		}
	}
	return num;
}
#endif

int timeval_substract(struct timeval* result, 
					  struct timeval* x, i
					  struct timeval* y)
{
	if(x->tv_sec > y-> tv_sec)
		return -1;
	if(x->tv_sec == y->tv_sec &&
		x->tv_usec > y->tv_usec)
		return -1;

	result->tv_sec = y->tv_sec - x->tv_sec;
	result->tv_usec = y->tv_usec - x->tv_usec;


	if(result->tv_usec < 0)
	{
		result->tv_sec--;
		result->tv_usec += 1000000;
	}
	return 0;
}

//获取字符个数 判断第一个字符
size_t getLength(const std::string &str)
{
	int n = 1;
	size_t len = 1, sz = str.size();
	char ch, tmp = 0;
	
	ch = str.front();

	if(ch > 0)
		return sz;

	while(n < 8)
	{
		tmp = ch << n;
		if(tmp < 0)
			++len;
		else
			break;
		++n;
	}
	return sz/len;
}


//比较string中index处的两个符号是否相等
bool compareCharUTF_8(const std::string lhs, ,
					  const std::string rhs, 
					  int indexl, 
					  int indexr, 
					  int bits)
{
	size_t lenl = lhs.size();
	size_t lenr = rhs.size();

	indexl *= bits, indexr *= bits;


	while(bits-- && 
		  indexr <= lenr && 
		  indexl <= lenl)
	{
		if(lhs[indexl] != rhs[indexr])
		{
			return false;
		}
		++indexl;
		++indexr;
	}

	//长度不等
	if(bits != -1)
	{
		return false;
	}

	return true;
}
//===========================================

Task::Task(std::string str, i
		   const TcpConnectionPtr& conn)
	:m_query(str), 
	 m_conn(conn),
	 m_pThread(NULL),
	 m_isEng(true)
{

}

void Task::setWordThreadPtr(WorkThread* pthread)
{
	m_pThread = pthread;
}

void Task::addQueryToThreadCache()
{
	m_pThread->getWorkThreadCache().addQueryToCache(m_query, m_result);
}

void Task::saveResultData()
{
	int n = RESULT_SIZE;

	while(n-- && !m_que.empty())
	{
		m_result.push_back(m_que.top());
		m_que.pop();
	}
}

//检查客户端单词
void Task::checkQueryWord()
{
	if(m_query.back() == '\n')
	{
		m_query.pop_back();
	}
	if(m_query.empty())
		return;

	//删除单词空白部分
	m_query.erase(std::remove_if(m_query.begin(),
								 m_query.end(),
								 [](char x) { return std::isspace(x); }),
								 m_query.end());

	int len = getLength(m_query);
	size_t sz = m_query.size();

	std::cout << "len = " << len << std::endl;

	if(sz == len)
	{
		m_isEng = true;
	}
	else
	{
		m_isEng = false;
	}
}

void Task::process()
{
	checkQueryWord();

	struct timeval start, stop, diff;

	//空词直接返回
	if(m_query.empty())
	{
		sendResultToClient();
		return;
	}
	std::cout << "m_query = " << m_query << std::endl;

	//在线程缓存中查找
	auto result = m_pThread->getWorkThreadCache().searchCacheData(m_query);

	//Cache中查找	
	if(0 != result.size())
	{
		m_result = result;
	}
	//索引中查找
	else
	{
		::gettimeofday(&start, 0);
		if(m_isEng)
		{
			doQuery_EN();
		}
		else 
		{
			doQuery_CN();
		}
		::gettimeofday(&stop, 0);

		saveResultData();
		addQueryToThreadCache();
	}

	timeval_substract(&diff, &start, &stop);
	std::cout << "time: " << diff.tv_usec/1000 << " ms" << std::endl;
	print();

	sendResultToClient();

}

void Task::sendResultToClient()
{
	std::ostringstream oss;
	for(const auto &result : m_result)
	{
		oss << result.m_candidate_word << "\t";
	}
	oss << "\t";  //查询词为空字符串时发送\t

	std::string tmp_str = oss.str();

	//发送数据长度信息
	size_t len = tmp_str.size();
	int sockfd = m_conn->getSocket().fd();
	::write(sockfd, &len, sizeof(len));

	m_conn->send(tmp_str);
}

void Task::print()
{

	std::cout << "Reslt:" << std::endl;
	for(const auto &result : m_result)
	{
		std::cout << std::left << std::setw(10) <<result.m_candidate_word << "  "
			<< std::setw(5)  << result.m_distance
			<< "  "  << std::setw(8) << result.m_frequency	
			<< std::endl;
	}
	std::cout << std::endl;
}

void Task::doQuery_EN()
{
	int dis = 0, fre = 0;
	std::string word;

	Index* pIndex = Singleton<Index>::getInstance();
	auto dict_vec = pIndex->getDictVec_EN();
	auto index_map = pIndex->getIndexMap_EN();

	std::set<Index::set_size_type> indexAll_set;


	for(const auto &ch : m_query)
	{
		auto index_set = index_map[ch];

		
		for(const auto &index : index_set)
		{
			//判断该index指向的单词是否已加入que
			auto it_ret = indexAll_set.find(index);
			if(it_ret != indexAll_set.end())
			{
				continue;
			}
			indexAll_set.insert(index);

			word = dict_vec[index].first;
			fre = dict_vec[index].second;
			dis = calDistance(word);
			m_que.push(MyResult(word, fre, dis));
		}
	}
}

void Task::doQuery_CN()
{
	int dis = 0, fre = 0;
	std::string word;
	long value = 0;
	int len = getLength(m_query);
	int nbytes = m_query.size()/len, step = 0, cnt = 0;

	Index* pIndex = Singleton<Index>::getInstance();
	auto dict_vec = pIndex->getDictVec_CN();
	auto index_map = pIndex->getIndexMap_CN();

	std::set<Index::set_size_type> indexAll_set;

	while(cnt < len)
	{
		step = cnt * nbytes;
		value = 0x10000 * m_query[step] + 0x100 * m_query[step+1] + m_query[step+2];
		auto index_set = index_map[value];
		for(const auto& index: index_set)
		{
			auto ret_it = indexAll_set.insert(index);
			if(!ret_it.second)	
				continue;
			word = dict_vec[index].first;
			fre = dict_vec[index].second;
			dis = calDistance(word);
			m_que.push(MyResult(word, fre, dis));
		}
		++cnt;
	}
}
#if 0
int Task::calDistance(const std::string &rhs)
{
	size_t n = getLength(m_query);
	size_t m = getLength(rhs);
	
	int step = m_query.size()/n;

	int **arr= new int*[n + 1];
	
	int idx ;

	for(idx = 0; idx != n + 1; ++idx)
		arr[idx] = new int[m + 1];

	for(idx = 0; idx != m + 1; ++idx)
		arr[0][idx] = idx;
	for(idx = 1; idx != n + 1; ++idx)
		arr[idx][0] = idx;

	int d, tmp;
	for(int i = 1; i != n + 1; ++i)
		for(int j = 1; j != m + 1; ++j)
		{
			tmp = Min(arr[i-1][j] + 1, arr[i][j-1]+1);
			if(compareCharUTF_8(m_query, rhs, i-1, j-1, step))
			{
				d = 0;
			}
			else
			{
				d = 2;
			}
			arr[i][j] = Min(tmp, arr[i-1][j-1] + d);
		}

	d = arr[n][m];
	for(idx = 0 ; idx != n + 1; ++idx)
	{
		delete[] arr[idx];
		arr[idx] = NULL;
	}
	delete []arr;
	arr = NULL;
	return d;
}
#endif


int Task::calDistance(const std::string& rhs)
{
	size_t n = getLength(m_query);
	size_t m = getLength(rhs);
	int step = m_query.size()/n;
	
	int *arr[] = new int*[n+1];
	for(int i = 0; i != n+1; ++i)
		arr[i] = new int[m+1];

	for(int i = 0; i != n+1; ++i)
		a[i][0] = i;
	for(int i = 0; i != m+1; ++i)
		a[0][i] = i;

	int d = 0, tmp = 0;
	for(int i = 1; i != n + 1; ++i)
		for(int j = 1; j != m + 1; ++j)
		{
			tmp = Min(arr[i-1][j], arr[i][j-1]);
			if(compareCharUTF_8(m_query, rhs, i - 1, j - 1, step))
			{
				d = 0;
			}
			else
			{
				d = 2;
			}
			arr[i][j] = Min(tmp, a[i-1][j-1] + d);
		}
	d = arr[n][m];

	for(int i = 0; i != n+1; ++i)
		delete []arr[i];
	delete []arr;

	return d;
}

}//namespace wd
