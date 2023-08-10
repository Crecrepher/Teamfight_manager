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
#include "RectGo.h"

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

	AddGo(new RectGo("UiShade"));

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

	if (TEAM_MGR.GetTodayDate() == 0)
	{
		NewYear();
	}

	MakeMainUi();
	MakeSubUi();
	UiTrainingOpen(false);

	MakeSubUiSponsorContract();
	//UiSponsorContractOpen(true);
	//

	RectGo* rect = (RectGo*)FindGo("UiShade");
	rect->SetSize(FRAMEWORK.GetWindowSize());
	rect->sortLayer = 110;
	rect->sortOrder = -1;
	rect->rectangle.setFillColor(sf::Color(0, 0, 0, 150));
}

void SceneHome::Exit()
{
	Scene::Exit();
}

void SceneHome::Update(float dt)
{
	if (!backClick)
	{
		backClick = true;
	}

	Scene::Update(dt);

	if (INPUT_MGR.GetMouseButtonUp(sf::Mouse::Left) && backClick
		&& !isMenuOn)
	{
		MainUiClose();
	}

	TestingCheats();
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
	AddGo(new SpriteGo("graphics/Origin/Sprite/training_ui_bg.png", "TrainingUi"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/scroll_bar.png", "ScrollBar"));
	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "TrainingSlotUi" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/training_slot_bg.png", ss.str()));
		ss.str("");
		ss << "XpBack" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/exp_guage_0.png", ss.str()));
		ss.str("");
		ss << "XpCur" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/exp_guage_1.png", ss.str()));
		ss.str("");
		ss << "XpFuture" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/exp_guage_2.png", ss.str()));
	}
	for (int i = 0; i < 48; i++)
	{
		std::stringstream ss;
		ss << "TrainingPointGuage" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/training_point_guage_0.png", ss.str()));
	}
	for (int i = 0; i < 8; i++)
	{
		std::stringstream ss;
		ss << "LeftTrainingPointGuage" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/unused_point_guage_0.png", ss.str()));
	}
	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "CharacterIcons" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/character_icons_0.png", ss.str()));
		ss << "Back";
		AddGo(new SpriteGo("graphics/Origin/Sprite/champion&player_slot_0.png", ss.str()));
		ss << "Line";
		AddGo(new SpriteGo("graphics/Origin/Sprite/champion&player_slot_1.png", ss.str()));
		ss.str("");
		ss << "CharacterStatIcons" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/character_icons_0.png", ss.str()));
	}
	AddGo(new SpriteGo("graphics/Origin/Sprite/attack_icon.png", "IconAtk0"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/attack_icon.png", "IconAtk1"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/defense_icon.png", "IconDef0"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/defense_icon.png", "IconDef1"));

	//Sponsor
	AddGo(new SpriteGo("graphics/Origin/Sprite/sponsor_ui_bg.png", "SponBg"));
	for (int i = 0; i < 9; i++)
	{
		std::stringstream ss;
		ss << "SponLogo" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/sponser_logo_0.png", ss.str()));
	}
	AddGo(new SpriteGo("graphics/Origin/Sprite/sponser_logo_0.png", "SelectedLogo"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_gold_icon.png", "GoldUiEx"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/keyboard_switch.png", "KeyboardSwitch"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/sound_chip.png", "SoundChip"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/screw.png", "Screw"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/fabric_piece.png", "FabricPiece"));
	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "PartBack" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/upgrade_icon_bg.png", ss.str()));
	}
	AddGo(new SpriteGo("graphics/Origin/Sprite/champ_bg #43858.png", "SponLogoBack"));

}

void SceneHome::AddGoUiButton()
{
	AddGo(new UiButton("graphics/Origin/Sprite/proceed_button_0.png", "PlayB"));
	for (int i = 0; i < 23; i++)
	{
		std::stringstream ss;
		ss << "MainB" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/main_menu_button_0.png", ss.str()));
		ss << "Text";
		AddGo(new TextGo(ss.str()));
	}

	AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", "TrainPointReturnB"));
	AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", "TrainCloseB"));
	for (int i = 0; i < 12; i++)
	{
		std::stringstream ss;
		ss << "PlayerList" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/player_list_bg_0.png", ss.str()));
	}
	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "TrainingPointDownB" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/training_point_arrow_button_0.png", ss.str()));
		ss.str("");
		ss << "TrainingPointUpB" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/training_point_arrow_button_2.png", ss.str()));
	}

	for (int i = 0; i < 9; i++)
	{
		std::stringstream ss;
		ss << "SponSlot" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/sponsor_card_bg_0.png", ss.str()));
	}
	AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", "SponsorContract"));
}

void SceneHome::AddGoText()
{
	AddGo(new TextGo("MoneyInfoT"));

	for (int i = 0; i < 9; i++)
	{
		std::stringstream ss;
		ss << "SponsorName" << i;
		AddGo(new TextGo(ss.str()));
	}
	AddGo(new TextGo("SelectedSponsorName"));
	AddGo(new TextGo("SponsorQuest"));
	AddGo(new TextGo("SponsorMoney"));
	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "SponsorParts" << i;
		AddGo(new TextGo(ss.str()));
	}
}


void SceneHome::MakeMainUi()
{

	SpriteGo* ground = (SpriteGo*)FindGo("Ground");
	ground->SetOrigin(Origins::TC);
	ground->SetSize(FRAMEWORK.GetWindowSize().x / ground->GetSize().x,
		FRAMEWORK.GetWindowSize().x / ground->GetSize().x);
	ground->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f, FRAMEWORK.GetWindowSize().y * 0.8f);
	ground->sortLayer = 5;

	SpriteGo* spr = (SpriteGo*)FindGo("House");
	spr->SetOrigin(Origins::BC);
	spr->SetSize(3, 3);
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
	leagueUi->SetPosition(0, spr->GetSize().y * 2.f);
	leagueUi->SetOrigin(Origins::TL);
	leagueUi->SetSize(2, 2);
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
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f + 15.f, 10.f);
	spr->SetOrigin(Origins::TL);
	spr->SetSize(uiSizeHead, uiSizeHead);
	spr->sortLayer = 106;

	sprIc = (SpriteGo*)FindGo("GoldUi");
	sprIc->SetSize(uiSizeHead, uiSizeHead);
	sprIc->SetPosition(spr->GetPosition().x + 10.f,
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
		bt->SetSize(uiSizeHead + 0.45f, uiSizeHead);
		bt->SetPosition(15.f + 15.f * i + (bt->GetSize().x * i * uiSizeMainB),
			FRAMEWORK.GetWindowSize().y - 30.f);
		bt->sortLayer = 100;
		bt->OnClick = [this, i]() {
			if (isMenuOn)
			{
				return;
			}
			MainUiClose();
			MainUiOpen((MainMenuType)i);
			backClick = false;
		};
		int startNum = 0;
		int endNum = 0;
		ReturnMainUiIndex(startNum, endNum, (MainMenuType)i);
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
				FRAMEWORK.GetWindowSize().y - (50.f * boundNum) + 20.f);
			bt2->sortLayer = 100;
			bt2->OnClick = [this, j]() {
				MainUiFunc(j);
				backClick = false;
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

		btText->SetPosition(bt->GetPosition().x + bt->GetSize().x + 15.f, bt->GetPosition().y - bt->GetSize().y);
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
		TEAM_MGR.DayPass();
		TeamMgr::Schedule schedule = TEAM_MGR.GetSchedule(TEAM_MGR.GetTodayDate() - 1);
		if (schedule != TeamMgr::Schedule::Recruit &&
			schedule != TeamMgr::Schedule::Vacation)
		{
			SCENE_MGR.ChangeScene(SceneId::Game);
		}
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


void SceneHome::MakeSubUi()
{
	MakeSubUiTraining();
}

void SceneHome::MakeSubUiTraining()
{
	SpriteGo* spr = (SpriteGo*)FindGo("TrainingUi");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f,
		FRAMEWORK.GetWindowSize().y * 0.44f);
	spr->SetSize((FRAMEWORK.GetWindowSize().x / spr->GetSize().x) - 0.05f,
		(FRAMEWORK.GetWindowSize().x / spr->GetSize().x) - 0.05f);
	spr->sortLayer = 110;

	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "TrainingSlotUi" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::TL);
		spr->SetSize(2, 2);
		spr->SetPosition(483 + (spr->GetSize().x * 2.f + 10.f) * (i % 2), 275 + (spr->GetSize().y * 2.f + 10.f) * (i / 2));
		spr->sortLayer = 111;
		ss.str("");
		ss << "XpBack" << i;
		SpriteGo* xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetOrigin(Origins::TL);
		xpBar->SetSize(2, 2);
		xpBar->SetPosition(spr->GetPosition().x+55.f, spr->GetPosition().y + 56.f);
		xpBar->sortLayer = 112;
		ss.str("");
		ss << "XpCur" << i;
		xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetOrigin(Origins::TL);
		xpBar->SetSize(2, 2);
		xpBar->SetPosition(spr->GetPosition().x + 55.f, spr->GetPosition().y + 56.f);
		xpBar->sortLayer = 114;
		ss.str("");
		ss << "XpFuture" << i;
		xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetOrigin(Origins::TL);
		xpBar->SetSize(2, 2);
		xpBar->SetPosition(spr->GetPosition().x + 55.f, spr->GetPosition().y + 56.f);
		xpBar->sortLayer = 113;

		ss.str("");
		ss << "TrainingPointDownB" << i;
		UiButton* trainPointB = (UiButton*)FindGo(ss.str());
		trainPointB->SetOrigin(Origins::MC);
		trainPointB->SetSize(2, 2);
		trainPointB->SetPosition(spr->GetPosition().x + 320.f, spr->GetPosition().y + 27.f);
		trainPointB->sortLayer = 113;
		ss.str("");
		ss << "TrainingPointUpB" << i;
		trainPointB = (UiButton*)FindGo(ss.str());
		trainPointB->SetOrigin(Origins::MC);
		trainPointB->SetSize(2, 2);
		trainPointB->SetPosition(spr->GetPosition().x + 350.f, spr->GetPosition().y + 27.f);
		trainPointB->sortLayer = 113;

		for (int j = i * 8; j < i*8+8; j++)
		{
			std::stringstream ss;
			ss << "TrainingPointGuage" << j;
			SpriteGo* trainPoint = (SpriteGo*)FindGo(ss.str());
			trainPoint->SetOrigin(Origins::MC);
			trainPoint->SetSize(2, 2);
			trainPoint->SetPosition(spr->GetPosition().x + 295.f-(j%8*6), spr->GetPosition().y + 27.f);
			trainPoint->sortLayer = 112;
		}
		if (i>1)
		{
			ss.str("");
			ss << "CharacterStatIcons" << i-2;
			SpriteGo* charIcon = (SpriteGo*)FindGo(ss.str());
			charIcon->SetOrigin(Origins::TL);
			charIcon->SetSize(2, 2);
			charIcon->sprite.setTextureRect({0,0,25,25});
			charIcon->SetPosition(spr->GetPosition().x , spr->GetPosition().y);
			charIcon->sortLayer = 112;
		}
		if (i == 0)
		{
			SpriteGo* statIcon = (SpriteGo*)FindGo("IconAtk1");
			statIcon->SetOrigin(Origins::MC);
			statIcon->SetPosition(spr->GetPosition().x + 25.f, spr->GetPosition().y + 25.f);
			statIcon->SetSize(2, 2);
			statIcon->sortLayer = 111;
		}
		if (i == 1)
		{
			SpriteGo* statIcon = (SpriteGo*)FindGo("IconDef1");
			statIcon->SetOrigin(Origins::MC);
			statIcon->SetPosition(spr->GetPosition().x + 25.f, spr->GetPosition().y+25.f);
			statIcon->SetSize(2, 2);
			statIcon->sortLayer = 111;
		}
	}

	for (int i = 0; i < 8; i++)
	{
		std::stringstream ss;
		ss << "LeftTrainingPointGuage" << i;
		SpriteGo* leftTrainPoint = (SpriteGo*)FindGo(ss.str());
		leftTrainPoint->SetOrigin(Origins::MC);
		leftTrainPoint->SetPosition(1155 - (i * 18), 246);
		leftTrainPoint->SetSize(2, 2);
		leftTrainPoint->sortLayer = 113;
	}

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "CharacterIcons" << i;
		SpriteGo* characterIcons = (SpriteGo*)FindGo(ss.str());
		characterIcons->SetOrigin(Origins::TL);
		characterIcons->sprite.setTextureRect({ 0,0,25,25 });
		characterIcons->SetPosition(250+(55*i), 190);
		characterIcons->SetSize(2, 2);
		characterIcons->sortLayer = 112;
		ss << "Back";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::TL);
		spr->SetPosition(characterIcons->GetPosition());
		spr->SetSize(2, 2);
		spr->sortLayer = 111;
		ss << "Line";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::TL);
		spr->SetPosition(characterIcons->GetPosition());
		spr->SetSize(2, 2);
		spr->sortLayer = 113;
		
	}
	spr = (SpriteGo*)FindGo("IconAtk0");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(60,215);
	spr->SetSize(2, 2);
	spr->sortLayer = 111;

	spr = (SpriteGo*)FindGo("IconDef0");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(150, 215);
	spr->SetSize(2, 2);
	spr->sortLayer = 111;

	UiButton* bt = (UiButton*)FindGo("TrainPointReturnB");
	bt->SetOrigin(Origins::ML);
	bt->SetPosition(495, 245);
	bt->SetSize(1.5, 1.2);
	bt->sortLayer = 111;

	bt = (UiButton*)FindGo("TrainCloseB");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(1175, 555);
	bt->SetSize(2, 2);
	bt->sortLayer = 111;
	bt->OnClick = [this]() {
		isMenuOn = false;
		UiTrainingOpen(false);
		UiSponsorContractOpen(true,false);
		UiSponsorContractOpen(false, false);
	};

	for (int i = 0; i < 12; i++)
	{
		std::stringstream ss;
		ss << "PlayerList" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::TL);
		bt->SetPosition(42, 308+(i*35));
		bt->SetSize(2, 2);
		bt->sortLayer = 111;
		bt->OnClick = [this,i]() {
			UiTrainingPlayerSelect(i);
		};
	}
}

