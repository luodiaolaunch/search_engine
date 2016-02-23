#ifndef WD_CACHE_H
#define WD_CACHE_H

#include "MyResult.h"
#include "MutexLock.h"

//#include "CacheData.h"
#include <unordered_map>
#include <utility>
#include <string>
#include <vector>

namespace wd
{
struct Hasher
{
	std::size_t operator()(const std::string str) const
	{
		return static_cast<size_t>(str.front());
	}
};

struct KeyEqual
{
	bool operator()(const std::string lhs, const std::string rhs) const
	{
		return lhs == rhs;
	}
};

class Cache
{
public:
	using CacheData = std::vector<MyResult>;
	using CacheMap = std::unordered_map<std::string, CacheData>;
	using CachePair = std::pair<std::string, CacheData>;

	void writeCacheFile();
	void loadCacheFile();

	CacheData& searchCacheData(const std::string &keyword);
	void addQueryToCache(CachePair pair_data);
	void addQueryToCache(std::string keyword, CacheData result);
	void setCacheMap(Cache& cache);
	CacheMap& getCacheMap();
	size_t size() const;

private:
//	std::unordered_map<std::string, CacheData> m_cache_map;
	CacheMap  m_cache_map;
	MutexLock m_mutex;
};

std::ostream& operator<<(std::ostream &os, Cache::CacheData &result_vec);
std::istream& operator>>(std::istream &is, Cache::CacheData &result_vec);

}//namespace wd

#endif
