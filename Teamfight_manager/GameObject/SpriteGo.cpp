#include "stdafx.h"
#include "SpriteGo.h"
#include "Utils.h"
#include "ResourceMgr.h"

SpriteGo::SpriteGo(const std::string id, const std::string n,sf::Vector2f spriteDir)
	:GameObject(n),textureId(id)
{

}

SpriteGo::~SpriteGo()
{
}

void SpriteGo::SetPosition(float x, float y)
{
	position.x = x;
	position.y = y;
	sprite.setPosition(position);
}

void SpriteGo::SetPosition(const sf::Vector2f& p)
{
	position = p;
	sprite.setPosition(p);
}

void SpriteGo::SetOrigin(Origins origin)
{
	GameObject::SetOrigin(origin);
	if (this->origin != Origins::CUSTOM)
	{
		Utils::SetOrigin(sprite, origin);
	}
}

void SpriteGo::SetOrigin(float x, float y)
{
	GameObject::SetOrigin(x, y);
	sprite.setOrigin(x, y);
}

void SpriteGo::SetSize(float xSize, float ySize)
{
	sprite.setScale(xSize, ySize);
}

sf::Vector2f SpriteGo::GetSize() const
{
	sf::FloatRect rect = sprite.getLocalBounds();
	return sf::Vector2f(rect.width, rect.height);
}

void SpriteGo::Init()
{

}

void SpriteGo::Reset()
{
	sf::Texture* tex = RESOURCE_MGR.GetTexture(textureId);
	if (tex != nullptr)
	{
		sprite.setTexture(*tex);
	}
	SetOrigin(origin);

}

void SpriteGo::Release()
{
}

void SpriteGo::Update(float dt)
{
	
}

void SpriteGo::Draw(sf::RenderWindow& window)
{
	window.draw(sprite);
}
