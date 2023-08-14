#pragma once
#include "DataTable.h"
#include "TeamMgr.h"

struct ItemInfo
{
	std::string textureId;
	int grade;
	int atk;
	int def;
	int atkSpeed;
	int coolDown;
	int hpDrain;
	int proficiencyType;
	int proficiencyValue;
	int champProficiency;
	int champProficiencyValue;
};

class ItemTable : public DataTable
{
protected:
	std::vector<std::unordered_map<int, ItemInfo>> tables;
public:
	ItemTable() : DataTable(DataTable::Ids::Item), tables((int)TeamMgr::ItemType::TypeCount) {};
	virtual ~ItemTable() override { Release(); }

	virtual bool Load() override;
	//virtual bool LoadFromCsv(std::string path) override;
	virtual void Release() override;

	const ItemInfo& Get(TeamMgr::ItemType item);
};


