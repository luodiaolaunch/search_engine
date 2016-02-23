#include "MyResult.h"

namespace wd
{
MyResult::MyResult()
	:m_candidate_word(),
	 m_frequency(0),
	 m_distance(0)
{
}

MyResult::MyResult(std::string word, int fre, int distance)
	:m_candidate_word(word),
	 m_frequency(fre),
	 m_distance(distance)
{
}

}//namespace wd
