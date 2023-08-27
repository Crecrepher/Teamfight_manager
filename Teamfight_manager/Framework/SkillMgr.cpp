#include "stdafx.h"
#include "SkillMgr.h"
#include "ResourceMgr.h"
#include "SceneMgr.h"
#include "rapidcsv.h"
#include "BuffState.h"
#include "ChampionEffect.h"


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
	champ->SetOrigin(Origins::MC);
	if (!champ->GetCurretState().animaition.GetLastFrame() && champ->GetTarget()->GetHp() == 0)
	{
		//std::cout << "이미 죽음" << std::endl;
		champ->SkillChangeIdle();
		return;
	}

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
	case 3:
	{
		BerserkerSkill(champ);
		break;
	}

	case 4:
	{
		FighterSkill(champ);
		break;
	}
	case 5:
	{
		FighterUltiSkill(champ);
		break;
	}
	case 6:
	{
		IcemageSkill(champ);
		break;
	}
	case 7:
	{
		IcemageUltiSkill(champ);
		break;
	}
	case 8:
	{
		KnightSkill(champ);
		break;
	}
	case 9:
	{
		KnightUltiSkill(champ);
		break;
	}
	case 10:
	{
		MagicknightSkill(champ);
		break;
	}
	case 11:
	{
		MagicknightUltiSkill(champ);
		break;
	}
	case 12:
	{
		MonkSkill(champ);
		break;
	}
	case 13:
	{
		MonkUltiSkill(champ);
		break;
	}
	case 14:
	{
		NinjaSkill(champ);
		break;
	}
	case 15:
	{
		NinjaUltiSkill(champ);
		break;
	}
	case 16:
	{
		PriestSkill(champ);
		break;
	}
	case 17:
	{
		PriestUltiSkill(champ);
		break;
	}
	case 18:
	{
		PyromancerSkill(champ);
		break;
	}
	case 19:
	{
		PyromancerUltiSkill(champ);
		break;
	}
	case 20:
	{
		PythonessSkill(champ);
		break;
	}
	case 21:
	{
		PythonessUltiSkill(champ);
		break;
	}
	case 22:
	{
		ShieldbearerUltiSkill(champ);
		break;
	}
	case 23:
	{
		SoldierSkill(champ);
		break;
	}
	case 24:
	{
		SoldierUltiSkill(champ);
		break;
	}
	case 25:
	{
		SwordmanSkill(champ);
		break;
	}
	case 26:
	{
		SwordmanUltiSkill(champ);
		break;
	}
	case 27:
	{
		BerserkerUltiSkill(champ);
		break;
	}
	case 28:
	{
		ShieldbearerSkill(champ);
		break;
	}
	}
	champ->SetOrigin(Origins::MC);
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
	if (champ->GetCurretState().animaition.GetCurrFrame() == 5)
	{
		if(!champ->GetAttackFrame())
		{ 
			ChampionEffect* arrow = champ->GetEffectPool();
			sf::IntRect setting = { 0,0,16,16 };
			arrow->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/arrow.png"));
			arrow->SetOrigin(Origins::MC);
			arrow->sprite.setTextureRect(setting);
			arrow->SetChampion(champ);
			arrow->SetEffectType(5);
			arrow->sortLayer = 2;
			arrow->SetActive(true);
			arrow->SetPosition(champ->GetPosition());
			champ->SetAttackFrame(true);
			champ->SetAMoveT(0.f);
			SCENE_MGR.GetCurrScene()->AddGo(arrow);
		}
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		champ->SetAttackFrame(false);
		//std::cout << "스킬" << std::endl;
		champ->DamageCalculate(champ->GetCurretState().attack - 10);
		champ->GetTarget()->SetBind(0.5f);
		champ->SkillChangeIdle();
		champ->SetAir(false);
		return;
	}
}
// 완료


