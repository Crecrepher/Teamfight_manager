#include "stdafx.h"
#include "SpriteEffect.h"
#include "SceneMgr.h"
#include "Utils.h"

SpriteEffect::SpriteEffect(const std::string id, const std::string n)
	:SpriteGo(id,n)
{
}


void SpriteEffect::Init()
{
	SpriteGo::Init();
	SetOrigin(Origins::MC);
}

void SpriteEffect::Reset()
{
	SpriteGo::Reset();
	timer = 0.f;
	sprite.setColor({ 255, 255, 255, 255 });
}

void SpriteEffect::Release()
{
	SpriteGo::Release();
}

void SpriteEffect::Update(float dt)
{
	SpriteGo::Update(dt);
	timer += dt;
	sf::Uint8 a = Utils::Lerp(255,0,(timer/duration));
	sprite.setColor({ 255, 255, 255, a });
	//SetPosition(Utils::Lerp(position, { 0,0 }, (timer / duration), false));

	if (timer > duration)
	{
		if (pool != nullptr)
		{
			SCENE_MGR.GetCurrScene()->RemoveGo(this);
			pool->Return(this);
		}
		else
		{
			SetActive(false);
		}
	}
}
