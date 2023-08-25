#include "stdafx.h"
#include "SceneChampEdit.h"
#include <filesystem>

#include "DataTableMgr.h"
#include "TeamMgr.h"
#include "SkillMgr.h"
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
#include "RectGo.h"
#include "BindPlayerEffect.h"
#include "rapidcsv.h"

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
	
	InitDefaultUi();
	InitCSVMaker();

	AddGo(new RectGo("CharCenter"));

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

	frameInfo = std::vector<FrameInfo>(100);
	curChampCode = 0;
	isCSVMakerOn = false;
	curChampAni = 1;
	
	EnterDefaultUi();
	EnterCSVMaker();
	ActivceDefaultUi(false);
	ActiveCSVMaker();
}

void SceneChampEdit::Exit()
{
	Scene::Exit();
}

void SceneChampEdit::Update(float dt)
{
	Scene::Update(dt);	
	animation.Update(dt);
	champ.SetOrigin(Origins::MC);

	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Escape))
	{
		SCENE_MGR.ChangeScene(SceneId::Title);
	}
}

void SceneChampEdit::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
}


void SceneChampEdit::InitDefaultUi()
{
	// UiButton
	AddGo(new UiButton("graphics/Origin/Sprite/green_arrow_button_0.png", "ChangeChampL"));
	AddGo(new UiButton("graphics/Origin/Sprite/green_arrow_button_3.png", "ChangeChampR"));
	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "ChangeMotionB" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", ss.str()));
	}
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "MotionPosTypeB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_0.png", "CsvMakeB"));

	// SpriteGo
	champ = SpriteGo("graphics/UiFix/character_icons_0.png", "Champ");
	AddGo(&champ);

	AddGo(new SpriteGo("graphics/Origin/Sprite/weekly_event_button_3.png", "ChampNameBox"));

	//TextGo
	AddGo(new TextGo("ChampNameText"));

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "ChampMotionText" << i;
		AddGo(new TextGo(ss.str()));
	}
	AddGo(new TextGo("MotionPosTypeBText"));
	AddGo(new TextGo("CsvMakeBText"));
}

void SceneChampEdit::InitCSVMaker()
{
	AddGo(new SpriteGo("", "CSVEffectPreview"));
	AddGo(new SpriteGo("", "CSVEffectEditPreview"));

	AddGo(new RectGo("PreviewBound"));

	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "MaxFrameB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "CurFrameB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "LeftB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "TopB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "WidthB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "HeightB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "FpsB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "LoopTypeB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_0.png", "CSVLoadB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_0.png", "CSVSaveB"));

	AddGo(new TextGo("MaxFrameBText"));
	AddGo(new TextGo("CurFrameBText"));
	AddGo(new TextGo("LeftBText"));
	AddGo(new TextGo("TopBText"));
	AddGo(new TextGo("WidthBText"));
	AddGo(new TextGo("HeightBText"));
	AddGo(new TextGo("FpsBText"));
	AddGo(new TextGo("LoopTypeBText"));
	AddGo(new TextGo("CSVLoadBText"));
	AddGo(new TextGo("CSVSaveBText"));
}

