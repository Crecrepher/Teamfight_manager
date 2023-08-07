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
	//------------------���----------------//
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

	int ableChamp = 9; //è�Ǿ𰡴�
	int ableCharacteristic = 36; //Ư������
	int money = 200;

	std::vector<PlayerInfo> player;
	int playerNum = 0;
	int MaxPlayer = 6;

	std::vector<std::pair<bool,PlayerInfo>>recruiting_players;
	//���Լ���<���԰���/����>
	int recruit_able = 1;

	std::vector<bool> gear;
	//��� [�ε��� = ����ڵ�] <��������>
	int gearNum = 4;
	std::vector<int> gearParts; //4
	//����ǰ [�ε��� = ��ǰ�ڵ�(����Ĩ,����ġ,����,õ ����)] <����>
	std::vector<bool> facility; //40
	//�ü� [�ε��� = �ü��ڵ�] <��������>


public:
	void Init();

	void ShowPlayer();
	void RecruitLocal(int index); //�������� ����
	void Employ(int index);

	int GetMoney() { return money; }
	void EarnMoney(int money) { this->money += money; }
};

#define TEAM_MGR (TeamMgr::Instance())
