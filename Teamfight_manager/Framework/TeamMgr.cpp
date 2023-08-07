#include "stdafx.h"
#include "TeamMgr.h"
#include "InputMgr.h"
#include "Utils.h"
#include "DataTableMgr.h"
#include "StringTable.h"

void TeamMgr::Init()
{
	gear = std::vector<bool>(gearNum, false);
	gearParts = std::vector<int>(4, 0);
	facility = std::vector<bool>(40, false);
	recruiting_players = std::vector<std::pair<bool, PlayerInfo>>(4,{false,PlayerInfo()});
	return;
}

void TeamMgr::ShowPlayer()
{
	std::cout << "���� ����: " << playerNum<<"��" << std::endl;
	for (int i = 0; i < playerNum; i++)
	{
		std::string st = "";
		st.assign(player[i].name.begin(), player[i].name.end());
		std::cout <<"�̸�: " << st << std::endl;
		std::cout << "����: " << player[i].age << std::endl;
		std::cout << "�����: " << player[i].condition << std::endl;
		std::cout << "���ݷ�: " << player[i].attack << std::endl;
		std::cout << "����: " << player[i].defence << std::endl;
		std::cout << "����: " << player[i].contract_cost << std::endl;
		std::cout << "���ĳ���� ����: " << player[i].knownChamp << std::endl;
		for (int j = 0; j < player[i].knownChamp; j++)
		{
			std::cout << "���ĳ���� �ڵ�: "<< player[i].proficiency[j].first << " ";
			std::cout << "   ���õ�: " << player[i].proficiency[j].second << std::endl;
		}
		std::cout << "Ư�� ����: " << player[i].knownCharacter << std::endl;
		for (int j = 0; j < player[i].knownCharacter; j++)
		{
			std::cout << "Ư���ڵ�: " << player[i].characteristic[j] << std::endl;
		}
		std::cout << std::endl;
	}
}

void TeamMgr::RecruitLocal(int index)
{
	if (money < 10 || recruiting_players[index].first == true)
	{
		return;
	}
	money -= 10;
	PlayerInfo* recruitPlayer = &recruiting_players[index].second;
	recruiting_players[index].first = true;
	{
		std::stringstream ss;
		ss << "PlayerName" << Utils::RandomRange(0,9);
		auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
		recruitPlayer->name = stringtable->GetW(ss.str());
	}
	recruitPlayer->age = Utils::RandomRange(19,21);
	recruitPlayer->attack = Utils::RandomRange(9, 16);
	recruitPlayer->defence = 25 - recruitPlayer->attack;
	recruitPlayer->knownChamp = Utils::RandomRange(1, 3);
	for (int i = 0; i < recruitPlayer->knownChamp; i++)
	{
		recruitPlayer->proficiency.push_back({ 
			Utils::RandomRange(0, ableChamp-1),
			Utils::RandomRange(5, 7) });
	}
	recruitPlayer->knownCharacter = Utils::RandomRange(0, 3);
	for (int i = 0; i < recruitPlayer->knownCharacter; i++)
	{
		recruitPlayer->characteristic.push_back(
			Utils::RandomRange(0, ableCharacteristic - 1));
	}
	recruitPlayer->contract_cost = Utils::RandomRange(95, 105+(recruitPlayer->knownChamp*10)+(recruitPlayer->knownCharacter * 10));
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
}
