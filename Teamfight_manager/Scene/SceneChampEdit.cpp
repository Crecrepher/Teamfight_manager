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

	worldView.setSize(windowSize * zoom);
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
	worldView.setSize(FRAMEWORK.GetWindowSize() * zoom);
	worldView.setCenter(FRAMEWORK.GetWindowSize() * 0.5f);

	std::string path = "graphics/CustomSprite";

	for (const auto& file : std::filesystem::directory_iterator(path))
		std::cout << file.path() << std::endl;

	frameInfo = std::vector<FrameInfo>(100);
	freeset = std::vector<FrameInfo>(4);

	curChampCode = 0;
	isCSVMakerOn = false;
	curChampAni = 0;
	isDragMode = false;

	EnterDefaultUi();
	EnterCSVMaker();
	ActivceDefaultUi();
	ActiveCSVMaker(false);
}

void SceneChampEdit::Exit()
{
	Scene::Exit();
}

void SceneChampEdit::Update(float dt)
{
	if (isCSVMakerOn && isDragMode)
	{
		isButtonInDrag = false;
	}

	Scene::Update(dt);
	animation.Update(dt);
	skillAni.Update(dt);

	champ.SetOrigin(Origins::MC);

	if (isCSVMakerOn)
	{
		UpdateCsvMaker(dt);
	}

	UpdateEffectPreviewAni();


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
	skillEffect = SpriteGo("", "SkillEffect");

	AddGo(&champ);
	AddGo(&skillEffect);

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
	AddGo(new TextGo("FrameText"));
}

void SceneChampEdit::InitCSVMaker()
{
	AddGo(new SpriteGo("", "CSVEffectPreview"));
	AddGo(new SpriteGo("", "CSVEffectEditPreview"));
	AddGo(new UiButton("", "CSVEffectEditPreviewBack"));

	AddGo(new RectGo("PreviewBound"));
	AddGo(new RectGo("PreviewBoundBack"));
	AddGo(new RectGo("EditCSVViewBound"));
	AddGo(new RectGo("EditCSVViewBoundBack"));

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
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "DragModeB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "DistXB"));
	AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "DistYB"));
	for (int i = 0; i < 4; i++)
	{
		AddGo(new UiButton("graphics/Origin/Sprite/weekly_event_button_3.png", "FreeSet" + std::to_string(i)));
	}


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
	AddGo(new TextGo("DragModeBText"));
	AddGo(new TextGo("DistXBText"));
	AddGo(new TextGo("DistYBText"));
	for (int i = 0; i < 4; i++)
	{
		AddGo(new TextGo("FreeSet" + std::to_string(i) + "T"));
	}
}

void SceneChampEdit::EnterDefaultUi()
{
	State* defaultChamp = CHAMPION_MGR.GetChampion(0);

	champ.SetPosition(FRAMEWORK.GetWindowSize() * 0.5f);
	champ.SetOrigin(Origins::MC);
	champ.SetSize(2, 2);
	champ.sortLayer = 100;

	animation = defaultChamp->animaition;
	animation.SetTarget(&champ.sprite);
	animation.Play("Idle");
	champ.SetOrigin(Origins::MC);

	skillEffect.SetPosition(FRAMEWORK.GetWindowSize() * 0.5f);
	skillEffect.SetOrigin(Origins::TL);
	skillEffect.SetSize(2, 2);
	skillEffect.sortLayer = 101;

	UiButton* bt;
	SpriteGo* spr;
	TextGo* text;
	RectGo* rect;

	rect = (RectGo*)FindGo("CharCenter");
	rect->SetSize({ 2.f,2.f });
	rect->rectangle.setFillColor(sf::Color::Red);
	rect->rectangle.setOutlineColor(sf::Color::Black);
	rect->rectangle.setOutlineThickness(1);
	rect->SetPosition(champ.GetPosition());
	rect->SetOrigin(Origins::MC);
	rect->sortLayer = 101;

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
		bt->SetPosition((FRAMEWORK.GetWindowSize().x * 0.8f) + (i * 60.f), 50.f);
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
				isClipWorks = false;
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
				if (!isCSVMakerOn)
				{
					LoadEffectPreview();
				}
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
				if (!isCSVMakerOn)
				{
					LoadEffectPreview();
				}
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
				if (!isCSVMakerOn)
				{
					LoadEffectPreview();
				}
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
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 100);
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
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 150);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnClick = [this]() {
		if (curChampAni == 0)
		{
			return;
		}
		ActivceDefaultUi(isCSVMakerOn);
		isCSVMakerOn = !isCSVMakerOn;
		ActiveCSVMaker(isCSVMakerOn);
		};

	text = (TextGo*)FindGo("CsvMakeBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("CSV Edit");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	text = (TextGo*)FindGo("FrameText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Frame : 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(champ.GetPosition().x, champ.GetPosition().y + 80);
	text->sortLayer = 101;
}

