#pragma once
#include "Scene.h"
#include "ObjectPool.h"
#include "Champion.h"
#include "GameState.h"
#include "AnimatioControler.h"

class SceneGame : public Scene
{
protected:
	Phase currentPhase=Phase::None;
	Mode mode=Mode::Duo;
	Team team;
	Turn currentTurn;

	float battleTimer;
	float readyTimer;

	int fullStep; // ¹ê, ÇÈ ÃÑÈ½¼ö
	int step; // ÁøÇàÁßÀÎ ¹ê, ÇÈ

	ObjectPool<Champion> championPool;
	
	std::vector<Champion*> redTeam;
	std::vector<Champion*> blueTeam;
	std::vector<Champion*> cemetery;

	SpriteGo banSheet;
	AnimatioControler banAnimation;

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

	void LeaguePhase(float dt);
	void BanPhase(float dt);
	void PickPhase(float dt);
	void ReadyPhase(float dt);
	void BattlePhase(float dt);

	template <typename T>
	void ClearObjectPool(ObjectPool<T>& pool);

	void UiInit();
	void ButtonInit();
	void lineUpFalse();
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
