#ifndef WD_INDEX_VALUE_H
#define WD_INDEX_VALUE_H

#include <iostream>

namespace wd
{
struct IndexValue
{
	IndexValue()
		:m_docid(0),
		 m_fre(0),
		 m_weight(0)
	{
	}

	IndexValue(unsigned long docid,
			   int fre,
			   double weight)
		:m_docid(docid),
		 m_fre(fre),
		 m_weight(weight)
	{
	}

	unsigned long m_docid;
	int 		  m_fre;
	double 		  m_weight;
};

std::ostream& operator<<(std::ostream& os, const IndexValue& indexObj);
std::istream& operator>>(std::istream& is, IndexValue& indexObj);
bool operator<(const IndexValue& lhs, const IndexValue& rhs);

}//namespace wd 

#endif
