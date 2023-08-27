#pragma once
#include "Scene.h"
#include "ObjectPool.h"
#include "Champion.h"
#include "ChampionEffect.h"
#include "GameState.h"
#include "AnimatioControler.h"
#include "TextGo.h"
#include "TeamMgr.h"

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
	float aiBanTimer = 0.5f;
	float aiPickTimer = 0.5f;

	bool banPickCheck = true;

	float textSpeed = 1000.f;
	float textDuration = 1.2f;

	int redScore;
	int blueScore;

	int fullStep; // 밴, 픽 총횟수
	int step; // 진행중인 밴, 픽
	float speedUp = 1.f;

	ObjectPool<Champion> championPool;
	ObjectPool<ChampionEffect> effectPool;
	
	std::vector<Champion*> redTeam;
	std::vector<Champion*> blueTeam;
	std::vector<Champion*> cemetery;
	std::vector<int> banChamps;

	std::vector<SpriteGo*> banEffectSheet;
	std::vector<AnimatioControler> banEffectAnimation;
	int effectCount = 6;
	int effectAniCount = 6;

	AiBanPick aiBanPick;

	SpriteGo* banSheet;
	SpriteGo* banSheetBlueTeam; // 블루
	SpriteGo* banSheetRedTeam; // 레드

	AnimatioControler banAnimation;
	AnimatioControler banAnimation2;
	AnimatioControler banAnimation3;

	SpriteGo* Waterfall;
	SpriteGo* Waterfall2;

	AnimatioControler waterfallAnimation;
	AnimatioControler waterfallAnimation2;

	SpriteGo* stadiumSkyBg;

	SpriteGo* turnArrowBlue;
	SpriteGo* banBlueLine;
	AnimatioControler turnArrowBlueAnimation;

	SpriteGo* turnArrowRed;
	SpriteGo* banRedLine;
	AnimatioControler turnArrowRedAnimation;

	// csv하나로 됨, sheet.png도 하나로 됨
	// 

	std::vector<UiButton*> championSlot;
	int champCount = 14; // 챔피언 최대수를 결정
	std::vector<UiButton*> swapSlot;
	int swapChampCount = 3; // 3~5 팀원수 결정
	std::vector<UiButton*> pickSlot;
	std::vector<UiButton*> enemySlot;

	std::vector<SpriteGo*> pickDoingSlot;
	std::vector<SpriteGo*> enemyPickDoingSlot;

	std::vector<TextGo*> championSlotName;
	std::vector<TextGo*> banPickText;
	int banPickTextCount = 15; // 일단 최대값기준
	int banPickTextClickCount = 0;
	int pickDoingSlotClickCount = 0;

	int pickSlotCount = 3; // 우리 팀원의 수
	int pickSlotEnemyCount = 3; // 적 팀원의 수

	int pickDoingSlotCount = 0;
	int enemyPickDoingSlotCount = 0;

	int banCount = 0;
	int pickCount = 0;
	int pickEnemyCount = 0;

	int swapChampCountCheck = 3;
	UiButton* selectedButton = nullptr;
	sf::Vector2f firstPosition = { 0, 0 };
	bool selectCheck = true;

	// 스왑단계 클릭 중복
	bool isSwapCheck1 = false;
	bool isSwapCheck2 = true;
	bool isSwapComplete = true;

	bool canClick = true;

	sf::Vector2f BanSlotPosition1 = { 363, 661 };
	sf::Vector2f BanSlotPosition2 = { 919, 661 };

	TextGo* banText;
	TextGo* pickText;
	RectGo* grayScreen;

	std::vector<PlayerInfo> playerInfo;
	AiTeam enemyInfo;
	bool isPlayerEnough = true;
	bool isClickBlocker = false;
	int swapIndex = -1;
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
	void UiEnter();
	void LineUpInit();
	void LineUpTrue();
	void LineUpFalse();
	void SetLineUp();

	void BanPickInit();
	void BanPickTrue(bool on = true);
	void BanPickToBattleFalse();
	void BanPickPlayersOn(int index, bool on = true);
	void SetBanPick();
	
	void ChampSlotFalse();

	void AiSelect();

	void SwapSlotFalse();
	void GetPlayers();


	// 함수를 두개 만들고
	// 2:2, 3:3일때 스트링 테이블을 위쪽에서 가져오고
	// 4:4 일때는 아래쪽으로
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
