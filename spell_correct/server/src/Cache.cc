#include "Cache.h"
#include "Config.h"

namespace wd
{

#if 1


Cache::CacheMap& Cache::getCacheMap()
{
	return m_cache_map;
}


void Cache::setCacheMap(Cache& cache)
{
	MutexLockGuard mutexguard(m_mutex);
	m_cache_map = cache.m_cache_map;
}

Cache::CacheData& Cache::searchCacheData(const std::string &keyword)
{
	auto it = m_cache_map.find(keyword);
	if(it != m_cache_map.end())
	{
		MutexLockGuard mutexguard(m_mutex);
		return m_cache_map[keyword];
	}
	else
	{
		static std::vector<MyResult> vec;
		return vec;
	}
}
#endif 

void Cache::addQueryToCache(std::string keyword, CacheData result)
{
	MutexLockGuard mutexguard(m_mutex);
	m_cache_map.insert(std::make_pair(keyword, result));
}


void Cache::addQueryToCache(CachePair pair_data)
{
	MutexLockGuard mutexguard(m_mutex);
	m_cache_map.insert(pair_data);
}

size_t Cache::size() const 
{
	return m_cache_map.size();
}

#if 1
void Cache::writeCacheFile()
{
	MyConf* pMyConf = Singleton<MyConf>::getInstance();
	std::string filename = pMyConf->getConfigByName(CACHE_NAME);

	std::ofstream ofs(filename);
	if(!ofs)
	{
		logError("writeCacheFile");
		::exit(EXIT_FAILURE);
	}
	SafeFile<std::ofstream> safe_ofs(ofs);


	MutexLockGuard mutexguard(m_mutex);

	for(auto &pair : m_cache_map)
	{
		ofs << pair.first << '\t' << pair.second << std::endl;
	}

}

void Cache::loadCacheFile()
{
	MyConf* pMyConf = Singleton<MyConf>::getInstance();
	std::string filename = pMyConf->getConfigByName(CACHE_NAME);

	std::ifstream ifs(filename);
	if(!ifs)
	{
		return;
		//logError("loadCacheFile");
		//::exit(EXIT_FAILURE);
	}
	SafeFile<std::ifstream> safe_ifs(ifs);
	
	MutexLockGuard mutexguard(m_mutex);

	std::string line, keyword;
	CacheData result_vec;

	while(std::getline(ifs, line))
	{
		std::istringstream record(line);
		record >> keyword >> result_vec;

		//判断是否读取空行
		if(keyword.empty())
			continue;
		m_cache_map.insert({keyword, result_vec});
		result_vec.clear();
	}
}


std::ostream& operator<<(std::ostream &os, Cache::CacheData &result_vec)
{
	for(const auto & result : result_vec)
	{
		os << result.m_candidate_word << '\t'
		   << result.m_frequency <<'\t'
		   << result.m_distance << '\t';
	}
	return os;
}

std::istream& operator>>(std::istream &is, Cache::CacheData &result_vec)
{
	MyResult tmp;
	while(is >> tmp.m_candidate_word
			 >> tmp.m_frequency
			 >> tmp.m_distance)
	{
		result_vec.push_back(tmp);
	}
	return is;
}

#endif

}//namespace wd