void SceneChampEdit::ActivceDefaultUi(bool on)
{
	State* defaultChamp = CHAMPION_MGR.GetChampion(0);

	if (on)
	{
		champ.sprite.setColor(sf::Color::White);
	}
	else
	{
		champ.sprite.setColor(sf::Color(255, 255, 255, 50));
	}

	skillEffect.SetActive(on);

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
		if (curChampAni == i)
		{
			continue;
		}
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
	if (on)
	{
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/weekly_event_button_0.png"));
	}
	else
	{
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/weekly_event_button_1.png"));
	}

}

void SceneChampEdit::LoadEffectPreview()
{

	std::string str = "animations/BindPlayerEffect/" + std::to_string(curChampCode) + std::to_string(curChampAni) + ".csv";
	if (!std::filesystem::exists(str))
	{
		isClipWorks = false;
		return;
	}
	isClipWorks = true;
	LoadCsv();
	std::stringstream ss;
	ss << "graphics/BindPlayerEffect/" << curChampCode << ".png";
	RESOURCE_MGR.Load(ResourceTypes::Texture, ss.str());
	RESOURCE_MGR.UnLoad(ResourceTypes::AnimationClip, str);
	RESOURCE_MGR.Load(ResourceTypes::AnimationClip, str);
	skillAni = AnimatioControler();
	skillAni.AddClip(*RESOURCE_MGR.GetAnimationClip(str));
	skillAni.SetTarget(&skillEffect.sprite);
	skillAni.Play("BP" + std::to_string(curChampAni));
}

void SceneChampEdit::UpdateEffectPreviewAni()
{
	TextGo* text = (TextGo*)FindGo("FrameText");
	text->text.setString("Frame: " + std::to_string(animation.GetCurrFrame()));
	if (!isClipWorks)
	{
		return;
	}
	FrameInfo curF = frameInfo[skillAni.GetCurrFrame()];
	skillEffect.SetPosition(FRAMEWORK.GetWindowSize() / 2.f + sf::Vector2f{ (float)curF.disX,(float)curF.disY });
}

