#include "stdafx.h"
#include "BindPlayerEffect.h"
#include "SceneMgr.h"
#include "Utils.h"
#include "ResourceMgr.h"
#include "rapidcsv.h"
#include <filesystem>
#include "Champion.h"

BindPlayerEffect::BindPlayerEffect(const std::string id, const std::string n)
	:SpriteGo(id, n)
{

}

BindPlayerEffect::~BindPlayerEffect()
{
}


bool BindPlayerEffect::ClipChecker()
{
	if (curAnimation != 0)
	{
		switch (curAnimation)
		{
		case 1:
			if (!isClipWorks1)
			{
				return false;
			}
			break;
		case 2:
			if (!isClipWorks2)
			{
				return false;
			}
			break;
		case 3:
			if (!isClipWorks3)
			{
				return false;
			}
			break;
		default:
			return false;
			break;
		}
		return true;
	}
	return false;
}

void BindPlayerEffect::Play(int code)
{
	if (champCode == 13)
	{
		SetActive(true);
		if (code == 3)
		{
			curAnimation = code;
			skillAni.Play("BP3");
		}
		else if (code == 2 && (!skillAni.IsPlaying() || skillAni.GetCurrFrame()== 43))
		{
			curAnimation = code;
			skillAni.Play("BP2");
		}
		return;
	}
	curAnimation = code;
	if (ClipChecker())
	{
		SetActive(true);
		skillAni.Play("BP" + std::to_string(curAnimation));
	}
}

void BindPlayerEffect::Stop()
{
	if (ClipChecker())
	{
		SetActive(true);
		skillAni.Stop();
	}
}

void BindPlayerEffect::Hide()
{
	SetActive(false);
}

void BindPlayerEffect::SetPlayer(int champCode, Champion* player)
{
	if (champCode == 13)
	{
		sortLayer = 3;
		sortOrder = -1;
	}
	else
	{
		sortLayer = 3;
		sortOrder = 2000;
	}
	this->champCode = champCode;
	this->player = player;
	LoadEffect();
}

void BindPlayerEffect::Init()
{
	SpriteGo::Init();

}

void BindPlayerEffect::Reset()
{
	SpriteGo::Reset();
}

void BindPlayerEffect::Release()
{
	SpriteGo::Release();
}

void BindPlayerEffect::Update(float dt)
{
	SpriteGo::Update(dt);

	if (ClipChecker())
	{
		sf::Vector2f scale = player->sprite.getScale();
		SetPosition(player->GetPosition() 
			+ sf::Vector2f{ dist[curAnimation][skillAni.GetCurrFrame()].x * scale.x,
				 dist[curAnimation][skillAni.GetCurrFrame()].y * scale.y });
		sprite.setScale(scale);
		skillAni.Update(dt);
		if (champCode != 13)
		{
			sortOrder = player->sortOrder+1;
		}
		
	}
}

void BindPlayerEffect::LoadEffect()
{
	dist = std::vector<std::vector<sf::Vector2f>>(4);
	for (int i = 0; i < 4; i++)
	{
		dist[i] = std::vector<sf::Vector2f>(100);
	}
	skillAni = AnimatioControler();
	for (int i = 1; i < 4; i++)
	{
		std::string str = "animations/BindPlayerEffect/" + std::to_string(champCode) + std::to_string(i) + ".csv";
		bool isExists = std::filesystem::exists(str);
		switch (i)
		{
		case 1:
			isClipWorks1 = isExists;
			break;
		case 2:
			isClipWorks2 = isExists;
			break;
		case 3:
			isClipWorks3 = isExists;
			break;
		default:
			break;
		}
		if (!isExists)
		{
			continue;
		}

		rapidcsv::Document doc(str, rapidcsv::LabelParams(-1, -1));

		if (doc.GetRowCount() < 1)
		{
			continue;
		}
		int count = 0;
		for (int j = 4; j < doc.GetRowCount(); ++j)
		{
			auto rows = doc.GetRow<std::string>(j);
			dist[i][count] = { (float)std::stoi(rows[5]),(float)std::stoi(rows[6]) };
			count++;
		}
		RESOURCE_MGR.UnLoad(ResourceTypes::AnimationClip, str);
		RESOURCE_MGR.Load(ResourceTypes::AnimationClip, str);

		skillAni.AddClip(*RESOURCE_MGR.GetAnimationClip(str));
	}
	std::stringstream ss;
	ss << "graphics/BindPlayerEffect/" << champCode << ".png";
	RESOURCE_MGR.Load(ResourceTypes::Texture, ss.str());

	skillAni.SetTarget(&sprite);
}