void SceneHome::MakeSubUiSponsorContract()
{
	SpriteGo* spr = (SpriteGo*)FindGo("SponBg");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f,
		FRAMEWORK.GetWindowSize().y * 0.44f);
	spr->SetSize((FRAMEWORK.GetWindowSize().x / spr->GetSize().x) - 0.05f,
		(FRAMEWORK.GetWindowSize().x / spr->GetSize().x) - 0.05f);
	spr->sortLayer = 110;
	spr->SetActive(false);

	for (int i = 0; i < 9; i++)
	{
		std::stringstream ss;
		ss << "SponSlot" << i;
		UiButton* bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::TL);
		bt->SetPosition(65+((bt->GetSize().x*2+5) *(i%3)), 175+ ((bt->GetSize().y * 2+5) * (i / 3)));
		bt->SetSize(2,2);
		bt->sortLayer = 111;
		bt->OnClick = [this,i]() {
			UiSponsorContractSelect(i);
		};
		bt->SetActive(false);
		
		ss.str("");
		ss << "SponLogo" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::TL);
		spr->SetPosition(bt->GetPosition().x+5, bt->GetPosition().y+7);
		spr->SetSize(2, 2);
		spr->sortLayer = 112;
		spr->SetActive(false);

		ss.str("");
		ss << "SponsorName" << i;
		TextGo* text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(L"스폰서 이름");
		text->text.setCharacterSize(25);
		text->SetOrigin(Origins::ML);
		text->SetPosition(bt->GetPosition().x + 76, bt->GetPosition().y + 35);
		text->sortLayer = 112;
		text->SetActive(false);
	}

	spr = (SpriteGo*)FindGo("SponLogoBack");
	spr->SetOrigin(Origins::TL);
	spr->SetPosition(950,195);
	spr->SetSize(2.3,2.3);
	spr->sortLayer = 111;
	spr->SetActive(false);

	spr = (SpriteGo*)FindGo("SelectedLogo");
	spr->SetOrigin(Origins::TL);
	spr->SetPosition(950, 195);
	spr->SetSize(2, 2);
	spr->sortLayer = 112;
	spr->SetActive(false);

	spr = (SpriteGo*)FindGo("GoldUiEx");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(1225, 407);
	spr->SetSize(2, 2);
	spr->sortLayer = 112;
	spr->SetActive(false);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "PartBack" << i;
		spr = (UiButton*)FindGo(ss.str());
		spr->SetOrigin(Origins::TL);
		spr->SetPosition(950+72*i, 425);
		spr->SetSize(1.7, 1.7);
		spr->sortLayer = 112;
		spr->SetActive(false);
	}

	spr = (SpriteGo*)FindGo("SoundChip");
	spr->SetOrigin(Origins::TL);
	spr->SetPosition(950, 430);
	spr->SetSize(1, 1);
	spr->sortLayer = 113;
	spr->SetActive(false);

	spr = (SpriteGo*)FindGo("KeyboardSwitch");
	spr->SetOrigin(Origins::TL);
	spr->SetPosition(950 + 72, 430);
	spr->SetSize(1, 1);
	spr->sortLayer = 113;
	spr->SetActive(false);

	spr = (SpriteGo*)FindGo("Screw");
	spr->SetOrigin(Origins::TL);
	spr->SetPosition(950 + 74 * 2, 430);
	spr->SetSize(1, 1);
	spr->sortLayer = 113;
	spr->SetActive(false);

	spr = (SpriteGo*)FindGo("FabricPiece");
	spr->SetOrigin(Origins::TL);
	spr->SetPosition(950 + 73 * 3, 430);
	spr->SetSize(1, 1);
	spr->sortLayer = 113;
	spr->SetActive(false);

	UiButton* bt = (UiButton*)FindGo("SponsorContract");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(485,470);
	bt->SetSize(2, 2);
	bt->sortLayer = 111;
	bt->OnClick = [this]() {

	};
	bt->SetActive(false);

	TextGo* text = (TextGo*)FindGo("SelectedSponsorName");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"스폰서 이름");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::ML);
	text->SetPosition(1020, 210);
	text->sortLayer = 112;
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorQuest");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"- 요구사항");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::ML);
	text->SetPosition(1020, 240);
	text->sortLayer = 112;
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorMoney");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("1000");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::MR);
	text->SetPosition(1200, 405);
	text->sortLayer = 112;
	text->SetActive(false);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "SponsorParts" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("x10");
		text->text.setCharacterSize(14);
		text->SetOrigin(Origins::BL);
		text->SetPosition(987+72*i, 455);
		text->sortLayer = 112;
		text->SetActive(false);
	}
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
		if (i == 11 || i == 12)
		{
			continue;
		}
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
	if (isMenuOn)
	{
		return;
	}
	switch (index)
	{
	case 5:
		Recruit(0, 0);
		TEAM_MGR.Employ(0);
		UpdateMoney();
		break;
	case 6:
		UiTrainingOpen();
		isMenuOn = true;//임시코드
		break;
	case 7:
		TEAM_MGR.ShowPlayer();
		break;
	case 8:
		UiSponsorContractOpen(false,true);
		break;
	case 13:
		std::cout << "오늘의 일정은 " ;
		{
			switch (TEAM_MGR.GetSchedule(TEAM_MGR.GetTodayDate()))
			{
			case TeamMgr::Schedule::Recruit:
				std::cout << "선수모집일";
				break;
			case TeamMgr::Schedule::PracticeLeague:
				std::cout << "연습경기일";
				break;
			case TeamMgr::Schedule::League:
				std::cout << "경기일";
				break;
			case TeamMgr::Schedule::Vacation:
				std::cout << "선수단 전체 휴가";
				break;
			case TeamMgr::Schedule::EventLeague:
				std::cout << "이벤트 경기";
				break;
			default:
				break;
			}
		}
		std::cout << " 입니다." << std::endl;
		break;
	case 15:
	{
		std::vector<AiTeam> aiT = TEAM_MGR.GetAiTeamInfo();
		for (int i = 0; i < 7; i++)
		{
			std::string name;
			name.assign(aiT[i].name.begin(), aiT[i].name.end());
			std::cout << name << std::endl;
			for (int j = 0; j < 4; j++)
			{
				std::string pname;
				pname.assign(aiT[i].player[j].name.begin(), aiT[i].player[j].name.end());
				std::cout << pname << std::endl;
			}
			std::cout << std::endl;
		}
	}
		break;
	default:
		break;
	}
	//isMenuOn = true;
	//나중에 주석풀기
}

