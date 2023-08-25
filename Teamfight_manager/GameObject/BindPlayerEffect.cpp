#include "stdafx.h"
#include "BindPlayerEffect.h"
#include "SceneMgr.h"
#include "Utils.h"

BindPlayerEffect::BindPlayerEffect(const std::string id, const std::string n)
	:SpriteGo(id,n)
{
}


void BindPlayerEffect::Init()
{
	SpriteGo::Init();
	SetOrigin(Origins::MC);
}

void BindPlayerEffect::Reset()
{
	SpriteGo::Reset();
	timer = 0.f;
	sprite.setColor({ 255, 255, 255, 255 });
}

void BindPlayerEffect::Release()
{
	SpriteGo::Release();
}

void BindPlayerEffect::Update(float dt)
{
	SpriteGo::Update(dt);
	//timer += dt;
	//sf::Uint8 a = Utils::Lerp(255,0,(timer/duration));
	//sprite.setColor({ 255, 255, 255, a });
	////SetPosition(Utils::Lerp(position, { 0,0 }, (timer / duration), false));

	//if (timer > duration)
	//{
	//	if (pool != nullptr)
	//	{
	//		SCENE_MGR.GetCurrScene()->RemoveGo(this);
	//		pool->Return(this);
	//	}
	//	else
	//	{
	//		SetActive(false);
	//	}
	//}
}
