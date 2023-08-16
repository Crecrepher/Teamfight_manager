#pragma once
#include "SpriteGo.h"
#include "RectGo.h"
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
	Team team;

	ChampionStance currentStance;
	TagetingOrder currentOrder;
	
	State champMgrState;
	ChampionSkill skillMgrSkill;

	State currentState;
	std::vector<ChampionSkill> currentSkill;

	
	float hp;
	float skillTimer;
	float reviveTimer;
	float attackDelay = 0.f;

	int kill = 0;
	int death = 0;

	int* teamScore;
	float total_Damage = 0.f;
	float total_OnHit = 0.f;

	float bind = 0.f;
	bool air = false;

//	bool ActiveUltiSkill = true;
	//skill�� ��ġ ���� ���� ( SkillMgr �� �̱��� �̱� ������ ��ġ ���� ���� �ʿ� )
	sf::Vector2f startPos;
	sf::Vector2f endPos;
	float sMoveT = 0.f;
	

	std::vector<Champion*>* enemyTeam;
	std::vector<Champion*>* myTeam;
	std::vector<Champion*>* cemetery;

	RectGo* field;

	Champion* target=nullptr;

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
	void ChangeStance(ChampionStance stance);
	void SetState(State path);
	void SetSkill(ChampionSkill code);
	void ReleaseSkill();
	void SetOrder(TagetingOrder order) { this->currentOrder = order; }
	void SetSacleX(float x);
	void SetTeamColor(Team color) { team = color; }
	Team GetTeamColor() { return this->team; }
	void Hit(float attack);
	void Heal(float heal);
	float GetHp() { return this->hp; }
	void DamageCalculate(float attack);
	void HealCalculate(float attack);

	// Ÿ����
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
	void UpdateState();


	State GetCurretState() { return this->currentState; }

	//Skill Mgr ��ȣ�ۿ�
	void UseSkill();
	void SkillChangeIdle();
	Champion* GetTarget();
	void SetBind(float t) { this->bind = t; }
	float GetMoveTime() { return this->sMoveT; }
	void SetMoveTime(float t) { this->sMoveT = t; }
	bool GetAir() { return this->air; }
	void SetAir(bool change) { this->air = change; }
	

	void SetStartPos(sf::Vector2f pos) { this->startPos = pos; }
	void SetEndPos(sf::Vector2f pos) { this->endPos = pos; }
	sf::Vector2f GetStartPos() { return this->startPos; }
	sf::Vector2f GetEndPos() { return this->endPos; }


	// �������� �ѱ� �������ͽ�
	int GetKillScore() { return this->kill; }
	int GetDeathScore() { return this->death; }
	float GetTotalDamage() { return this->total_Damage; }
	float GetTotalOnHit() { return this->total_OnHit; }

	void SetDeathScore(int death) { this->death = death; }
	void SetKillScore(int kill) { this->kill = kill; }
	void SetTotalDamage(float damage) { this->total_Damage = damage; }
	void SetTotlaOnHit(float onHit) { this->total_OnHit = onHit; }
};