void SceneHome::UiTrainingOpen(bool on)
{
	SpriteGo* spr = (SpriteGo*)FindGo("TrainingUi");
	spr->SetActive(on);

	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "TrainingSlotUi" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);
		ss.str("");
		ss << "XpBack" << i;
		SpriteGo* xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetActive(false);
		ss.str("");
		ss << "XpCur" << i;
		xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetActive(false);
		ss.str("");
		ss << "XpFuture" << i;
		xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetActive(false);

		ss.str("");
		ss << "TrainingPointDownB" << i;
		UiButton* trainPointB = (UiButton*)FindGo(ss.str());
		trainPointB->SetActive(false);
		ss.str("");
		ss << "TrainingPointUpB" << i;
		trainPointB = (UiButton*)FindGo(ss.str());
		trainPointB->SetActive(false);

		for (int j = i * 8; j < i * 8 + 8; j++)
		{
			std::stringstream ss;
			ss << "TrainingPointGuage" << j;
			SpriteGo* trainPoint = (SpriteGo*)FindGo(ss.str());
			trainPoint->SetActive(false);
		}
		if (i > 1)
		{
			ss.str("");
			ss << "CharacterStatIcons" << i - 2;
			SpriteGo* charIcon = (SpriteGo*)FindGo(ss.str());
			charIcon->SetActive(false);
		}
		if (i == 0)
		{
			SpriteGo* statIcon = (SpriteGo*)FindGo("IconAtk1");
			statIcon->SetActive(false);
		}
		if (i == 1)
		{
			SpriteGo* statIcon = (SpriteGo*)FindGo("IconDef1");
			statIcon->SetActive(false);
		}
	}

	for (int i = 0; i < 8; i++)
	{
		std::stringstream ss;
		ss << "LeftTrainingPointGuage" << i;
		SpriteGo* leftTrainPoint = (SpriteGo*)FindGo(ss.str());
		leftTrainPoint->SetActive(false);
	}

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "CharacterIcons" << i;
		SpriteGo* characterIcons = (SpriteGo*)FindGo(ss.str());
		characterIcons->SetActive(false);
		ss << "Back";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);
		ss << "Line";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);

	}
	spr = (SpriteGo*)FindGo("IconAtk0");
	spr->SetActive(on);

	spr = (SpriteGo*)FindGo("IconDef0");
	spr->SetActive(on);

	UiButton* bt = (UiButton*)FindGo("TrainPointReturnB");
	bt->SetActive(false);

	bt = (UiButton*)FindGo("TrainCloseB");
	bt->SetActive(on);

	RectGo* rect = (RectGo*)FindGo("UiShade");
	rect->SetActive(on);

	int playerNum = TEAM_MGR.GetPlayerNum();
	for (int i = 0; i < playerNum; i++)
	{
		std::stringstream ss;
		ss << "PlayerList" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/player_list_bg_0.png"));
		bt->SetActive(on);
	}
	for (int i = playerNum; i < 12; i++)
	{
		std::stringstream ss;
		ss << "PlayerList" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);
	}
	if (on)
	{
		gainTrainingInfo = TEAM_MGR.GetTrainingInfo();
		maxTrainingPoint = TEAM_MGR.GetMaxTrainingPoint();
	}
	else
	{
		TEAM_MGR.SetTrainingInfo(gainTrainingInfo);
	}
}

