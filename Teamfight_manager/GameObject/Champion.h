#pragma once
#include "SpriteGo.h"
#include "GameState.h"

class Champion : public SpriteGo
{
protected:
	//// Ban/Pick 선택 값
	
	//// ChampionMgr 기반 선택 값 (임시) struct 로 받을 예정
	//AnimatioControler championAnimation;
	//float championmaxHp;
	//float championAttack;
	//float championDefend;
	//float championAttackSpeed;
	//float championAttackDelay;
	//float championMoveSpeed;
	//ChampionType type;

	//// SkillMgr 기반 선택 값 (임시) struct 로 받을 예정
	//int championSkill;
	//float championSkillDelay;
	//int championUltimateSkill;

	////User 값 (임시) struct 로 받을 예정
	//float userAttack;
	//float userDefend;
	//std::vector<int> userSkill;

	// 상태
	Team team;

	ChampionStance currentStance;
	TagetingOrder currentOrder;
	
	State currentState;
	
	float hp;
	float skillCoolTime=10.f;
	float skillTimer;
	float reviveTimer;
	float attackDelay = 0.f;

	int kill = 0;
	int death = 0;

	float total_Damage = 0.f;
	float total_OnHit = 0.f;

//	bool ActiveUltiSkill = true;

	std::vector<Champion*>* enemyTeam;
	std::vector<Champion*>* myTeam;
	std::vector<Champion*>* cemetery;

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
	void SetDieChampion(std::vector<Champion*>* cemetery);
	void ChangeStance(ChampionStance stance);
	void SetState(State path);
	void SetOrder(TagetingOrder order) { this->currentOrder = order; }
	void SetSacleX(float x);
	void SetCoolTime(float t) { this->skillCoolTime = t; }
	void SetTeamColor(Team color) { team = color; }
	void Hit(float attack);
	float GetHp() { return this->hp; }

	// 타겟팅
	void FindTaget();
	void TagetOrderSR();
	void TagetOrderCP();
	void TagetOrderH();
	void TagetOrderRH();
	void TagetOrderLR();

	float GetRowHealth();

	void Idle(float dt);
	void Move(float dt);
	void Action(float dt);
	void Attack(float dt);
	void Skill(float dt);
	void UltimateSkill(float dt);
	void Dead(float dt);
	void ChampionDie();


	// 선수에게 넘길 스테이터스
	int GetKillScore() { return this->kill; }
	int GetDeathScore() { return this->death; }
	float GetTotalDamage() { return this->total_Damage; }
	float GetTotalOnHit() { return this->total_OnHit; }

	void SetDeathScore(int death) { this->death = death; }
	void SetKillScore(int kill) { this->kill = kill; }
	void SetTotalDamage(float damage) { this->total_Damage = damage; }
	void SetTotlaOnHit(float onHit) { this->total_OnHit = onHit; }
};

