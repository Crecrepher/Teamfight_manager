#pragma once

class GameObject;
class Scene
{
protected:

	Scene(const Scene&) = delete;
	Scene& operator = (const Scene&) = delete;

	SceneId sceneId;

	std::string resourceListPath;

	std::list<GameObject*> gameObjects;
	std::list<GameObject*> removeGameObjects;

	std::list<GameObject*> uiObjects;

	sf::View worldView;
	sf::View uiView;

	sf::RenderWindow& window;

public:
	Scene(SceneId id = SceneId::None);
	virtual ~Scene() = 0;

	GameObject* FindGo(const std::string& name);
	void FindGos(std::list<GameObject*> list, const std::string& name);
	//이 이름을 가진 모든 오브젝트 찾기

	bool Exist(GameObject* go);
	GameObject* AddGo(GameObject* go);
	void RemoveGo(GameObject* go);
	void SortGos();

	sf::Vector2f ScreenToWorldPos(sf::Vector2f screenPos);
	sf::Vector2f ScreenToUiPos(sf::Vector2f screenPos);

	sf::Vector2f worldPosToScreen(sf::Vector2f worldPos);
	sf::Vector2f uiPosToScreen(sf::Vector2f uiPos);

	virtual void Init() = 0;
	virtual void Release() = 0;

	virtual void Enter();
	virtual void Exit();

	virtual void Update(float dt);
	virtual void Draw(sf::RenderWindow& window);
};