void SceneHome::UiTrainingPlayerSelect(int index)
{
	PlayerInfo selectedPlayer = TEAM_MGR.GetPlayerInfo()[index];
	TrainingInfo* tInfo = &gainTrainingInfo[index];
	SpriteGo* spr;
	for (int i = 0; i < 2+ selectedPlayer.knownChamp; i++)
	{
		std::stringstream ss;
		ss << "TrainingSlotUi" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(true);
		ss.str("");
		ss << "XpBack" << i;
		SpriteGo* xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetActive(true);
		ss.str("");
		ss << "XpCur" << i;
		xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetActive(true);
		switch (i)
		{
		case 0: 
			xpBar->SetSize(tInfo->xpAtk * 2.f
				/(1300 + (selectedPlayer.attack * 10)), 2.f);
			break;
		case 1:
			xpBar->SetSize(tInfo->xpDef * 2.f
				/ (1300 + (selectedPlayer.defence * 10)), 2.f);
			break;
		default:
			xpBar->SetSize(tInfo->xpChamp[i-2] * 2.f
				/ (1300 + (selectedPlayer.proficiency[i - 2].second * 10)), 2.f);
			break;
		}
		
		ss.str("");
		ss << "XpFuture" << i;
		xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetActive(true);
		TrainingInfo futureTrainingInfo =
			TEAM_MGR.GetGrowStats(gainTrainingInfo)[index];
		switch (i)
		{
		case 0:
			xpBar->SetSize(std::min(futureTrainingInfo.xpAtk * 2.f
				/ (1300 + (selectedPlayer.attack * 10)), 2.f), 2.f);
			break;
		case 1:
			xpBar->SetSize(std::min(futureTrainingInfo.xpDef * 2.f
				/ (1300 + (selectedPlayer.defence * 10)),2.f), 2.f);
			break;
		default:
			xpBar->SetSize(std::min(futureTrainingInfo.xpChamp[i - 2] * 2.f
				/ (1300 + (selectedPlayer.proficiency[i - 2].second * 10)), 2.f)
				, 2.f);
			break;
		}


		ss.str("");
		ss << "TrainingPointDownB" << i;
		UiButton* trainPointB = (UiButton*)FindGo(ss.str());
		trainPointB->SetActive(true); 
		switch (i)
		{
		case 0:
			trainPointB->OnClick = [this, index]() {
				if (0 < gainTrainingInfo[index].trainingAtk)
				{
					gainTrainingInfo[index].trainingAtk--;
					gainTrainingInfo[index].usedTrainingPoint--;
					UiTrainingGaugeUpdate(index);
				}
			};
			break;
		case 1:
			trainPointB->OnClick = [this, index]() {
				if (0 < gainTrainingInfo[index].trainingDef)
				{
					gainTrainingInfo[index].trainingDef--;
					gainTrainingInfo[index].usedTrainingPoint--;
					UiTrainingGaugeUpdate(index);
				}
			};
			break;
		default:
			trainPointB->OnClick = [this, index,i]() {
				if (0 < gainTrainingInfo[index].trainingChamp[i-2])
				{
					gainTrainingInfo[index].trainingChamp[i - 2]--;
					gainTrainingInfo[index].usedTrainingPoint--;
					UiTrainingGaugeUpdate(index);
				}
			};
			break;
		}

		ss.str("");
		ss << "TrainingPointUpB" << i;
		trainPointB = (UiButton*)FindGo(ss.str());
		trainPointB->SetActive(true);
		switch (i)
		{
		case 0:
			trainPointB->OnClick = [this, index]() {
				if (gainTrainingInfo[index].usedTrainingPoint
					< maxTrainingPoint)
				{
					gainTrainingInfo[index].trainingAtk++;
					gainTrainingInfo[index].usedTrainingPoint++;
					UiTrainingGaugeUpdate(index);
				}
			};
			break;
		case 1:
			trainPointB->OnClick = [this, index]() {
				if (gainTrainingInfo[index].usedTrainingPoint
					<maxTrainingPoint)
				{
					gainTrainingInfo[index].trainingDef++;
					gainTrainingInfo[index].usedTrainingPoint++;
					UiTrainingGaugeUpdate(index);
				}
			};
			break;
		default:
			trainPointB->OnClick = [this, index, i]() {
				if (gainTrainingInfo[index].usedTrainingPoint
					< maxTrainingPoint)
				{
					gainTrainingInfo[index].trainingChamp[i - 2]++;
					gainTrainingInfo[index].usedTrainingPoint++;
					UiTrainingGaugeUpdate(index);
				}
			};
			break;
		}

		for (int j = i * 8; j < i * 8 + maxTrainingPoint; j++)
		{
			ss.str("");
			ss << "TrainingPointGuage" << j;
			SpriteGo* trainPoint = (SpriteGo*)FindGo(ss.str());
			trainPoint->SetActive(true);
		}

		if (i > 1)
		{
			ss.str("");
			ss << "CharacterStatIcons" << i - 2;
			SpriteGo* charIcon = (SpriteGo*)FindGo(ss.str());
			charIcon->SetActive(true);
			std::stringstream champ;
			champ << "graphics/Origin/Sprite/character_icons_";
			switch (selectedPlayer.proficiency[i-2].first)
			{
			case 0:
				champ << 0;
				break;
			case 1:
				champ << 2;
				break;
			default:
				champ << selectedPlayer.proficiency[i - 2].first + 3;
				break;
			}
			champ << ".png";
			charIcon->sprite.setTexture((*RESOURCE_MGR.GetTexture(champ.str())));
		}
		if (i == 0)
		{
			SpriteGo* statIcon = (SpriteGo*)FindGo("IconAtk1");
			statIcon->SetActive(true);
		}
		if (i == 1)
		{
			SpriteGo* statIcon = (SpriteGo*)FindGo("IconDef1");
			statIcon->SetActive(true);
		}
	}

	for (int i = 2 + selectedPlayer.knownChamp; i < 6; i++)
	{
		std::stringstream ss;
		ss << "TrainingSlotUi" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);
		ss.str("");
		ss << "XpBack" << i;
		SpriteGo* xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetActive(false);
		ss.str("");
		ss << "XpCur" << i;
		xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetActive(false);
		ss.str("");
		ss << "XpFuture" << i;
		xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetActive(false);

		ss.str("");
		ss << "TrainingPointDownB" << i;
		UiButton* trainPointB = (UiButton*)FindGo(ss.str());
		trainPointB->SetActive(false);
		ss.str("");
		ss << "TrainingPointUpB" << i;
		trainPointB = (UiButton*)FindGo(ss.str());
		trainPointB->SetActive(false);

		for (int j = i * 8; j < i * 8 + 8; j++)
		{
			std::stringstream ss;
			ss << "TrainingPointGuage" << j;
			SpriteGo* trainPoint = (SpriteGo*)FindGo(ss.str());
			trainPoint->SetActive(false);
		}
		ss.str("");
		ss << "CharacterStatIcons" << i - 2;
		SpriteGo* charIcon = (SpriteGo*)FindGo(ss.str());
		charIcon->SetActive(false);

	}

	for (int i = 0; i < 8; i++)
	{
		std::stringstream ss;
		ss << "LeftTrainingPointGuage" << i;
		SpriteGo* leftTrainPoint = (SpriteGo*)FindGo(ss.str());
		if (i< maxTrainingPoint)
		{
			leftTrainPoint->SetActive(true);
		}
	}

	UiTrainingGaugeUpdate(index);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "CharacterIcons" << i;
		SpriteGo* characterIcons = (SpriteGo*)FindGo(ss.str());
		if (i < selectedPlayer.knownChamp)
		{
			characterIcons->SetActive(true);
			std::stringstream champ;
			champ << "graphics/Origin/Sprite/character_icons_";
			switch (selectedPlayer.proficiency[i].first)
			{
			case 0:
				champ << 0;
				break;
			case 1:
				champ << 2;
				break;
			default:
				champ << selectedPlayer.proficiency[i].first + 3;
				break;
			}
			champ << ".png";
			characterIcons->sprite.setTexture((*RESOURCE_MGR.GetTexture(champ.str())));
			ss << "Back";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetActive(true);
			ss << "Line";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetActive(true);
		}
		else
		{
			characterIcons->SetActive(false);
			ss << "Back";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetActive(false);
			ss << "Line";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetActive(false);
		}
	}

	UiButton* bt = (UiButton*)FindGo("TrainPointReturnB");
	bt->SetActive(true);
	bt->OnClick = [this,index]() {
		gainTrainingInfo[index].usedTrainingPoint = 0;
		gainTrainingInfo[index].trainingAtk = 0;
		gainTrainingInfo[index].trainingDef = 0;
		for (int i = 0; i < 4; i++)
		{
			gainTrainingInfo[index].trainingChamp[i] = 0;
		}
		UiTrainingGaugeUpdate(index);
	};

	int playerNum = TEAM_MGR.GetPlayerNum();
	for (int i = 0; i < playerNum; i++)
	{
		std::stringstream ss;
		ss << "PlayerList" << i;
		bt = (UiButton*)FindGo(ss.str());
		if (i == index)
		{
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/player_list_bg_2.png"));
		}
		else
		{
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/player_list_bg_0.png"));
		}
	}
}