void SceneChampEdit::EnterCSVMaker()
{
	UiButton* bt;
	SpriteGo* spr;
	TextGo* text;
	RectGo* rect;

	spr = (SpriteGo*)FindGo("CSVEffectPreview");
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f, 0);
	spr->SetOrigin(Origins::TC);
	spr->SetSize(1, 1);
	spr->sortLayer = 100;

	rect = (RectGo*)FindGo("PreviewBound");
	rect->rectangle.setFillColor(sf::Color(0, 0, 0, 0));
	rect->rectangle.setOutlineColor(sf::Color::Red);
	rect->rectangle.setOutlineThickness(1);
	rect->SetSize({ spr->sprite.getGlobalBounds().width,spr->sprite.getGlobalBounds().height });
	rect->SetPosition(spr->sprite.getGlobalBounds().left, spr->sprite.getGlobalBounds().top);
	rect->SetOrigin(Origins::TL);
	rect->sortLayer = 100;
	rect->sortOrder = -1;

	rect = (RectGo*)FindGo("PreviewBoundBack");
	rect->rectangle.setFillColor(sf::Color(0, 0, 0, 0));
	rect->rectangle.setOutlineColor(sf::Color::Green);
	rect->rectangle.setOutlineThickness(1);
	rect->SetSize({ spr->sprite.getGlobalBounds().width,spr->sprite.getGlobalBounds().height });
	rect->SetPosition(spr->sprite.getGlobalBounds().left, spr->sprite.getGlobalBounds().top);
	rect->SetOrigin(Origins::TL);
	rect->sortLayer = 100;
	rect->sortOrder = -1;

	bt = (UiButton*)FindGo("CSVEffectEditPreviewBack");
	bt->SetPosition(FRAMEWORK.GetWindowSize() * 0.5f);
	bt->sprite.setColor(sf::Color(255, 255, 255, 50));
	bt->SetOrigin(Origins::MC);
	bt->SetSize(1, 1);

	rect = (RectGo*)FindGo("EditCSVViewBoundBack");
	rect->rectangle.setFillColor(sf::Color(0, 0, 0, 0));
	rect->rectangle.setOutlineColor(sf::Color::Green);
	rect->rectangle.setOutlineThickness(1);
	rect->SetSize({ bt->sprite.getGlobalBounds().width,bt->sprite.getGlobalBounds().height });
	rect->SetPosition(bt->sprite.getGlobalBounds().left, bt->sprite.getGlobalBounds().top);
	rect->SetOrigin(Origins::TL);
	rect->sortOrder = -1;

	spr = (SpriteGo*)FindGo("CSVEffectEditPreview");
	spr->SetPosition(FRAMEWORK.GetWindowSize() * 0.5f);
	spr->SetOrigin(Origins::MC);
	spr->SetSize(1, 1);

	rect = (RectGo*)FindGo("EditCSVViewBound");
	rect->rectangle.setFillColor(sf::Color(0, 0, 0, 0));
	rect->rectangle.setOutlineColor(sf::Color::Red);
	rect->rectangle.setOutlineThickness(1);
	rect->SetSize({ spr->sprite.getGlobalBounds().width,spr->sprite.getGlobalBounds().height });
	rect->SetPosition(spr->GetPosition());
	rect->SetOrigin(Origins::TL);
	rect->sortOrder = -1;

	bt = (UiButton*)FindGo("MaxFrameB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 200);
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
			ResetFrameInfo();
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
			ResetFrameInfo();
		}
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("MaxFrameBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Max Frame: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("CurFrameB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 250);
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
			isDragMode = false;
			ResetCsvEffectPreview();
			LoadCSVDistanceCalculator();
			ResetFrameInfo();
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
			isDragMode = false;
			ResetCsvEffectPreview();
			LoadCSVDistanceCalculator();
			ResetFrameInfo();
		}
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("CurFrameBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Cur Frame: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("LeftB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 300);
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
			ResetFrameInfo();
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
			ResetFrameInfo();
			ResetCsvEffectPreview();
		}
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("LeftBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Left: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("TopB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 350);
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
			ResetFrameInfo();
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
			ResetFrameInfo();
			ResetCsvEffectPreview();
		}
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("TopBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Top: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("WidthB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 400);
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
			ResetFrameInfo();
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
			ResetFrameInfo();
			ResetCsvEffectPreview();
		}
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("WidthBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Width: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("HeightB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 450);
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
			ResetFrameInfo();
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
			ResetFrameInfo();
			ResetCsvEffectPreview();
		}
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("HeightBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Height: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("FpsB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 500);
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
			ResetFrameInfo();
		}
		else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				fps = std::max(fps - 5, 1);
			}
			else
			{
				fps = std::max(fps - 1, 1);
			}
			ResetFrameInfo();
		}
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("FpsBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("FPS: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("LoopTypeB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 550);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left) ||
			INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			loopType = (loopType + 1) % 2;
			ResetFrameInfo();
		}
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("LoopTypeBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("LoopType: 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("CSVLoadB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.1f, 50);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnClick = [this]() {
		LoadCsv();
		ResetFrameInfo();
		ResetCsvEffectPreview();
		};
	bt->OnStay = [this]() {
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("CSVLoadBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Load CSV");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("CSVSaveB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.1f, 100);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnClick = [this]() {
		SaveCsv();
		};
	bt->OnStay = [this]() {
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("CSVSaveBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Save CSV");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("DragModeB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.1f, 200);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnClick = [this]() {
		isDragMode = !isDragMode;
		ResetFrameInfo();
		};
	bt->OnStay = [this]() {
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("DragModeBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("DragMode : false");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("DistXB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.1f, 300);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].disX = frameInfo[curFrame].disX + 10;
			}
			else
			{
				frameInfo[curFrame].disX = frameInfo[curFrame].disX + 1;
			}
			ResetCsvEffectPreview();
			LoadCSVDistanceCalculator();
			ResetFrameInfo();
		}
		else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].disX = frameInfo[curFrame].disX - 10;
			}
			else
			{
				frameInfo[curFrame].disX = frameInfo[curFrame].disX - 1;
			}
			ResetCsvEffectPreview();
			LoadCSVDistanceCalculator();
			ResetFrameInfo();
		}

		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("DistXBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("DistX : 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	bt = (UiButton*)FindGo("DistYB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.1f, 350);
	bt->SetOrigin(Origins::MC);
	bt->SetSize(2, 2);
	bt->sortLayer = 100;
	bt->OnStay = [this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].disY = frameInfo[curFrame].disY + 10;
			}
			else
			{
				frameInfo[curFrame].disY = frameInfo[curFrame].disY + 1;
			}
			ResetCsvEffectPreview();
			LoadCSVDistanceCalculator();
			ResetFrameInfo();
		}
		else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
		{
			if (INPUT_MGR.GetKey(sf::Keyboard::LShift))
			{
				frameInfo[curFrame].disY = frameInfo[curFrame].disY - 10;
			}
			else
			{
				frameInfo[curFrame].disY = frameInfo[curFrame].disY - 1;
			}
			ResetCsvEffectPreview();
			LoadCSVDistanceCalculator();
			ResetFrameInfo();
		}
		isButtonInDrag = true;
		};

	text = (TextGo*)FindGo("DistYBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("DistY : 0");
	text->text.setCharacterSize(24);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 101;

	for (int i = 0; i < 4; i++)
	{
		bt = (UiButton*)FindGo("FreeSet" + std::to_string(i));
		bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.1f, 450 + (i * 50));
		bt->SetOrigin(Origins::MC);
		bt->SetSize(2, 2);
		bt->sortLayer = 100;
		bt->OnStay = [this, i]() {
			if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
			{
				frameInfo[curFrame] = freeset[i];
				ResetCsvEffectPreview();
				LoadCSVDistanceCalculator();
				ResetFrameInfo();

			}
			else if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Right))
			{
				freeset[i] = frameInfo[curFrame];
				ResetFrameInfo();
				ResetCsvEffectPreview();
				UiButton* bt = (UiButton*)FindGo("FreeSet" + std::to_string(i));
				bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/weekly_event_button_4.png"));
			}
			isButtonInDrag = true;
			};

		text = (TextGo*)FindGo("FreeSet" + std::to_string(i) + "T");
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("FreeSet " + std::to_string(i));
		text->text.setCharacterSize(24);
		text->SetOrigin(Origins::MC);
		text->SetPosition(bt->GetPosition());
		text->sortLayer = 101;
	}
}

