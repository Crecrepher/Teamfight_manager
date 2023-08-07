#pragma once
#include "Scene.h"
#include "SpriteGo.h"


class SceneChampEdit : public Scene
{
protected:
public:
	SceneChampEdit();
	virtual ~SceneChampEdit() override;

	virtual void Init() override;
	virtual void Release() override;

	virtual void Enter() override;
	virtual void Exit() override;

	virtual void Update(float dt) override;
	virtual void Draw(sf::RenderWindow& window) override;
};

