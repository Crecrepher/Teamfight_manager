#include "stdafx.h"
#include "SkillObject.h"
#include "SceneMgr.h"
#include "Champion.h"
#include "BuffState.h"
#include "ResourceMgr.h"

SkillObject::SkillObject(const std::string id, const std::string n)
{
}

SkillObject::~SkillObject()
{
}

void SkillObject::Init()
{
	SpriteGo::Init();
}

void SkillObject::Reset()
{
	SpriteGo::Reset();
	this->champ = nullptr;
	this->type = SkillType::None;
	this->effectTime = 0.f;
	this->effectTimer = 0.f;
	this->objectTimer = 0.f;
	this->dir = { 0,0 };
	this->check.clear();
	this->SetActive(false);
}

void SkillObject::Release()
{
	SpriteGo::Release();

	this->sprite.setTexture(sf::Texture());
	this->champ = nullptr;
	this->type = SkillType::None;
	this->effectTime = 0.f;
	this->effectTimer = 0.f;
	this->objectTimer = 0.f;
	this->dir = { 0,0 };
	this->check.clear();
	this->SetActive(false);

	SCENE_MGR.GetCurrScene()->RemoveGo(this);
	pool->Return(this);
}

void SkillObject::Update(float dt)
{
	this->sortOrder = this->GetPosition().y;
	this->effectTimer -= dt;
	this->objectTimer -= dt;

	switch (type)
	{
	case SkillType::None:
	{
		this->SetActive(false);
		break;
	}
	case SkillType::Public:
	{
		PublicUpdate(dt);
		break;
	}
	case SkillType::Icemage:
	{
		IcemageUpdate(dt);
		break;
	}
	case SkillType::Pyromancer:
	{
		PyromancerUpdate(dt);
		break;
	}
	case SkillType::Magicknight:
	{
		MagicknightUpdate(dt);
		break;
	}
	case SkillType::Priest:
	{
		PriestUpdate(dt);
		break;
	}
	case SkillType::PyromancerPSkill:
	{
		PyromancerSkillUpdate(dt);
		break;
	}
	case SkillType::Pythoness:
	{
		PythonessUpdate(dt);
		break;
	}
	case SkillType::MagicknightSkill:
	{
		MagicknightSkillUpdate(dt);
		break;
	}
	case SkillType::SwordUltiSkill:
	{
		SwordUltiSkillUpdate(dt);
		break;
	}
	case SkillType::Team:
	{
		TeamUpdate(dt);
		break;
	}
	}
}

void SkillObject::PublicUpdate(float dt)
{
	effect.Update(dt);
	if (this->champ->GetTarget() == nullptr)
	{
		this->objectTimer = 0.f;
		return;
	}
	this->SetOrigin(Origins::MC);
	this->SetPosition(champ->GetTarget()->GetPosition());
}

void SkillObject::TeamUpdate(float dt)
{
	effect.Update(dt);
	if (this->champ == nullptr)
	{
		this->objectTimer = 0.f;
		return;
	}
	this->SetOrigin(Origins::MC);
	this->SetPosition(champ->GetPosition());
}

void SkillObject::IcemageUpdate(float dt)
{

	if (this->objectTimer < 0.f)
	{
		BuffState* buff = new BuffState;
		buff->SetType(BuffType::STUN);
		buff->SetCount(2);
		this->champ->TargetOrderCIEB(1, 175, buff, this->GetPosition());
		this->champ->TargetOrderCIE(1, 175, champ->GetCurretState().attack,this->GetPosition());
		return;
	}

	if (this->effectTimer<0.f)
	{
		this->effectTimer = this->effectTime;
		BuffState* buff = new BuffState;
		buff->SetType(BuffType::SPEED);
		buff->SetValue(-0.2f);
		buff->SetCount(1);
		this->champ->TargetOrderCIEB(1, 175, buff, this->GetPosition());
		return;
	}
}

void SkillObject::MagicknightUpdate(float dt)
{
	if (this->objectTimer < 0.f)
	{
		return;
	}

	if (this->effectTimer < 0.f)
	{
		this->effectTimer = this->effectTime;
		this->champ->TargetOrderCIE(2, 175, 1, this->GetPosition());
		this->champ->TargetOrderCIE(1, 175, champ->GetCurretState().attack, this->GetPosition());
		return;
	}
}

void SkillObject::PriestUpdate(float dt)
{

	if (this->objectTimer < 0.f)
	{
		return;
	}

	if (this->effectTimer < 0.f)
	{
		this->effectTimer = this->effectTime;
		this->champ->TargetOrderCIT(1, 175, 20, this->GetPosition());
		return;
	}
}

void SkillObject::PyromancerUpdate(float dt)
{
	effect.Update(dt);

	if (this->objectTimer < 0.f)
	{
		return;
	}

	if (this->effectTimer < 0.f&&!this->torgle)
	{
		this->torgle = true;
		this->SetAni("animations/Effect/PyromancerSprit_2.csv");
		this->PlayAni("SkillEffect");
		this->effectTimer = this->effectTime;
		this->champ->TargetOrderCIE(1, 175, champ->GetCurretState().attack, this->GetPosition());
		return;
	}

	if (this->torgle && this->effect.GetLastFrame())
	{
		this->torgle = false;
		this->SetAni("animations/Effect/PyromancerSprit.csv");
		this->PlayAni("SKillEffect");
		return;
	}
}

