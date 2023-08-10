#include "stdafx.h"
#include "Champion.h"
#include "ResourceMgr.h"
#include "SceneGame.h"
#include "SceneMgr.h"
#include "AnimatioControler.h"

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

	this->champMgrState.charId = "";
	this->champMgrState.maxHp = 0.f;
	this->champMgrState.attack = 0.f;
	this->champMgrState.defend = 0.f;
	this->champMgrState.attackSpeed = 0.f;
	this->champMgrState.attackRange = 0.f;
	this->champMgrState.speed = 0.f;
	this->champMgrState.skillCode1 = -1;
	this->champMgrState.skillCode2 = -1;
	this->champMgrState.type = ChampionType::None;

	team = Team::None;
}

void Champion::Reset()
{
	SpriteGo::Reset();
	this->hp = currentState.maxHp;
	this->death = 0;
	this->kill = 0;
	this->total_Damage = 0.f;
	this->total_OnHit = 0.f;
	this->currentState.animaition.SetTarget(&sprite);
	SetOrigin(Origins::MC);
}

void Champion::Release()
{
	SpriteGo::Release();
}

void Champion::Update(float dt)
{
	if (this->currentStance == ChampionStance::None)
		return;

	SpriteGo::Update(dt);

	if (this->currentState.animaition.GetCurrentClipId() == "Die")
	{
		this->currentState.animaition.Update(dt);
	}

	UpdateState();

	skillTimer += dt;
	attackDelay -= dt*this->currentState.attackSpeed;

	if (skillTimer >= skillCoolTime)
	{
		skillTimer = skillCoolTime;
	}

	if (this->hp <= 0 && this->currentStance!=ChampionStance::Dead)
	{
		if (this->currentState.animaition.GetCurrentClipId() != "Die")
		{
			this->currentState.animaition.Play("Die");
		}

		if (this->currentState.animaition.GetLastFrame())
		{
			ChampionDie();
		}
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
	this->currentState.animaition.Update(dt);

	if (this->currentState.animaition.GetCurrentClipId() != "Idle")
	{
		this->currentState.animaition.Play("Idle");
	}

	if (this->currentState.charId == "priest")
	{
		if (!this->myTeam->empty())
		{
			FindTaget();
		}
		else if (this->myTeam->empty())
		{
			this->taget = nullptr;
		}
	}
	else
	{
		if (!this->enemyTeam->empty())
		{
			FindTaget();
		}
		else if (this->enemyTeam->empty())
		{
			this->taget = nullptr;
		}
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
	this->currentState.animaition.Update(dt);

	if (this->currentState.animaition.GetCurrentClipId() != "Move")
	{
		this->currentState.animaition.Play("Move");
	}

	dt *= 5.f;

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
	if (abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) <= this->currentState.attackRange && skillTimer >= skillCoolTime)
	{
		ChangeStance(ChampionStance::Skill);
		return;
	}
	else if (abs(Utils::Distance(this->GetPosition(), this->taget->GetPosition())) <= this->currentState.attackRange&&attackDelay<=0.f)
	{
		ChangeStance(ChampionStance::Attack);
		return;
	}
	else
	{
		ChangeStance(ChampionStance::Idle);
	}
}

void Champion::Attack(float dt)
{
	this->currentState.animaition.Update(dt*this->currentState.attackSpeed);

	if (this->currentState.animaition.GetCurrentClipId() != "Attack")
	{
		this->currentState.animaition.Play("Attack");
	}
	if (this->currentState.animaition.GetLastFrame())
	{
		std::cout << "공격" << std::endl;
		if (this->currentState.charId == "priest" && this->taget->currentState.maxHp != this->taget->hp)
		{
			float heal = this->currentState.attack;
			if (heal >= this->taget->GetHp())
			{
				heal = this->taget->GetHp();
			}
			this->total_Damage += heal;
			std::cout << this->GetName() << " 회복량 : " << this->total_Damage << std::endl;
			Heal(heal);
			std::cout << this->taget->GetName() << " hp : " << this->taget->hp << std::endl;
		}
		else if (this->currentState.charId != "priest")
		{
			float damage = (this->currentState.attack * 100 + 99 + this->taget->currentState.defend) / (100 + this->taget->currentState.defend);
			if (damage <= 0.f)
			{
				damage = 0.f;
			}
			else if (damage >= this->taget->GetHp())
			{
				damage = this->taget->GetHp();
			}
			this->total_Damage += damage;
			std::cout << this->GetName() << " 입힌 피해 : " << this->total_Damage << std::endl;
			this->taget->Hit(damage);
			if (this->taget->GetHp() == 0)
			{
				this->kill++;
				std::cout << this->GetName() << " 킬 : " << this->kill << std::endl;
			}
		}
		attackDelay = 1.f;
		ChangeStance(ChampionStance::Idle);
	}
}

void Champion::Skill(float dt)
{
	this->currentState.animaition.Update(dt);

	if (this->currentState.animaition.GetCurrentClipId() != "Skill")
	{
		this->currentState.animaition.Play("Skill");
	}
	if (this->currentState.animaition.GetLastFrame())
	{
		std::cout << "스킬" << std::endl;
		skillTimer = 0;
		ChangeStance(ChampionStance::Idle);
	}
}

void Champion::SetState(State path)
{
	this->champMgrState.charId = path.charId;
	this->currentState.charId = path.charId;
	this->champMgrState.maxHp = path.maxHp;
	this->champMgrState.attack = path.attack;
	this->champMgrState.defend = path.defend;
	this->champMgrState.attackSpeed = path.attackSpeed;
	this->champMgrState.attackRange = path.attackRange;
	this->champMgrState.speed = path.speed;
	this->champMgrState.skillCode1 = path.skillCode1;
	this->currentState.skillCode1 = path.skillCode1;
	this->champMgrState.skillCode2 = path.skillCode2;
	this->currentState.skillCode2 = path.skillCode2;
	this->champMgrState.type = path.type;
	this->currentState.type = path.type;
	this->champMgrState.animaition = path.animaition;
	this->currentState.animaition = path.animaition;
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
		auto it = std::find(cemetery->begin(), cemetery->end(), this);
		this->myTeam->push_back(*it);
		this->cemetery->erase(it);

		switch (team)
		{
		case Team::Red:
		{
			SetPosition((Utils::RandomRange(420, 520)), (Utils::RandomRange(300, 450)));
			break;
		}
		case Team::Blue:
		{
			SetPosition((Utils::RandomRange(750, 850)), (Utils::RandomRange(300, 450)));
			break;
		}
		}
		this->sprite.setColor(sf::Color(255, 255, 255, 255));
		ChangeStance(ChampionStance::Idle);
	}
}

