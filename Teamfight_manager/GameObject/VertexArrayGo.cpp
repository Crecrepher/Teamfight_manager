#include "stdafx.h"
#include "VertexArrayGo.h"
#include "ResourceMgr.h"


VertexArrayGo::VertexArrayGo(const std::string id, const std::string n)
	:GameObject(n), textureId(id), texture(nullptr)
{
}

VertexArrayGo::~VertexArrayGo()
{
}

void VertexArrayGo::SetPosition(float x, float y)
{
	SetPosition({ x,y });
}

void VertexArrayGo::SetPosition(const sf::Vector2f& p)
{
	sf::Vector2f diff = p - position;
	position = p;

	for (int i = 0; i < vertexArray.getVertexCount(); i++)
	{
		vertexArray[i].position += diff;
	}
}

void VertexArrayGo::SetOrigin(Origins origin)
{
	GameObject::SetOrigin(origin);

	if (origin == Origins::CUSTOM)
	{
		return;
	}

	sf::Vector2f prevOrigin = originPosition;
	sf::FloatRect bounds = vertexArray.getBounds();
	originPosition.x = bounds.width * ((int)origin % 3)*0.5f;
	originPosition.y = bounds.height * ((int)origin / 3)*0.5f;

	sf::Vector2f diff = prevOrigin - originPosition;
	for (int i = 0; i < vertexArray.getVertexCount(); i++)
	{
		vertexArray[i].position += diff;
	}
}

void VertexArrayGo::SetOrigin(float x, float y)
{
	GameObject::SetOrigin(x,y);

	if (origin == Origins::CUSTOM)
	{
		return;
	}

	sf::Vector2f prevOrigin = originPosition;
	originPosition.x = x;
	originPosition.y =y;

	sf::Vector2f diff = prevOrigin - originPosition;
	for (int i = 0; i < vertexArray.getVertexCount(); i++)
	{
		vertexArray[i].position += diff;
	}
}

void VertexArrayGo::Init()
{
}

void VertexArrayGo::Release()
{
}

void VertexArrayGo::Reset()
{
	texture = RESOURCE_MGR.GetTexture(textureId);
}

void VertexArrayGo::Update(float dt)
{
}

void VertexArrayGo::Draw(sf::RenderWindow& window)
{
	window.draw(vertexArray, texture);
}

void VertexArrayGo::MakeMap(int height, int wide , int size,sf::Vector2f startPos)
{
	vertexArray.setPrimitiveType(sf::Quads);
	vertexArray.resize(height*wide*4);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < wide; j++)
		{
			vertexArray[(4 * j) + (i * wide * 4)].position = { static_cast<float>(j) * size + startPos.x,static_cast<float>(i) * size + startPos.y };
			vertexArray[(4 * j) + (i * wide * 4) + 1].position = { static_cast<float>(j) * size + size + startPos.x,static_cast<float>(i) * size + startPos.y };
			vertexArray[(4 * j) + (i * wide * 4) + 2].position = { static_cast<float>(j) * size + size + startPos.x, static_cast<float>(i) * size + size + startPos.y };
			vertexArray[(4 * j) + (i * wide * 4) + 3].position = { static_cast<float>(j) * size + startPos.x,static_cast<float>(i) * size + size + startPos.y };
		}
	}
	

}