void SceneHome::UiTrainingGaugeUpdate(int index)
{
	for (int i = 0; i < 8; i++)
	{
		std::stringstream ss;
		ss << "TrainingPointGuage" << i;
		SpriteGo* trainPoint = (SpriteGo*)FindGo(ss.str());
		if (i < gainTrainingInfo[index].trainingAtk)
		{
			trainPoint->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/training_point_guage_1.png"));
		}
		else
		{
			trainPoint->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/training_point_guage_0.png"));
		}
		ss.str("");
		ss << "LeftTrainingPointGuage" << i;
		SpriteGo* leftTrainPoint = (SpriteGo*)FindGo(ss.str());
		if (i< maxTrainingPoint - gainTrainingInfo[index].usedTrainingPoint)
		{
			leftTrainPoint->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/unused_point_guage_1.png"));
		}
		else
		{
			leftTrainPoint->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/unused_point_guage_0.png"));
		}
	}
	for (int i = 8; i < 16; i++)
	{
		std::stringstream ss;
		ss << "TrainingPointGuage" << i;
		SpriteGo* trainPoint = (SpriteGo*)FindGo(ss.str());
		if (i-8 < gainTrainingInfo[index].trainingDef)
		{
			trainPoint->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/training_point_guage_1.png"));
		}
		else
		{
			trainPoint->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/training_point_guage_0.png"));
		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			std::stringstream ss;
			ss << "TrainingPointGuage" << 16 + (i * 8) + j;
			SpriteGo* trainPoint = (SpriteGo*)FindGo(ss.str());
			if (j < gainTrainingInfo[index].trainingChamp[i])
			{
				trainPoint->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/training_point_guage_1.png"));
			}
			else
			{
				trainPoint->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/training_point_guage_0.png"));
			}
		}
	}
}

