#pragma once
#include "DataTable.h"
#include "Singleton.h"

class DataTableMgr : public Singleton<DataTableMgr>
{
protected:
	std::unordered_map<DataTable::Ids, DataTable*> tables;

	DataTableMgr(const DataTableMgr& other) = delete;
	bool operator== (const DataTableMgr& other) const = delete;


public:
	DataTableMgr() = default;
	virtual ~DataTableMgr() {  }

	virtual void LoadAll();
	virtual void ReleaseAll();

	template <typename T>
	T* Get(DataTable::Ids id)const;
};

template<typename T>
inline T* DataTableMgr::Get(DataTable::Ids id) const
{
	auto find = tables.find(id);
	if (find == tables.end())
	{
		return nullptr;
	}
	return dynamic_cast<T*>(find->second);
}

#define DATATABLE_MGR (DataTableMgr::Instance())