void SkillMgr::ArcherUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		champ->UseUltiSkill();
		BuffState* ulti = new BuffState;
		ulti->SetType(BuffType::ULTIMATE);
		ulti->SetCount(5);
		ulti->SetValue(0);
		champ->SetBuff(ulti);
		champ->SetMoveTime(0.f);
		champ->SetAir(true);
		return;
	}
	if (champ->GetMoveTime() >= 0.2f)
	{
		if (!champ->GetAttackFrame())
		{
			ChampionEffect* arrow = champ->GetEffectPool();
			sf::IntRect setting = { 0,0,16,16 };
			arrow->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/arrow.png"));
			arrow->SetOrigin(Origins::MC);
			arrow->sprite.setTextureRect(setting);
			arrow->SetChampion(champ);
			arrow->SetEffectType(5);
			arrow->sortLayer = 2;
			arrow->SetActive(true);
			arrow->SetPosition(champ->GetPosition());
			champ->SetAttackFrame(true);
			champ->SetAMoveT(0.f);
			SCENE_MGR.GetCurrScene()->AddGo(arrow);
		}
	}
	if (champ->GetMoveTime() >= 0.3f)
	{
		champ->SetAttackFrame(false);
		champ->TargetOrderSR();
		champ->DamageCalculate(champ->GetCurretState().attack - 10);
		champ->SetMoveTime(0.f);
		return;
	}
}
// 완료


void SkillMgr::BerserkerSkill(Champion* champ)
{
	if (champ->CurrBuffEmpty())
	{
		BuffState* attack = new BuffState;
		attack->SetType(BuffType::ATTACK);
		attack->SetValue(0);
		attack->SetCount(999);
		champ->SetBuff(attack);
		BuffState* speed = new BuffState;
		speed->SetType(BuffType::SPEED);
		speed->SetValue(0);
		speed->SetCount(999);
		champ->SetBuff(speed);
		BuffState* atkSP = new BuffState;
		atkSP->SetType(BuffType::ATTACKSPEED);
		atkSP->SetValue(0);
		atkSP->SetCount(999);
		champ->SetBuff(atkSP);

		champ->SkillChangeIdle();
		return;
	}
	else
	{	
		if (abs(Utils::Distance(champ->GetPosition(), champ->GetTarget()->GetPosition()) > champ->GetCurretState().attackRange))
		{
			champ->SkillAniChange();
			champ->ChangeStance(ChampionStance::Move);
			return;
		}
		else if (abs(Utils::Distance(champ->GetPosition(), champ->GetTarget()->GetPosition()) <= champ->GetCurretState().attackRange))
		{
			if (champ->GetHpPercent() <= 0.4f)
			{
				if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
				{
					champ->UseSkill();
					return;
				}

				if (!champ->GetCurretState().animaition.GetLastFrame() && champ->GetTarget()->GetHp() == 0)
				{
					champ->SkillChangeIdle();
					return;
				}

				if (champ->GetCurretState().animaition.GetLastFrame())
				{
					champ->DamageCalculate(champ->GetCurretState().attack);
					champ->Heal(champ->GetCurretState().attack / 10.f);
					champ->SkillChangeIdle();
					return;
				}
			}
			else
			{
				champ->SkillAniChange();
				champ->ChangeStance(ChampionStance::Action);
			}
			return;
		}
	}

}
// 완료


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
// 완료


