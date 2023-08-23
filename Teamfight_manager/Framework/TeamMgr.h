#pragma once
#include "Singleton.h"
#include <bitset>

class Scene;

struct PlayerInfo
{
	std::string name ="";
	int age = 19;
	int condition = 2;

	int attack = 6;
	int defence = 6;
	int contract_cost = 96; //재계약비용
	int experience = 1;//활동시즌

	std::vector<int> proficiencyCode; 
	std::vector<int> proficiencyLevel;
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

	void readF(std::ifstream& ifs)
	{
		std::size_t size;
		ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
		name.resize(size);
		ifs.read(&name[0], size);

		ifs.read((char*)&age, sizeof(int));
		ifs.read((char*)&condition, sizeof(int));
		ifs.read((char*)&attack, sizeof(int));
		ifs.read((char*)&defence, sizeof(int));
		ifs.read((char*)&contract_cost, sizeof(int));
		ifs.read((char*)&experience, sizeof(int));

		ifs.read((char*)&knownChamp, sizeof(int));
		proficiencyCode.resize(knownChamp);
		proficiencyLevel.resize(knownChamp);

		ifs.read((char*)proficiencyCode.data(), sizeof(int) * knownChamp);
		ifs.read((char*)proficiencyLevel.data(), sizeof(int) * knownChamp);

		ifs.read((char*)&knownCharacter, sizeof(int));
		characteristic.resize(knownCharacter);
		ifs.read((char*)characteristic.data(), sizeof(int) * knownCharacter);

		ifs.read((char*)&potential, sizeof(int));
		ifs.read((char*)&kill, sizeof(int));
		ifs.read((char*)&assist, sizeof(int));
		ifs.read((char*)&totalKill, sizeof(int));
		ifs.read((char*)&totalAssist, sizeof(int));
		ifs.read((char*)&death, sizeof(int));
	}

	void writeF(std::ofstream& ofs)
	{
		std::size_t size = name.size();
		ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));
		ofs.write(name.data(), size);

		ofs.write((char*)&age, sizeof(int));
		ofs.write((char*)&condition, sizeof(int));
		ofs.write((char*)&attack, sizeof(int));
		ofs.write((char*)&defence, sizeof(int));
		ofs.write((char*)&contract_cost, sizeof(int));
		ofs.write((char*)&experience, sizeof(int));

		ofs.write((char*)&knownChamp, sizeof(int));
		ofs.write((char*)proficiencyCode.data(), sizeof(int) * knownChamp);
		ofs.write((char*)proficiencyLevel.data(), sizeof(int) * knownChamp);

		ofs.write((char*)&knownCharacter, sizeof(int));
		ofs.write((char*)characteristic.data(), sizeof(int) * knownCharacter);

		ofs.write((char*)&potential, sizeof(int));
		ofs.write((char*)&kill, sizeof(int));
		ofs.write((char*)&assist, sizeof(int));
		ofs.write((char*)&totalKill, sizeof(int));
		ofs.write((char*)&totalAssist, sizeof(int));
		ofs.write((char*)&death, sizeof(int));
	}
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

	void readF(std::ifstream& ifs)
	{
		ifs.read((char*)&usedTrainingPoint, sizeof(int));
		ifs.read((char*)&xpAtk, sizeof(int));
		ifs.read((char*)&trainingAtk, sizeof(int));
		ifs.read((char*)&xpDef, sizeof(int));
		ifs.read((char*)&trainingDef, sizeof(int));

		xpChamp.resize(4);
		trainingChamp.resize(4);
		ifs.read((char*)xpChamp.data(), sizeof(int) * 4);
		ifs.read((char*)trainingChamp.data(), sizeof(int) * 4);
	}

	void writeF(std::ofstream& ofs)
	{
		ofs.write((char*)&usedTrainingPoint, sizeof(int));
		ofs.write((char*)&xpAtk, sizeof(int));
		ofs.write((char*)&trainingAtk, sizeof(int));
		ofs.write((char*)&xpDef, sizeof(int));
		ofs.write((char*)&trainingDef, sizeof(int));

		ofs.write((char*)xpChamp.data(), sizeof(int) * 4);
		ofs.write((char*)trainingChamp.data(), sizeof(int) * 4);
	}
};

struct AiTeam
{
	std::string name = "";
	std::vector<PlayerInfo> player = std::vector<PlayerInfo>(4);
	int win = 0;
	int lose = 0;

	void readF(std::ifstream& ifs)
	{
		std::size_t size;
		ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
		name.resize(size);
		ifs.read(&name[0], size);

		ifs.read((char*)&win, sizeof(int));
		ifs.read((char*)&lose, sizeof(int));

		for (int i = 0; i < 4; i++)
		{
			player[i].readF(ifs);
		}
	}

