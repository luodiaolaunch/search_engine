#ifndef WD_MYSPLIT_H
#define WD_MYSPLIT_H

#include <vector>
#include <string>
#include <utility>
#include "jieba/src/Jieba.hpp"

namespace wd 
{
using std::string;
using std::vector;
using std::pair;

class MySplit
{
public:
	MySplit(const char *dict, const char *hmm, const char *usr_dict);
	void Cut(string &filedata, vector<string> &words);

private:
	cppjieba::Jieba m_jieba;
};


}//namespace wd

#endif
