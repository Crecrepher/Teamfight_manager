#include "stdafx.h"
#include "SceneTitile.h"

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

SceneTitile::SceneTitile() : Scene(SceneId::Title)
{
	resourceListPath = "tables/TitleResourceList.csv";
}

SceneTitile::~SceneTitile()
{
}

void SceneTitile::Init()
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

void SceneTitile::Release()
{
	for (auto go : gameObjects)
	{
		delete go;
	}
}

void SceneTitile::Enter()
{
	Scene::Enter();
	RESOURCE_MGR.LoadFromCsv("tables/TitleResourceList.csv");
}

void SceneTitile::Exit()
{
	Scene::Exit();
}

void SceneTitile::Update(float dt)
{
	Scene::Update(dt);	
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Enter))
	{
		SCENE_MGR.ChangeScene(SceneId::Stage);
	}
}

void SceneTitile::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
}