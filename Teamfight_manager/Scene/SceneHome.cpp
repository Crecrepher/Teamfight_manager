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
	resourceListPath = "tables/HomeResourceList.csv";
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

	AddGoSprites();
	AddGoUiButton();
	
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
	RESOURCE_MGR.LoadFromCsv("tables/HomeResourceList.csv");

	MakeMainUi();
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

void SceneHome::AddGoSprites()
{
	AddGo(new SpriteGo("graphics/Origin/Sprite/ground.png", "Ground"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/house_bg.png", "House"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/sky_day.png", "Sky"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_bg.png", "HeadUi"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_slot_bg.png", "HeadSlot0"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_slot_bg.png", "HeadSlot1"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_gold_icon.png", "GoldUi"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_calendar_icon.png", "CalendarUi"));
}

void SceneHome::AddGoUiButton()
{

	for (int i = 0; i < 23; i++)
	{
		std::stringstream ss;
		ss << "MainB" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/main_menu_button_0.png", ss.str()));
	}
	AddGo(new UiButton("graphics/Origin/Sprite/proceed_button_0.png", "PlayB"));
}

void SceneHome::MakeMainUi()
{

	SpriteGo* ground = (SpriteGo*)FindGo("Ground");
	ground->SetOrigin(Origins::TC);
	ground->SetSize(FRAMEWORK.GetWindowSize().x / ground->GetSize().x,
		FRAMEWORK.GetWindowSize().x / ground->GetSize().x);
	ground->SetPosition(FRAMEWORK.GetWindowSize().x/2.f, FRAMEWORK.GetWindowSize().y*0.8f);
	ground->sortLayer = 5;

	SpriteGo* spr = (SpriteGo*)FindGo("House");
	spr->SetOrigin(Origins::BC);
	spr->SetSize(3,3);
	spr->SetPosition(ground->GetPosition());
	spr->sortLayer = 2;

	spr = (SpriteGo*)FindGo("Sky");
	spr->SetPosition(0, ground->GetPosition().y);
	spr->SetOrigin(Origins::BL);
	spr->SetSize(FRAMEWORK.GetWindowSize().x / spr->GetSize().x,
		FRAMEWORK.GetWindowSize().y / spr->GetSize().y);
	spr->sortLayer = 0;

	spr = (SpriteGo*)FindGo("HeadUi");
	spr->SetPosition(0, 0);
	spr->SetOrigin(Origins::TL);
	spr->SetSize(FRAMEWORK.GetWindowSize().x / spr->GetSize().x,
		FRAMEWORK.GetWindowSize().x / spr->GetSize().x);
	spr->sortLayer = 105;

	float uiSizeHead = 2.0f;

	spr = (SpriteGo*)FindGo("HeadSlot0");
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.6f + 30.f, 10.f);
	spr->SetOrigin(Origins::TL);
	spr->SetSize(uiSizeHead, uiSizeHead);
	spr->sortLayer = 106;

	SpriteGo* sprIc = (SpriteGo*)FindGo("CalendarUi");
	sprIc->SetSize(uiSizeHead, uiSizeHead);
	sprIc->SetPosition(spr->GetPosition().x + 10.f,
		spr->GetPosition().y + 10.f);
	sprIc->SetOrigin(Origins::TL);
	sprIc->sortLayer = 107;

	spr = (SpriteGo*)FindGo("HeadSlot1");
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f+15.f, 10.f);
	spr->SetOrigin(Origins::TL);
	spr->SetSize(uiSizeHead, uiSizeHead);
	spr->sortLayer = 106;

	sprIc = (SpriteGo*)FindGo("GoldUi");
	sprIc->SetSize(uiSizeHead, uiSizeHead);
	sprIc->SetPosition(spr->GetPosition().x+10.f,
		spr->GetPosition().y + 10.f);
	sprIc->SetOrigin(Origins::TL);
	sprIc->sortLayer = 107;


	UiButton* bt;
	float uiSizeMainB = 2.5f;
	for (int i = 0; i < 5; i++)
	{
		std::stringstream ss;
		ss << "MainB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::BL);
		bt->SetSize(uiSizeHead, uiSizeHead);
		bt->SetPosition(15.f + 15.f * i + (bt->GetSize().x * i* uiSizeMainB),
			FRAMEWORK.GetWindowSize().y - 30.f);
		bt->sortLayer = 100;
		bt->OnClick = []() {
		};
	}

	float uiSizeStart = 2.0f;
	bt = (UiButton*)FindGo("PlayB");
	bt->SetOrigin(Origins::BR);
	bt->SetSize(uiSizeStart, uiSizeStart);
	bt->SetPosition(FRAMEWORK.GetWindowSize().x - 30.f,
		FRAMEWORK.GetWindowSize().y - 30.f);
	bt->sortLayer = 101;
	bt->OnClick = []() {
		SCENE_MGR.ChangeScene(SceneId::Game);
	};
}
