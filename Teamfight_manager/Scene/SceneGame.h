#pragma once
#include "Scene.h"
#include "ObjectPool.h"
#include "Champion.h"
#include "GameState.h"

class SceneGame : public Scene
{
protected:
	Phase currentPhase=Phase::None;
	Mode mode=Mode::Duo;
	Team team;
	Turn currentTurn;
	std::string pick = "";

	float battleTimer;
	float readyTimer;

	int redScore;
	int blueScore;

	int fullStep; // ¹ê, ÇÈ ÃÑÈ½¼ö
	int step; // ÁøÇàÁßÀÎ ¹ê, ÇÈ

	ObjectPool<Champion> championPool;
	
	std::vector<Champion*> redTeam;
	std::vector<Champion*> blueTeam;
	std::vector<Champion*> cemetery;

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
	void ChampionPick(std::string id, Team team);

	void BanPhase(float dt);
	void PickPhase(float dt);
	void ReadyPhase(float dt);
	void BattlePhase(float dt);
	void ResultPhase(float dt);

	template <typename T>
	void ClearObjectPool(ObjectPool<T>& pool);
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
