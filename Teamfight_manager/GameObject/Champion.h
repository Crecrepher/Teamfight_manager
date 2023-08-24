#pragma once
#include "SpriteGo.h"
#include "RectGo.h"
#include "GameState.h"
#include "ObjectPool.h"
#include "BuffState.h"

class ChampionEffect;

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
	TargetingOrder currentOrder;
	
	State champMgrState;
	ChampionSkill skillMgrSkill;

	State currentState;
	std::vector<ChampionSkill> currentSkill;
	std::vector<BuffState*> currentBuff;
	
	float hp;
	float skillTimer;
	float reviveTimer;
	float attackDelay = 0.f;

	int kill = 0;
	int death = 0;

	int* teamScore;
	float total_Damage = 0.f;
	float total_OnHit = 0.f;

	int bloodingStack = 0;
	float bloodingTimer = 0.5f;

	float bind = 0.f;
	bool air = false;
	bool limit = false;
	bool interaction = false;

	bool ActiveUltiSkill = true;
	float UesUltiSkillTiming = 0.f;

	//skill용 위치 지정 변수 ( SkillMgr 이 싱글톤 이기 때문에 위치 변수 따로 필요 )
	sf::Vector2f startPos;
	sf::Vector2f endPos;
	float sMoveT = 0.f;
	

	std::vector<Champion*>* enemyTeam;
	std::vector<Champion*>* myTeam;
	std::vector<Champion*>* cemetery;

	RectGo* field;

	Champion* target=nullptr;
	Champion* dotDamage = nullptr;

	ObjectPool<ChampionEffect>* pool = nullptr;

	int playerIndex = -1;

	int getedAtk = 0;
	int getedDef = 0;
	int getedAtkSpeed = 0;
	int getedCoolDown = 0;
	int getedHpDrain = 0;
public:
	Champion(const std::string id = "", const std::string n = "");
	virtual ~Champion();

	virtual void Init()override;
	virtual void Reset()override;
	virtual void Release() override;

	void BattleUpdate(float dt);

	void SetField(RectGo* field) { this->field = field; }
	void SetTeamScore(int* score) { this->teamScore = score; }
	void SetMyTeam(std::vector<Champion*>* myTeam);
	void SetEnemyTeam(std::vector<Champion*>* enemyTeam);
	void SetDieChampion(std::vector<Champion*>* cemetery);
	void SetEffectPool(ObjectPool<ChampionEffect>* effect) { this->pool = effect; }
	void ChangeStance(ChampionStance stance);
	void SetState(State path);
	void SetSkill(ChampionSkill code);
	void ReleaseSkill();
	void SetOrder(TargetingOrder order) { this->currentOrder = order; }
	TargetingOrder GetOrder() { return this->currentOrder; }
	void SetSacleX(float x);
	float GetSacleX() { return this->sprite.getScale().x; }
	void SetTeamColor(Team color) { team = color; }
	Team GetTeamColor() { return this->team; }
	void Hit(float attack);
	void Heal(float heal);
	float GetHp() { return this->hp; }
	void DamageCalculate(float attack);
	void HealCalculate(float attack);
	float GetHpPercent() { return this->GetHp() / this->currentState.maxHp; }
	float GetCoolTimePercent() { return this->skillTimer / this->currentSkill[0].skillCoolTime; }
	float GetUltiTimer() {return this->UesUltiSkillTiming;}
	void SetUltiTimer(float num) { this->UesUltiSkillTiming = num; }
	void SetInteraction(bool on) { this->interaction = on; }
	bool GetInteranction() { return this->interaction; }

	// 타겟팅
	void FindTaget();
	void TargetOrderSR();
	void TargetOrderCP();
	void TargetOrderH();
	void TargetOrderRH();
	void TargetOrderLR();
	void TargetOrderCIE(int code, float range, float value);
	void TargetOrderCIT(int code, float range, float value);
	void TargetOrderCITB(int code, float range, BuffState* state);
	void TargetOrderCIEB(int code, float range, BuffState* state);
	void TargetAggro();

	float GetRowHealth();

	void Idle(float dt);
	void Move(float dt);
	void Action(float dt);
	void Attack(float dt);
	void Skill(float dt);
	void UltimateSkill(float dt);
	void Dead(float dt);
	void ChampionDie();
	void UpdateState(float dt);


	State GetCurretState() { return this->currentState; }
	ChampionStance GetCurrentStance() { return this->currentStance; }

	//Skill Mgr 상호작용
	void UseSkill();
	void UseUltiSkill();
	void SkillChangeIdle();
	void SkillAniChange();
	Champion* GetTarget();
	void SetTarget(Champion* champ);
	void SetBind(float t) { this->bind = t; }
	float GetMoveTime() { return this->sMoveT; }
	void SetMoveTime(float t) { this->sMoveT = t; }
	bool GetAir() { return this->air; }
	void SetAir(bool change) { this->air = change; }
	void SetBloodingStack(int stack) { this->bloodingStack = stack; }
	void UseBloodingPlayer(Champion* champ) { this->dotDamage = champ; }
	void SetFrameLimit(bool limit) { this->limit = limit; }
	bool GetFrameLimit() { return this->limit; }
	void SetUltiSkill(bool setting) { this->ActiveUltiSkill = setting; }
	bool GetUltiSkill() { return this->ActiveUltiSkill; }
	void SetStartPos(sf::Vector2f pos) { this->startPos = pos; }
	void SetEndPos(sf::Vector2f pos) { this->endPos = pos; }
	sf::Vector2f GetStartPos() { return this->startPos; }
	sf::Vector2f GetEndPos() { return this->endPos; }
	void SetBuff(BuffState* state);
	bool GetUseBuff(BuffType type);
	void SetMyTeamBuff(BuffState* state);
	float GetBarrier();
	void TargetRangeDamage(float range, float value);
	bool CurrBuffEmpty();
	void BuffUpdate(float dt);


	// 선수에게 넘길 스테이터스
	int GetKillScore() { return this->kill; }
	int GetDeathScore() { return this->death; }
	float GetTotalDamage() { return this->total_Damage; }
	float GetTotalOnHit() { return this->total_OnHit; }

	void SetDeathScore(int death) { this->death = death; }
	void SetKillScore(int kill) { this->kill = kill; }
	void SetTotalDamage(float damage) { this->total_Damage = damage; }
	void SetTotlaOnHit(float onHit) { this->total_OnHit = onHit; }

	//effect
	void SetShadow();
	void SetHpGuage();

	// 선수 / 장비스텟 적용
	void GetStat(int atk, int def, int atkSpeed, int coolDown,int drain,int index);
	int GetPlayerIndex() { return playerIndex; }
};

