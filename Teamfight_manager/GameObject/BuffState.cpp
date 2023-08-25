#include "stdafx.h"
#include "BuffState.h"

BuffState::BuffState()
{
}

BuffState::~BuffState()
{
}

void BuffState::Update(float dt)
{
	if (this->buffCount > 0)
	{
		this->buffTimer += dt/2.f;

		if (this->buffTimer >= 1.f)
		{
			this->buffTimer = 0.f;
			this->buffCount--;
			if (this->GetType() == BuffType::COPY)
			{
				std::cout << "닌자 카운트 " << this->buffCount << std::endl;
			}
		}
	}
}
