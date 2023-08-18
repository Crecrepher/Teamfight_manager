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
	int potential = 0;

	//------------------통계----------------//
	int kill = 0;
	int assist = 0;
	int totalKill = 0;
	int totalAssist = 0;
	int death = 0;
};

struct TrainingInfo
{
	int usedTrainingPoint = 0;

	int xpAtk = 0;
	int	trainingAtk = 0;

	int xpDef = 0;
	int	trainingDef = 0;

	std::vector<int> xpChamp = std::vector<int>(4);
	std::vector<int> trainingChamp = std::vector<int>(4);
};

struct AiTeam
{
	std::wstring name = L"";
	std::vector<PlayerInfo> player = std::vector<PlayerInfo>(4);
	int win = 0;
	int lose = 0;
};

struct Sponsor
{
	int sponsorType = -1;
	std::string sponsorTextureId = "";
	std::wstring sponsorName = L"";
	int questCode = 0;
	int questDifficulty = 0;
	int rewardMoney = 1000;
	std::vector<int> rewardParts = std::vector<int>(4);
	bool success = false;
	int currentProcess = 0;
};

struct ItemMakeSlot
{
	bool inCrafting = false;
	int itemType = 0;
	int itemCode = 0;
	int leftDate = -1;
	std::vector<int> usedParts = std::vector<int>(4);
};

class TeamMgr : public Singleton<TeamMgr>
{
	friend Singleton<TeamMgr>;

public:
	enum class Schedule
	{
		Recruit,
		PracticeLeague,
		League,
		Vacation,
		EventLeague
	};

	enum class LeagueGrade
	{
		Amateur,
		SemiPro,
		SecondLeague,
		FirstLeague,
		WorldChamp
	};
	enum class ItemType
	{
		HeadSet,
		Controller,
		Chair,
		Uniform,
		TypeCount,
	};
	enum class ItemEffect
	{
		Atk,
		Def,
		AtkSpeed,
		CoolDown,
		HpDrain,
		Proficiency,
		ChampProficiency,
	};
protected:
	TeamMgr() = default;
	virtual	~TeamMgr() = default;

	int ableChamp = 9; //챔피언가능
	int ableCharacteristic = 36; //특성갯수

	int maxTrainingPoint = 3;
	int money = 200;

	std::vector<PlayerInfo> player;
	std::vector<PlayerInfo> roster;
	std::vector<TrainingInfo> playerTraining;
	std::vector<std::vector<int>> dayGrowTable;
	std::vector<std::vector<int>> trainingGrowTable;
	int playerNum = 0;
	int MaxPlayer = 6;

	std::vector<std::pair<bool,PlayerInfo>>recruiting_players;
	//영입선수<영입가능/선수>
	int recruit_able = 1;

	std::vector<AiTeam> aiTeams;

	std::vector<std::vector<bool>> gear;
	//장비 [장비유형][장비코드] <보유여부>
	std::vector<int> equipedGear;
	int gearNum = 37;
	int gearMakeSlotCount = 1;
	std::vector<ItemMakeSlot> craftSlot;
	std::vector<int> gearParts; //4
	//장비부품 [인덱스 = 부품코드(사운드칩,스위치,나사,천 조각)] <갯수>

	std::vector<bool> facility; //40
	//시설 [인덱스 = 시설코드] <보유여부>

	LeagueGrade curGrade = LeagueGrade::Amateur;
	int curRank = 8;
	
	int year = 2021;
	int date = 0;

	int win = 0;
	int winPerfect = 0;
	int winContinuity = 0;
	int lose = 0;

	std::vector<Sponsor> sponsors;
	int MaxSponsor = 1;
	int contractedSponsor = 0;
	int sponsorQuestCount = 5;
public:
	void Init();
	void InitGrowTable();

	void ShowPlayer();
	void Recruit(int index, PlayerInfo player);
	void Employ(int index);
	int GetPlayerNum() { return playerNum; }
	std::vector<PlayerInfo> GetPlayerInfo() { return player; }
	std::vector<TrainingInfo> GetTrainingInfo() { return playerTraining; }
	void SetTrainingInfo(std::vector<TrainingInfo> trainingInfo) { playerTraining = trainingInfo; }
	int GetMaxTrainingPoint() { return maxTrainingPoint; }

	bool CheckRecruitSlot(int index) { return recruiting_players[index].first; }

	int GetMoney() { return money; }
	void EarnMoney(int money) { this->money += money; }
	void UseMoney(int money) { this->money -= money; }

	int GetAbleChamps() { return ableChamp; }
	int GetAbleCharacteristic() { return ableCharacteristic; }

	int GetTodayDate() { return date; }
	int GetTodayYear() { return year; }
	Schedule GetSchedule(int date);

	void DayPass();
	void CheckQuest();
	LeagueGrade GetLeagueGrade() { return curGrade; }
	void SetAiTeams(std::vector<AiTeam> aiTeam);
	std::vector<PlayerInfo> GetRoster();
	std::vector<AiTeam> GetAiTeamInfo();
	std::vector<TrainingInfo> GetGrowStats(std::vector<TrainingInfo> playerTraining);
	void LevelUpdate(int& xp, int& level);

	int GetQuestCount() { return sponsorQuestCount; }
	int GetContractedSponsor() { return contractedSponsor; }
	int GetMaxSponsor() { return MaxSponsor; }
	Sponsor GetSponsor(int index) { return sponsors[index]; }
	void DeleteContractedSponsor();
	void ContractSponsor(Sponsor sponsor);

	std::vector<int>GetEquipedGear() { return equipedGear; }
	void SetEquipedGear(int type, int code) { equipedGear[type] = code; }
	bool GetGearOpen(int type, int code) { return gear[type][code]; }
	int GetCraftSlotCount() { return gearMakeSlotCount; }
	std::vector<ItemMakeSlot>GetCraftSlot() { return craftSlot; }
	void SetCraftSlot(int index, ItemMakeSlot craftInfo) { craftSlot[index] = craftInfo; }
	int GetParts(int type) { return gearParts[type]; }
	int UseParts(int type, int count) { return gearParts[type] = std::max(gearParts[type] - count, 0); }
	void OpenItem(int type, int code) { gear[type][code] = true; }

	void SaveLoad();
};

#define TEAM_MGR (TeamMgr::Instance())

