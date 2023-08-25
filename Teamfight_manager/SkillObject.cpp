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

	SCENE_MGR.GetCurrScene()->RemoveGo(this);
	pool->Return(this);
}

void SkillObject::Update(float dt)
{
	this->effectTimer -= dt;
	this->objectTimer -= dt;

	switch (type)
	{
	case SkillType::None:
	{
		this->SetActive(false);
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
	}
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
	if (this->objectTimer < 0.f)
	{
		return;
	}

	if (this->effectTimer < 0.f)
	{
		this->effectTimer = this->effectTime;
		this->champ->TargetOrderCIE(1, 175, champ->GetCurretState().attack, this->GetPosition());
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

