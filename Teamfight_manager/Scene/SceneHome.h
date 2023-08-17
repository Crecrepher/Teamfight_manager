#pragma once
#include "Scene.h"
#include "TeamMgr.h"

class TextGo;

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
	bool isPopupOn = false;

	std::vector<Sponsor> newSponsor;
	int selectedSponsorIndex = -1;
	std::vector<TrainingInfo> gainTrainingInfo;
	int maxTrainingPoint = 3;

	std::vector<int> equipCraftParts;
	int craftCost = 0;
	int craftTime = 0;
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
	void MakeSubUiSponsorContract();
	void MakeSubUiEquip();

	void ReturnMainUiIndex(int& startNum, int& endNum, MainMenuType Type);
	void MainUiOpen(MainMenuType Type);
	void MainUiClose();
	void MainUiFunc(int index);

	void SubUiBaseOpen(int index,bool on = true);

	void UiTrainingOpen(bool on = true);
	void UiTrainingPlayerSelect(int index);
	void UiTrainingGaugeUpdate(int index);

	void UiSponsorContractOpen(bool contract = false ,bool on = true);
	void UiSponsorContractSelect(Sponsor sponsor, int index);
	void SponsorContract(int index);
	void UiSponsorSelect(int index);

	void UiEquipOpen(bool on = true);
	void UiEquipChangeOpen(int type,bool on = true);
	void UiEquipMakeOpen(int index, bool on = true);
	void UiCraftFinish(bool on = true);
	void UpdateCraftVal();
	std::wstring ReturnEquipStats(int itemType, int itemNum);
	void ReturnItemName(TextGo& text,int type, int num);

	void UpdateMoney();
	void Recruit(int grade,int slotNum);
	void NewYear();
	PlayerInfo MakeDefaultPlayer();
	PlayerInfo MakeLocalPlayer();
	
	bool isPartsEmpty();
	
	void TestingCheats();
};

