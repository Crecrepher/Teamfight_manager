#include "stdafx.h"
#include "ChampionEffect.h"
#include "ResourceMgr.h"
#include "Champion.h"

ChampionEffect::ChampionEffect(const std::string id, const std::string n)
{
	type = EffectType::None;
	champ = nullptr;
}

ChampionEffect::~ChampionEffect()
{
}

void ChampionEffect::Init()
{
	SpriteGo::Init();
	this->type = EffectType::None;
	this->champ = nullptr;
}

void ChampionEffect::Reset()
{
	SpriteGo::Reset();
}

void ChampionEffect::Release()
{
	SpriteGo::Release();
}

void ChampionEffect::Update(float dt)
{
	SpriteGo::Update(dt);

	switch (type)
	{
	case EffectType::Ui:
	{
		UiUpdate(dt);
		break;
	}
	case EffectType::Base:
	{
		BaseUpdate(dt);
		break;
	}
	case EffectType::MoveSkill:
	{
		break;
	}
	case EffectType::DoneMoveSkill:
	{
		break;
	}
	case EffectType::Hp:
	{
		HpUpdate(dt);
		break;
	}
	case EffectType::CoolTime:
	{
		CoolTimeUpdate(dt);
		break;
	}
	}
}

void ChampionEffect::Setting()
{
	this->type = EffectType::None;
	this->champ = nullptr;
	this->plusWidth = 0.f;
	this->plusHight = 0.f;
	this->sprite.setScale(1.f, 1.f);
	SCENE_MGR.GetCurrScene()->RemoveGo(this);
	pool->Return(this);
}

void ChampionEffect::SetChampion(Champion* champion)
{
	this->champ = champion;
}


void ChampionEffect::SetAttack(Champion* champ, float dt)
{
	if (champ->GetCurretState().charId == "archer")
	{
		if (champ->GetCurretState().animaition.GetCurrFrame() == 0&&!champ->GetFrameLimit())
		{
			ChampionEffect* attack = this->pool->Get();
			attack->sortLayer = 4;
			attack->effect.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/Champions/archer/Attack_Effect.csv"));
			attack->effect.SetTarget(&attack->sprite);
			if (champ->GetSacleX() > 0)
			{
				attack->sprite.setScale(1, 1);
				attack->SetPosition(champ->GetPosition());
			}
			else if (champ->GetSacleX() < 0)
			{
				attack->sprite.setScale(-1, 1);
				attack->SetPosition(champ->GetPosition());
			}
			champ->SetFrameLimit(true);
		}
		else if (champ->GetCurretState().animaition.GetLastFrame())
		{
			champ->SetFrameLimit(false);
			Setting();
		}
	}
	this->AnimationUpdate(dt);
}

void ChampionEffect::BaseUpdate(float dt)
{
	if (this->champ->GetCurrentStance() == ChampionStance::Dead)
	{
		Setting();
	}
	if (this->champ != nullptr)
	{
		this->SetPosition(this->champ->GetPosition().x + this->plusWidth, this->champ->GetPosition().y + this->plusHight);
	}
}

void ChampionEffect::UiUpdate(float dt)
{
	if (this->champ->GetCurrentStance() == ChampionStance::Dead)
	{
		Setting();
	}
	if (this->champ != nullptr)
	{
		this->SetPosition(this->champ->GetPosition().x + this->plusWidth, this->champ->GetPosition().y + this->plusHight);
	}
}

void ChampionEffect::HpUpdate(float dt)
{
	this->sprite.setScale(this->champ->GetHpPercent(), 1.f);
	this->SetOrigin(Origins::ML);
	if (this->champ->GetCurrentStance() == ChampionStance::Dead)
	{
		Setting();
	}
	if (this->champ != nullptr)
	{
		this->SetPosition(this->champ->GetPosition().x + this->plusWidth, this->champ->GetPosition().y + this->plusHight);
	}
}

void ChampionEffect::CoolTimeUpdate(float dt)
{
	this->sprite.setScale(this->champ->GetCoolTimePercent(), 1.f);
	this->SetOrigin(Origins::ML);
	if (this->champ->GetCurrentStance() == ChampionStance::Dead)
	{
		Setting();
	}
	if (this->champ != nullptr)
	{
		this->SetPosition(this->champ->GetPosition().x + this->plusWidth, this->champ->GetPosition().y + this->plusHight);
	}
}

void ChampionEffect::AnimationUpdate(float dt)
{
	this->effect.Update(dt);
}

