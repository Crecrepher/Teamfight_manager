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

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "Logo" << i;
		AddGo(new UiButton("graphics/UiFix/menuBindBox.png", ss.str()));
		ss << "Text";
		AddGo(new TextGo(ss.str()));
	}

	AddGo(new SpriteGo("graphics/Origin/Sprite/logo_tp.png", "Logo"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/stadium.png", "Back"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/stadium_sky_bg.png", "BackSky"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/teamfight_manager_title_bg.png", "Monitor"));

	std::stringstream ss;
	ss << "³»¸¾ÀÌ¾ß" << std::endl;
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

	SpriteGo* spr = (SpriteGo*)FindGo("Back");
	spr->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f, FRAMEWORK.GetWindowSize().y * 0.55f);
	spr->SetOrigin(Origins::MC);
	spr->SetSize(1.25f, 1.25f);
	spr->sortLayer = 2;

	spr = (SpriteGo*)FindGo("Monitor");
	spr->SetPosition(0, 0);
	spr->SetOrigin(Origins::TL);
	spr->SetSize(FRAMEWORK.GetWindowSize().x / spr->GetSize().x,
		FRAMEWORK.GetWindowSize().y / spr->GetSize().y);
	spr->sortLayer = 3;

	spr = (SpriteGo*)FindGo("BackSky");
	spr->SetPosition(0, 0);
	spr->SetOrigin(Origins::TL);
	spr->SetSize(FRAMEWORK.GetWindowSize().x / spr->GetSize().x, FRAMEWORK.GetWindowSize().y / spr->GetSize().y);
	spr->sortLayer = 1;

	spr = (SpriteGo*)FindGo("Logo");
	spr->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f, FRAMEWORK.GetWindowSize().y * 0.25f);
	spr->SetOrigin(Origins::MC);
	spr->SetSize(2, 2);
	spr->sortLayer = 4;
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