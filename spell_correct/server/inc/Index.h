#ifndef WD_INDEX_H
#define WD_INDEX_H

#include "Diction.h"
#include <unordered_map>
#include <set>

namespace wd
{


class Index
{
public:
	using DictData = Diction::DictDataVec;
	using set_size_type = DictData::size_type;
	using IndexData_CN = std::unordered_map<long, std::set<set_size_type> >;
	using IndexData_EN = std::unordered_map<long , std::set<set_size_type> >;

	Index();

	void loadIndexMapFromIndexFile_EN();
	void loadIndexMapFromIndexFile_CN();

	void loadDictVecFromDictFile_EN();
	void loadDictVecFromDictFile_CN();

	void writeIndexMapToIndexFile_EN();
	void writeIndexMapToIndexFile_CN();

	DictData& getDictVec_CN();
	DictData& getDictVec_EN();

	IndexData_CN& getIndexMap_CN();
	IndexData_EN& getIndexMap_EN();

	void establishIndexMap_CN();
	void establishIndexMap_EN();

private:
	DictData 	 m_vecDict_CN;
	DictData 	 m_vecDict_EN;
	IndexData_EN m_mapIndex_EN;
	IndexData_CN m_mapIndex_CN;

};

std::ostream& operator<<(std::ostream &os, std::set<Index::set_size_type>& index_set);
std::istream& operator>>(std::istream &is, std::set<Index::set_size_type>& index_set);

}//namespace wd


#endif
