#include "stdafx.h"
#include "ItemTable.h"
#include "rapidcsv.h"

bool ItemTable::Load()
{
	std::vector< std::string >filenames;
	filenames.push_back("items/ItemHeadsetList.csv");
	filenames.push_back("items/ItemControllerList.csv");
	filenames.push_back("items/ItemChairList.csv");
	filenames.push_back("items/ItemUniformList.csv");
	
	for (int i = 0; i < tables.size(); ++i)
	{
		rapidcsv::Document doc(filenames[i]);
		std::vector<int> itemCode = doc.GetColumn<int>(0);
		std::vector<std::string> textureId = doc.GetColumn<std::string>(1);
		std::vector<int> grade = doc.GetColumn<int>(2);
		std::vector<int> atk = doc.GetColumn<int>(3);
		std::vector<int> def = doc.GetColumn<int>(4);
		std::vector<int> atkSpeed = doc.GetColumn<int>(5);
		std::vector<int> coolDown = doc.GetColumn<int>(6);
		std::vector<int> hpDrain = doc.GetColumn<int>(7);
		std::vector<int> proficiencyType = doc.GetColumn<int>(8);
		std::vector<int> proficiencyValue = doc.GetColumn<int>(9);
		std::vector<int> champProficiency = doc.GetColumn<int>(10);
		std::vector<int> champProficiencyValue = doc.GetColumn<int>(11);
		for (int j = 0; j < itemCode.size(); ++j)
		{
			tables[i].insert({ itemCode[j],{textureId[j],grade[j],
				atk[j],def[j],atkSpeed[j],coolDown[j],hpDrain[j]
				,proficiencyType[j],proficiencyValue[j],
				champProficiency[j],champProficiencyValue[j]}});
		}
	}

	return true;
}


void ItemTable::Release()
{
	for (auto table : tables)
	{
		table.clear();
	}
}

const ItemInfo& ItemTable::Get(TeamMgr::ItemType item)
{
	auto& table = tables[(int)item];

	auto find = table.find((int)item);
	if (find == table.end())
	{
		std::cout << "ERR: Undefined" << std::endl;
		return ItemInfo();
	}
	//예외 throw / 포인터로 넘겨주기

	return find->second;
}
