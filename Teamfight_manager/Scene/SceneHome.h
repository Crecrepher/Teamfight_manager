#pragma once
#include "Scene.h"

class SceneHome : public Scene
{
protected:
public:
	SceneHome();
	virtual ~SceneHome() override;

	virtual void Init() override;
	virtual void Release() override;

	virtual void Enter() override;
	virtual void Exit() override;

	virtual void Update(float dt) override;
	virtual void Draw(sf::RenderWindow& window) override;
};

