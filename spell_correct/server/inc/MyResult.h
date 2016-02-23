#ifndef WD_MYRESULT_H
#define WD_MYRESULT_H
#include <string>

namespace wd
{

struct MyResult
{
	MyResult();
	MyResult(std::string word, int fre, int distance);
	std::string m_candidate_word;
	int m_frequency;
	int m_distance;
};

inline bool operator<(const MyResult &lhs, const MyResult &rhs)
{
	if(lhs.m_distance > rhs.m_distance)
		return true;
	else if(lhs.m_distance == rhs.m_distance)
	{
		if(lhs.m_frequency < rhs.m_frequency)
			return true;
	}
	return false;	
}

}//namespace wd

#endif
