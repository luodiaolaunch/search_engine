#ifndef WD_RIPEPAGE_H
#define WD_RIPEPAGE_H

#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <fstream>
#include "Noncopyable.h"
#include "MyPage.h"
#include "MySplit.h"
#include "IndexValue.h"

namespace wd
{

using std::vector;
using std::string;
using std::pair;
using std::set;
using std::unordered_map;

struct pairHash;

class Ripepage : private Noncopyable
{
	friend class Task;
public:

	using pos_type = std::fstream::pos_type;
	using INDEX_MAP = unordered_map<string, set<IndexValue> >;
	using WEIGHT_MAP = unordered_map<pair<string, unsigned long>, double, pairHash>;
	using WEIGHT_ROOT_MAP = unordered_map<unsigned long, double>;
	using OFFSET_MAP = unordered_map<unsigned long, pair<pos_type, pos_type> >;

	Ripepage();
	
	void LoadSourceFile();
	void CreateRipepageAndOffsetLib();	
	void CreateIndexLib();
	void DeletePageVector();

	void LoadRipepageLib();
	void LoadOffsetLib();
	void LoadIndexLib();

protected:
	void LoadStopWordsFile();
	void LoadSourceFilePath(vector<string> &pathVec);
	void ProcessRipeFile(const string ripefile, string &title, string &content);
	void TraverseDir(const string dir, vector<string> &pathVec);

	void CutRipepageAndCalcTopK();
	void PagedupRemove();	
	void WriteRipepageAndOffsetLib();
	void CalcWeightMap(WEIGHT_MAP &weightMap, WEIGHT_ROOT_MAP &weightRootMap);
	void WriteIndexLib() const; 


private:
	vector<MyPage> 	m_pageVec;
	OFFSET_MAP		m_offset;
	INDEX_MAP	  	m_index;
	
	MySplit			m_split;
	set<string>		m_stopWords;
};

struct pairHash
{
	template<typename T, typename U>
	std::size_t operator()(const std::pair<T, U> &x)const
	{
		return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
	}
};


}//namespace wd


#endif
