#include "MyPage.h"
#include "Config.h"
#include <queue>
#include <iostream>
#include <iomanip>


namespace wd 
{


void MyPage::printTopK()
{
	std::cout << "TopKey:\n";
	for(const auto & topdata : m_topKey)
	{
		std::cout << std::left << std::setw(10) << topdata.m_word << "   " <<	topdata.m_fre << std::endl;
	}
}

void MyPage::CalcTopK(const set<string> &stop_list)
{
	std::priority_queue<TopData> queData;

	for(const auto & pair : m_dict)
	{
		queData.push(TopData(pair));
	}
	
	for(int idx = 0; idx < TOPK_VAL; ++idx)
	{
		auto word = queData.top().m_word;
		auto iter = stop_list.find(word);
		
		//属于停用词，不作为TOPK
		if(iter != stop_list.end())
		{
			--idx;
		}
		else
		{
			m_topKey.push_back(queData.top());
		}
//		std::cout << "Mypage line 49:" <<queData.top().m_word << "  " << queData.top().m_fre << std::endl;
		queData.pop();
	}
	
}


bool PageCompare(MyPage &lhs, MyPage &rhs)
{

	int cnt = 0;
	for(const auto &pair_l : lhs.m_topKey)
	{
		for(const auto &pair_r : rhs.m_topKey)
		{
			if(pair_l == pair_r)
			{
				++cnt;
				break;
			}
		}
	}
	
	if(cnt >= PAGEDUP_VAL)
		return true;
	return false;
}

bool operator<(const TopData& lhs, const TopData& rhs)
{
	if(lhs.m_fre < rhs.m_fre)
		return true;
	return false;
}

bool operator==(const TopData& lhs, const TopData& rhs)
{
	if(lhs.m_word != rhs.m_word)
		return false;
	if(lhs.m_fre != rhs.m_fre)
		return false;
	return true;
}




}//namespace wd
