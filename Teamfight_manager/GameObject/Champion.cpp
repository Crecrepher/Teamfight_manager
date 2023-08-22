#include "stdafx.h"
#include "Champion.h"
#include "ResourceMgr.h"
#include "SceneGame.h"
#include "SceneMgr.h"
#include "SkillMgr.h"
#include "ChampionEffect.h"
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
	this->sprite.setColor(sf::Color::White);
	this->hp = currentState.maxHp;
	this->death = 0;
	this->kill = 0;
	this->total_Damage = 0.f;
	this->total_OnHit = 0.f;
	this->currentState.animaition = this->champMgrState.animaition;
	this->currentState.animaition.SetTarget(&sprite);
	this->currentState.animaition.Play("Idle");
	SetOrigin(Origins::MC);
}

void Champion::Release()
{
	SpriteGo::Release();
}

void Champion::BattleUpdate(float dt)
{
	if (this->currentStance == ChampionStance::None)
		return;

	if (this->bind >= 0.f)
	{
		bind -= dt;
	}

	SpriteGo::Update(dt);


	if (!field->rectangle.getGlobalBounds().contains(this->GetPosition()))
	{
		this->SetPosition(Utils::Clamp(this->GetPosition(), { 367,191 }, { 911,499 }));
	}

	this->SetOrigin(Origins::MC);

	if (this->currentState.animaition.GetCurrentClipId() == "Die")
	{
		this->currentState.animaition.Update(dt);
	}

	UpdateState(dt);

	this->skillTimer += dt/1.5f;
	this->UesUltiSkillTiming -= dt / 1.5f;

	if (this->UesUltiSkillTiming <= 0.f)
	{
		this->UesUltiSkillTiming = 0.f;
	}

	this->attackDelay -= dt * this->currentState.attackSpeed;


	//일시적 평타버그해결용 *******나중에지워야함*******
	////////////////////////////////////////////////////////
	///////////////////////지워주세요///////////////////////
	if (currentState.skillCode1 == 10 &&
		currentState.skillCode1 == 18)
	{
		skillTimer = 0;
	}

	if (this->currentState.charId !="knight" && this->currentState.charId != "monk")
	{
		ActiveUltiSkill = false;
	}
	////////////////////////////////////////////////////////

	if (this->skillTimer >= this->currentSkill[0].skillCoolTime)
	{
		this->skillTimer = this->currentSkill[0].skillCoolTime;
	}

	if (this->bloodingStack > 0 && this->dotDamage != nullptr)
	{
		this->bloodingTimer -= dt;

		if (this->bloodingTimer <= 0.f)
		{
			this->bloodingTimer = 0.5f;
			this->bloodingStack--;

			if (this->bloodingStack == 0||this->GetHp()==0)
			{
				//std::cout << "dot 끝" << std::endl;
				this->dotDamage == nullptr;
				return;
			}

			float damage = ((this->dotDamage->currentState.attack/15.f) * 100 + 99 +
				this->currentState.defend) / (100 + this->currentState.defend);

			if (damage >= this->GetHp())
			{
				damage = this->GetHp();
			}

			this->dotDamage->total_Damage += damage;
			//std::cout << this->dotDamage->GetName() << " 입힌 피해 : " << this->dotDamage->total_Damage << std::endl;
			this->Hit(damage);

			if (this->GetHp() == 0)
			{
				this->dotDamage->kill++;
				(*this->dotDamage->teamScore)++;
				this->bloodingStack = 0;
				//std::cout << this->dotDamage->GetName() << " 킬 : " << this->dotDamage->kill << std::endl;
				return;
			}
			//std::cout << "dot deal" << damage << std::endl;
		}
	}

	if (this->hp <= 0 && this->currentStance!=ChampionStance::Dead)
	{
		if (this->currentState.animaition.GetCurrentClipId() != "Die")
		{
			if (this->currentState.animaition.GetCurrentClipId() == "Skill"&&
				this->currentState.charId == "fighter")
			{
				this->target->SetAir(false);
			}
			this->currentState.animaition.Stop();
			this->currentState.animaition = this->champMgrState.animaition;
			this->currentState.animaition.SetTarget(&this->sprite);

			if(!this->currentBuff.empty())
			{ 

				for (auto it = this->currentBuff.begin(); it != this->currentBuff.end(); ++it)
				{
					std::cout << "캐릭터 :" << "\t" << this->currentState.charId << std::endl;
					std::cout << "Delete All" << std::endl;
					std::cout << "사용자 : " << "\t" << this << std::endl;
					std::cout << "BuffType : " << "\t" << (int)(*it)->GetType() << std::endl;
					std::cout << "주소값 : " << "\t" << (*it) << std::endl << std::endl;
					
					delete (*it);
				}
				this->currentBuff.clear();
			}


			auto it = std::find(myTeam->begin(), myTeam->end(), this);
			this->cemetery->push_back(*it);
			this->myTeam->erase(it);
			this->currentState.animaition.Play("Die");
			this->dotDamage = nullptr;
			this->bloodingStack = 0;
			this->limit = false;
			this->air = false;
			return;
		}

		if (this->currentState.animaition.GetLastFrame())
		{
			ChampionDie();
		}
		return;
	}	
	
	if (this->currentState.animaition.GetCurrentClipId() != "Die")
	{
		for (auto deBuff : this->currentBuff)
		{
			if (deBuff->GetType() == BuffType::STUN)
			{
				return;
			}
		}
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
		if (this->currentState.charId == "fighter" || this->currentState.charId == "berserker" || this->currentState.charId == "priest")
		{
			Skill(dt / 1.5f);
			break;
		}
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
	this->SetOrigin(Origins::MC);

	if (this->currentState.animaition.GetCurrentClipId() != "Idle")
	{
		this->currentState.animaition.Play("Idle");
		return;
	}

	if (this->currentState.charId == "priest")
	{
		if (!this->myTeam->empty())
		{
			FindTaget();
		}
		else if (this->myTeam->empty())
		{
			this->target = nullptr;
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
			this->target = nullptr;
		}
	}

	if (this->target != nullptr)
	{
		SetSacleX(Utils::Normalize(target->GetPosition() - this->position).x);

		if (this->currentState.charId=="ninja"|| this->currentState.charId=="knight"|| this->currentState.charId == "priest")
		{
			if (this->skillTimer >= this->currentSkill[0].skillCoolTime && this->currentSkill[0].skillCoolTime != 0.f)
			{
				ChangeStance(ChampionStance::Skill);
				return;
			}
		}
		else if (this->currentState.charId == "berserker"|| this->currentState.charId == "shieldbearer")
		{
			if (this->skillTimer >= this->currentSkill[0].skillCoolTime)
			{
				ChangeStance(ChampionStance::Skill);
				return;
			}
		}

		if (abs(Utils::Distance(this->GetPosition(), this->target->GetPosition())) > this->currentState.attackRange && bind<0.f)
		{
			ChangeStance(ChampionStance::Move);
			return;
		}
		else if (abs(Utils::Distance(this->GetPosition(), this->target->GetPosition())) <= this->currentState.attackRange)
		{
			ChangeStance(ChampionStance::Action);
			return;
		}
	}
}

void Champion::Move(float dt)
{
	this->currentState.animaition.Update(dt);
	this->SetOrigin(Origins::MC);

	if (this->bind >= 0.f)
	{
		ChangeStance(ChampionStance::Idle);
		return;
	}

	if (this->currentState.animaition.GetCurrentClipId() != "Move")
	{
		this->currentState.animaition.Play("Move");
		return;
	}

	if (!this->enemyTeam->empty())
	{
		FindTaget();
	}

	if (this->target != nullptr)
	{
		if (abs(Utils::Distance(this->GetPosition(), this->target->GetPosition())) <= this->currentState.attackRange)
		{
			ChangeStance(ChampionStance::Idle);
			return;
		}
	}
	SetPosition(this->position + Utils::Normalize(target->GetPosition() - this->position) * this->currentState.speed * dt *5.f);
}

void Champion::Action(float dt)
{
	if (abs(Utils::Distance(this->GetPosition(), this->target->GetPosition())) <= this->currentState.attackRange && this->UesUltiSkillTiming == 0.f && this->ActiveUltiSkill==true)
	{
		ChangeStance(ChampionStance::UltimateSkill);
		return;
	}

	if (abs(Utils::Distance(this->GetPosition(), this->target->GetPosition())) <= this->currentState.attackRange && this->skillTimer >= this->currentSkill[0].skillCoolTime)
	{
		if(this->currentState.charId=="berserker"&&this->attackDelay>0.f)
		{ 
			return;
		}
		else if (this->currentState.charId == "shieldbearer" && this->attackDelay < 0.f)
		{
			this->ChangeStance(ChampionStance::Attack);
			return;
		}
		ChangeStance(ChampionStance::Skill);
		return;
	}
	else if (abs(Utils::Distance(this->GetPosition(), this->target->GetPosition())) <= this->currentState.attackRange && this->attackDelay <= 0.f && this->currentState.charId=="priest")
	{
		if (this->currentState.charId == "priest" && this->GetTarget()->currentState.maxHp == this->GetTarget()->GetHp())
		{
			ChangeStance(ChampionStance::Idle);
			return;
		}
		else if (this->currentState.charId == "priest" && this->GetTarget()->GetHp() == 0)
		{
			ChangeStance(ChampionStance::Idle);
			return;
		}
		else
		{
			ChangeStance(ChampionStance::Attack);
			return;
		}
	}
	else if (abs(Utils::Distance(this->GetPosition(), this->target->GetPosition())) <= this->currentState.attackRange&&this->attackDelay<=0.f)
	{
		ChangeStance(ChampionStance::Attack);
		return;
	}
	else
	{
		ChangeStance(ChampionStance::Idle);
		return;
	}
}

void Champion::Attack(float dt)
{
	this->currentState.animaition.Update(dt*this->currentState.attackSpeed);
	this->sMoveT += dt * this->currentState.attackSpeed;
	this->SetOrigin(Origins::MC);

	if (this->currentState.animaition.GetCurrentClipId() != "Attack")
	{
		this->currentState.animaition.Play("Attack");
		return;
	}

	if (!this->currentState.animaition.GetLastFrame() && this->GetTarget()->GetHp() == 0)
	{
		//std::cout << "이미 죽음" << std::endl;
		ChangeStance(ChampionStance::Idle);
		return;
	}

	if (this->currentState.animaition.GetLastFrame())
	{
		//std::cout << "공격" << std::endl;
		if (this->currentState.charId == "priest")
		{
			HealCalculate(this->currentState.attack);
		}
		else if (this->currentState.charId != "priest")
		{
			DamageCalculate(this->currentState.attack);
		}
		this->attackDelay = 1.f;
		ChangeStance(ChampionStance::Idle);
		return;
	}
}

void Champion::Skill(float dt)
{
	if (this->currentState.charId == "berserker"&& abs(Utils::Distance(this->GetPosition(), this->target->GetPosition())) > this->currentState.attackRange)
	{
		ChangeStance(ChampionStance::Move);
		return;
	}
	this->currentState.animaition.Update(dt*this->currentState.attackSpeed);
	this->SetOrigin(Origins::MC);
	this->sMoveT += dt * this->currentState.attackSpeed;
	SKILL_MGR.ActiveSkill(this->currentState.skillCode1, this);
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
	SetShadow();
	SetHpGuage();
}

void Champion::SetSkill(ChampionSkill code)
{
	this->skillMgrSkill = code;
	this->currentSkill.push_back(this->skillMgrSkill);
}

void Champion::ReleaseSkill()
{
	if (this->currentSkill.empty())
	{
		return;
	}
	
	this->currentSkill.clear();
	return;
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
	this->currentState.animaition.Update(dt * this->currentState.attackSpeed);
	this->SetOrigin(Origins::MC);
	this->sMoveT += dt * this->currentState.attackSpeed;
	SKILL_MGR.ActiveSkill(this->currentState.skillCode2, this);
}

void Champion::Dead(float dt)
{
	this->reviveTimer -= dt;

	if (this->reviveTimer <= 0)
	{
		this->hp = this->currentState.maxHp;
		this->target = nullptr;
		auto it = std::find(cemetery->begin(), cemetery->end(), this);
		this->myTeam->push_back(*it);
		this->cemetery->erase(it);

		switch (team)
		{
		case Team::Red:
		{
			SetPosition((Utils::RandomRange(750, 850)), (Utils::RandomRange(300, 450)));
			break;
		}
		case Team::Blue:
		{
			SetPosition((Utils::RandomRange(420, 520)), (Utils::RandomRange(300, 450)));
			break;
		}
		}
		this->sprite.setColor(sf::Color(255, 255, 255, 255));
		ChangeStance(ChampionStance::Idle);
		this->SetShadow();
		this->SetHpGuage();
		return;
	}
}

void Champion::ChampionDie()
{
	this->reviveTimer = 3.f;
	this->death++;
	//std::cout << this->GetName() << "챔피언 죽음" << std::endl;
	//std::cout << this->GetName() << " 데스 : " << this->death << std::endl;
	this->sprite.setColor(sf::Color(0, 0, 0, 0));
	ChangeStance(ChampionStance::Dead);
	return;
}

void Champion::UpdateState(float dt)
{
	this->currentState.maxHp = this->champMgrState.maxHp;
	this->currentState.attack = this->champMgrState.attack;
	this->currentState.defend = this->champMgrState.defend;
	this->currentState.attackSpeed = this->champMgrState.attackSpeed;
	this->currentState.attackRange = this->champMgrState.attackRange;
	this->currentState.speed = this->champMgrState.speed;

	if (!this->currentBuff.empty())
	{
		for (auto it = this->currentBuff.begin(); it != this->currentBuff.end(); )
		{
			(*it)->Update(dt);

			if ((*it)->GetCount() == 0)
			{
				if ((*it)->GetType() == BuffType::AGGRO)
				{
					this->SetOrder(TargetingOrder::Default);
				}

					std::cout << "캐릭터 :" << "\t" << this->currentState.charId << std::endl;
					std::cout << "erase Buff" << std::endl;
					std::cout << "사용자 : " << "\t" << this << std::endl;
					std::cout << "BuffType : " << "\t" << (int)(*it)->GetType() << std::endl;
					std::cout << "주소값 : " << "\t" << (*it) << std::endl << std::endl;

				delete (*it);
				it = this->currentBuff.erase(it);
				continue;
			}

			if ((*it)->GetType() == BuffType::BARRIER && (*it)->GetValue() == 0)
			{
				std::cout << "캐릭터 :" << "\t" << this->currentState.charId << std::endl;
				std::cout << "erase barrier" << std::endl;
				std::cout << "사용자 : " << "\t" << this << std::endl;
				std::cout << "BuffType : " << "\t" << (int)(*it)->GetType() << std::endl;
				std::cout << "주소값 : " << "\t" << (*it) << std::endl << std::endl;

				delete (*it);
				it = this->currentBuff.erase(it);
				continue;
			}
			switch ((*it)->GetType())
			{
			case BuffType::MAXHP:
			{
				this->currentState.maxHp += (*it)->GetValue();
				break;
			}
			case BuffType::ATTACK:
			{
				this->currentState.attack += (*it)->GetValue();
				break;
			}
			case BuffType::DEFEND:
			{
				this->currentState.defend += (*it)->GetValue();
				break;
			}
			case BuffType::ATTACKSPEED:
			{
				this->currentState.attackSpeed += (*it)->GetValue();
				break;
			}
			case BuffType::ATTACKRANGE:
			{
				this->currentState.attackRange += (*it)->GetValue();
				break;
			}
			case BuffType::SPEED:
			{
				this->currentState.speed += (*it)->GetValue();
				break;
			}
			case BuffType::BARRIER:
			{
				if ((*it)->GetSide())
				{
					switch ((*it)->GetSideType())
					{
					case BuffType::MAXHP:
					{
						this->currentState.maxHp += (*it)->GetSideValue();
						break;
					}
					case BuffType::ATTACK:
					{
						this->currentState.attack += (*it)->GetSideValue();
						break;
					}
					case BuffType::DEFEND:
					{
						this->currentState.defend += (*it)->GetSideValue();
						break;
					}
					case BuffType::ATTACKSPEED:
					{
						this->currentState.attackSpeed += (*it)->GetSideValue();
						break;
					}
					case BuffType::ATTACKRANGE:
					{
						this->currentState.attackRange += (*it)->GetSideValue();
						break;
					}
					case BuffType::SPEED:
					{
						this->currentState.speed += (*it)->GetSideValue();
						break;
					}
					}
				}
				break;
			}
			}
			++it;
		}
	}
}

void Champion::UseSkill()
{
	this->currentState.animaition = this->currentSkill[0].animaition;
	this->currentState.animaition.SetTarget(&this->sprite);
	this->currentState.animaition.Play("Skill");
}

void Champion::UseUltiSkill()
{
	this->currentState.animaition = this->currentSkill[1].animaition;
	this->currentState.animaition.SetTarget(&this->sprite);
	this->currentState.animaition.Play("UltiSkill");
}

void Champion::SkillChangeIdle()
{
	this->skillTimer = 0.f;
	this->currentState.animaition = this->champMgrState.animaition;
	this->currentState.animaition.SetTarget(&this->sprite);
	this->attackDelay = 1.0f;
	ChangeStance(ChampionStance::Idle);
}

Champion* Champion::GetTarget()
{
	return this->target;
}

void Champion::SetTarget(Champion* champ)
{
	this->target = champ;
}

void Champion::SetBuff(BuffState* state)
{

	auto it = std::find(currentBuff.begin(), currentBuff.end(), state);

	std::cout << "캐릭터 :" << "\t" << this->currentState.charId << std::endl;
		std::cout << "SetBuff" << std::endl;
		std::cout << "사용자 : " << "\t" << this << std::endl;
		std::cout << "BuffType : " << "\t" << (int)state->GetType() << std::endl;
		std::cout << "주소값 : " << "\t" << state << std::endl << std::endl;

	this->currentBuff.push_back(state);
}

void Champion::SetMyTeamBuff(BuffState* state)
{
	for (auto team : *this->myTeam)
	{

		BuffState* cState = new BuffState;

		*cState = *state;

		std::cout << "캐릭터 :" << "\t" << this->currentState.charId << std::endl;
			std::cout << "사용자 : " << "\t" << this << std::endl;
			std::cout << "BuffType : " << "\t" << (int)state->GetType() << std::endl;
			std::cout << "주소값 : " << "\t" << cState << std::endl << std::endl;

		team->currentBuff.push_back(cState);
	}
	delete state;
}

float Champion::GetBarrier()
{
	float total_Barrier = 0.f;
	for (auto it = this->currentBuff.begin(); it != this->currentBuff.end(); ++it)
	{
		if ((*it)->GetType() == BuffType::BARRIER)
		{
			total_Barrier = (*it)->GetValue();
		}
	}

	return total_Barrier;
}

void Champion::TargetRangeDamage(float range, float value)
{
	if (this->enemyTeam->empty())
	{
		return;
	}

	for (auto enemy : *this->enemyTeam)
	{
		if (abs(Utils::Distance(this->GetTarget()->GetPosition(), enemy->GetPosition())) <= range)
		{
			float damage = (value * 100 + 99 +
				enemy->currentState.defend) / (100 + enemy->currentState.defend);
			if (damage <= 0.f)
			{
				damage = 0.f;
			}
			else if (damage >= enemy->GetHp() + enemy->GetBarrier())
			{
				damage = enemy->GetHp() + enemy->GetBarrier();
			}
			this->total_Damage += damage;
			//std::cout << this->GetName() << " mu 입힌 피해 : " << enemy->currentState.charId << std::endl;
			enemy->Hit(damage);
			if (enemy->GetHp() == 0)
			{
				this->kill++;
				(*this->teamScore)++;
				//std::cout << this->GetName() << " 킬 : " << this->kill << std::endl;
				return;
			}
		}
	}
}

bool Champion::CurrBuffEmpty()
{
	if (this->currentBuff.empty())
	{
		return true;
	}

	return false;
}

void Champion::BuffUpdate(float dt)
{
	if (!this->currentBuff.empty())
	{
		for (auto it = this->currentBuff.begin(); it != this->currentBuff.end(); ++it)
		{
			if ((*it)->GetCount() > 900)
			{
				switch ((*it)->GetType())
				{
				case BuffType::ATTACK:
				{
					(*it)->SetValue(3.f / dt);
					(*it)->SetCount(999);
					break;
				}
				case BuffType::ATTACKSPEED:
				{
					(*it)->SetValue(0.2f / dt);
					(*it)->SetCount(999);
					break;
				}
				case BuffType::SPEED:
				{
					(*it)->SetValue(0.8f / dt);
					(*it)->SetCount(999);
					break;
				}
				}
			}
		}
	}
}

void Champion::SetShadow()
{
	ChampionEffect* shadow = this->pool->Get();
	sf::IntRect setting = { 0, 0, 20, 10 };
	shadow->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/player_shadow.png"));
	shadow->SetOrigin(Origins::MC);
	shadow->sprite.setTextureRect(setting);
	shadow->SetChampion(this);
	shadow->SetEffectType(2);
	shadow->SetHight(12.f);
	shadow->sortLayer = 1;

	SCENE_MGR.GetCurrScene()->AddGo(shadow);
}

void Champion::SetHpGuage()
{
	sf::IntRect setting = { 0,0,30,6 };
	ChampionEffect* Guage = this->pool->Get();
	Guage->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/player_ingame_guage_5.png"));
	Guage->sprite.setTextureRect(setting);
	Guage->SetOrigin(Origins::MC);
	Guage->SetChampion(this);
	Guage->SetEffectType(1);
	Guage->SetHight(23.f);
	Guage->sortLayer = 4;
	Guage->sortOrder = 2;
	SCENE_MGR.GetCurrScene()->AddGo(Guage);

	ChampionEffect* hpGuageBg = this->pool->Get();
	hpGuageBg->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/player_ingame_guage_0.png"));
	hpGuageBg->sprite.setTextureRect(setting);
	hpGuageBg->SetOrigin(Origins::MC);
	hpGuageBg->SetChampion(this);
	hpGuageBg->SetEffectType(1);
	hpGuageBg->SetHight(23.f);
	hpGuageBg->sortLayer = 4;
	hpGuageBg->sortOrder = 0;
	SCENE_MGR.GetCurrScene()->AddGo(hpGuageBg);


	setting = { 0,0,29,2 };
	ChampionEffect* hpGuage = this->pool->Get();
	if (this->team == Team::Red)
	{
		hpGuage->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/player_ingame_guage_4.png"));
	}
	else if (this->team == Team::Blue)
	{
		hpGuage->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/player_ingame_guage_3.png"));
	}
	hpGuage->sprite.setTextureRect(setting);
	hpGuage->SetOrigin(Origins::ML);
	hpGuage->SetChampion(this);
	hpGuage->SetEffectType(5);
	hpGuage->SetHight(22.f);
	hpGuage->SetWidth(-14.5f);
	hpGuage->sortLayer = 4;
	hpGuage->sortOrder = 1;
	SCENE_MGR.GetCurrScene()->AddGo(hpGuage);

	setting = { 0,0,30,1 };
	ChampionEffect* coolGuage = this->pool->Get();
	coolGuage->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/CustomSprite/player_ingame_guage_2.png"));
	coolGuage->sprite.setTextureRect(setting);
	coolGuage->SetOrigin(Origins::ML);
	coolGuage->SetChampion(this);
	coolGuage->SetEffectType(6);
	coolGuage->SetHight(24.f);
	coolGuage->SetWidth(-14.5f);
	coolGuage->sortLayer = 4;
	coolGuage->sortOrder = 1;
	SCENE_MGR.GetCurrScene()->AddGo(coolGuage);
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
	this->total_OnHit += attack;
	//std::cout << this->GetName() << " 당한 피해 : " << this->total_OnHit << std::endl;
	if (this->GetBarrier() > 0.f)
	{
		if (this->GetBarrier() >= attack)
		{
			for (auto it = this->currentBuff.begin(); it != this->currentBuff.end(); )
			{
				if ((*it)->GetType() == BuffType::BARRIER)
				{
					(*it)->CalculateValue(-attack);
					//std::cout << "배리어 수치" << " " << (*it)->GetValue()<<std::endl;
					return;
				}
				else
				{
					++it;
				}
			}
		}
		else if (this->GetBarrier() < attack)
		{
			attack -= this->GetBarrier();
			for (auto it = this->currentBuff.begin(); it != this->currentBuff.end(); )
			{
				if ((*it)->GetType() == BuffType::BARRIER)
				{
					(*it)->SetValue(0.f);
					//std::cout << "배리어 수치" << " " << (*it)->GetValue();
					break;
				}
				else
				{
					++it;
				}
			}
		}
	}
	this->hp -= attack;
}

void Champion::Heal(float heal)
{
	this->hp += heal;
}

void Champion::DamageCalculate(float attack)
{
	float damage = (attack * 100 + 99 +
		this->target->currentState.defend) / (100 + this->target->currentState.defend);
	
	for (auto enemy : *this->enemyTeam)
	{
		if (enemy->currentState.charId == "shieldbearer" && this->GetTarget()->GetInteranction())
		{
			Champion* temp = this->GetTarget();
			this->target = enemy;

			float interactionDamage = damage / 3;

			damage -= interactionDamage;

			if (interactionDamage <= 0.f)
			{
				interactionDamage = 0.f;
			}
			else if (interactionDamage >= this->target->GetHp() + this->target->GetBarrier())
			{
				interactionDamage = this->target->GetHp() + this->target->GetBarrier();
			}
			this->total_Damage += interactionDamage;
			//std::cout << this->GetName() << " 입힌 피해 : " << this->total_Damage << std::endl;
			this->target->Hit(interactionDamage);
			if (this->target->GetHp() == 0)
			{
				this->kill++;
				(*this->teamScore)++;
				//std::cout << this->GetName() << " 킬 : " << this->kill << std::endl;
				this->target=temp;
				break;
			}
			this->target = temp;
		}
	}
	
	if (damage <= 0.f)
	{
		damage = 0.f;
	}

	else if (damage >= this->target->GetHp()+this->target->GetBarrier())
	{
		damage = this->target->GetHp() + this->target->GetBarrier();
	}
	
	this->total_Damage += damage;
	//std::cout << this->GetName() << " 입힌 피해 : " << this->total_Damage << std::endl;
	this->target->Hit(damage);
	if (this->target->GetHp() == 0)
	{
		this->kill++;
		(*this->teamScore)++;
		//std::cout << this->GetName() << " 킬 : " << this->kill << std::endl;
		return;
	}
}

void Champion::HealCalculate(float attack)
{
	float heal = attack;
	if (heal+this->target->GetHp() >= this->target->GetCurretState().maxHp)
	{
		heal = this->target->GetCurretState().maxHp - this->target->GetHp();
	}
	this->total_Damage += heal;
	//std::cout << this->GetName() << " 회복량 : " << this->total_Damage << std::endl;
	this->target->Heal(heal);
	//std::cout << this->target->GetName() << " hp : " << this->target->hp << std::endl;
}

void Champion::FindTaget()
{
	switch (this->currentOrder)
	{
	case TargetingOrder::Default:
	{
		if (this->currentState.charId == "priest")
		{
			TargetOrderH();
		}
		else
		{
			TargetOrderSR();
		}
		break;
	}
	case TargetingOrder::ShortRange:
	{
		TargetOrderSR();
		break;
	}
	case TargetingOrder::LongRange:
	{
		TargetOrderLR();
		break;
	}
	case TargetingOrder::CarryPlayer:
	{
		TargetOrderCP();
		break;
	}
	case TargetingOrder::Heal:
	{
		TargetOrderH();
		break;
	}
	case TargetingOrder::RowHealth:
	{
		TargetOrderRH();
		break;
	}
	case TargetingOrder::CircleInRangeEnemy:
	{
		TargetOrderCIE(0, 0.f,0.f);
		break;
	}
	case TargetingOrder::CircleInRangeTeam:
	{
		TargetOrderCIT(0, 0.f,0.f);
		break;
	}
	case TargetingOrder::Aggro:
	{
		TargetAggro();
		break;
	}
	}
}

void Champion::TargetOrderCP()
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
				kda = enemy->GetKillScore() / enemy->GetDeathScore();
			}
			if (kda > tagetRange)
			{
				tagetRange = kda;
				this->target = enemy;
			}
		}
	}
}

