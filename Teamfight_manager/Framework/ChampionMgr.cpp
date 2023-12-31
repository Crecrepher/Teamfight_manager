#include "stdafx.h"
#include "ChampionMgr.h"
#include "ResourceMgr.h"
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
	std::vector<std::string> animation = doc.GetColumn<std::string>(10);


	for (size_t i = 0; i < id.size(); ++i)
	{
		rapidcsv::Document ani(animation[i]);

		std::vector<std::string> aniPath = ani.GetColumn<std::string>(0);

		AnimatioControler temp;
		for (size_t j = 0; j < aniPath.size(); ++j)
		{
			temp.AddClip(*RESOURCE_MGR.GetAnimationClip(aniPath[j]));
		}
		champions.insert({ i, {id[i],mHp[i],atk[i],def[i],atkSp[i],atkRg[i],
			sp[i],skill_1[i],skill_2[i],(ChampionType)type[i],temp} });
	}
}

State* ChampionMgr::GetChampion(int id)
{
	auto findChamp = champions.find(id);

	if (findChamp == champions.end())
	{
		std::cout << "ERR : NOT EXIST CHAMPION" << std::endl;
		return nullptr;
	}

	return &findChamp->second;
}

State* ChampionMgr::GetChampion(const std::string& id)
{
	for (auto it : champions)
	{
		if (it.second.charId == id)
		{
			return &it.second;
		}
	}
	std::cout << "ERR : NOT EXIST CHAMPION" << std::endl;
	return nullptr;
}

int ChampionMgr::GetChampionCode(const std::string& id)
{
	for (auto it : champions)
	{
		if (it.second.charId == id)
		{
			return it.first;
		}
	}
	std::cout << "ERR : NOT EXIST CHAMPION" << std::endl;
	return 0;
}