void SceneHome::UiSponsorContractOpen(bool contract, bool on)
{
	SpriteGo* spr = (SpriteGo*)FindGo("SponBg");
	spr->SetActive(on);

	spr = (SpriteGo*)FindGo("SponLogoBack");
	spr->SetActive(on);

	spr = (SpriteGo*)FindGo("SelectedLogo");
	spr->SetActive(on);

	spr = (SpriteGo*)FindGo("GoldUiEx");
	spr->SetActive(on);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "PartBack" << i;
		spr = (UiButton*)FindGo(ss.str());
		spr->SetActive(on);
	}

	spr = (SpriteGo*)FindGo("SoundChip");
	spr->SetActive(on);

	spr = (SpriteGo*)FindGo("KeyboardSwitch");
	spr->SetActive(on);

	spr = (SpriteGo*)FindGo("Screw");
	spr->SetActive(on);

	spr = (SpriteGo*)FindGo("FabricPiece");
	spr->SetActive(on);

	TextGo* text = (TextGo*)FindGo("SelectedSponsorName");
	text->SetActive(on);

	text = (TextGo*)FindGo("SponsorQuest");
	text->SetActive(on);

	text = (TextGo*)FindGo("SponsorMoney");
	text->SetActive(on);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "SponsorParts" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);
	}

	UiButton* bt = (UiButton*)FindGo("TrainCloseB");
	bt->SetActive(on);

	RectGo* rect = (RectGo*)FindGo("UiShade");
	rect->SetActive(on);

	if (contract)
	{
		for (int i = 0; i < 9; i++)
		{
			std::stringstream ss;
			ss << "SponSlot" << i;
			UiButton* bt = (UiButton*)FindGo(ss.str());
			bt->SetActive(on);

			ss.str("");
			ss << "SponLogo" << i;
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetActive(on);

			ss.str("");
			ss << "SponsorName" << i;
			TextGo* text = (TextGo*)FindGo(ss.str());
			text->SetActive(on);
		}
		bt = (UiButton*)FindGo("SponsorContract");
		bt->SetOrigin(Origins::MC);
		bt->SetPosition(485, 470);
		bt->SetSize(2, 2);
		bt->sortLayer = 111;
		bt->OnClick = [this]() {

		};
		bt->SetActive(on);
	}
	else
	{

	}
}

