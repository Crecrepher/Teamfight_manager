#pragma once

enum class Phase
{
	None=-1,
	Ban,
	Pick,
	Ready,
	Battle,
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

enum class TagetingOrder
{
	Default=-1,
	ShortRange,
	LongRange,
	CarryPlayer,
	Healer,
	RowHealth,
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
};