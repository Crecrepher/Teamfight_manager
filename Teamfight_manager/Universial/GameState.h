#pragma once
#include "AnimatioControler.h"

enum class Phase
{
	None=-1,
	League,
	Ban,
	Pick,
	Ready,
	Battle,
	Result,
};

enum class Team
{
	None=-1,
	Red,
	Blue,
};

enum class Turn
{
	None=-1,
	Enemy,
	Player,
};

enum class Mode
{
	None=-1,
	Duo,
	Trio,
	Sqaud,
};

enum class ChampionStance
{
	None=-1,
	Idle,
	Move,
	Action,
	Attack,
	Skill,
	UltimateSkill,
	Dead,
};

enum class ChampionType
{
	None=-1,
	Warrios,
	Ranger,
	Assassin,
	Mage,
	Assist,
};

enum class TargetingOrder
{
	Default=-1,
	ShortRange,
	LongRange,
	CarryPlayer,
	Heal,
	RowHealth,
	CircleInRangeEnemy,
	CircleInRangeTeam,
	Aggro,
};

struct State
{
	std::string charId;
	float maxHp;
	float attack;
	float defend;
	float attackSpeed;
	float attackRange;
	float speed;
	int skillCode1;
	int skillCode2;
	ChampionType type;
	AnimatioControler animaition;
};

struct BanPick
{
	State state;
	float winRate;
};

struct ChampionSkill
{
	std::string skillId;
	float skillCoolTime;
	AnimatioControler animaition;
};

enum class BuffType
{
	MAXHP,
	ATTACK,
	DEFEND,
	ATTACKSPEED,
	ATTACKRANGE,
	SPEED,
	BLOODING,
	BIND,
	STUN,
	COOLTIME,
	AGGRO,
	BARRIER,
};