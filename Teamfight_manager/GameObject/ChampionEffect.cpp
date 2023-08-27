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
	case EffectType::Hp:
	{

		this->sortOrder = this->GetPosition().y;
		HpUpdate(dt);
		break;
	}
	case EffectType::CoolTime:
	{
		this->sortOrder = this->GetPosition().y;
		CoolTimeUpdate(dt);
		break;
	}
	case EffectType::Object:
	{
		this->sortOrder = this->GetPosition().y;
		ObjectUpdate(dt/1.5f);
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
	this->sprite.setRotation(0);
	this->SetActive(false);
	SCENE_MGR.GetCurrScene()->RemoveGo(this);
	pool->Return(this);
}

void ChampionEffect::SetChampion(Champion* champion)
{
	this->champ = champion;
}

void ChampionEffect::BaseUpdate(float dt)
{
	if (this->champ->GetCurrentStance() == ChampionStance::Dead)
	{
		Setting();
	}
	if (this->champ != nullptr)
	{
		if (this->champ->GetCurretState().charId == "berserker" && this->champ->GetUseBuff(BuffType::ULTIMATE))
		{
			this->SetPosition(this->champ->GetPosition().x + this->plusWidth, this->champ->GetPosition().y + this->plusHight + 5.f);
		}
		else
		{
			this->SetPosition(this->champ->GetPosition().x + this->plusWidth, this->champ->GetPosition().y + this->plusHight);
		}
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

void ChampionEffect::ObjectUpdate(float dt)
{
	float a = Utils::Angle(Utils::Normalize(this->champ->GetTarget()->GetPosition() - this->champ->GetPosition()));
	this->sprite.setRotation(a);
	
	float t = this->champ->GetAMoveT() / 0.1f;

	if (t >= 1.f)
	{
		t = 1.f;
	}

	sf::Vector2f temp = this->champ->GetPosition() + t * (this->champ->GetTarget()->GetPosition() - this->champ->GetPosition());

	this->SetPosition(temp);

	if (this->champ->GetCurrentStance() == ChampionStance::Dead)
	{
		Setting();
	}
	if (t==1)
	{
		Setting();
	}
}