void Champion::TargetOrderSR()
{
	if (!enemyTeam->empty())
	{
		float tagetRange = 9999.f;
		for (auto enemy : *enemyTeam)
		{
			if (abs(Utils::Distance(this->GetPosition(), enemy->GetPosition())) <= tagetRange)
			{
				tagetRange = abs(Utils::Distance(this->GetPosition(), enemy->GetPosition()));
				this->target = enemy;
			}
		}
	}
}
	
void Champion::TargetOrderRH()
{
	if (!enemyTeam->empty())
	{
		float tagetRange = 9999.f;
		for (auto enemy : *enemyTeam)
		{
			if (enemy->GetRowHealth() <= tagetRange)
			{
				tagetRange = enemy->GetRowHealth();
				this->target = enemy;
			}
		}
	}
}

void Champion::TargetOrderH()
{
	if (!myTeam->empty())
	{
		float tagetRange = 9999.f;
		for (auto team : *myTeam)
		{
			if (team == this)
			{
				continue;
			}

			if (team->GetRowHealth() <= tagetRange)
			{
				if (team->GetRowHealth() == 0)
				{
					this->target = nullptr;
				}
				tagetRange = team->GetRowHealth();
				this->target = team;
			}
		}
	}
}

void Champion::TargetOrderLR()
{
	if (!enemyTeam->empty())
	{
		float tagetRange = 0.f;
			for (auto enemy : *enemyTeam)
			{
				if (abs(Utils::Distance(this->GetPosition(), enemy->GetPosition())) >= tagetRange)
				{
					tagetRange = abs(Utils::Distance(this->GetPosition(), enemy->GetPosition()));
					this->target = enemy;
				}
			}
		}
	}

