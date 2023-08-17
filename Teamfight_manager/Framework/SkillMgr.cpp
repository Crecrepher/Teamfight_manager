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
	case 2:
	{
		ArcherUltiSkill(champ);
		break;
	}
	case 4:
	{
		FighterSkill(champ);
		break;
	}
	case 12:
	{
		MonkSkill(champ);
		break;
	}
	case 14:
	{
		NinjaSkill(champ);
		break;
	}
	}
}

void SkillMgr::passiveSkill(int code, Champion* champ)
{
}

void SkillMgr::ArcherSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		champ->SetStartPos(champ->GetPosition());
		sf::Vector2f dir = Utils::Normalize(champ->GetTarget()->GetPosition() - champ->GetPosition());
		float magnitude = Utils::Magnitude(dir);
		dir.x *= -1;
		dir.y *= -1;
		champ->SetEndPos(champ->GetPosition() + (dir * 15.f));
		champ->SetMoveTime(0.f);
		champ->SetAir(true);
		return;
	}

	if (champ->GetCurretState().animaition.GetCurrFrame() > 1)
	{
		float t = champ->GetMoveTime() / 0.6f;
		sf::Vector2f temp = champ->GetStartPos() + t * (champ->GetEndPos() - champ->GetStartPos());
		champ->SetPosition(temp);
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		std::cout << "스킬" << std::endl;
		champ->DamageCalculate(champ->GetCurretState().attack - 10);
		champ->GetTarget()->SetBind(0.5f);
		champ->SkillChangeIdle();
		champ->SetAir(false);
		return;
	}
}

void SkillMgr::ArcherUltiSkill(Champion* champ)
{
}

void SkillMgr::BerserkerSkill(Champion* champ)
{
}

void SkillMgr::FighterSkill(Champion* champ)
{
	champ->GetTarget()->SetBind(1.f);

	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		champ->SetStartPos(champ->GetPosition());
		sf::Vector2f dir = Utils::Normalize(champ->GetTarget()->GetPosition() - champ->GetPosition());
		champ->SetEndPos(champ->GetTarget()->GetPosition() + (dir * 30.f));
		champ->SetMoveTime(0.f);
		return;
	}

	 if (champ->GetCurretState().animaition.GetCurrFrame() < 3)
	{
		float t = champ->GetMoveTime() / 0.3f;
		sf::Vector2f temp = champ->GetStartPos() + t * (champ->GetEndPos() - champ->GetStartPos());
		champ->SetPosition(temp);
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() == 3)
	{
		champ->SetStartPos(champ->GetPosition());
		sf::Vector2f dir = Utils::Normalize(champ->GetTarget()->GetPosition() - champ->GetPosition());
		if (!champ->GetTarget()->GetAir())
		{
			champ->GetTarget()->SetStartPos(champ->GetTarget()->GetPosition());
			champ->GetTarget()->SetEndPos(champ->GetTarget()->GetPosition() + dir * 64.f);
			champ->GetTarget()->SetAir(true);
		}
		champ->SetEndPos(champ->GetTarget()->GetPosition() + dir * 64.f);
		champ->SetMoveTime(0.f);
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() > 3)
	{
		if (champ->GetMoveTime() >= 0.5f)
		{
			champ->SetMoveTime(0.5f);
		}
		float t = champ->GetMoveTime() / 0.5f;

		sf::Vector2f temp = champ->GetStartPos() + t * (champ->GetEndPos() - champ->GetStartPos());
		sf::Vector2f temp_enemy = champ->GetTarget()->GetStartPos() + t * (champ->GetTarget()->GetEndPos() - champ->GetTarget()->GetStartPos());
		champ->GetTarget()->SetPosition(temp_enemy);
		champ->SetPosition(temp);
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		champ->DamageCalculate(champ->GetCurretState().attack +10);
		champ->GetTarget()->SetBind(0.f);
		champ->GetTarget()->SetAir(false);
		champ->SkillChangeIdle();
		return;
	}
}

void SkillMgr::FighterUltiSkill(Champion* champ)
{
}

void SkillMgr::MonkSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetCurrFrame() == 3)
	{
		if (!champ->GetFrameLimit())
		{
			champ->TagetOrderCIT(1, 250.f, 30.f);
			champ->SetFrameLimit(true);
		}
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		std::cout << "스킬" << std::endl;
		champ->SetFrameLimit(false);
		champ->SkillChangeIdle();
		return;
	}
}

void SkillMgr::NinjaSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetCurrFrame() == 7)
	{
		champ->TagetOrderLR();

		sf::Vector2f dir = Utils::Normalize(champ->GetTarget()->GetPosition() - champ->GetPosition());

		if (dir.x >= 0)
		{
			champ->SetEndPos({ champ->GetTarget()->GetPosition().x + 30.f, champ->GetTarget()->GetPosition().y });
			champ->SetSacleX(-1);
		}
		else if (dir.x <0)
		{
			champ->SetEndPos({ champ->GetTarget()->GetPosition().x - 30.f, champ->GetTarget()->GetPosition().y });
			champ->SetSacleX(1);
		}

		champ->SetPosition(champ->GetEndPos());
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		champ->DamageCalculate(champ->GetCurretState().attack);
		champ->GetTarget()->SetBloodingStack(5);
		champ->GetTarget()->UseBloodingPlayer(champ);
		champ->SkillChangeIdle();
		return;
	}
}
