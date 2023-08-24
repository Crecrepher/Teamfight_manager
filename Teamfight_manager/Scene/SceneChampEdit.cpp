#include "stdafx.h"
#include "SceneChampEdit.h"
#include <filesystem>

#include "DataTableMgr.h"
#include "StringTable.h"
#include "SceneMgr.h"
#include "InputMgr.h"
#include "ResourceMgr.h"
#include "Framework.h"
#include "ChampionMgr.h"

#include "SpriteGo.h"
#include "TextGo.h"
#include "SoundGo.h"
#include "UiButton.h"

SceneChampEdit::SceneChampEdit() : Scene(SceneId::ChampEdit)
{
	resourceListPath = "tables/GameResourceList.csv";
}

SceneChampEdit::~SceneChampEdit()
{
}

void SceneChampEdit::Init()
{
	Release();
	sf::Vector2f windowSize = FRAMEWORK.GetWindowSize();
	sf::Vector2f centerPos = windowSize * 0.5f;

	worldView.setSize(windowSize);
	worldView.setCenter(centerPos);
	uiView.setSize(windowSize);
	uiView.setCenter(centerPos);

	InitUiButton();
	InitSpriteGo();
	InitTextGo();

	for (auto go : gameObjects)
	{
		go->Init();
	}
}

void SceneChampEdit::Release()
{
	for (auto go : gameObjects)
	{
		delete go;
	}
}

void SceneChampEdit::Enter()
{
	Scene::Enter();
	RESOURCE_MGR.LoadFromCsv("tables/GameResourceList.csv");

	std::string path = "graphics/CustomSprite";

	for (const auto& file : std::filesystem::directory_iterator(path))
		std::cout << file.path() << std::endl;

	champ.SetPosition(FRAMEWORK.GetWindowSize() * 0.5f);
	champ.SetOrigin(Origins::MC);
	champ.SetSize(2, 2);

	animation = CHAMPION_MGR.GetChampion(0)->animaition;
	animation.SetTarget(&champ.sprite);
	animation.Play("Idle");
	champ.SetOrigin(Origins::MC);

	UiButton* bt;
	SpriteGo* spr;
	TextGo* text;

	spr = (SpriteGo*)FindGo("ChampNameBox");
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f, 50);
	spr->SetOrigin(Origins::MC);
	spr->SetSize(2, 2);
	spr->sortLayer = 100;

}

void SceneChampEdit::Exit()
{
	Scene::Exit();
}

void SceneChampEdit::Update(float dt)
{
	Scene::Update(dt);	
	animation.Update(dt);

	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Escape))
	{
		SCENE_MGR.ChangeScene(SceneId::Title);
	}
}

void SceneChampEdit::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
}

void SceneChampEdit::InitUiButton()
{
	AddGo(new UiButton("graphics/Origin/Sprite/green_arrow_button_0.png", "ChangeChampL"));
	AddGo(new UiButton("graphics/Origin/Sprite/green_arrow_button_3.png", "ChangeChampR"));
}

void SceneChampEdit::InitSpriteGo()
{
	champ = SpriteGo("graphics/UiFix/character_icons_0.png", "Champ");
	AddGo(&champ);

	AddGo(new SpriteGo("graphics/Origin/Sprite/weekly_event_button_3.png", "ChampNameBox"));
}

void SceneChampEdit::InitTextGo()
{
}