void SceneChampEdit::EnterDefaultUi()
{
	State* defaultChamp = CHAMPION_MGR.GetChampion(0);

	champ.SetPosition(FRAMEWORK.GetWindowSize() * 0.5f);
	champ.SetOrigin(Origins::MC);
	champ.SetSize(2, 2);

	animation = defaultChamp->animaition;
	animation.SetTarget(&champ.sprite);
	animation.Play("Idle");
	champ.SetOrigin(Origins::MC);

	UiButton* bt;
	SpriteGo* spr;
	TextGo* text;
	RectGo* rect;

	rect = (RectGo*)FindGo("CharCenter");
	rect->SetSize({ 5.f,5.f });
	rect->rectangle.setFillColor(sf::Color::Red);
	rect->rectangle.setOutlineColor(sf::Color::Black);
	rect->rectangle.setOutlineThickness(2);
	rect->SetPosition(champ.GetPosition());
	rect->SetOrigin(Origins::MC);
	rect->sortLayer = 1;

	spr = (SpriteGo*)FindGo("ChampNameBox");
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f, 50);
	spr->SetOrigin(Origins::MC);
	spr->SetSize(2, 2);
	spr->sortLayer = 100;

	bt = (UiButton*)FindGo("ChangeChampL");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.35f, 50);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnClick = [this]() {
		if (curChampCode - 1 >= 0)
		{
			curChampCode = curChampCode - 1;
		}
		else
		{
			curChampCode = TEAM_MGR.GetTotalChampsCount() - 1;
		}
		State* CurChamp = CHAMPION_MGR.GetChampion(curChampCode);
		animation = CurChamp->animaition;
		animation.SetTarget(&champ.sprite);
		animation.Play("Idle");
		champ.SetOrigin(Origins::MC);
		TextGo* text = (TextGo*)FindGo("ChampNameText");
		text->text.setString(CurChamp->charId);
		text->SetOrigin(Origins::MC);
	};

	bt = (UiButton*)FindGo("ChangeChampR");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.65f, 50);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnClick = [this]() {
		curChampCode = (curChampCode + 1) % TEAM_MGR.GetTotalChampsCount();
		State* CurChamp = CHAMPION_MGR.GetChampion(curChampCode);
		animation = CurChamp->animaition;
		animation.SetTarget(&champ.sprite);
		animation.Play("Idle");
		champ.SetOrigin(Origins::MC);
		TextGo* text = (TextGo*)FindGo("ChampNameText");
		text->text.setString(CurChamp->charId);
		text->SetOrigin(Origins::MC);
	};

	text = (TextGo*)FindGo("ChampNameText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(defaultChamp->charId);
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(spr->GetPosition());
	text->sortLayer = 101;

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "ChangeMotionB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetPosition((FRAMEWORK.GetWindowSize().x * 0.725f) + (i * 60.f), 50.f);
		bt->SetOrigin(Origins::MC);
		bt->SetSize(0.3, 2);
		bt->sortLayer = 100;
		switch (i)
		{
		case 0:
			bt->OnClick = [this]() {
				State* CurChamp = CHAMPION_MGR.GetChampion(curChampCode);
				animation = CurChamp->animaition;
				animation.SetTarget(&champ.sprite);
				animation.Play("Idle");
				champ.SetOrigin(Origins::MC);
				curChampAni = 0;
			};
			break;
		case 1:
			bt->OnClick = [this]() {
				State* CurChamp = CHAMPION_MGR.GetChampion(curChampCode);
				animation = CurChamp->animaition;
				animation.SetTarget(&champ.sprite);
				animation.Play("Attack");
				champ.SetOrigin(Origins::MC);
				curChampAni = 1;
			};
			break;
		case 2:
			bt->OnClick = [this]() {
				ChampionSkill* CurChamp = SKILL_MGR.GetSkill(CHAMPION_MGR.GetChampion(curChampCode)->skillCode1);
				animation = CurChamp->animaition;
				animation.SetTarget(&champ.sprite);
				animation.Play("Skill");
				champ.SetOrigin(Origins::MC);
				curChampAni = 2;
			};
			break;
		case 3:
			bt->OnClick = [this]() {
				ChampionSkill* CurChamp = SKILL_MGR.GetSkill(CHAMPION_MGR.GetChampion(curChampCode)->skillCode2);
				animation = CurChamp->animaition;
				animation.SetTarget(&champ.sprite);
				animation.Play("UltiSkill");
				champ.SetOrigin(Origins::MC);
				curChampAni = 3;
			};
			break;
		}

	
		ss.str("");
		ss << "ChampMotionText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		switch (i)
		{
		case 0:
			text->text.setString("I");
			break;
		case 1:
			text->text.setString("A");
			break;
		case 2:
			text->text.setString("S");
			break;
		case 3:
			text->text.setString("U");
			break;
		}
		text->text.setCharacterSize(24);
		text->SetOrigin(Origins::MC);
		text->SetPosition(bt->GetPosition());
		text->sortLayer = 101;
	}

	bt = (UiButton*)FindGo("MotionPosTypeB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 100);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnClick = [this]() {
	};
	text = (TextGo*)FindGo("MotionPosTypeBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("BindPlayerPos");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("CsvMakeB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 150);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnClick = [this]() {
		ActivceDefaultUi(isCSVMakerOn);
		isCSVMakerOn = !isCSVMakerOn;
	};

	text = (TextGo*)FindGo("CsvMakeBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("CSV Edit");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;
}

