#include "stdafx.h"
#include "TeamMgr.h"

void TeamMgr::Init()
{
	gear = std::vector<std::vector<bool>>(4, { std::vector<bool>(gearNum,false)});
	for (int i = 0; i < 4; i++)
	{
		gear[i][0] = true;
		gear[i][1] = true;
	}
	equipedGear = std::vector<int>(4,0);
	gearParts = std::vector<int>(4, 1);
	facility = std::vector<bool>(40, false);
	recruiting_players = std::vector<std::pair<bool, PlayerInfo>>(4,{false,PlayerInfo()});
	dayGrowTable = std::vector<std::vector<int>>(10);
	trainingGrowTable = std::vector<std::vector<int>>(10);
	sponsors = std::vector<Sponsor>(3);
	roster = std::vector<PlayerInfo>(6);
	craftSlot = std::vector<ItemMakeSlot>(3);
	InitGrowTable();
	
	return;
}

void TeamMgr::InitGrowTable()
{
	dayGrowTable[0] = { 300,600,400,700 };
	dayGrowTable[1] = { 300,600,400,800 };
	dayGrowTable[2] = { 300,600,600,400 };
	dayGrowTable[3] = { 200,250,400,400 };
	dayGrowTable[4] = { 100,0,400,0 };
	dayGrowTable[5] = { 0,0,200,0 };
	dayGrowTable[6] = { 0,0,0,-100 };
	dayGrowTable[7] = { -50,-100,0,-200 };
	dayGrowTable[8] = { -100,-200,-100,-300 };
	dayGrowTable[9] = { -200,-200,-120,-400 };

	trainingGrowTable[0] = { 30,60,72,80 };
	trainingGrowTable[1] = { 24,48,64,72 };
	trainingGrowTable[2] = { 20,40,52,60 };
	trainingGrowTable[3] = { 16,32,40,48 };
	trainingGrowTable[4] = { 10,20,32,40 };
	trainingGrowTable[5] = { 6,12,24,32 };
	trainingGrowTable[6] = { 4,8,16,24 };
	trainingGrowTable[7] = { 2,4,8,16 };
	trainingGrowTable[8] = { 2,4,8,16 };
	trainingGrowTable[9] = { 2,4,8,16 };
}


void TeamMgr::ShowPlayer()
{
	std::cout << "보유 선수: " << playerNum<<"명" << std::endl;
	for (int i = 0; i < playerNum; i++)
	{
		std::string st = "";
		st.assign(player[i].name.begin(), player[i].name.end());
		std::cout <<"이름: " << st << std::endl;
		std::cout << "나이: " << player[i].age << std::endl;
		std::cout << "컨디션: " << player[i].condition << std::endl;
		std::cout << "공격력: " << player[i].attack << std::endl;
		std::cout << "방어력: " << player[i].defence << std::endl;
		std::cout << "계약금: " << player[i].contract_cost << std::endl;
		std::cout << "사용캐릭터 갯수: " << player[i].knownChamp << std::endl;
		for (int j = 0; j < player[i].knownChamp; j++)
		{
			std::cout << "사용캐릭터 코드: "<< player[i].proficiency[j].first << " ";
			std::cout << "   숙련도: " << player[i].proficiency[j].second << std::endl;
		}
		std::cout << "특성 갯수: " << player[i].knownCharacter << std::endl;
		for (int j = 0; j < player[i].knownCharacter; j++)
		{
			std::cout << "특성코드: " << player[i].characteristic[j] << std::endl;
		}
		std::cout << std::endl;
	}
}

void TeamMgr::Recruit(int index, PlayerInfo player)
{
	recruiting_players[index].second = player;
	recruiting_players[index].first = true;
}

void TeamMgr::Employ(int index)
{
	if (money < recruiting_players[index].second.contract_cost)
	{
		return;
	}
	playerNum++;
	money -= recruiting_players[index].second.contract_cost;
	player.push_back(recruiting_players[index].second);
	recruiting_players[index] = { false,PlayerInfo() };
	playerTraining.push_back(TrainingInfo());
}

void TeamMgr::FirePlayer(int index)
{
	std::vector<PlayerInfo>::iterator it = player.begin();
	it += index;
	player.erase(it);
	playerNum--;
}

void TeamMgr::DeleteRecruitSlot(int index) 
{
	recruiting_players[index].second = PlayerInfo();
	recruiting_players[index].first = false;
}

TeamMgr::Schedule TeamMgr::GetSchedule(int date)
{
	std::vector<int> vacation = { 27,28,29,32,33,34,43,44,45,46 };
	if (date == 1)
	{
		return TeamMgr::Schedule::Recruit;
	}
	if ((date>5 && date < 13)||((date > 18 && date < 26)))
	{
		return TeamMgr::Schedule::League;
	}
	for (int i = 0; i < vacation.size(); i++)
	{
		if (date == vacation[i])
		{
			return TeamMgr::Schedule::Vacation;
		}
	}
	return TeamMgr::Schedule::Recruit;
}

void TeamMgr::DayPass()
{
	date++;
	CheckQuest();
	for (int i = 0; i < craftSlot.size(); i++)
	{
		if (craftSlot[i].leftDate > 0)
		{
			craftSlot[i].leftDate--;
		}
	}
	
	if (date >= 48)
	{
		date = 0;
		year++;
		for (int i = 0; i < playerNum; i++)
		{
			player[i].age++;
		}
		sponsors = std::vector<Sponsor>(3);
		contractedSponsor = 0;
		for (int i = 0; i < playerNum; i++)
		{
			player[i].kill = 0;
			player[i].death = 0;
		}
	}

	playerTraining = GetGrowStats(playerTraining);

	for (int i = 0; i < playerNum; i++)
	{
		LevelUpdate(playerTraining[i].xpAtk, player[i].attack);
		LevelUpdate(playerTraining[i].xpDef, player[i].defence);
		for (int j = 0; j < player[i].knownChamp; j++)
		{
			LevelUpdate(playerTraining[i].xpChamp[j], player[i].proficiency[j].second);
		}
	}
}

