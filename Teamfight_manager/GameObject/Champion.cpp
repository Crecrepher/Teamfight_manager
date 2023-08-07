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

		if (this->currentStance == ChampionStance::None)
			return;

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
	if (this->hp <= 0)
	{
		ChampionDie();
		return;
	}

	if (!this->enemyTeam->empty())
	{
		FindTaget();
	}

	if (this->taget != nullptr)
	{
		SetSacleX(Utils::Normalize(taget->GetPosition() - this->position).x);
		if (abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) >= 25.f)
		{
			ChangeStance(ChampionStance::Move);
			return;
		}
		else if (abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) <= 25.f)
		{
			ChangeStance(ChampionStance::Attack);
			return;
		}
	}
}

void Champion::Move(float dt)
{
	if (this->hp <= 0)
	{
		ChampionDie();
		return;
	}

	if (!this->enemyTeam->empty())
	{
		FindTaget();
	}

	if (this->taget != nullptr)
	{
		if (abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) <= 25.f)
		{
			ChangeStance(ChampionStance::Idle);
			return;
		}
	}

	SetPosition(this->position + Utils::Normalize(taget->GetPosition() - this->position) * this->currentState.speed * dt);
}

void Champion::Attack(float dt)
{
	if (this->hp <= 0)
	{
		ChampionDie();
		return;
	}

	//std::cout << "공격" << std::endl;
	ChangeStance(ChampionStance::Idle);
}

void Champion::Skill(float dt)
{
	if (this->hp <= 0)
	{
		ChampionDie();
		return;
	}

}

void Champion::SetState(float mHp, float atk, float def,
	float atkS, float atkR, float spd)
{
	this->currentState.maxHp = mHp;
	this->currentState.attack = atk;
	this->currentState.defend = def;
	this->currentState.attackSpeed = atkS;
	this->currentState.attackRange = atkR;
	this->currentState.speed = spd;
}

void Champion::UltimateSkill(float dt)
{
	if (this->hp <= 0)
	{
		ChampionDie();
		return;
	}
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
		switch (this->currentType)
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
