#pragma once
#include "SpriteGo.h"
#include "GameState.h"

class Champion : public SpriteGo
{
protected:
	//// Ban/Pick ���� ��
	
	//// ChampionMgr ��� ���� �� (�ӽ�) struct �� ���� ����
	//AnimatioControler championAnimation;
	//float championmaxHp;
	//float championAttack;
	//float championDefend;
	//float championAttackSpeed;
	//float championAttackDelay;
	//float championMoveSpeed;
	//ChampionType type;

	//// SkillMgr ��� ���� �� (�ӽ�) struct �� ���� ����
	//int championSkill;
	//float championSkillDelay;
	//int championUltimateSkill;

	////User �� (�ӽ�) struct �� ���� ����
	//float userAttack;
	//float userDefend;
	//std::vector<int> userSkill;

	// ����
	bool team;

	ChampionStance currentStance;
	TagetingOrder currentOrder;
	
	State currentState;
	
	float hp;
	float skillCoolTime=10.f;
	float skillTimer;
	float reviveTimer;

//	bool ActiveUltiSkill = true;

	std::vector<Champion*>* enemyTeam;
	std::vector<Champion*>* myTeam;

	Champion* taget=nullptr;

public:
	Champion(const std::string id = "", const std::string n = "");
	virtual ~Champion();

	virtual void Init()override;
	virtual void Reset()override;
	virtual void Release() override;
	virtual void Update(float dt)override;

	void SetMyTeam(std::vector<Champion*>* myTeam);
	void SetEnemyTeam(std::vector<Champion*>* enemyTeam);
	void ChangeStance(ChampionStance stance);
	void SetState(State path);
	void SetOrder(TagetingOrder order) { this->currentOrder = order; }
	void SetSacleX(float x);
	void SetCoolTime(float t) { this->skillCoolTime = t; }
	void SetPlayerTeam() { team = true; }

	void FindTaget();
	void TagetOrderSR();
	void TagetOrderCP();
	void TagetOrderH();
	void TagetOrderRH();
	void TagetOrderLR();

	void Idle(float dt);
	void Move(float dt);
	void Action(float dt);
	void Attack(float dt);
	void Skill(float dt);
	void UltimateSkill(float dt);
	void Dead(float dt);

	void ChampionDie();
};

