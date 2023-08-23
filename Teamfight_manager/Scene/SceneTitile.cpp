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
#include "SoundGo.h"

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


	//애니메이션 및 벡터 추가위치
	textSlot = std::vector<TextGo*>(titleTextCount);
	menuSlot = std::vector<UiButton*>(menuSlotCount);

	indicatorLeft = new SpriteGo("", "IndicatorLeft");
	AddGo(indicatorLeft);
	indicatorLeft->sortLayer = 105;
	indicatorLeft->SetSize(2, 2);
	indicatorLeftAnimation.SetTarget(&indicatorLeft->sprite);
	indicatorLeftAnimation.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/select_indicator_left.csv"));

	indicatorRight = new SpriteGo("", "IndicatorRight");
	AddGo(indicatorRight);
	indicatorRight->sortLayer = 105;
	indicatorRight->SetSize(2, 2);
	indicatorRightAnimation.SetTarget(&indicatorRight->sprite);
	indicatorRightAnimation.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/select_indicator_right.csv"));

	TitleUiInit();

	std::stringstream ss;
	ss << "내맘이야" << std::endl;
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

	// 애니메이션 한번만 호출되게
	indicatorLeftAnimation.Play("Idle");
	indicatorRightAnimation.Play("Idle");
	indicatorLeft->SetOrigin(Origins::MC);
	indicatorRight->SetOrigin(Origins::MC);
	indicatorLeft->SetActive(false);
	indicatorRight->SetActive(false);


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

void SceneTitile::TitleUiInit()
{
	// 나머지는 정규화해서 작성하기

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "Logo" << i;
		AddGo(new UiButton("graphics/UiFix/menuBindBox.png", ss.str()));
		ss << "Text";
		AddGo(new TextGo(ss.str()));
	}

	for (int i = 0; i < titleTextCount; i++)
	{
		std::stringstream ssMenuSlot;
		ssMenuSlot << "MenuSlot" << i + 1;
		menuSlot[i] = (UiButton*)AddGo(new UiButton("graphics/UiFix/menuBindBox.png", ssMenuSlot.str()));
		menuSlot[i]->sprite.setScale(0.5, 1);
		menuSlot[i]->SetOrigin(Origins::MC);
		menuSlot[i]->sortLayer = 105;
		menuSlot[i]->sortOrder = 1;
		menuSlot[i]->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f, 313 + (i * 60));
		menuSlot[i]->SetActive(true);

		std::stringstream ss;
		ss << "TextSlot" << i + 1;
		textSlot[i] = (TextGo*)AddGo(new TextGo(ss.str()));
		textSlot[i]->sortLayer = 105;
		textSlot[i]->sortOrder = 1;
		textSlot[i]->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri9.ttf"));
		textSlot[i]->text.setCharacterSize(30);
		textSlot[i]->text.setFillColor(sf::Color(128, 128, 128));
		auto stringTable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
		if (i == 0)
		{
			textSlot[i]->text.setString(stringTable->GetW("NewGame"));
		}
		else if (i == 1)
		{
			textSlot[i]->text.setString(stringTable->GetW("Continue"));
		}
		else if (i == 2)
		{
			textSlot[i]->text.setString(stringTable->GetW("EditorOpen"));
		}
		if(i == 3)
		{
			textSlot[i]->text.setString(stringTable->GetW("DoExit"));
		}
		textSlot[i]->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f, 300 + (i * 60));
		textSlot[i]->SetOrigin(Origins::MC);
		textSlot[i]->text.setOutlineColor(sf::Color::Black);
		textSlot[i]->text.setOutlineThickness(2);
		textSlot[i]->SetActive(true);

		sf::Vector2f firstPosition = textSlot[i]->GetPosition();
		sf::Vector2f tempPosition = firstPosition * 0.99f;
		int widthChangePosition = 88;

		menuSlot[i]->OnEnter = [this, i, tempPosition, widthChangePosition]() {
			indicatorLeft->SetActive(true);
			indicatorRight->SetActive(true);
			indicatorLeft->SetPosition(menuSlot[i]->GetPosition().x - widthChangePosition, menuSlot[i]->GetPosition().y);
			indicatorRight->SetPosition(menuSlot[i]->GetPosition().x + widthChangePosition, menuSlot[i]->GetPosition().y);

			textSlot[i]->SetPosition(tempPosition);
			textSlot[i]->text.setCharacterSize(35);
			textSlot[i]->text.setFillColor(sf::Color::White);
		};
		menuSlot[i]->OnExit = [this, i, firstPosition]() {
			indicatorLeft->SetActive(false);
			indicatorRight->SetActive(false);

			textSlot[i]->SetPosition(firstPosition);
			textSlot[i]->text.setCharacterSize(30);
			textSlot[i]->text.setFillColor(sf::Color(128, 128, 128));

			slotCheck = false;
		};
		menuSlot[i]->OnClick = [this, i]() {
			if (i == 0)
			{
				SCENE_MGR.ChangeScene(SceneId::Home);
			}
			else if (i == 1)
			{
				TEAM_MGR.SaveLoad(0);
				SCENE_MGR.ChangeScene(SceneId::Home);
			}
			else if (i == 2)
			{
				std::cout << "에디터 열기!" << std::endl;
				// 에디터 열기
			}
			if (i == 3)
			{
				std::cout << "종료처리" << std::endl;
				// 종료처리
			}
		};
	}

	AddGo(new SpriteGo("graphics/Origin/Sprite/logo_tp.png", "Logo"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/stadium.png", "Back"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/stadium_sky_bg.png", "BackSky"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/teamfight_manager_title_bg.png", "Monitor"));

}

void SceneTitile::Update(float dt)
{
	indicatorLeftAnimation.Update(dt);
	indicatorRightAnimation.Update(dt);
	Scene::Update(dt);

	if (INPUT_MGR.GetKeyUp(sf::Keyboard::Enter))
		
	{// 새게임
		SCENE_MGR.ChangeScene(SceneId::Home);
	}
	else if (INPUT_MGR.GetKeyUp(sf::Keyboard::Num1))
	{// 이어하기
		TEAM_MGR.SaveLoad(0);
		SCENE_MGR.ChangeScene(SceneId::Home);
	}

}

void SceneTitile::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
}