void Champion::TargetOrderCIE(int code, float range, float value)
{
	if (this->enemyTeam->empty())
	{
		return;
	}
		for (auto enemy : *this->enemyTeam)
		{
			if (abs(Utils::Distance(this->GetPosition(), enemy->GetPosition())) <= range)
			{
				switch (code)
				{
				case 0:
				{
					//std::cout << "아무 일도 없었다." << std::endl;
					break;
				}
				case 1:
				{
					DamageCalculate(value);
					break;
				}

				}
			}
		}
	}

void Champion::TargetOrderCIT(int code, float range, float value)
{
	if (this->myTeam->empty())
	{
		return;
	}

	for (auto team : *this->myTeam)
	{
		if (abs(Utils::Distance(this->GetPosition(), team->GetPosition())) <= range)
		{
			this->target = team;
			switch (code)
			{
			case 0:
			{
				//std::cout << "아무 일도 없었다." << std::endl;
				break;;
			}
			case 1:
			{
				if (this->target->GetHp() < this->target->currentState.maxHp)
				{
					//std::cout << "monkHeal" << std::endl;
					HealCalculate(value);
				}
				break;
			}
			case 2:
			{
				if (team == this)
				{
					break;
				}
				HealCalculate(value);
				break;
			}
			case 3:
			{
				this->GetTarget()->SetInteraction(true);
				break;
			}
			}
		}
		
		if (code == 3 && abs(Utils::Distance(this->GetPosition(), team->GetPosition())) > range)
		{
			team->SetInteraction(false);
		}
	}
}

void Champion::TargetOrderCITB(int code, float range, BuffState* state)
{
	if (this->myTeam->empty())
	{
		return;
	}

	for (auto team : *this->myTeam)
	{
		if (abs(Utils::Distance(this->GetPosition(), team->GetPosition())) <= range)
		{
			this->target = team;
			switch (code)
			{
			case 0:
			{
				//std::cout << "아무 일도 없었다." << std::endl;
				break;;
			}
			case 1:
			{
				//std::cout << "monkbarrier" << std::endl;
				BuffState* cState = new BuffState;

				*cState = *state;

				this->GetTarget()->SetBuff(cState);
				break;
			}
			}
		}
		
	}
	delete state;
}

void Champion::TargetAggro()
{
	if (!this->enemyTeam->empty())
	{
		for (auto enemy : *this->enemyTeam)
		{
			if (enemy == this->GetTarget())
			{
				return;
			}
		}

		this->SetOrder(TargetingOrder::Default);
	}
}

float Champion::GetRowHealth()
{
	return (this->hp/this->currentState.maxHp);
}
