#ifndef WD_CACHEDATA_H
#define WD_CACHEDATA_h

#include "MyResult.h"
#include <utility>
#include <vector>

namespace wd
{

struct CacheData
{
	CacheData(const std::vector<MyResult> data)
		:m_cachedata(data)
	{}

	std::vector<MyResult> m_cachedata;
};

}//namespace wd

#endif