void SkillMgr::FighterUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		BuffState* buff = new BuffState;
		buff->SetType(BuffType::STUN);
		buff->SetCount(0.5);
		champ->TargetOrderCIEB(1, 175, buff);
		champ->TargetOrderCIE(1, 175, champ->GetCurretState().attack);
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::IcemageSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetCurrFrame() < 3 && champ->GetTarget()->GetHp() == 0)
	{
		//std::cout << "이미 죽음" << std::endl;
		champ->SkillChangeIdle();
		return;
	}

	if (champ->GetCurretState().animaition.GetCurrFrame() == 3)
	{
		if (!champ->GetFrameLimit())
		{
			BuffState* state = new BuffState;
			state->SetType(BuffType::STUN);
			state->SetCount(1.5);
			champ->GetTarget()->SetBuff(state);
			champ->DamageCalculate(champ->GetCurretState().attack + 5);
			champ->SetFrameLimit(true);
		}
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		//std::cout << "스킬" << std::endl;
		champ->SetFrameLimit(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::IcemageUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		std::cout << "ice ground" << std::endl;
		champ->SetSkillObj(1, 8.f, 0.5, 0.f);
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::KnightSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		champ->TargetOrderLR();
		BuffState* aggro = new BuffState;
		aggro->SetType(BuffType::AGGRO);
		aggro->SetCount(4);
		if (champ->GetTarget()->GetCurretState().charId != "priest")
		{
			champ->TargetOrderLR();
			champ->GetTarget()->SetTarget(champ);
		}
		else if (champ->GetTarget()->GetCurretState().charId == "priest")
		{
			champ->TargetOrderSR();
			champ->GetTarget()->SetTarget(champ);
		}
		champ->GetTarget()->SetBuff(aggro);

		BuffState* buff = new BuffState;
		buff->SetType(BuffType::DEFEND);
		buff->SetValue(20);
		buff->SetCount(4);
		champ->SetBuff(buff);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::KnightUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		BuffState* buff = new BuffState;
		buff->SetType(BuffType::DEFEND);
		buff->SetValue(50);
		buff->SetCount(5);
		champ->SetMyTeamBuff(buff);
		//std::cout << "스킬" << std::endl;
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::MagicknightSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		std::cout << "스킬 : " << champ->GetCurretState().charId << std::endl;
		champ->UseSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		sf::Vector2f temp = Utils::Normalize(champ->GetTarget()->GetPosition() - champ->GetPosition());
		champ->SetSkillObj(7, 30.f, 0.01f, 0.f,temp,"animations/Effect/MagicknightSkill.csv","SkillEffect");
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}



void SkillMgr::MagicknightUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		std::cout << "궁극기 : " << champ->GetCurretState().charId << std::endl;
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		std::cout << "blackhole ground" << std::endl;
		champ->SetSkillObj(3, 15.f, 2.f, 0.f);
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


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
			champ->TargetOrderCIT(1, 200.f, 30.f);
			champ->SetFrameLimit(true);
		}
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		//std::cout << "스킬" << std::endl;
		champ->SetFrameLimit(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::MonkUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		BuffState* berrier = new BuffState;
		berrier->SetType(BuffType::BARRIER);
		berrier->SetCount(5);
		berrier->SetValue(70);
		berrier->SetSide(true);
		berrier->SetSideType(BuffType::SPEED);
		berrier->SetSideValue(2.f);
		champ->TargetOrderCITB(1,200.f, berrier);
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::NinjaSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetCurrFrame() == 7)
	{
		if (!champ->GetFrameLimit())
		{
			if (!champ->GetAir())
			{
				if (champ->GetTarget()->GetOrder() != TargetingOrder::Aggro)
				{
					champ->TargetOrderLR();
				}

				sf::Vector2f dir = Utils::Normalize(champ->GetTarget()->GetPosition() - champ->GetPosition());

				if (dir.x >= 0)
				{
					champ->SetEndPos({ champ->GetTarget()->GetPosition().x + 30.f, champ->GetTarget()->GetPosition().y });
					champ->SetSacleX(-1);
				}
				else if (dir.x < 0)
				{
					champ->SetEndPos({ champ->GetTarget()->GetPosition().x - 30.f, champ->GetTarget()->GetPosition().y });
					champ->SetSacleX(1);
				}

				champ->SetPosition(champ->GetEndPos());
				champ->SetFrameLimit(true);
			}
		}
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		champ->SetFrameLimit(false);
		champ->SetAir(false);
		champ->DamageCalculate(champ->GetCurretState().attack);
		champ->GetTarget()->SetBloodingStack(5);
		champ->GetTarget()->UseBloodingPlayer(champ);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::NinjaUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		champ->SetUltiSkill(false);
		Champion* dummyChamp = champ->GetPool();

		*dummyChamp = *champ;

		dummyChamp->SetShadow();
		dummyChamp->SetHpGuage();
		dummyChamp->SetFullHp();
		dummyChamp->SetPosition(champ->GetPosition().x - 10.f, champ->GetPosition().y);
		champ->SetCopyChar(dummyChamp);
		SCENE_MGR.GetCurrScene()->AddGo(dummyChamp);

		BuffState* dummy = new BuffState;
		dummy->SetType(BuffType::COPY);
		dummy->SetCount(10);
		dummy->SetValue(15.f);
		
		dummyChamp->SetBuff(dummy);
		dummyChamp->SkillChangeIdle();
		champ->SkillChangeIdle();

		std::cout << "닌자 생성" << std::endl;
		return;
	}
}
// 완료


