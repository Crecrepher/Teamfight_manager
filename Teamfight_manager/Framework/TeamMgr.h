#pragma once
#include "Singleton.h"

class Scene;

struct PlayerInfo
{
	std::wstring name =L"";
	int age = 19;
	int condition = 2;

	int attack = 6;
	int defence = 6;
	int contract_cost = 96; //재계약비용
	int experience = 1;//활동시즌

	std::vector<std::pair<int, int>> proficiency; 
	//캐릭터 숙련도 <캐릭터 코드 / 숙련도>
	int knownChamp = 0; //숙련도 갯수

	std::vector<int> characteristic; 
	//특성 <특성코드>
	int knownCharacter = 0; //특성 갯수
	//------------------통계----------------//
	int kill = 0;
	int assist = 0;
	int totalKill = 0;
	int totalAssist = 0;
};

class TeamMgr : public Singleton<TeamMgr>
{
	friend Singleton<TeamMgr>;

protected:
	TeamMgr() = default;
	virtual	~TeamMgr() = default;

	int ableChamp = 9; //챔피언가능
	int ableCharacteristic = 36; //특성갯수
	int money = 200;

	std::vector<PlayerInfo> player;
	int playerNum = 0;
	int MaxPlayer = 6;

	std::vector<std::pair<bool,PlayerInfo>>recruiting_players;
	//영입선수<영입가능/선수>
	int recruit_able = 1;

	std::vector<bool> gear;
	//장비 [인덱스 = 장비코드] <보유여부>
	int gearNum = 4;
	std::vector<int> gearParts; //4
	//장비부품 [인덱스 = 부품코드(사운드칩,스위치,나사,천 조각)] <갯수>
	std::vector<bool> facility; //40
	//시설 [인덱스 = 시설코드] <보유여부>


public:
	void Init();

	void ShowPlayer();
	void RecruitLocal(int index); //지역인재 영입
	void Employ(int index);

	int GetMoney() { return money; }
	void EarnMoney(int money) { this->money += money; }
};

#define TEAM_MGR (TeamMgr::Instance())
