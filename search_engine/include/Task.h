#ifndef WD_TASK_H
#define WD_TASK_H

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include "Noncopyable.h"
#include "TcpConnection.h"
#include "Ripepage.h"
#include "acl_cpp/lib_acl.hpp"

namespace wd 
{
using std::string;
using std::vector;
using std::map;
using std::unordered_map;
using std::multimap;


class Task : private Noncopyable
{
	using pos_type = Ripepage::pos_type;
	using PAGE_VEC = vector<unsigned long>;
	using WEIGHT_MAP = map<string, double>;

public:
	Task(string queryData, 
		 const TcpConnectionPtr conn, 
		 acl::redis_client_pool& redisPool);

	void Run();

	string GetQueryData()const ;
	const TcpConnectionPtr GetTcpConnectionPtr()const;

protected:
	bool DoRedisQuery();
	void InitQueryData();
	void DoQuery();
	void GetResultAndSendToClient();
	double CalcSimilarDegree(const vector<double> &lhs, const vector<double> &rhs);
	string GetPageAbstract(string pageText);
	string DecomposePageData(string &pageData, const string &startTag, const string &endTag);

private:
	string					 m_query;
	WEIGHT_MAP				 m_weight;
	PAGE_VEC				 m_page;
	const TcpConnectionPtr	 m_conn;
	Ripepage*				 m_pRipepage;
	acl::redis_client_pool&  m_redisClientPool;
	acl::redis_client*		 m_pRedisClientConn;
	acl::redis_string  	 	 m_redisString;

};

}//namespace wd

#endif
