#pragma once
#include "Scene.h"
#include "ObjectPool.h"
#include "Champion.h"
#include "ChampionEffect.h"
#include "GameState.h"
#include "AnimatioControler.h"
#include "TextGo.h"

#include "AiBanPick.h"
#include "vector"

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
	float aiBanTimer = 5.f;
	float textSpeed = 1000.f;
	float textDuration = 1.2f;

	int redScore;
	int blueScore;

	int fullStep; // ��, �� ��Ƚ��
	int step; // �������� ��, ��
	float speedUp = 1.f;

	ObjectPool<Champion> championPool;
	ObjectPool<ChampionEffect> effectPool;
	
	std::vector<Champion*> redTeam;
	std::vector<Champion*> blueTeam;
	std::vector<Champion*> cemetery;
	std::vector<int> banChamps;

	SpriteGo* banSheet;
	SpriteGo* banSheetBlueTeam; // ���
	SpriteGo* banSheetRedTeam; // ����

	AiBanPick aiBanPick;

	AnimatioControler banAnimation;
	AnimatioControler banAnimation2;
	AnimatioControler banAnimation3;

	std::vector<UiButton*> championSlot;
	int champCount = 14; // è�Ǿ� �ִ���� ����
	std::vector<UiButton*> swapSlot;
	int swapChampCount = 5; // 3~5 ������ ����
	std::vector<UiButton*> pickSlot;

	int pickSlotCount = 3; // �츮 ������ ��
	int pickSlotEnemyCount = 3; // �� ������ ��

	int pickCount = 0;
	int pickEnemyCount = 0;

	int swapChampCountCheck = 3;
	UiButton* selectedButton = nullptr;
	sf::Vector2f firstPosition = { 0, 0 };
	bool selectCheck = true;

	// ���Ҵܰ� Ŭ�� �ߺ�
	bool isSwapCheck1 = false;
	bool isSwapCheck2 = true;
	bool isSwapComplete = true;

	bool canClick = true;

	sf::Vector2f BanSlotPosition1 = { 363, 661 };
	sf::Vector2f BanSlotPosition2 = { 919, 661 };

	TextGo* banText;
	TextGo* pickText;
	RectGo* grayScreen;

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