void SceneHome::UiSponsorContractSelect(int index)
{
}

void SceneHome::UpdateMoney()
{
	TextGo* text = (TextGo*)FindGo("MoneyInfoT");
	text->text.setString(std::to_string(TEAM_MGR.GetMoney()));
	text->SetOrigin(Origins::TL);
}

void SceneHome::Recruit(int grade, int slotNum)
{
	PlayerInfo player;
	std::wstring name;
	int age;
	int attack;
	int defence;
	int knownChamp;
	std::vector<std::pair<int, int>> proficiency;
	int knownCharacter;
	std::vector<int> characteristic;
	int contract_cost;
	int potential;
	switch (grade)
	{
	case 0:
		if (TEAM_MGR.GetMoney() < 10 || TEAM_MGR.CheckRecruitSlot(slotNum) == true)
		{
			return;
		}
		MakeLocalPlayer(player);
		TEAM_MGR.UseMoney(10);
		break;
	default:
		break;
	}
	TEAM_MGR.Recruit(slotNum, player);
}

void SceneHome::NewYear()
{
	std::vector<AiTeam> newAiTeams = std::vector<AiTeam>(7);
	for (int i = 0; i < 7; i++)
	{
		std::stringstream ss;
		ss << "Team" << i+1;
		std::string newName = ss.str();
		newAiTeams[i].name.assign(newName.begin(), newName.end());
	}
	switch (TEAM_MGR.GetLeagueGrade())
	{
	case TeamMgr::LeagueGrade::Amateur:
		for (int i = 0; i < 7; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				MakeLocalPlayer(newAiTeams[i].player[j]);
			}
		}
		break;
	default:
		break;
	}
	TEAM_MGR.SetAiTeams(newAiTeams);
}

