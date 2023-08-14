#include "stdafx.h"
#include "SkillMgr.h"
#include "ResourceMgr.h"
#include "rapidcsv.h"

void SkillMgr::Init()
{
	LoadFromCsv("tables/SkillTable.csv");
}

void SkillMgr::LoadFromCsv(const std::string path)
{
	rapidcsv::Document doc(path);

	std::vector<int> code = doc.GetColumn<int>(0);
	std::vector<std::string> skillId = doc.GetColumn<std::string>(1);
	std::vector<float> skillCoolTime = doc.GetColumn<float>(2);
	std::vector<std::string> animation = doc.GetColumn<std::string>(3);

	for (size_t i = 0; i < code.size(); ++i)
	{
		rapidcsv::Document ani(animation[i]);

		std::vector<std::string> aniPath = ani.GetColumn<std::string>(0);

		AnimatioControler temp;
		for (size_t j = 0; j < aniPath.size(); ++j)
		{
			temp.AddClip(*RESOURCE_MGR.GetAnimationClip(aniPath[j]));
		}
		ChampionSkills.insert({ code[i],{ skillId[i], skillCoolTime[i], temp}});
	}
}

ChampionSkill* SkillMgr::GetSkill(const int code)
{
	auto findSkill = ChampionSkills.find(code);

	if (findSkill == ChampionSkills.end())
	{
		std::cout << "ERR : NOT EXIST SKILL" << std::endl;
		return nullptr;
	}

	return &findSkill->second;
}

void SkillMgr::ActiveSkill(int code, Champion* champ)
{
	switch (code)
	{
	case 1:
	{
		ArcherSkill(champ);
		break;
	}
	}
}

void SkillMgr::passiveSkill(int code, Champion* champ)
{
}

void SkillMgr::ArcherSkill(Champion* champ)
{
	//SKILL_MGR.ActiveSkill(champ->GetCurretState().skillCode1, champ);
	return;
}