void SceneChampEdit::ActivceDefaultUi(bool on)
{
	State* defaultChamp = CHAMPION_MGR.GetChampion(0);

	champ.SetActive(on);

	UiButton* bt;
	SpriteGo* spr;
	TextGo* text;
	RectGo* rect;

	rect = (RectGo*)FindGo("CharCenter");
	rect->SetActive(on);

	spr = (SpriteGo*)FindGo("ChampNameBox");
	spr->SetActive(on);

	bt = (UiButton*)FindGo("ChangeChampL");
	bt->SetActive(on);

	bt = (UiButton*)FindGo("ChangeChampR");
	bt->SetActive(on);

	text = (TextGo*)FindGo("ChampNameText");
	text->SetActive(on);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "ChangeMotionB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(on);

		ss.str("");
		ss << "ChampMotionText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);
	}

	bt = (UiButton*)FindGo("MotionPosTypeB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("MotionPosTypeBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("CsvMakeB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 150);
	if (on)
	{
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/weekly_event_button_0.png"));
	}
	else
	{
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/weekly_event_button_1.png"));
	}

}

void SceneChampEdit::EnterCSVMaker()
{
	UiButton* bt;
	SpriteGo* spr;
	TextGo* text;
	RectGo* rect;

	spr = (SpriteGo*)FindGo("CSVEffectPreview");
	spr->SetPosition(0,0);
	spr->SetOrigin(Origins::TL);
	spr->SetSize(2, 2);

	spr = (SpriteGo*)FindGo("CSVEffectEditPreview");
	spr->SetPosition(FRAMEWORK.GetWindowSize() * 0.5f);
	spr->SetOrigin(Origins::MC);
	spr->SetSize(2, 2);

	rect = (RectGo*)FindGo("PreviewBound");
	rect->rectangle.setFillColor(sf::Color(0, 0, 0, 0));
	rect->rectangle.setOutlineColor(sf::Color::Red);
	rect->rectangle.setOutlineThickness(1);
	rect->SetSize({ spr->sprite.getGlobalBounds().width,spr->sprite.getGlobalBounds().height });
	rect->SetPosition(spr->GetPosition());
	rect->SetOrigin(Origins::MC);
	rect->sortOrder = -1;
	

	bt = (UiButton*)FindGo("MaxFrameB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 200);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				maxFrame = std::min(maxFrame + 5, 99);
			}
			else
			{
				maxFrame = std::min(maxFrame + 1, 99);
			}
			TextGo* tx = (TextGo*)FindGo("MaxFrameBText");
			tx->text.setString("Max Frame: " + std::to_string(maxFrame));
			tx->SetOrigin(Origins::MC);
		}
		else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				maxFrame = std::max(maxFrame - 5, 0);
			}
			else
			{
				maxFrame = std::max(maxFrame - 1, 0);
			}
			TextGo* tx = (TextGo*)FindGo("MaxFrameBText");
			tx->text.setString("Max Frame: " + std::to_string(maxFrame));
			tx->SetOrigin(Origins::MC);
		}
	};

	text = (TextGo*)FindGo("MaxFrameBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Max Frame: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("CurFrameB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 250);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				curFrame = std::min(curFrame + 5, maxFrame);
			}
			else
			{
				curFrame = std::min(curFrame + 1, maxFrame);
			}
			TextGo* tx = (TextGo*)FindGo("CurFrameBText");
			tx->text.setString("Cur Frame: " + std::to_string(curFrame));
			tx->SetOrigin(Origins::MC);
			ResetFrameInfo();
			ResetCsvEffectPreview();
		}
		else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				curFrame = std::max(curFrame - 5, 0);
			}
			else
			{
				curFrame = std::max(curFrame - 1, 0);
			}
			TextGo* tx = (TextGo*)FindGo("CurFrameBText");
			tx->text.setString("Cur Frame: " + std::to_string(curFrame));
			tx->SetOrigin(Origins::MC);
			ResetFrameInfo();
			ResetCsvEffectPreview();
		}
	};

	text = (TextGo*)FindGo("CurFrameBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Cur Frame: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("LeftB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 300);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].left = frameInfo[curFrame].left + 10;
			}
			else
			{
				frameInfo[curFrame].left = frameInfo[curFrame].left + 1;
			}
			TextGo* tx = (TextGo*)FindGo("LeftBText");
			tx->text.setString("Left: " + std::to_string(frameInfo[curFrame].left));
			tx->SetOrigin(Origins::MC);
			ResetCsvEffectPreview();
		}
		else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].left = std::max(frameInfo[curFrame].left - 10, 0);
			}
			else
			{
				frameInfo[curFrame].left = std::max(frameInfo[curFrame].left - 1, 0);
			}
			TextGo* tx = (TextGo*)FindGo("LeftBText");
			tx->text.setString("Left: " + std::to_string(frameInfo[curFrame].left));
			tx->SetOrigin(Origins::MC);
			ResetCsvEffectPreview();
		}
	};

	text = (TextGo*)FindGo("LeftBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Left: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("TopB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 350);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].top = frameInfo[curFrame].top + 10;
			}
			else
			{
				frameInfo[curFrame].top = frameInfo[curFrame].top + 1;
			}
			TextGo* tx = (TextGo*)FindGo("TopBText");
			tx->text.setString("Top: " + std::to_string(frameInfo[curFrame].top));
			tx->SetOrigin(Origins::MC);
			ResetCsvEffectPreview();
		}
		else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].top = std::max(frameInfo[curFrame].top - 10, 0);
			}
			else
			{
				frameInfo[curFrame].top = std::max(frameInfo[curFrame].top - 1, 0);
			}
			TextGo* tx = (TextGo*)FindGo("TopBText");
			tx->text.setString("Top: " + std::to_string(frameInfo[curFrame].top));
			tx->SetOrigin(Origins::MC);
			ResetCsvEffectPreview();
		}
	};

	text = (TextGo*)FindGo("TopBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Top: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("WidthB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 400);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].width = frameInfo[curFrame].width + 10;
			}
			else
			{
				frameInfo[curFrame].width = frameInfo[curFrame].width + 1;
			}
			TextGo* tx = (TextGo*)FindGo("WidthBText");
			tx->text.setString("Width: " + std::to_string(frameInfo[curFrame].width));
			tx->SetOrigin(Origins::MC);
			ResetCsvEffectPreview();
		}
		else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].width = std::max(frameInfo[curFrame].width - 10, 0);
			}
			else
			{
				frameInfo[curFrame].width = std::max(frameInfo[curFrame].width - 1, 0);
			}
			TextGo* tx = (TextGo*)FindGo("WidthBText");
			tx->text.setString("Width: " + std::to_string(frameInfo[curFrame].width));
			tx->SetOrigin(Origins::MC);
			ResetCsvEffectPreview();
		}
	};

	text = (TextGo*)FindGo("WidthBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Width: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("HeightB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 450);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].height = frameInfo[curFrame].height + 10;
			}
			else
			{
				frameInfo[curFrame].height = frameInfo[curFrame].height + 1;
			}
			TextGo* tx = (TextGo*)FindGo("HeightBText");
			tx->text.setString("Height: " + std::to_string(frameInfo[curFrame].height));
			tx->SetOrigin(Origins::MC);
			ResetCsvEffectPreview();
		}
		else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].height = std::max(frameInfo[curFrame].height - 10, 0);
			}
			else
			{
				frameInfo[curFrame].height = std::max(frameInfo[curFrame].height - 1, 0);
			}
			TextGo* tx = (TextGo*)FindGo("HeightBText");
			tx->text.setString("Height: " + std::to_string(frameInfo[curFrame].height));
			tx->SetOrigin(Origins::MC);
			ResetCsvEffectPreview();
		}
	};

	text = (TextGo*)FindGo("HeightBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Height: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("FpsB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 500);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				fps += 5;
			}
			else
			{
				fps += 1;
			}
			TextGo* tx = (TextGo*)FindGo("FpsBText");
			tx->text.setString("FPS: " + std::to_string(fps));
			tx->SetOrigin(Origins::MC);
			ResetFrameInfo();
		}
		else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				fps = std::max(fps - 5, 0);
			}
			else
			{
				fps = std::max(fps - 1, 0);
			}
			TextGo* tx = (TextGo*)FindGo("FpsBText");
			tx->text.setString("FPS: " + std::to_string(fps));
			tx->SetOrigin(Origins::MC);
			ResetFrameInfo();
		}
	};

	text = (TextGo*)FindGo("FpsBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("FPS: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("LoopTypeB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 550);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left) || 
			INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			loopType = (loopType + 1) % 2;
			TextGo* tx = (TextGo*)FindGo("LoopTypeBText");
			tx->text.setString("LoopType: " + std::to_string(loopType));
			tx->SetOrigin(Origins::MC);
			ResetFrameInfo();
		}
	};

	text = (TextGo*)FindGo("LoopTypeBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("LoopType: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("CSVLoadB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 600);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnClick = [this]() {
		LoadCsv();
		ResetFrameInfo();
		ResetCsvEffectPreview();
	};

	text = (TextGo*)FindGo("CSVLoadBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Load CSV");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("CSVSaveB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.8f, 650);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnClick = [this]() {
		SaveCsv();
	};

	text = (TextGo*)FindGo("CSVSaveBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Save CSV");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;
}

