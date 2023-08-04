#include "stdafx.h"
#include "SceneHome.h"

#include "DataTableMgr.h"
#include "StringTable.h"
#include "SceneMgr.h"
#include "InputMgr.h"
#include "ResourceMgr.h"
#include "Framework.h"

#include "SpriteGo.h"
#include "TextGo.h"
#include "SoundGo.h"
#include "UiButton.h"

SceneHome::SceneHome() : Scene(SceneId::Home)
{
	resourceListPath = "tables/StageResourceList.csv";
}

SceneHome::~SceneHome()
{
}

void SceneHome::Init()
{
	Release();
	
	sf::Vector2f windowSize = FRAMEWORK.GetWindowSize();
	sf::Vector2f centerPos = windowSize * 0.5f;

	worldView.setSize(windowSize);
	worldView.setCenter(centerPos);
	uiView.setSize(windowSize);
	uiView.setCenter(centerPos);

	for (auto go : gameObjects)
	{
		go->Init();
	}
}

void SceneHome::Release()
{
	for (auto go : gameObjects)
	{
		delete go;
	}
}

void SceneHome::Enter()
{
	Scene::Enter();
	RESOURCE_MGR.LoadFromCsv("tables/StageResourceList.csv");
}

void SceneHome::Exit()
{
	Scene::Exit();
}

void SceneHome::Update(float dt)
{
	Scene::Update(dt);

	// Title Back
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Escape))
	{
		SCENE_MGR.ChangeScene(SceneId::Title);
	}
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Enter))
	{
		SCENE_MGR.ChangeScene(SceneId::Game);
	}
}

void SceneHome::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
}