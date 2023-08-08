#include "stdafx.h"
#include "ChampionMgr.h"
#include "rapidcsv.h"

void ChampionMgr::Init()
{
	LoadFromCsv("tables/ChampionTable.csv");
}

void ChampionMgr::LoadFromCsv(const std::string path)
{
	rapidcsv::Document doc(path);

	std::vector<std::string> id = doc.GetColumn<std::string>(0);
	std::vector<float> mHp = doc.GetColumn<float>(1);
	std::vector<float> atk = doc.GetColumn<float>(2);
	std::vector<float> def = doc.GetColumn<float>(3);
	std::vector<float> atkSp = doc.GetColumn<float>(4);
	std::vector<float> atkRg = doc.GetColumn<float>(5);
	std::vector<float> sp = doc.GetColumn<float>(6);
	std::vector<int> skill_1 = doc.GetColumn<int>(7);
	std::vector<int> skill_2 = doc.GetColumn<int>(8);
	std::vector<int> type = doc.GetColumn<int>(9);


	for (size_t i = 0; i < id.size(); ++i)
	{
		champions.insert({ id[i], {id[i],mHp[i],atk[i],def[i],atkSp[i],atkRg[i],
			sp[i],skill_1[i],skill_2[i],(ChampionType)type[i]} });
	}
}

State ChampionMgr::GetChampion(const std::string& id)
{
	auto findChamp = champions.find(id);
	if (findChamp == champions.end())
	{
		std::cout << "ERR : NOT EXIST CHAMPION" << std::endl;
		State null{ "",0,0,0,0,0,0,0,0,(ChampionType)0 };
		return null;
	}

	return findChamp->second;
}
