#pragma once
#include "GameObject.h"
#include <SFML/Audio.hpp>

class SoundGo : public GameObject
{
public:
	sf::Sound sound;
	std::string soundBufferId;
	SoundGo(const std::string id = "", const std::string n = "");
	virtual ~SoundGo() override;

	virtual void Init()override;
	virtual void Reset()override;
	virtual void Release() override;
	virtual void Update(float dt)override;
	virtual void Draw(sf::RenderWindow& window)override;

	virtual void Play();

};

