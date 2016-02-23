#ifndef WD_MYPAGE_H
#define WD_MYPAGE_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <utility>

namespace wd
{
using std::vector;
using std::string;
using std::map;
using std::pair;
using std::set;

struct TopData;

struct MyPage
{
	MyPage(unsigned long docid, string title, string url, string content)
		:m_docid(docid),
		 m_title(title),
		 m_url(url),
		 m_content(content)
	{
	}

	void CalcTopK(const set<string> &stopList);
	void printTopK();

	unsigned long  		m_docid;
	string		   		m_title;
	string 				m_url;
	string 		   		m_content;
	map<string, int>	m_dict; 
	vector<TopData>     m_topKey;
};

struct TopData
{
	TopData(pair<string, int> pairdata)
		:m_word(pairdata.first),
		 m_fre(pairdata.second)
	{	
	}

	string	m_word;
	int		m_fre;
};


bool PageCompare(MyPage& lhs, MyPage& rhs);

bool operator<(const TopData& lhs, const TopData &rhs);
bool operator==(const TopData& lhs, const TopData &rhs);

}//namespace wd

#endif
