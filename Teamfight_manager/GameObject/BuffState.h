#pragma once
#include "GameState.h"

class BuffState
{
private:
	BuffType buff;
	float buffValue = 0.f;
	int buffCount =0;
	float buffTimer = 0.f;

	bool side = false;

	BuffType sideBuff;
	float sideBuffValue = 0.f;
public:
	BuffState();
	virtual ~BuffState();

	void SetType(BuffType pick) { this->buff = pick; }
	BuffType GetType() { return this->buff; }
	void SetCount(int num) { this->buffCount = num; }
	int GetCount() { return this->buffCount; }
	void SetValue(float value) { this->buffValue = value; }
	void CalculateValue(float value) { this->buffValue += value; }
	float GetValue() { return this->buffValue; }

	void SetSide(bool set) { this->side = set; }
	bool GetSide() { return this->side; }

	void SetSideType(BuffType pick) { this->sideBuff = pick; }
	BuffType GetSideType() { return this->sideBuff; }
	void SetSideValue(float value) { this->sideBuffValue = value; }
	void CalculateSideValue(float value) { this->sideBuffValue += value; }
	float GetSideValue() { return this->sideBuffValue; }

	void Update(float dt);
};

