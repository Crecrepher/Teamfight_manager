#pragma once
#include "Scene.h"
#include "TeamMgr.h"

class SceneHome : public Scene
{
public:
	enum class MainMenuType
	{
		NONE = -1,
		TEAM_MANAGE,
		OPERATE,
		LEAGUE,
		GAME,
		SYSTEM,
	};
protected:
	bool cheatMode = true;

	bool backClick = true;
public:
	SceneHome();
	virtual ~SceneHome() override;

	virtual void Init() override;
	virtual void Release() override;

	virtual void Enter() override;
	virtual void Exit() override;

	virtual void Update(float dt) override;
	virtual void Draw(sf::RenderWindow& window) override;

	void AddGoSprites();
	void AddGoUiButton();
	void AddGoText();

	void MakeMainUi();
	void MakeSubUi();
	void MakeSubUiTraining();

	void ReturnMainUiIndex(int& startNum, int& endNum, MainMenuType Type);
	void MainUiOpen(MainMenuType Type);
	void MainUiClose();
	void MainUiFunc(int index);

	void UpdateMoney();
	void Recruit(int grade,int slotNum);
	void MakeLocalPlayer(PlayerInfo& player);

	void TestingCheats();
};

