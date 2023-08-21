#include "stdafx.h"
#include "TextGo.h"

TextGo::TextGo(const std::string n)
	:GameObject(n)
{
}

TextGo::~TextGo()
{
}

void TextGo::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
	text.setPosition(position);
}

void TextGo::SetPosition(const sf::Vector2f& p)
{
	position = p;
	text.setPosition(p);
}

void TextGo::SetOrigin(Origins origin)
{
	Utils::SetOrigin(text, origin);
}

void TextGo::SetOrigin(float x, float y)
{
	text.setOrigin(x, y);
}


void TextGo::Init()
{

}

void TextGo::Reset()
{
}

void TextGo::Release()
{
}

void TextGo::Update(float dt)
{
	
}

void TextGo::Draw(sf::RenderWindow& window)
{
	window.draw(text);
}