void SceneChampEdit::ActiveCSVMaker(bool on)
{
	UiButton* bt;
	SpriteGo* spr;
	TextGo* text;
	RectGo* rect;


	if (on)
	{
		LoadCsvMaker();
	}
}

void SceneChampEdit::LoadCsvMaker()
{
	switch (curChampAni)
	{
	case 1:
	{
		std::stringstream ss;
		ss << "graphics/BindPlayerEffect/" << curChampCode << ".png";
		RESOURCE_MGR.Load(ResourceTypes::Texture, ss.str());
		SpriteGo* spr = (SpriteGo*)FindGo("CSVEffectPreview");
		spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(ss.str()));
		spr->SetOrigin(Origins::TL);
		spr = (SpriteGo*)FindGo("CSVEffectEditPreview");
		spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(ss.str()));
		spr->SetOrigin(Origins::MC);
	}
		break;

	default:
		break;
	}
	
}

void SceneChampEdit::ResetFrameInfo()
{
	TextGo* tx = (TextGo*)FindGo("LeftBText");
	tx->text.setString("Left: " + std::to_string(frameInfo[curFrame].left));
	tx->SetOrigin(Origins::MC);
	tx = (TextGo*)FindGo("TopBText");
	tx->text.setString("Top: " + std::to_string(frameInfo[curFrame].top));
	tx->SetOrigin(Origins::MC);
	tx = (TextGo*)FindGo("WidthBText");
	tx->text.setString("Width: " + std::to_string(frameInfo[curFrame].width));
	tx->SetOrigin(Origins::MC);
	tx = (TextGo*)FindGo("HeightBText");
	tx->text.setString("Height: " + std::to_string(frameInfo[curFrame].height));
	tx->SetOrigin(Origins::MC);
}

