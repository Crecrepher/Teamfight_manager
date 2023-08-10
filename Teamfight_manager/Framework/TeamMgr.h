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
	int contract_cost = 96; //������
	int experience = 1;//Ȱ������

	std::vector<std::pair<int, int>> proficiency; 
	//ĳ���� ���õ� <ĳ���� �ڵ� / ���õ�>
	int knownChamp = 0; //���õ� ����

	std::vector<int> characteristic; 
	//Ư�� <Ư���ڵ�>
	int knownCharacter = 0; //Ư�� ����
	int potential = 0;

	//------------------���----------------//
	int kill = 0;
	int assist = 0;
	int totalKill = 0;
	int totalAssist = 0;
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
	int QuestCode = 0;
	int QuestDifficulty = 0;
	int RewardMoney = 1000;
	std::vector<int> RewardParts = std::vector<int>(4);
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

protected:
	TeamMgr() = default;
	virtual	~TeamMgr() = default;

	int ableChamp = 9; //è�Ǿ𰡴�
	int ableCharacteristic = 36; //Ư������

	int maxTrainingPoint = 3;
	int money = 200;

	std::vector<PlayerInfo> player;
	std::vector<TrainingInfo> playerTraining;
	std::vector<std::vector<int>> dayGrowTable;
	std::vector<std::vector<int>> trainingGrowTable;
	int playerNum = 0;
	int MaxPlayer = 6;

	std::vector<std::pair<bool,PlayerInfo>>recruiting_players;
	//���Լ���<���԰���/����>
	int recruit_able = 1;

	std::vector<AiTeam> aiTeams;

	std::vector<bool> gear;
	//��� [�ε��� = ����ڵ�] <��������>
	int gearNum = 4;
	std::vector<int> gearParts; //4
	//����ǰ [�ε��� = ��ǰ�ڵ�(����Ĩ,����ġ,����,õ ����)] <����>
	std::vector<bool> facility; //40
	//�ü� [�ε��� = �ü��ڵ�] <��������>

	LeagueGrade curGrade = LeagueGrade::Amateur;
	
	int year = 2021;
	int date = 0;

	int win = 0;
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
	std::vector<AiTeam> GetAiTeamInfo();
	std::vector<TrainingInfo> GetGrowStats(std::vector<TrainingInfo> playerTraining);
	void LevelUpdate(int& xp, int& level);

	int GetQuestCount() { return sponsorQuestCount; }
	int GetContractedSponsor() { return contractedSponsor; }
	int GetMaxSponsor() { return MaxSponsor; }
	void DeleteContractedSponsor();
	void ContractSponsor(Sponsor sponsor);
};

#define TEAM_MGR (TeamMgr::Instance())
