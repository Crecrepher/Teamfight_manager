#pragma once
#include "SpriteGo.h"
#include "ObjectPool.h"
#include "AnimatioControler.h"

class Champion;

class BindPlayerEffect : public SpriteGo
{
protected:
	AnimatioControler skillAni;
	int curAnimation = 0;
	int champCode = 0;

	bool isClipWorks1 = false;
	bool isClipWorks2 = false;
	bool isClipWorks3 = false;

	Champion* player;
	std::vector<std::vector<sf::Vector2f>> dist;
public:
	BindPlayerEffect(const std::string id = "", const std::string n = "");
	virtual ~BindPlayerEffect();

	bool ClipChecker();
	void Play(int code);
	void Stop();
	void Hide();
	void SetPlayer(int champCode, Champion* player);
	void LoadEffect();

	virtual void Init()override;
	virtual void Reset()override;
	virtual void Release() override;
	virtual void Update(float dt)override;


};