void SceneChampEdit::ResetCsvEffectPreview()
{
	SpriteGo* spr = (SpriteGo*)FindGo("CSVEffectEditPreview");
	FrameInfo frame = frameInfo[curFrame];
	spr->sprite.setTextureRect({ frame.left,frame.top,frame.width,frame.height });
	spr->SetOrigin(Origins::MC);
	RectGo* rect = (RectGo*)FindGo("PreviewBound");
	rect->SetSize({ spr->sprite.getGlobalBounds().width,spr->sprite.getGlobalBounds().height });
	rect->SetOrigin(Origins::MC);
}

void SceneChampEdit::LoadCsv()
{
	switch (curChampAni)
	{
	case 1:
	{
		if (!std::filesystem::exists("animations/BindPlayerEffect/" + std::to_string(curChampCode) + ".csv"))
		{
			return;
		}
		rapidcsv::Document doc("animations/BindPlayerEffect/" + std::to_string(curChampCode) + ".csv", rapidcsv::LabelParams(-1, -1));

		if (doc.GetRowCount() < 1)
		{
			return;
		}
		fps = doc.GetCell<int>(1, 1);
		loopType = doc.GetCell<int>(2, 1);

		int count = 0;
		for (int i = 4; i < doc.GetRowCount(); ++i)
		{
			auto rows = doc.GetRow<std::string>(i);
			frameInfo[count] = { std::stoi(rows[1]), std::stoi(rows[2]),
				std::stoi(rows[3]), std::stoi(rows[4]) };
			count++;
		}
	}
	break;

	default:
		break;
	}
	
	return;
}

