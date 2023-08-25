#pragma once
#include "SpriteGo.h"
#include "ObjectPool.h"


class BindPlayerEffect : public SpriteGo
{
protected:
	float duration = 0.f;
	float timer = 0.f;

	ObjectPool<BindPlayerEffect>* pool = nullptr;

public:
	BindPlayerEffect(const std::string id = "", const std::string n = "");
	virtual ~BindPlayerEffect() override {};

	void SetDuration(float duration) { this->duration = duration; }
	virtual void SetPool(ObjectPool<BindPlayerEffect>* pool) { this->pool = pool; }
	virtual void Init()override;
	virtual void Reset()override;
	virtual void Release() override;
	virtual void Update(float dt)override;
};

