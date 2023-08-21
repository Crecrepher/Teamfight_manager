#pragma once
#include "GameState.h"

class BuffState
{
private:
	BuffType buff;
	float buffValue = 0.f;
	int buffCount =0;
	float buffTimer = 0.f;
public:
	BuffState();
	virtual ~BuffState();

	void SetType(BuffType pick) { this->buff = pick; }
	BuffType GetType() { return this->buff; }
	void SetCount(int num) { this->buffCount = num; }
	int GetCount() { return this->buffCount; }
	void SetValue(float value) { this->buffValue = value; }
	float GetValue() { return this->buffValue; }

	void Update(float dt);
};