void SkillObject::PyromancerSkillUpdate(float dt)
{
	if (this->objectTimer < 0.f)
	{
		return;
	}

	if (this->effectTimer < 0.f)
	{
		this->effectTimer = this->effectTime;
		this->champ->TargetOrderCIE(3, 60, champ->GetCurretState().attack*0.8f, this->GetPosition());
		return;
	}
}

void SkillObject::PythonessUpdate(float dt)
{
	if (this->objectTimer < 0.f)
	{
		return;
	}

	if (this->effectTimer < 0.f)
	{
		int i = Utils::RandomRange(0, 1);
		this->effectTimer = this->effectTime;
		if (i == 1)
		{
			this->champ->TargetOrderCIE(3, 900, champ->GetCurretState().attack, this->GetPosition());
		}
		else if (i == 0)
		{
			this->champ->TargetOrderCIT(4, 900, champ->GetCurretState().attack, this->GetPosition());
		}

		return;
	}
}

void SkillObject::MagicknightSkillUpdate(float dt)
{
	effect.Update(dt/3.f);
	this->SetOrigin(Origins::ML);
	this->sprite.setRotation(Utils::Angle(this->dir));
	this->sprite.setScale(1.f, 1.f * this->scaleY);

	if (this->objectTimer > 0.5f)
	{
		if (this->effect.GetCurrFrame() == 2)
		{
			this->effect.SetCurrFrame(0);
		}
	}
	else if (this->objectTimer < 0.f)
	{
		return;
	}

	SetPosition(this->GetPosition() + dir * 100.f * dt);

	if (this->effectTimer < 0.f)
	{
		this->effectTimer = this->effectTime;

		for (auto enemy : this->champ->GetEnemyTeam())
		{
			this->champ->TargetOrderCIE(3, 30, champ->GetCurretState().attack*1.5f, this->GetPosition(), &this->check);
		}
	}
	return;
}

void SkillObject::SwordUltiSkillUpdate(float dt)
{
	effect.Update(dt);
	this->SetOrigin(Origins::ML);
	this->sprite.setRotation(Utils::Angle(this->dir));
	this->sprite.setScale(1.f, 1.f * this->scaleY);

	if (this->objectTimer < 0.f)
	{
		for (auto enemy : this->champ->GetEnemyTeam())
		{
			if (this->check.empty())
			{
				if (SwordmanDamageRange(enemy->GetPosition(), this->champ->GetStartPos(), this->champ->GetEndPos()) <= 30)
				{
					check.push_back(enemy);
					this->champ->SetTarget(enemy);
					this->champ->DamageCalculate(300);
					std::cout << this->champ->GetCurretState().charId << " °¡ " << this->champ->GetTarget()->GetCurretState().charId << " ¿¡°Ô µô" << std::endl;
					continue;
				}
			}
			else if(!this->check.empty())
			{
				for (auto c : check)
				{
					if (c == enemy)
					{
						continue;
					}
					else if (SwordmanDamageRange(enemy->GetPosition(), this->champ->GetStartPos(), this->champ->GetEndPos()) <= 30)
					{
						check.push_back(enemy);
						this->champ->SetTarget(enemy);
						this->champ->DamageCalculate(300);
						std::cout << this->champ->GetCurretState().charId << " °¡ " << this->champ->GetTarget()->GetCurretState().charId << " ¿¡°Ô µô" << std::endl;
						break;
					}
				}
			}
		}
		this->champ->SetPosition(champ->GetEndPos());
		this->champ->SetUltiSkill(false);
		this->champ->SkillChangeIdle();
		return;
	}
}

void SkillObject::SetChampion(Champion* champion)
{
	this->champ = champion;
}

void SkillObject::SetAni(std::string path)
{
	this->effect.AddClip(*RESOURCE_MGR.GetAnimationClip(path));
	this->effect.SetTarget(&this->sprite);
	this->SetOrigin(Origins::BL);
}

float SkillObject::SwordmanDamageRange(sf::Vector2f enemy, sf::Vector2f start, sf::Vector2f end)
{
	sf::Vector2f targetLine = end - start;
	float targetLength = Utils::Magnitude(targetLine);
	
	targetLine /= targetLength;

	sf::Vector2f enemyPos = enemy - start;
	float point = enemyPos.x * targetLine.x + enemyPos.y * targetLine.y;

	sf::Vector2f posDis;

	if (point <= 0)
	{
		posDis = start;
	}
	else if (point >= targetLength)
	{
		posDis = end;
	}
	else
	{
		posDis = start + targetLine * point;
	}

	sf::Vector2f distance;
	distance.x = enemy.x - posDis.x;
	distance.y = enemy.y - posDis.y;

	return Utils::Magnitude(distance);
}

