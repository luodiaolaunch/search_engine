#include "IndexValue.h"

namespace wd
{
std::ostream& operator<<(std::ostream &os, const IndexValue &indexObj)
{
	os << indexObj.m_docid << '\t'
	   << indexObj.m_fre << '\t'
	   << indexObj.m_weight;
	return os;
}

std::istream& operator>>(std::istream &is, IndexValue &indexObj)
{
	is >> indexObj.m_docid
	   >> indexObj.m_fre
	   >> indexObj.m_weight;
	return is;
}

bool operator<(const IndexValue& lhs, const IndexValue& rhs)
{
	return lhs.m_docid < rhs.m_docid;
}

}//namespace wd
