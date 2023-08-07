#include "stdafx.h"
#include "SceneHome.h"

#include "DataTableMgr.h"
#include "StringTable.h"
#include "SceneMgr.h"
#include "InputMgr.h"
#include "ResourceMgr.h"
#include "Framework.h"
#include "TeamMgr.h"

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
	AddGoText();
	
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
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_league_bg.png", "LeagueUi"));
}

void SceneHome::AddGoUiButton()
{

	for (int i = 0; i < 23; i++)
	{
		std::stringstream ss;
		ss << "MainB" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/main_menu_button_0.png", ss.str()));
		ss << "Text";
		AddGo(new TextGo(ss.str()));
	}
	AddGo(new UiButton("graphics/Origin/Sprite/proceed_button_0.png", "PlayB"));
}

void SceneHome::AddGoText()
{
	AddGo(new TextGo("MoneyInfoT"));
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

	SpriteGo* leagueUi = (SpriteGo*)FindGo("LeagueUi");
	leagueUi->SetPosition(0, spr->GetSize().y*2.f);
	leagueUi->SetOrigin(Origins::TL);
	leagueUi->SetSize(2,2);
	leagueUi->sortLayer = 106;

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
	TextGo* btText;
	float uiSizeMainB = 2.5f;
	for (int i = 0; i < 5; i++)
	{
		std::stringstream ss;
		ss << "MainB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::BL);
		bt->SetSize(uiSizeHead+0.45f, uiSizeHead);
		bt->SetPosition(15.f + 15.f * i + (bt->GetSize().x * i* uiSizeMainB),
			FRAMEWORK.GetWindowSize().y - 30.f);
		bt->sortLayer = 100;
		bt->OnClick = [this,i]() {
			MainUiClose();
			MainUiOpen((MainMenuType)i);
		};
		int startNum = 0;
		int endNum = 0;
		ReturnMainUiIndex(startNum, endNum,(MainMenuType)i);
		int boundNum = 1;
		for (int j = startNum; j < endNum; j++)
		{
			boundNum++;
			std::stringstream ss2;
			ss2 << "MainB" << j; 
			UiButton* bt2 = (UiButton*)FindGo(ss2.str());
			bt2->SetOrigin(Origins::BL);
			bt2->SetSize(uiSizeHead + 0.45f, uiSizeHead);
			bt2->SetPosition(15.f + 15.f * i + (bt->GetSize().x * i * uiSizeMainB),
				FRAMEWORK.GetWindowSize().y - (50.f * boundNum) +20.f);
			bt2->sortLayer = 100;
			bt2->OnClick = [this,j]() {
				MainUiFunc(j);
			};
			bt2->SetActive(false);

			ss2 << "Text";
			btText = (TextGo*)FindGo(ss2.str());
			btText->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
			auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
			btText->text.setString(stringtable->GetW(ss2.str()));
			btText->text.setFillColor(sf::Color::White);
			btText->text.setCharacterSize(20);
			btText->SetOrigin(Origins::MC);
			btText->SetPosition(bt2->GetPosition().x + bt2->GetSize().x + 15.f, bt2->GetPosition().y - bt2->GetSize().y);
			btText->sortLayer = 101;
			btText->SetActive(false);
		}

		ss << "Text";
		btText = (TextGo*)FindGo(ss.str());
		btText->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
		btText->text.setString(stringtable->GetW(ss.str()));
		btText->text.setFillColor(sf::Color::White);
		btText->text.setCharacterSize(20);
		btText->SetOrigin(Origins::MC);
		btText->SetPosition(bt->GetPosition().x + bt->GetSize().x+15.f , bt->GetPosition().y - bt->GetSize().y);
		btText->sortLayer = 101;
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

	TextGo* text = (TextGo*)FindGo("MoneyInfoT");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(std::to_string(TEAM_MGR.GetMoney()));
	text->text.setFillColor(sf::Color::White);
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::TL);
	text->SetPosition(FRAMEWORK.GetWindowSize().x * 0.85f, 22.f);
	text->sortLayer = 107;

}

void SceneHome::ReturnMainUiIndex(int& startNum, int& endNum, MainMenuType Type)
{
	switch (Type)
	{
	case MainMenuType::TEAM_MANAGE:
		startNum = 5;
		endNum = 8;
		break;
	case MainMenuType::OPERATE:
		startNum = 8;
		endNum = 13;
		break;
	case MainMenuType::LEAGUE:
		startNum = 13;
		endNum = 17;
		break;
	case MainMenuType::GAME:
		startNum = 17;
		endNum = 19;
		break;
	case MainMenuType::SYSTEM:
	default:
		startNum = 19;
		endNum = 23;
		break;
	}
	return;
}

void SceneHome::MainUiOpen(MainMenuType Type)
{
	int startNum = 0;
	int endNum = 0;
	ReturnMainUiIndex(startNum, endNum, Type);
	for (int i = startNum; i < endNum; i++)
	{
		std::stringstream ss;
		ss << "MainB" << i;
		UiButton* bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(true);
		ss << "Text";
		TextGo* btText = (TextGo*)FindGo(ss.str());
		btText->SetActive(true);
	}
}

void SceneHome::MainUiClose()
{
	for (int i = 5; i < 23; i++)
	{
		std::stringstream ss;
		ss << "MainB" << i;
		UiButton* bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);
		ss << "Text";
		TextGo* btText = (TextGo*)FindGo(ss.str());
		btText->SetActive(false);
	}
}

void SceneHome::MainUiFunc(int index)
{
	switch (index)
	{
	case 5:
		TEAM_MGR.RecruitLocal(0);
		TEAM_MGR.Employ(0);
		UpdateMoney();
		break;
	case 7:
		TEAM_MGR.ShowPlayer();
		break;
	default:
		break;
	}
}

void SceneHome::UpdateMoney()
{
	TextGo* text = (TextGo*)FindGo("MoneyInfoT");
	text->text.setString(std::to_string(TEAM_MGR.GetMoney()));
	text->SetOrigin(Origins::TL);
}