void SceneChampEdit::SaveCsv()
{
	switch (curChampAni)
	{
	case 1:
	{
		std::ofstream ofs("animations/BindPlayerEffect/" + std::to_string(curChampCode) + ".csv", std::ios::out | std::ios::trunc);
		
		if (!ofs) {
			std::cerr << "파일을 열 수 없습니다." << std::endl;
			return;
		}

		ofs << "ID,FPS,LOOPTYPE" << std::endl;
		ofs << "Alice," + std::to_string(fps) + "," + std::to_string(loopType) << std::endl;
		ofs << std::endl;
		ofs << "TEXTUREID,LEFT,TOP,WIDTH,HEIGHT" << std::endl;
		for (int i = 0; i < maxFrame; ++i)
		{
			ofs << "graphics/BindPlayerEffect/" << curChampCode << ".png" << ","
				<< frameInfo[i].left << ","
				<< frameInfo[i].top << ","
				<< frameInfo[i].width << ","
				<< frameInfo[i].height << ","
				<< std::endl;
		}
		
		ofs.close();
	}
	break;

	default:
		break;
	}

	return;
}

int SceneChampEdit::ReturnSkillCode(int champCode, bool isUlt)
{
	switch (champCode)
	{
	case 0:
		if (!isUlt)
		{
			return 1;
		}
		return 2;
		break;
	case 1:
		if (!isUlt)
		{
			return 4;
		}
		return 5;
		break;
	case 2:
		if (!isUlt)
		{
			return 8;
		}
		return 9;
		break;
	case 3:
		if (!isUlt)
		{
			return 12;
		}
		return 13;
		break;
	case 4:
		if (!isUlt)
		{
			return 14;
		}
		return 15;
		break;
	case 5:
		if (!isUlt)
		{
			return 16;
		}
		return 17;
		break;
	case 6:
		if (!isUlt)
		{
			return 18;
		}
		return 19;
		break;
	case 7:
		if (!isUlt)
		{
			return 25;
		}
		return 26;
		break;
	case 8:
		if (!isUlt)
		{
			return 20;
		}
		return 21;
		break;
	case 9:
		if (!isUlt)
		{
			return 3;
		}
		return 27;
		break;
	case 10:
		if (!isUlt)
		{
			return 23;
		}
		return 24;
		break;
	case 11:
		if (!isUlt)
		{
			return 6;
		}
		return 7;
		break;
	case 12:
		if (!isUlt)
		{
			return 10;
		}
		return 11;
		break;
	case 13:
		if (!isUlt)
		{
			return 28;
		}
		return 22;
		break;
	default:
		return 0;
		break;
	}
	return 0;
}