void SceneHome::MakeLocalPlayer(PlayerInfo& player)
{
	{
		std::stringstream ss;
		ss << "PlayerName" << Utils::RandomRange(0, 9);
		auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
		player.name = stringtable->GetW(ss.str());
	}
	player.age = Utils::RandomRange(18, 21);
	player.attack = Utils::RandomRange(9, 16);
	player.defence = 25 - player.attack;
	player.knownChamp = Utils::RandomRange(1, 3);
	for (int i = 0; i < player.knownChamp; i++)
	{	
		while (true)
		{
			int champCode = Utils::RandomRange(0, TEAM_MGR.GetAbleChamps() - 1);
			bool checker = true;
			for (int j = 0; j < i; j++)
			{
				if (player.proficiency[j].first == champCode)
				{
					checker = false;
				}
			}
			if (checker)
			{
				player.proficiency.push_back({champCode,Utils::RandomRange(5, 7) });
				break;
			}
		}
	}
	player.knownCharacter = Utils::RandomRange(0, 3);
	for (int i = 0; i < player.knownCharacter; i++)
	{
		player.characteristic.push_back(
			Utils::RandomRange(0, TEAM_MGR.GetAbleCharacteristic() - 1));
	}
	player.contract_cost = Utils::RandomRange(95, 105 + (player.knownChamp * 10) + (player.knownCharacter * 10));
	player.potential = 0;
}

void SceneHome::TestingCheats()
{
	//***********************************************
	//					Cheat Mode
	//***********************************************
	//
	//	M :				Earn money 100
	//	LShift + M :	Earn money 10000
	// 
	// ESC :			SceneTitle
	// Enter :			SceneGame
	//
	//-----------------------------------------------

	if (!cheatMode)
	{
		return;
	}

	//Point Mouse vector2f
	if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
	{
		std::cout << INPUT_MGR.GetMousePos().x << "\t"
			<< INPUT_MGR.GetMousePos().y << std::endl;
	}

	//ShowMeTheMoney!!!
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::M))
	{
		if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
		{
			TEAM_MGR.EarnMoney(10000);
		}
		else
		{
			TEAM_MGR.EarnMoney(100);
		}
		UpdateMoney();
	}

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
