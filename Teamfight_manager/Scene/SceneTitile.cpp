#include "stdafx.h"
#include "SceneTitile.h"

#include "DataTableMgr.h"
#include "StringTable.h"
#include "SceneMgr.h"
#include "InputMgr.h"
#include "TeamMgr.h"
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

	AddGo(new UiButton("graphics/Origin/Sprite/sponser_logo_new_16.png","ChampEdit"));
	AddGo(new UiButton("graphics/Origin/Sprite/default_button_1.png", "StartB"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/stadium.png", "Back"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/teamfight_manager_title_bg.png", "Monitor"));
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

	UiButton* bt = (UiButton*)FindGo("ChampEdit");
	bt->SetPosition(1000, 300);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(1.5, 1.5);
	bt->sortLayer = 100;
	bt->OnClick = []() {
		SCENE_MGR.ChangeScene(SceneId::ChampEdit);
	};

	bt = (UiButton*)FindGo("StartB");
	bt->SetPosition(FRAMEWORK.GetWindowSize()/2.f);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(1.5, 1.5);
	bt->sortLayer = 100;
	bt->OnClick = []() {
		SCENE_MGR.ChangeScene(SceneId::Home);
	};

	SpriteGo* spr = (SpriteGo*)FindGo("Back");
	spr->SetPosition(0, 0);
	spr->SetOrigin(Origins::TL);

	spr = (SpriteGo*)FindGo("Monitor");
	spr->SetPosition(0, 0);
	spr->SetOrigin(Origins::TL);
	spr->SetSize(FRAMEWORK.GetWindowSize().x/ spr->GetSize().x,
		FRAMEWORK.GetWindowSize().y/ spr->GetSize().y);
}

void SceneTitile::Exit()
{
	Scene::Exit();
}

void SceneTitile::Update(float dt)
{
	Scene::Update(dt);	
	if (INPUT_MGR.GetKeyUp(sf::Keyboard::Enter))
	{
		SCENE_MGR.ChangeScene(SceneId::Home);
	}
	else if (INPUT_MGR.GetKeyUp(sf::Keyboard::Num1))
	{
		TEAM_MGR.SaveLoad(0);
		SCENE_MGR.ChangeScene(SceneId::Home);
	}
	else if (INPUT_MGR.GetKeyUp(sf::Keyboard::Num2))
	{
		TEAM_MGR.SaveLoad(1);
		SCENE_MGR.ChangeScene(SceneId::Home);
	}
	else if (INPUT_MGR.GetKeyUp(sf::Keyboard::Num3))
	{
		TEAM_MGR.SaveLoad(2);
		SCENE_MGR.ChangeScene(SceneId::Home);
	}
}

void SceneTitile::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
}