void SceneChampEdit::ActiveCSVMaker(bool on)
{
	UiButton* bt;
	SpriteGo* spr;
	TextGo* text;
	RectGo* rect;

	spr = (SpriteGo*)FindGo("CSVEffectPreview");
	spr->SetActive(on);

	rect = (RectGo*)FindGo("PreviewBound");
	rect->SetActive(on);

	rect = (RectGo*)FindGo("PreviewBoundBack");
	rect->SetActive(on);

	bt = (UiButton*)FindGo("CSVEffectEditPreviewBack");
	bt->SetActive(on);

	rect = (RectGo*)FindGo("EditCSVViewBoundBack");
	rect->SetActive(on);

	spr = (SpriteGo*)FindGo("CSVEffectEditPreview");
	spr->SetActive(on);

	rect = (RectGo*)FindGo("EditCSVViewBound");
	rect->SetActive(on);

	bt = (UiButton*)FindGo("MaxFrameB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("MaxFrameBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("CurFrameB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("CurFrameBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("LeftB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("LeftBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("TopB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("TopBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("WidthB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("WidthBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("HeightB");
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.9f, 450);
	bt->SetActive(on);

	text = (TextGo*)FindGo("HeightBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("FpsB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("FpsBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("LoopTypeB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("LoopTypeBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("CSVLoadB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("CSVLoadBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("CSVSaveB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("CSVSaveBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("DragModeB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("DragModeBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("DistXB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("DistXBText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("DistYB");
	bt->SetActive(on);

	text = (TextGo*)FindGo("DistYBText");
	text->SetActive(on);

	for (int i = 0; i < 4; i++)
	{
		bt = (UiButton*)FindGo("FreeSet" + std::to_string(i));
		bt->SetActive(on);

		text = (TextGo*)FindGo("FreeSet" + std::to_string(i) + "T");
		text->SetActive(on);
	}

	worldView.setSize(FRAMEWORK.GetWindowSize() * zoom);
	worldView.setCenter(FRAMEWORK.GetWindowSize() * 0.5f);
	if (on)
	{
		LoadCsvMaker();
	}
}

void SceneChampEdit::LoadCsvMaker()
{
	maxFrame = 0;
	curFrame = 0;
	fps = 1;
	loopType = 0;
	frameInfo = std::vector<FrameInfo>(100);
	isDragMode = false;

	switch (aniPosType)
	{
	case 0:
	{
		std::stringstream ss;
		ss << "graphics/BindPlayerEffect/" << curChampCode << ".png";
		RESOURCE_MGR.Load(ResourceTypes::Texture, ss.str());
		SpriteGo* spr = (SpriteGo*)FindGo("CSVEffectPreview");
		spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(ss.str()));
		csvEffectPreviewSize = std::min((FRAMEWORK.GetWindowSize().y * 0.2) / spr->sprite.getTexture()->getSize().y, (FRAMEWORK.GetWindowSize().x * 0.2) / spr->sprite.getTexture()->getSize().x);
		spr->sprite.setTextureRect({ 0,0,(int)spr->sprite.getTexture()->getSize().x,(int)spr->sprite.getTexture()->getSize().y });
		spr->SetSize(csvEffectPreviewSize, csvEffectPreviewSize);
		spr->SetOrigin(Origins::TC);
		spr = (SpriteGo*)FindGo("CSVEffectEditPreview");
		spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(ss.str()));
		spr->sprite.setTextureRect({ 0,0,(int)spr->sprite.getTexture()->getSize().x,(int)spr->sprite.getTexture()->getSize().y });
		spr->SetOrigin(Origins::TL);
		UiButton* bt = (UiButton*)FindGo("CSVEffectEditPreviewBack");
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture(ss.str()));
		bt->sprite.setTextureRect({ 0,0,(int)spr->sprite.getTexture()->getSize().x,(int)spr->sprite.getTexture()->getSize().y });
		bt->SetOrigin(Origins::MC);
		spr->SetPosition(bt->sprite.getGlobalBounds().left, bt->sprite.getGlobalBounds().top);

		LoadCsv();
		ResetCsvEffectPreview();
		LoadCSVDistanceCalculator();
		ResetFrameInfo();
	}
	break;

	default:
		break;
	}

}

void SceneChampEdit::ResetFrameInfo()
{
	TextGo* tx = (TextGo*)FindGo("MaxFrameBText");
	tx->text.setString("Max Frame: " + std::to_string(maxFrame));
	tx->SetOrigin(Origins::MC);
	tx = (TextGo*)FindGo("CurFrameBText");
	tx->text.setString("Cur Frame: " + std::to_string(curFrame));
	tx->SetOrigin(Origins::MC);
	tx = (TextGo*)FindGo("LeftBText");
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
	tx = (TextGo*)FindGo("FpsBText");
	tx->text.setString("FPS: " + std::to_string(fps));
	tx->SetOrigin(Origins::MC);
	tx = (TextGo*)FindGo("LoopTypeBText");
	tx->text.setString("LoopType: " + std::to_string(loopType));
	tx->SetOrigin(Origins::MC);
	tx = (TextGo*)FindGo("DistXBText");
	tx->text.setString("DistX: " + std::to_string(frameInfo[curFrame].disX));
	tx->SetOrigin(Origins::MC);
	tx = (TextGo*)FindGo("DistYBText");
	tx->text.setString("DistY: " + std::to_string(frameInfo[curFrame].disY));
	tx->SetOrigin(Origins::MC);
	tx = (TextGo*)FindGo("DragModeBText");
	if (isDragMode)
	{
		tx->text.setString("DragMode : True");
	}
	else
	{
		tx->text.setString("DragMode : False");
	}
}

void SceneChampEdit::ResetCsvEffectPreview()
{
	FrameInfo frame = frameInfo[curFrame];
	UiButton* bt = (UiButton*)FindGo("CSVEffectEditPreviewBack");
	RectGo* rect = (RectGo*)FindGo("EditCSVViewBoundBack");
	rect->SetSize({ bt->sprite.getGlobalBounds().width,bt->sprite.getGlobalBounds().height });
	rect->SetPosition(bt->sprite.getGlobalBounds().left, bt->sprite.getGlobalBounds().top);
	rect->SetOrigin(Origins::TL);

	SpriteGo* spr = (SpriteGo*)FindGo("CSVEffectEditPreview");
	spr->sprite.setTextureRect({ frame.left,frame.top,frame.width,frame.height });
	spr->SetPosition(bt->sprite.getGlobalBounds().left + frame.left,
		bt->sprite.getGlobalBounds().top + frame.top);
	spr->SetOrigin(Origins::TL);
	rect = (RectGo*)FindGo("EditCSVViewBound");
	rect->SetSize({ spr->sprite.getGlobalBounds().width,spr->sprite.getGlobalBounds().height });
	rect->SetPosition(spr->GetPosition());
	rect->SetOrigin(Origins::TL);


	rect = (RectGo*)FindGo("PreviewBound");
	rect->SetSize({ spr->sprite.getGlobalBounds().width * csvEffectPreviewSize,spr->sprite.getGlobalBounds().height * csvEffectPreviewSize });
	spr = (SpriteGo*)FindGo("CSVEffectPreview");
	rect->SetPosition(spr->sprite.getGlobalBounds().left + (frame.left * csvEffectPreviewSize),
		spr->sprite.getGlobalBounds().top + (frame.top * csvEffectPreviewSize));
	rect->SetOrigin(Origins::TL);
	rect = (RectGo*)FindGo("PreviewBoundBack");
	rect->SetSize({ spr->sprite.getGlobalBounds().width,spr->sprite.getGlobalBounds().height });
	rect->SetPosition(spr->sprite.getGlobalBounds().left, spr->sprite.getGlobalBounds().top);


}

void SceneChampEdit::UpdateCsvMaker(float dt)
{

	if (isDragMode)
	{
		if (INPUT_MGR.GetMouseButton(sf::Mouse::Left) && !isButtonInDrag)
		{
			UiButton* bt = (UiButton*)FindGo("CSVEffectEditPreviewBack");
			if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
			{
				frameInfo[curFrame].left = std::max(0.f, this->ScreenToWorldPos(INPUT_MGR.GetMousePos()).x - bt->sprite.getGlobalBounds().left);
				frameInfo[curFrame].top = std::max(0.f, this->ScreenToWorldPos(INPUT_MGR.GetMousePos()).y - bt->sprite.getGlobalBounds().top);
			}
			frameInfo[curFrame].width = std::max(0.f, this->ScreenToWorldPos(INPUT_MGR.GetMousePos()).x - (bt->sprite.getGlobalBounds().left + frameInfo[curFrame].left));
			frameInfo[curFrame].height = std::max(0.f, this->ScreenToWorldPos(INPUT_MGR.GetMousePos()).y - (bt->sprite.getGlobalBounds().top + frameInfo[curFrame].top));
			ResetCsvEffectPreview();
			CSVDistanceCalculator();
			ResetFrameInfo();
		}
	}
	else
	{
		if (INPUT_MGR.GetMouseButton(sf::Mouse::Left))
		{
			if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
			{
				mouseMove = INPUT_MGR.GetMousePos() * zoom + worldView.getCenter();
			}
			worldView.setCenter(mouseMove - INPUT_MGR.GetMousePos() * zoom);
			CSVDistanceCalculator();
			ResetFrameInfo();
		}
	}
}

void SceneChampEdit::LoadCSVDistanceCalculator()
{
	SpriteGo* spr = (SpriteGo*)FindGo("CSVEffectEditPreview");
	worldView.setCenter(spr->GetPosition().x - frameInfo[curFrame].disX * zoom,
		spr->GetPosition().y - frameInfo[curFrame].disY * zoom);
}

void SceneChampEdit::CSVDistanceCalculator()
{
	SpriteGo* spr = (SpriteGo*)FindGo("CSVEffectEditPreview");
	sf::Vector2f sprPos = spr->sprite.getPosition();
	sf::Vector2f center = FRAMEWORK.GetWindowSize() / 2.f;
	frameInfo[curFrame].disX = (int)(this->worldPosToScreen(sprPos).x - center.x);
	frameInfo[curFrame].disY = (int)(this->worldPosToScreen(sprPos).y - center.y);
}

void SceneChampEdit::LoadCsv()
{
	switch (aniPosType)
	{
	case 0:
	{
		if (!std::filesystem::exists("animations/BindPlayerEffect/" + std::to_string(curChampCode) + std::to_string(curChampAni) + ".csv"))
		{
			return;
		}
		rapidcsv::Document doc("animations/BindPlayerEffect/" + std::to_string(curChampCode) + std::to_string(curChampAni) + ".csv", rapidcsv::LabelParams(-1, -1));

		if (doc.GetRowCount() < 1)
		{
			return;
		}
		fps = doc.GetCell<int>(1, 1);
		loopType = doc.GetCell<int>(2, 1);
		maxFrame = doc.GetRowCount() - 5;
		curFrame = 0;
		int count = 0;
		for (int i = 4; i < doc.GetRowCount(); ++i)
		{
			auto rows = doc.GetRow<std::string>(i);
			frameInfo[count] = { std::stoi(rows[1]), std::stoi(rows[2]),
				std::stoi(rows[3]), std::stoi(rows[4]),std::stoi(rows[5]),std::stoi(rows[6]) };
			frameInfo[count].disX /= zoom;
			frameInfo[count].disY /= zoom;
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
	switch (aniPosType)
	{
	case 0:
	{
		std::ofstream ofs("animations/BindPlayerEffect/" + std::to_string(curChampCode) + std::to_string(curChampAni) + ".csv", std::ios::out | std::ios::trunc);

		if (!ofs) {
			std::cerr << "파일을 열 수 없습니다." << std::endl;
			return;
		}

		ofs << "ID,FPS,LOOPTYPE" << std::endl;
		ofs << "BP" + std::to_string(curChampAni) + "," + std::to_string(fps) + "," + std::to_string(loopType) << std::endl;
		ofs << std::endl;
		ofs << "TEXTUREID,LEFT,TOP,WIDTH,HEIGHT,DisX,DisY" << std::endl;
		for (int i = 0; i < maxFrame + 1; ++i)
		{
			ofs << "graphics/BindPlayerEffect/" << curChampCode << ".png" << ","
				<< frameInfo[i].left << ","
				<< frameInfo[i].top << ","
				<< frameInfo[i].width << ","
				<< frameInfo[i].height << ","
				<< frameInfo[i].disX * zoom << ","
				<< frameInfo[i].disY * zoom << ","
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
