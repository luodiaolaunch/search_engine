#include "MySplit.h"

namespace wd
{

MySplit::MySplit(const char * dict, const char *hmm, const char *usr_dict)
	:m_jieba(dict, hmm, usr_dict)
{

}

void MySplit::Cut(string &filedata, vector<string> &words)
{
	m_jieba.Cut(filedata, words, true);
}

}//namespace wd

