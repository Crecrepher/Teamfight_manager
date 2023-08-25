#pragma once
#include "SpriteGo.h"
#include "ObjectPool.h"
#include "AnimatioControler.h"


class Champion;

enum class SkillType
{
	None=-1,
	Public,
	Icemage,
	Pyromancer,
	Magicknight,
	Priest,
	PyromancerPSkill,
	Pythoness,
	MagicknightSkill,
};

class SkillObject : public SpriteGo
{
protected:
	SkillType type;
	Champion* champ;

	AnimatioControler effect;

	float objectTimer = 0.f;
	float effectTime = 0.f;
	float effectTimer = 0.f;;
	float scaleY = 0.f;

	sf::Vector2f dir = { 0,0 };
	std::vector<Champion*> check;

	ObjectPool<SkillObject>* pool;
public:
	SkillObject(const std::string id = "", const std::string n = "");
	virtual ~SkillObject() override;

	virtual void Init()override;
	virtual void Reset()override;
	virtual void Release() override;
	void Update(float dt);

	void IcemageUpdate(float dt);
	void MagicknightUpdate(float dt);
	void PriestUpdate(float dt);
	void PyromancerUpdate(float dt);
	void PyromancerSkillUpdate(float dt);
	void PythonessUpdate(float dt);
	void MagicknightSkillUpdate(float dt);


	void SetPool(ObjectPool<SkillObject>* object) { this->pool = object; }
	void SetChampion(Champion* champion);
	Champion* GetChampion() { return this->champ; }

	void SetAni(std::string path);
	void SetScaleY(float y) { this->scaleY = y; }
	void PlayAni(std::string path) { this->effect.Play(path); }
	void SetType(int i) { this->type = (SkillType)i; }
	void SetObjectTimer(float t) { this->objectTimer = t; }
	void SetEffectTimer(float t) { this->effectTimer = t; }
	void SetEffectTime(float t) { this->effectTime = t; }
	void SetDir(sf::Vector2f d) { this->dir = d; }
	void SetAngle(sf::Vector2f d) { this->sprite.setRotation(Utils::Angle(d)); }
	float GetObjectTimer() { return this->objectTimer; }
};

