#pragma once
#include "Scene.h"
#include "TeamMgr.h"
 // 임시 추가

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
	bool isMenuOn = false;

	std::vector<TrainingInfo> gainTrainingInfo;
	int maxTrainingPoint = 3;
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

	void UiTrainingOpen(bool on = true);
	void UiTrainingPlayerSelect(int index);
	void UiTrainingGaugeUpdate(int index);

	void UpdateMoney();
	void Recruit(int grade,int slotNum);
	void NewYear();
	void MakeLocalPlayer(PlayerInfo& player);
	

	void TestingCheats();
};

