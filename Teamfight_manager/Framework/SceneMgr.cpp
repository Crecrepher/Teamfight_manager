#include "stdafx.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "SceneTitile.h"
#include "SceneHome.h"
#include "SceneGame.h"

#include "ResourceMgr.h"
#include "DataTableMgr.h"
#include "StringTable.h"
#include "InputMgr.h"
void SceneMgr::ChangeScene(SceneId id)
{
	currentScene->Exit();
	currentSceneId = id;
	currentScene = scenes[(int)currentSceneId];
	currentScene->Enter();
}

void SceneMgr::Init()
{
	if (!scenes.empty())
	{
		Release();
	}
			
	scenes.push_back(new SceneTitile());
	scenes.push_back(new SceneHome());
	scenes.push_back(new SceneGame());

	for (auto scene : scenes)
	{
		scene->Init();
	}

	currentSceneId = startSceneId;
	currentScene = scenes[(int)currentSceneId];
	currentScene->Enter();

	SaveLoad();
}

void SceneMgr::Release()
{
	if (scenes.empty())
	{
		return;
	}

	for (auto scene : scenes)
	{
		delete scene;
	}
	scenes.clear();

	currentSceneId = SceneId::None;
	currentScene = nullptr;
}

void SceneMgr::Update(float dt)
{
	currentScene->Update(dt);
}

void SceneMgr::Draw(sf::RenderWindow& window)
{
	currentScene->Draw(window);
}

Scene* SceneMgr::GetCurrScene() const
{
	return currentScene;
}

void SceneMgr::SaveLoad()
{
	int data;
	std::ifstream is{ "save.dat", std::ofstream::binary };
	if (is.fail()) {
		std::cout << "세이브 파일이 없습니다" << std::endl;
		return;
	}
	is.read((char*)&data, sizeof(int));
	hiScore = data;
	return;
}
