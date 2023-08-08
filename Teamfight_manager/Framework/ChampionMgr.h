#pragma once
#include "Singleton.h"
#include "GameState.h"
class ChampionMgr : public Singleton<ChampionMgr>
{
	friend Singleton<ChampionMgr>;
private:
	ChampionMgr() = default;
	virtual ~ChampionMgr() =default;

	std::unordered_map<std::string, State> champions;

public:	
	void Init();
	void LoadFromCsv(const std::string path);

	State GetChampion(const std::string& id);
};

#define CHAMPION_MGR (ChampionMgr::Instance())