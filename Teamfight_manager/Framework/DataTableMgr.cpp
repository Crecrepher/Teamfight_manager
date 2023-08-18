#include "stdafx.h"
#include "DataTableMgr.h"
#include "StringTable.h"
#include "ItemTable.h"

void DataTableMgr::LoadAll()
{
	tables.insert({ DataTable::Ids::String, new StringTable()});
	tables.insert({ DataTable::Ids::Item, new ItemTable() });
	for (auto pair : tables)
	{
		if (!pair.second->Load())
		{
			std::cout << "ERR: DATA TABLE LOAD FAIL" << std::endl;
			exit(-1);
		}
	}
}

void DataTableMgr::ReleaseAll()
{
	for (auto pair : tables)
	{
		delete pair.second;
	}
	tables.clear();
}
