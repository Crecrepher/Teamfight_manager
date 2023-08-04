#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"
#include "ResourceMgr.h"
#include "GameObject.h"
#include "Framework.h"
Scene::Scene(SceneId id)
	:sceneId(id), window(FRAMEWORK.GetWindow())
{
}

Scene::~Scene()
{
	//Release();
}

GameObject* Scene::FindGo(const std::string& name)
{
	auto compare = [name](GameObject* go) {return go->GetName() == name; };
	auto it = std::find_if(gameObjects.begin(), gameObjects.end(), compare);
	//람다 표현식 : compare에서 true가 나오는 녀석이 나타날때까지 돌리고 찾아서 나오면 돌려줌
	//함수의 레퍼런스라고 보면 됨

	if (it == gameObjects.end())
	{
		return nullptr;
	}
	return *it;
}

void Scene::FindGos(std::list<GameObject*> list, const std::string& name)
{
	for (auto go : gameObjects)
	{
		if (go->GetName() == name)
		{
			list.push_back(go);
		}
	}
}

bool Scene::Exist(GameObject* go)
{
	return std::find(gameObjects.begin(), gameObjects.end(), go) != gameObjects.end();
}

GameObject* Scene::AddGo(GameObject* go)
{
	if (!Exist(go))
	{
		gameObjects.push_back(go);	
	}
	return go;
}

void Scene::RemoveGo(GameObject* go)
{
	removeGameObjects.push_back(go);
}

void Scene::SortGos()
{
	gameObjects.sort([](GameObject* lhs, GameObject* rhs) {
		if (lhs->sortLayer != rhs->sortLayer)
		{
			return lhs->sortLayer < rhs->sortLayer;
		}
		return lhs->sortOrder < rhs->sortOrder;
		});
}

sf::Vector2f Scene::ScreenToWorldPos(sf::Vector2f screenPos)
{
	return window.mapPixelToCoords((sf::Vector2i)screenPos,worldView);
}

sf::Vector2f Scene::ScreenToUiPos(sf::Vector2f screenPos)
{
	return window.mapPixelToCoords((sf::Vector2i)screenPos, uiView);
}

sf::Vector2f Scene::worldPosToScreen(sf::Vector2f worldPos)
{
	return (sf::Vector2f)window.mapCoordsToPixel(worldPos, worldView);
}

sf::Vector2f Scene::uiPosToScreen(sf::Vector2f uiPos)
{
	return (sf::Vector2f) window.mapCoordsToPixel(uiPos, uiView);
}

void Scene::Enter()
{
	RESOURCE_MGR.LoadFromCsv(resourceListPath);

	for (auto go : gameObjects)
	{
		go->Reset();
	}
}

void Scene::Exit()
{
	for (auto go : removeGameObjects)
	{
		gameObjects.remove(go);
	}
	removeGameObjects.clear();

	RESOURCE_MGR.UnloadAll();
}

void Scene::Update(float dt)
{
	for (auto go : gameObjects)
	{
		if (go->GetActive())
		{
			go->Update(dt);
		}
	}
	for (auto go : removeGameObjects)
	{
		gameObjects.remove(go);
	}
	removeGameObjects.clear();
}

void Scene::Draw(sf::RenderWindow& window)
{
	SortGos();

	window.setView(worldView);
	for (auto go : gameObjects)
	{
		if (go->sortLayer >=100)
		{
			continue;
		}
		if (go->GetActive())
		{
			go->Draw(window);
		}
	}
	window.setView(uiView);
	for (auto go : gameObjects)
	{
		if (go->sortLayer < 100)
		{
			continue;
		}
		if (go->GetActive())
		{
			go->Draw(window);
		}
	}
}
