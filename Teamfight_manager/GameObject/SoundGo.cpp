#include "stdafx.h"
#include "SoundGo.h"
#include "ResourceMgr.h"
SoundGo::SoundGo(const std::string id, const std::string n)
	:GameObject(n), soundBufferId(id)
{
}

SoundGo::~SoundGo()
{
}

void SoundGo::Init()
{

}

void SoundGo::Reset()
{
	sf::SoundBuffer* sound = RESOURCE_MGR.GetSoundBuffer(soundBufferId);
	if (sound != nullptr)
	{
		this->sound.setBuffer(*sound);
	}
}

void SoundGo::Release()
{
}

void SoundGo::Update(float dt)
{
	
}

void SoundGo::Draw(sf::RenderWindow& window)
{
}

void SoundGo::Play()
{
	sound.play();
}