	void writeF(std::ofstream& ofs)
	{
		std::size_t size = name.size();
		ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));
		ofs.write(name.data(), size);

		ofs.write((char*)&win, sizeof(int));
		ofs.write((char*)&lose, sizeof(int));
		
		for (int i = 0; i < 4; i++)
		{
			player[i].writeF(ofs);
		}
	}
};

struct Sponsor
{
	int sponsorType = -1;
	std::string sponsorTextureId = "";
	std::string sponsorName = "";
	int questCode = 0;
	int questDifficulty = 0;
	int rewardMoney = 1000;
	std::vector<int> rewardParts = std::vector<int>(4);
	bool success = false;
	int currentProcess = 0;

	void readF(std::ifstream& ifs)
	{
		ifs.read((char*)&sponsorType, sizeof(int));

		std::size_t size;
		ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
		sponsorTextureId.resize(size);
		ifs.read(&sponsorTextureId[0], size);

		ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
		sponsorName.resize(size);
		ifs.read(&sponsorName[0], size);

		ifs.read((char*)&questCode, sizeof(int));
		ifs.read((char*)&questDifficulty, sizeof(int));
		ifs.read((char*)&rewardMoney, sizeof(int));
		ifs.read((char*)&success, sizeof(int));
		ifs.read((char*)&currentProcess, sizeof(int));

		rewardParts.resize(4);
		ifs.read((char*)rewardParts.data(), sizeof(int) * 4);
	}

	void writeF(std::ofstream& ofs)
	{
		ofs.write((char*)&sponsorType, sizeof(int));

		std::size_t size = sponsorTextureId.size();
		ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));
		ofs.write(sponsorTextureId.data(), size);
		size = sponsorName.size();
		ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));
		ofs.write(sponsorName.data(), size);

		ofs.write((char*)&questCode, sizeof(int));
		ofs.write((char*)&questDifficulty, sizeof(int));
		ofs.write((char*)&rewardMoney, sizeof(int));
		ofs.write((char*)&success, sizeof(int));
		ofs.write((char*)&currentProcess, sizeof(int));

		ofs.write((char*)rewardParts.data(), sizeof(int) * 4);
	}
};

struct ItemMakeSlot
{
	bool inCrafting = false;
	int itemType = 0;
	int itemCode = 0;
	int leftDate = -1;
	std::vector<int> usedParts = std::vector<int>(4);

	void readF(std::ifstream& ifs)
	{
		std::bitset<1> bitset;
		ifs.read(reinterpret_cast<char*>(&bitset), sizeof(bitset));
		inCrafting = bitset[0];

		ifs.read((char*)&itemType, sizeof(int));
		ifs.read((char*)&itemCode, sizeof(int));
		ifs.read((char*)&leftDate, sizeof(int));

		usedParts.resize(4);
		ifs.read((char*)usedParts.data(), sizeof(int) * 4);
	}

	void writeF(std::ofstream& ofs)
	{
		std::bitset<1> bitset2(inCrafting);
		ofs.write(reinterpret_cast<const char*>(&bitset2), sizeof(bitset2));

		ofs.write((char*)&itemType, sizeof(int));
		ofs.write((char*)&itemCode, sizeof(int));
		ofs.write((char*)&leftDate, sizeof(int));

		ofs.write((char*)usedParts.data(), sizeof(int) * 4);
	}
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
	virtual	~TeamMgr() override;

	int saveSlotNum = 0;

	int ableChamp = 9; //챔피언가능
	int ableCharacteristic = 36; //특성갯수

	int maxTrainingPoint = 3;
	int money = 200;

	std::vector<PlayerInfo> player;
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

	int GetWin() { return win; }
	int GetLose() { return lose; }

	void DoWin();
	void DoPerfectWin();
	void DoLose();

	int GetAiTeamFightNum() { return (win + lose) % 7; }
	AiTeam GetAiTeam(int index) { return aiTeams[index]; }

	void ShowPlayer();
	void Recruit(int index, PlayerInfo player);
	void Employ(int index);
	int GetPlayerNum() { return playerNum; }
	int MaxPlayerNum() { return MaxPlayer; }
	std::vector<PlayerInfo> GetPlayerInfo() { return player; }
	void FirePlayer(int index);

	bool CheckRecruitSlot(int index) { return recruiting_players[index].first; }
	void DeleteRecruitSlot(int index);
	PlayerInfo GetRecruitSlotInfo(int index) { return recruiting_players[index].second; }
	bool IsCanRecruit() { return playerNum < MaxPlayer; }

	std::vector<TrainingInfo> GetTrainingInfo() { return playerTraining; }
	void SetTrainingInfo(std::vector<TrainingInfo> trainingInfo) { playerTraining = trainingInfo; }
	int GetMaxTrainingPoint() { return maxTrainingPoint; }

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

	void SaveLoad(int saveSlot);
	void Save();
};

#define TEAM_MGR (TeamMgr::Instance())

