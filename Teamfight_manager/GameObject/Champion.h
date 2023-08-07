#pragma once
#include "SpriteGo.h"
#include "GameState.h"

class Champion : public SpriteGo
{
protected:
	//// Ban/Pick 선택 값
	std::string characterId;

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
	bool team;

	ChampionStance currentStance;
	ChampionType currentType;
	TagetingOrder currentOrder;
	
	State currentState;
	
	float hp;
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
	void SetState(float mHp, float atk, float def, float atkS, float atkR, float spd);
	void SetOrder(TagetingOrder order) { this->currentOrder = order; }
	void SetId(std::string id) { this->characterId = id; }
	std::string GetId() { return this->taget->characterId; }
	void SetSacleX(float x) { this->sprite.setScale({ x, 1 }); }
	void SetPlayerTeam() { team = true; }

	void FindTaget();
	void TagetOrderSR();
	void TagetOrderCP();
	void TagetOrderH();
	void TagetOrderRH();
	void TagetOrderLR();

	void Idle(float dt);
	void Move(float dt);
	void Attack(float dt);
	void Skill(float dt);
	void UltimateSkill(float dt);
	void Dead(float dt);

	void ChampionDie();
};

