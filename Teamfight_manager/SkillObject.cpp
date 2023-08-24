#include "stdafx.h"
#include "SkillObject.h"
#include "SceneMgr.h"
#include "Champion.h"
#include "BuffState.h"

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

void SkillObject::SetChampion(Champion* champion)
{
	this->champ = champion;
}