void Champion::ChampionDie()
{
		this->reviveTimer = 3.f;
		this->death++;
		auto it = std::find(myTeam->begin(), myTeam->end(), this);
		this->cemetery->push_back(*it);
		this->myTeam->erase(it);
		std::cout << this->GetName() << "챔피언 죽음" << std::endl;
		std::cout << this->GetName() << " 데스 : " << this->death << std::endl;
		this->sprite.setColor(sf::Color(0, 0, 0, 0));
		ChangeStance(ChampionStance::Dead);
}

void Champion::UpdateState()
{
	this->currentState.maxHp = this->champMgrState.maxHp;
	this->currentState.attack = this->champMgrState.attack;
	this->currentState.defend = this->champMgrState.defend;
	this->currentState.attackSpeed = this->champMgrState.attackSpeed;
	this->currentState.attackRange = this->champMgrState.attackRange;
	this->currentState.speed = this->champMgrState.speed;
}

void Champion::SetMyTeam(std::vector<Champion*>* myTeam)
{
	this->myTeam = myTeam;
}

void Champion::SetEnemyTeam(std::vector<Champion*>* enemyTeam)
{
	this->enemyTeam = enemyTeam;
}

void Champion::SetDieChampion(std::vector<Champion*>* cemetery)
{
	this->cemetery = cemetery;
}

void Champion::Hit(float attack)
{
	total_OnHit += attack;
	std::cout << this->GetName() << " 당한 피해 : " << this->total_OnHit << std::endl;
	this->hp -= attack;
}

void Champion::Heal(float heal)
{
	this->hp += heal;
}

void Champion::FindTaget()
{
	switch (this->currentOrder)
	{
	case TagetingOrder::Default:
	{
		if (this->currentState.charId == "priest")
		{
			TagetOrderH();
		}
		else
		{
			TagetOrderSR();
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
	case TagetingOrder::Heal:
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
	if (!enemyTeam->empty())
	{
		float tagetRange = -1.f;
		for (auto enemy : *enemyTeam)
		{
			float kda;
			if (enemy->GetDeathScore() == 0)
			{
				kda = 100.f;
			}
			else
			{
				kda =enemy->GetKillScore() / enemy->GetDeathScore();
			}
			if (kda > tagetRange)
			{
				tagetRange = kda;
				this->taget = enemy;
			}
		}
	}
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
	if (!enemyTeam->empty())
	{
		float tagetRange = 9999.f;
		for (auto enemy : *enemyTeam)
		{
			if (enemy->GetRowHealth() <= tagetRange)
			{
				tagetRange = enemy->GetRowHealth();
				this->taget = enemy;
			}
		}
	}
}

void Champion::TagetOrderH()
{
	if (!myTeam->empty())
	{
		float tagetRange = 9999.f;
		for (auto team : *myTeam)
		{
			if (team->GetRowHealth() <= tagetRange)
			{
				tagetRange = team->GetRowHealth();
				this->taget = team;
			}
		}
	}
}

void Champion::TagetOrderLR()
{
	if (!enemyTeam->empty())
	{
		float tagetRange = 0.f;
		for (auto enemy : *enemyTeam)
		{
			if (abs(Utils::Distance(this->GetPosition(), enemy->GetPosition())) >= tagetRange)
			{
				tagetRange = abs(Utils::Distance(this->GetPosition(), enemy->GetPosition()));
				this->taget = enemy;
			}
		}
	}
}

float Champion::GetRowHealth()
{
	return (this->hp/this->currentState.maxHp);
}
