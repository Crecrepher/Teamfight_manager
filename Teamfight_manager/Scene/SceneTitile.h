#pragma once
#include "Scene.h"
#include "SpriteGo.h"

class Player2;

class SceneTitile : public Scene
{
protected:
public:
	SceneTitile();
	virtual ~SceneTitile() override;

	virtual void Init() override;
	virtual void Release() override;

	virtual void Enter() override;
	virtual void Exit() override;

	virtual void Update(float dt) override;
	virtual void Draw(sf::RenderWindow& window) override;
};

