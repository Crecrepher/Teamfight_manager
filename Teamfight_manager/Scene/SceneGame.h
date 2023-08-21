#pragma once
#include "Scene.h"
#include "ObjectPool.h"
#include "Champion.h"
#include "GameState.h"
#include "AnimatioControler.h"

class UiButton;

class SceneGame : public Scene
{
protected:
	Phase currentPhase=Phase::None;
	Mode mode=Mode::Trio;
	Team team;
	Turn currentTurn = Turn::Player;
	std::string pick = "";

	float battleTimer;
	float readyTimer;

	int redScore;
	int blueScore;

	int fullStep; // ��, �� ��Ƚ��
	int step; // �������� ��, ��

	ObjectPool<Champion> championPool;
	
	std::vector<Champion*> redTeam;
	std::vector<Champion*> blueTeam;
	std::vector<Champion*> cemetery;

	std::vector<int> banChamps;
	SpriteGo* banSheet;

	SpriteGo* banSheetBlueTeam; // ���
	SpriteGo* banSheetRedTeam; // ����

	AnimatioControler banAnimation;
	AnimatioControler banAnimation2;
	AnimatioControler banAnimation3;

	std::vector<UiButton*> championSlot;
	int champCount = 14; // è�Ǿ� �ִ���� ����
	std::vector<UiButton*> swapSlot;
	int swapChampCount = 4; // 3~5 ������ ����

	int swapChampCountCheck = 3;
	int selectedButtonIndex = -1;
	UiButton* selectedButton = nullptr;
	sf::Vector2f tempPosition = { 0, 0 };
	bool selectCheck = true;
	bool isSwapCheck = true;
	bool isSwapCheck2 = false;


	sf::Vector2f BanSlotPosition1 = { 363, 661 };
	sf::Vector2f BanSlotPosition2 = { 919, 661 };

public:
	SceneGame();
	virtual ~SceneGame() override;

	virtual void Init() override;
	virtual void Release() override;

	virtual void Enter() override;
	virtual void Exit() override;

	virtual void Update(float dt) override;
	virtual void Draw(sf::RenderWindow& window) override;

	void ChangePhase(Phase cPhase);
	Phase GetPhase() { return currentPhase; }
	void ChangeTurn();
	void ChangeTeam();
	void ChampionPick(int id, Team team);

	void LeaguePhase(float dt);
	void BanPhase(float dt);
	void PickPhase(float dt);
	void ReadyPhase(float dt);
	void BattlePhase(float dt);
	void ResultPhase(float dt);

	template <typename T>
	void ClearObjectPool(ObjectPool<T>& pool);

	void UiInit();

	void ButtonInit();
	void ButtonTrue(bool on = true);

	void LineUpTrue();
	void LineUpFalse();
	
	void BanPickFalse();
	
	void AiSelect();

	void SwapSlot();
	void SwapSlotFalse();
};

template<typename T>
inline void SceneGame::ClearObjectPool(ObjectPool<T>& pool)
{
	for (auto object : pool.GetUseList())
	{
		RemoveGo(object);
	}
	pool.Clear();
}
