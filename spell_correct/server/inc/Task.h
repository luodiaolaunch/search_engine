#ifndef TASK_H
#define TASK_H

#include "TcpConnection.h"
#include "MyResult.h"
#include <string>
#include <queue>

namespace wd
{
class WorkThread;

class Task
{
public:
	Task(std::string str, const TcpConnectionPtr& conn);
	void process();
	void setWordThreadPtr(WorkThread* pthread);

private:
	int calDistance(const std::string &rhs);

	void doQuery_CN();
	void doQuery_EN();
	void doQuery_MIX();

	void print();
	void saveResultData();
	void addQueryToThreadCache();
	void sendResultToClient();
	void checkQueryWord();

private:
	std::string  				     m_query;
	const TcpConnectionPtr&			 m_conn;
	std::priority_queue<MyResult> 	 m_que;
	std::vector<MyResult> 			 m_result;
	WorkThread*						 m_pThread;
	bool 							 m_isEng;
}; 


}//namespace wd

#endif
