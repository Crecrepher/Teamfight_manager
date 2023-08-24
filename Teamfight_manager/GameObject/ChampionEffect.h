#pragma once
#include "SpriteGo.h"
#include "SceneMgr.h"
#include "ObjectPool.h"
#include "AnimatioControler.h"

class Champion;

enum class EffectType
{
	None,
	Ui,
	Base,
	Hp,
	CoolTime,
};

class ChampionEffect : public SpriteGo
{
protected:
	EffectType type;
	Champion* champ;
	float plusWidth = 0.f;
	float plusHight = 0.f;

	AnimatioControler effect;

	ObjectPool<ChampionEffect>* pool;
public:
	ChampionEffect(const std::string id = "", const std::string n = "");
	virtual ~ChampionEffect() override;

	virtual void Init()override;
	virtual void Reset()override;
	virtual void Release() override;
	virtual void Update(float dt)override;

	void SetPool(ObjectPool<ChampionEffect>* effect) { this->pool = effect; }
	void Setting();

	void SetEffectType(int pick) { this->type = (EffectType)pick; }

	void SetChampion(Champion* champion);
	Champion* GetChampion() { return this->champ; }
	void SetWidth(float num) { this->plusWidth = num; }
	void SetHight(float num) { this->plusHight = num; }

	void BaseUpdate(float dt);
	void UiUpdate(float dt);
	void HpUpdate(float dt);
	void CoolTimeUpdate(float dt);
};

