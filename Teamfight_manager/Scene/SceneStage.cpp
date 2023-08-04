#include "stdafx.h"
#include "SceneStage.h"

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

SceneStage::SceneStage() : Scene(SceneId::Stage)
{
	resourceListPath = "tables/StageResourceList.csv";
}

SceneStage::~SceneStage()
{
}

void SceneStage::Init()
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

void SceneStage::Release()
{
	for (auto go : gameObjects)
	{
		delete go;
	}
}

void SceneStage::Enter()
{
	Scene::Enter();
	RESOURCE_MGR.LoadFromCsv("tables/StageResourceList.csv");
}

void SceneStage::Exit()
{
	Scene::Exit();
}

void SceneStage::Update(float dt)
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

void SceneStage::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
	window.setMouseCursorVisible(true);
	//없어도 되나?
}