void TeamMgr::CheckQuest()
{
	for (int i = 0; i < contractedSponsor; i++)
	{
		if (sponsors[i].success)
		{
			continue;
		}
		switch (sponsors[i].questCode)
		{
		case 0:
			sponsors[i].currentProcess = curRank;
			if (curRank <= 3 - sponsors[i].questDifficulty)
			{
				sponsors[i].success = true;
			}
			break;
		case 1:
			sponsors[i].currentProcess = winPerfect;
			if (winPerfect >= 5 + sponsors[i].questDifficulty * 2)
			{
				sponsors[i].success = true;
			}
			break;
		case 2:			
			sponsors[i].currentProcess = winContinuity;
			if (winPerfect >= 5 + sponsors[i].questDifficulty * 2)
			{
				sponsors[i].success = true;
			}
			break;
		case 3:
		{
			int sumKill = 0;
			for (int i = 0; i < playerNum; i++)
			{
				sumKill += player[i].kill;
			}
			sponsors[i].currentProcess = sumKill;
			if (sumKill >= 300 + sponsors[i].questDifficulty * 50)
			{
				sponsors[i].success = true;
			}
		}
			break;
		case 4:
		{
			int sumDeath = 0;
			for (int i = 0; i < playerNum; i++)
			{
				sumDeath += player[i].death;
			}
			sponsors[i].currentProcess = sumDeath;
			if (sumDeath <= 300 + sponsors[i].questDifficulty * 50
				&& date >= 46)
			{
				sponsors[i].success = true;
			}
		}
			break;
		default:
			break;
		}
		if (sponsors[i].success)
		{
			EarnMoney(sponsors[i].rewardMoney);
			for (int j = 0; j < 4; j++)
			{
				gearParts[j] += sponsors[i].rewardParts[j];
			}
		}
	}
}

void TeamMgr::SetAiTeams(std::vector<AiTeam> aiTeam) 
{ 
	aiTeams = aiTeam;
}

std::vector<PlayerInfo> TeamMgr::GetRoster()
{
	return roster;
}

std::vector<AiTeam> TeamMgr::GetAiTeamInfo()
{ 
	return aiTeams; 
}

std::vector<TrainingInfo> TeamMgr::GetGrowStats(std::vector<TrainingInfo> playerTraining)
{
	std::vector<TrainingInfo> trainResult = playerTraining;
	for (int i = 0; i < playerNum; i++)
	{
		int ageIndex = player[i].age - 17;
		if (ageIndex < 0)
		{
			ageIndex = 0;
		}
		else if (ageIndex > 9)
		{
			ageIndex = 9;
		}
		trainResult[i].xpAtk
			+= dayGrowTable[ageIndex][player[i].potential]
			+ (trainingGrowTable[ageIndex][player[i].potential]
				* trainResult[i].trainingAtk);
		trainResult[i].xpDef
			+= dayGrowTable[ageIndex][player[i].potential]
			+ (trainingGrowTable[ageIndex][player[i].potential]
				* trainResult[i].trainingDef);
		for (int j = 0; j < player[i].knownChamp; j++)
		{
			trainResult[i].xpChamp[j]
				+= dayGrowTable[ageIndex][player[i].potential]
				+ (trainingGrowTable[ageIndex][player[i].potential]
					* trainResult[i].trainingChamp[j]);
		}
	}
	return trainResult;
}

void TeamMgr::LevelUpdate(int& xp, int& level)
{
	int levelUpVal = 1300 + (level * 10);
	if (xp >= levelUpVal)
	{
		xp -= levelUpVal;
		level++;
	}
}


void TeamMgr::DeleteContractedSponsor() 
{ 
	sponsors = std::vector<Sponsor>(3);
	contractedSponsor = 0;
}

void TeamMgr::ContractSponsor(Sponsor sponsor)
{
	if (MaxSponsor == contractedSponsor)
	{
		return;
	}
	sponsors[contractedSponsor] = sponsor;
	if (sponsor.questCode == 0)
	{
		sponsors[contractedSponsor].currentProcess = 8;
	}
	contractedSponsor++;
}

void TeamMgr::SaveLoad(int saveSlot)
{
	std::stringstream ss;
	ss << "TFMSave" << saveSlotNum << ".dat";
	std::ifstream is{ ss.str(), std::ofstream::binary};
	if (is.fail()) {
		std::cout << "세이브 파일이 없습니다" << std::endl;
		return;
	}

	is.read((char*)&ableChamp, sizeof(int));
	is.read((char*)&ableCharacteristic, sizeof(int));
	is.read((char*)&maxTrainingPoint, sizeof(int));
	is.read((char*)&money, sizeof(int));
	is.read((char*)&playerNum, sizeof(int));
	is.read((char*)&MaxPlayer, sizeof(int));
	for (int i = 0; i < playerNum; i++)
	{
		is.read((char*)&player[i].name, sizeof(std::wstring));
		is.read((char*)&player[i].age, sizeof(int));
		is.read((char*)&player[i].condition, sizeof(int));
		is.read((char*)&player[i].attack, sizeof(int));
		is.read((char*)&player[i].defence, sizeof(int));
	}
	return;
}
