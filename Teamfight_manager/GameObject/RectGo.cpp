#include "stdafx.h"
#include "RectGo.h"

RectGo::RectGo(const std::string n)
	:GameObject(n)
{
}

RectGo::~RectGo()
{
}

void RectGo::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
	rectangle.setPosition(position);
}

void RectGo::SetPosition(const sf::Vector2f& p)
{
	position = p;
	rectangle.setPosition(p);
}

void RectGo::SetOrigin(Origins origin)
{
	Utils::SetOrigin(rectangle, origin);
}

void RectGo::SetOrigin(float x, float y)
{
	rectangle.setOrigin(x, y);
}

void RectGo::SetSize(const sf::Vector2f& size)
{
	rectangle.setSize(size);
}

void RectGo::Init()
{

}

void RectGo::Reset()
{
}

void RectGo::Release()
{
}

void RectGo::Update(float dt)
{
	
}

void RectGo::Draw(sf::RenderWindow& window)
{
	window.draw(rectangle);
}
