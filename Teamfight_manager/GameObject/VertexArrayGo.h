#pragma once
#include "GameObject.h"
class VertexArrayGo : public GameObject
{
protected:
	std::string textureId;
	sf::Texture* texture;
public:
	sf::VertexArray vertexArray;
	sf::Vector2f originPosition;

	VertexArrayGo(const std::string id = "", const std::string n = "");
	virtual~VertexArrayGo() override;

	virtual void SetPosition(float x, float y) override;
	virtual void SetPosition(const sf::Vector2f& p) override;

	virtual void SetOrigin(Origins origin) override;
	virtual void SetOrigin(float x, float y) override;

	virtual void Init() override;
	virtual void Release() override;

	virtual void Reset() override;

	virtual void Update(float dt) override;
	virtual void Draw(sf::RenderWindow& window) override;

	void MakeMap(int height, int wide,int size, sf::Vector2f startPos);
};

