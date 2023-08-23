#pragma once
#include "Scene.h"
#include "SpriteGo.h"
#include "UiButton.h"
#include "TextGo.h"
#include "AnimatioControler.h"

class Player2;

class SceneTitile : public Scene
{
protected:

	std::vector<TextGo*> textSlot;
	int titleTextCount = 4;

	std::vector<UiButton*> menuSlot;
	int menuSlotCount = 4;

	bool slotCheck = true;

	SpriteGo* indicatorRight;
	SpriteGo* indicatorLeft;

	UiButton* firstButton = nullptr;

	AnimatioControler indicatorLeftAnimation;
	AnimatioControler indicatorRightAnimation;
public:
	SceneTitile();
	virtual ~SceneTitile() override;

	virtual void Init() override;
	virtual void Release() override;

	virtual void Enter() override;
	virtual void Exit() override;

	void TitleUiInit();

	virtual void Update(float dt) override;
	virtual void Draw(sf::RenderWindow& window) override;
};