void SkillMgr::PriestSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		BuffState* berrier = new BuffState;
		berrier->SetType(BuffType::BARRIER);
		berrier->SetCount(4);
		berrier->SetValue(25);
		berrier->SetSide(true);
		berrier->SetSideType(BuffType::ATTACKSPEED);
		berrier->SetSideValue(0.2f);
		champ->GetTarget()->SetBuff(berrier);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::PriestUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		std::cout << "궁극기 : " << champ->GetCurretState().charId << std::endl;
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		std::cout << "heal ground" << std::endl;
		champ->SetSkillObj(4, 5.f, 0.3, 0.f);
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::PyromancerSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		std::cout << "fire ball" << std::endl;
		champ->SetSkillObj(5, 5.f, 0.7f, 0.f);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::PyromancerUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		std::cout << "궁극기 : " << champ->GetCurretState().charId << std::endl;
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		std::cout << "fire ground" << std::endl;
		champ->SetSkillObj(2, 8.f, 1.0f, 0.f);
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::PythonessSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetCurrFrame() == 2)
	{
		if (!champ->GetFrameLimit())
		{
			champ->TargetOrderH();
			champ->HealCalculate(15);

			//std::cout << "mu heal" << std::endl;
			champ->SetFrameLimit(true);
		}
		return;
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() == 3)
	{
		champ->SetFrameLimit(false);
		return;
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() == 4)
	{
		if (!champ->GetFrameLimit())
		{
		champ->TargetOrderH();
		champ->TargetRangeDamage(40, 15);
		champ->SetFrameLimit(true);
		}
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		//std::cout << "스킬" << std::endl;
		champ->SetFrameLimit(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::PythonessUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		std::cout << "궁극기 : " << champ->GetCurretState().charId << std::endl;
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		std::cout << "door ground" << std::endl;
		champ->SetSkillObj(6, 10.f, 0.3f, 0.f);
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::ShieldbearerUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		BuffState* ulti = new BuffState;
		ulti->SetType(BuffType::ULTIMATE);
		ulti->SetCount(10);
		ulti->SetValue(0);
		ulti->SetSide(true);
		ulti->SetSideType(BuffType::DEFEND);
		ulti->SetSideValue(30.f);
		champ->SetBuff(ulti);

		BuffState* aggro = new BuffState;
		aggro->SetType(BuffType::AGGRO);
		aggro->SetCount(10);
		champ->TargetOrderCIEB(1, 200.f, aggro);
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::SoldierSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->SetSkillObj(0, 2.f, "animations/Effect/SoldierSkill.csv", "SkillEffect");
		champ->UseSkill();
		return;
	}

	if ((champ->GetCurretState().animaition.GetCurrFrame() < 4 && champ->GetTarget()->GetHp() == 0))
	{
		//std::cout << "이미 죽음" << std::endl;
		champ->SkillChangeIdle();
		return;
	}

	if (champ->GetCurretState().animaition.GetCurrFrame() == 3)
	{
		if (!champ->GetAttackFrame())
		{
			ChampionEffect* bullet = champ->GetEffectPool();
			sf::IntRect setting = { 0,0,5,3 };
			bullet->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/bullet.png"));
			bullet->SetOrigin(Origins::MC);
			bullet->sprite.setTextureRect(setting);
			bullet->SetChampion(champ);
			bullet->SetEffectType(5);
			bullet->sortLayer = 2;
			bullet->SetActive(true);
			bullet->SetPosition(champ->GetPosition());
			champ->SetAttackFrame(true);
			champ->SetAMoveT(0.f);
			SCENE_MGR.GetCurrScene()->AddGo(bullet);
		}
	}
	if (champ->GetCurretState().animaition.GetCurrFrame() == 4)
	{
		champ->SetAttackFrame(false);
		if (!champ->GetFrameLimit())
		{
			champ->DamageCalculate(champ->GetCurretState().attack);
			champ->SetFrameLimit(true);
		}
		return;
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() == 5)
	{
		if (!champ->GetAttackFrame())
		{
			ChampionEffect* bullet = champ->GetEffectPool();
			sf::IntRect setting = { 0,0,5,3 };
			bullet->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/bullet.png"));
			bullet->SetOrigin(Origins::MC);
			bullet->sprite.setTextureRect(setting);
			bullet->SetChampion(champ);
			bullet->SetEffectType(5);
			bullet->sortLayer = 2;
			bullet->SetActive(true);
			bullet->SetPosition(champ->GetPosition());
			champ->SetAttackFrame(true);
			champ->SetAMoveT(0.f);
			SCENE_MGR.GetCurrScene()->AddGo(bullet);
		}
		champ->SetFrameLimit(false);
		if (champ->GetTarget() == 0)
		{
			//std::cout << "이미 죽음" << std::endl;
			champ->SkillChangeIdle();
		}
		return;
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() == 6)
	{
		champ->SetAttackFrame(false);
		if (!champ->GetFrameLimit())
		{
			champ->DamageCalculate(champ->GetCurretState().attack);
			champ->SetFrameLimit(true);
		}
		return;
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() == 7)
	{
		if (!champ->GetAttackFrame())
		{
			ChampionEffect* bullet = champ->GetEffectPool();
			sf::IntRect setting = { 0,0,5,3 };
			bullet->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/bullet.png"));
			bullet->SetOrigin(Origins::MC);
			bullet->sprite.setTextureRect(setting);
			bullet->SetChampion(champ);
			bullet->SetEffectType(5);
			bullet->sortLayer = 2;
			bullet->SetActive(true);
			bullet->SetPosition(champ->GetPosition());
			champ->SetAttackFrame(true);
			champ->SetAMoveT(0.f);
			SCENE_MGR.GetCurrScene()->AddGo(bullet);
		}
		champ->SetFrameLimit(false);
		if (champ->GetTarget() == 0)
		{
			//std::cout << "이미 죽음" << std::endl;
			champ->SkillChangeIdle();
		}
		return;
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() == 8)
	{
		champ->SetAttackFrame(false);
		if (!champ->GetFrameLimit())
		{
			champ->DamageCalculate(champ->GetCurretState().attack);
			champ->SetFrameLimit(true);
		}
		return;
	}
	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		//std::cout << "스킬" << std::endl;
		champ->SetFrameLimit(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::SoldierUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		champ->UseUltiSkill();
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		std::cout << "fire in the holl" << std::endl;
		champ->TargetRangeDamage(200, 250);
		champ->SetUltiSkill(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::SwordmanSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "Skill")
	{
		champ->UseSkill();
		return;
	}

	if ((champ->GetCurretState().animaition.GetCurrFrame() < 3 && champ->GetTarget()->GetHp() == 0))
	{
		//std::cout << "이미 죽음" << std::endl;
		champ->SkillChangeIdle();
		return;
	}

	if (champ->GetCurretState().animaition.GetCurrFrame() == 3)
	{
		if (!champ->GetFrameLimit())
		{
			champ->DamageCalculate(champ->GetCurretState().attack/3.f);
			champ->SetFrameLimit(true);
		}
		return;
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() == 4)
	{
		champ->SetFrameLimit(false);
		if (champ->GetTarget() == 0)
		{
			//std::cout << "이미 죽음" << std::endl;
			champ->SkillChangeIdle();
		}
		return;
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() == 5)
	{
		if (!champ->GetFrameLimit())
		{
			champ->DamageCalculate(champ->GetCurretState().attack/3.f);
			champ->SetFrameLimit(true);
		}
		return;
	}
	else if (champ->GetCurretState().animaition.GetCurrFrame() == 6)
	{
		champ->SetFrameLimit(false);
		if (champ->GetTarget() == 0)
		{
			//std::cout << "이미 죽음" << std::endl;
			champ->SkillChangeIdle();
		}
		return;
	}

	if (champ->GetCurretState().animaition.GetLastFrame())
	{
		champ->DamageCalculate(champ->GetCurretState().attack / 3.f);
		BuffState* armorbreak = new BuffState;
		armorbreak->SetType(BuffType::DEFEND);
		armorbreak->SetCount(3);
		armorbreak->SetValue(-20);
		champ->GetTarget()->SetBuff(armorbreak);
		//std::cout << "방깍" << std::endl;
		champ->SetFrameLimit(false);
		champ->SkillChangeIdle();
		return;
	}
}
// 완료


void SkillMgr::SwordmanUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		std::cout << "스킬 : " << champ->GetCurretState().charId << std::endl;
		champ->UseUltiSkill();
		champ->SetStartPos(champ->GetPosition());
		sf::Vector2f temp = Utils::Normalize(champ->GetTarget()->GetPosition() - champ->GetPosition());
		champ->SetEndPos(champ->GetPosition() + temp * 300.f);
		champ->SetSkillObj(8, 1.f, temp, "animations/Effect/SwordmanUltiSkill.csv", "SkillEffect");
		return;
	}
}



