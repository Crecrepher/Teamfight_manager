#include "stdafx.h"
#include "Champion.h"
#include "ResourceMgr.h"
#include "SceneGame.h"

Champion::Champion(const std::string id, const std::string n)
	: SpriteGo(id, n)
{
}

Champion::~Champion()
{
}

void Champion::Init()
{
	SpriteGo::Init();

	team = false;
}


void Champion::Reset()
{
	SpriteGo::Reset();
	this->hp = currentState.maxHp;
	SetOrigin(Origins::BC);
}

void Champion::Release()
{
	SpriteGo::Release();
}

void Champion::Update(float dt)
{
	SpriteGo::Update(dt);

	dt *= 10.f;

	skillTimer += dt;

	if (skillTimer >= skillCoolTime)
	{
		skillTimer = skillCoolTime;
	}

	if (this->currentStance == ChampionStance::None)
		return;

	if (this->hp <= 0)
	{
		ChampionDie();
		return;
	}

	switch (this->currentStance)
	{
	case ChampionStance::Idle:
	{
		Idle(dt);
		break;
	}
	case ChampionStance::Move:
	{
		Move(dt);
		break;
	}
	case ChampionStance::Action:
	{
		Action(dt);
		break;
	}
	case ChampionStance::Attack:
	{
		Attack(dt);
		break;
	}
	case ChampionStance::Skill:
	{
		Skill(dt);
		break;
	}
	case ChampionStance::UltimateSkill:
	{
		UltimateSkill(dt);
		break;
	}
	case ChampionStance::Dead:
	{
		Dead(dt);
		break;
	}
	}
	
}

void Champion::ChangeStance(ChampionStance stance)
{
	this->currentStance = stance;
}

void Champion::Idle(float dt)
{
	if (!this->enemyTeam->empty())
	{
		FindTaget();
	}

	if (this->taget != nullptr)
	{
		//std::cout << abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) << std::endl;
		SetSacleX(Utils::Normalize(taget->GetPosition() - this->position).x);
		if (abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) > this->currentState.attackRange)
		{
			ChangeStance(ChampionStance::Move);
			return;
		}
		else if (abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) <= this->currentState.attackRange)
		{
			ChangeStance(ChampionStance::Action);
			return;
		}
	}
}

void Champion::Move(float dt)
{
	if (!this->enemyTeam->empty())
	{
		FindTaget();
	}

	if (this->taget != nullptr)
	{
		if (abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) <= this->currentState.attackRange)
		{
			ChangeStance(ChampionStance::Idle);
			return;
		}
	}
	SetPosition(this->position + Utils::Normalize(taget->GetPosition() - this->position) * this->currentState.speed * dt);
}

void Champion::Action(float dt)
{
	if (abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) <= this->currentState.attackRange && skillTimer == skillCoolTime)
	{
		ChangeStance(ChampionStance::Skill);
		return;
	}
	else if (abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) <= this->currentState.attackRange)
	{
		ChangeStance(ChampionStance::Attack);
		return;
	}
}

void Champion::Attack(float dt)
{
	std::cout << "공격" << std::endl;
	ChangeStance(ChampionStance::Idle);
}

void Champion::Skill(float dt)
{
	std::cout << "스킬" << std::endl;
	skillTimer = 0;
	ChangeStance(ChampionStance::Idle);
}

void Champion::SetState(State path)
{
	this->currentState.charId = path.charId;
	this->currentState.maxHp = path.maxHp;
	this->currentState.attack = path.attack;
	this->currentState.defend = path.defend;
	this->currentState.attackSpeed = path.attackSpeed;
	this->currentState.attackRange = path.attackRange;
	this->currentState.speed = path.speed;
	this->currentState.skillCode1 = path.skillCode1;
	this->currentState.skillCode2 = path.skillCode2;
	this->currentState.type = path.type;
}

void Champion::SetSacleX(float x)
{
	if (x >= 0)
	{
		this->sprite.setScale({ 1, 1 });
	}
	else if (x < 0)
	{
		this->sprite.setScale({ -1,1 });
	}
}

void Champion::UltimateSkill(float dt)
{

}

void Champion::Dead(float dt)
{
	this->reviveTimer -= dt;

	if (this->reviveTimer <= 0)
	{
		this->hp = this->currentState.maxHp;
		SetPosition(0, 0); // 리스폰 임시 포지션
		SetActive(true);
		ChangeStance(ChampionStance::Idle);
	}
}

void Champion::ChampionDie()
{
	this->reviveTimer = 3.f;
	ChangeStance(ChampionStance::Dead);
	SetActive(false);
}

void Champion::SetMyTeam(std::vector<Champion*>* myTeam)
{
	this->myTeam = myTeam;
}

void Champion::SetEnemyTeam(std::vector<Champion*>* enemyTeam)
{
	this->enemyTeam = enemyTeam;
}

void Champion::FindTaget()
{
	switch (this->currentOrder)
	{
	case TagetingOrder::Default:
	{
		switch (this->currentState.type)
		{
		case ChampionType::Warrios:
		{
			TagetOrderSR();
			break;
		}
		case ChampionType::Ranger:
		{
			TagetOrderLR();
			break;
		}
		case ChampionType::Assassin:
		{
			TagetOrderCP();
			break;
		}
		case ChampionType::Mage:
		{
			TagetOrderSR();
			break;
		}
		case ChampionType::Assist:
		{
			TagetOrderRH();
			break;
		}
		}
		break;
	}
	case TagetingOrder::ShortRange:
	{
		TagetOrderSR();
		break;
	}
	case TagetingOrder::LongRange:
	{
		TagetOrderLR();
		break;
	}
	case TagetingOrder::CarryPlayer:
	{
		TagetOrderCP();
		break;
	}
	case TagetingOrder::Healer:
	{
		TagetOrderH();
		break;
	}
	case TagetingOrder::RowHealth:
	{
		TagetOrderRH();
		break;
	}
	}
}

void Champion::TagetOrderCP()
{
}

void Champion::TagetOrderSR()
{
	if (!enemyTeam->empty())
	{
		float tagetRange=9999.f;
		for (auto enemy : *enemyTeam)
		{
			if (abs(Utils::Distance(this->GetPosition(), enemy->GetPosition())) <= tagetRange)
			{
				tagetRange = abs(Utils::Distance(this->GetPosition(), enemy->GetPosition()));
				this->taget = enemy;
			}
		}
	}
	
}

void Champion::TagetOrderRH()
{
}

void Champion::TagetOrderH()
{
}

void Champion::TagetOrderLR()
{
}