void SkillMgr::BerserkerUltiSkill(Champion* champ)
{
	if (champ->GetCurretState().animaition.GetCurrentClipId() != "UltiSkill")
	{
		champ->UseUltiSkill();
		BuffState* ulti = new BuffState;
		ulti->SetType(BuffType::ULTIMATE);
		ulti->SetCount(10);
		ulti->SetValue(0);
		ulti->SetSide(true);
		ulti->SetSideType(BuffType::ATTACKRANGE);
		ulti->SetSideValue(1.2f);
		champ->SetBuff(ulti);
		champ->sprite.setScale({ 1.5f, 1.5f });
		champ->SetUltiSkill(false);
		champ->ChangeStance(ChampionStance::Idle);
		return;
	}
}
// 완료


void SkillMgr::ShieldbearerSkill(Champion* champ)
{
	if (champ->GetUseBuff(BuffType::ULTIMATE))
	{
		champ->TargetOrderCIT(3, 200, 0);
	}
	else
	{
		champ->TargetOrderCIT(3, 75, 0);
	}

	champ->TargetOrderSR();


	if (abs(Utils::Distance(champ->GetPosition(), champ->GetTarget()->GetPosition()) > champ->GetCurretState().attackRange) ||
		(abs(Utils::Distance(champ->GetPosition(), champ->GetTarget()->GetPosition()) < champ->GetCurretState().attackRange * 0.66f)))
	{
		champ->ChangeStance(ChampionStance::Move);
		return;
	}
	else if (abs(Utils::Distance(champ->GetPosition(), champ->GetTarget()->GetPosition())<= champ->GetCurretState().attackRange) ||
		(abs(Utils::Distance(champ->GetPosition(), champ->GetTarget()->GetPosition())>= champ->GetCurretState().attackRange*0.66f)))
	{
		champ->ChangeStance(ChampionStance::Action);
		return;
	}
}
// 완료


