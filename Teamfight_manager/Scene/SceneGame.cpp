#include "stdafx.h"
#include "SceneGame.h"

#include "DataTableMgr.h"
#include "StringTable.h"
#include "SceneMgr.h"
#include "InputMgr.h"
#include "ResourceMgr.h"
#include "ChampionMgr.h"
#include "Framework.h"

#include "SpriteGo.h"
#include "TextGo.h"
#include "SoundGo.h"
#include "UiButton.h"


SceneGame::SceneGame() : Scene(SceneId::Game)
{
	resourceListPath = "tables/GameResourceList.csv";
}

SceneGame::~SceneGame()
{
	Release();
}

void SceneGame::Init()
{
	Release();

	// 카메라
	sf::Vector2f windowSize = FRAMEWORK.GetWindowSize();
	sf::Vector2f centerPos = windowSize * 0.5f;

	worldView.setSize(windowSize);
	worldView.setCenter(centerPos);
	uiView.setSize(windowSize);
	uiView.setCenter(centerPos);

	SpriteGo* bg = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Texture2D/stadium.png"));
	bg->sortLayer = -1;
	bg->sortOrder = 2;
	bg->SetPosition(centerPos);
	bg->SetOrigin(Origins::MC);
	bg->SetActive(false);

	UiInit();

	championPool.OnCreate = [this](Champion* champion){
		champion->ChangeStance(ChampionStance::None);
	};
	championPool.Init();

	// 경기장 x : 366 - 910
	// 경기장 y : 198 - 490

	for (auto go : gameObjects)
	{
		go->Init();
	}
}

void SceneGame::Release()
{
	for (auto go : gameObjects)
	{
		delete go;
	}
}

void SceneGame::Enter()
{
	Scene::Enter();
	RESOURCE_MGR.LoadFromCsv("tables/GameResourceList.csv");

	currentPhase = Phase::None;
}

void SceneGame::Exit()
{
	if (!redTeam.empty())
	{
		for (auto team : redTeam)
		{
			RemoveGo(team);
		}
		redTeam.clear();
	}
	if (!blueTeam.empty())
	{
		for (auto team : blueTeam)
		{
			RemoveGo(team);
		}
		blueTeam.clear();
	}
	ClearObjectPool(championPool);
	Scene::Exit();
}

void SceneGame::Update(float dt)
{
	Scene::Update(dt);

	switch (currentPhase)
	{
	case Phase::None:
	{
		switch (mode)
		{
		case Mode::Duo:
		{
			fullStep = 6;
			break;
		}
		case Mode::Trio:
		{
			fullStep = 8;
			break;
		}
		case Mode::Sqaud:
		{
			fullStep = 14;
			break;
		}
		}
		step = 1;
		team = Team::Blue;
		ChangePhase(Phase::League);
		break;
	}
	case Phase::League:
	{
		LeaguePhase(dt);
		break;
	}
	case Phase::Ban:
	{
		BanPhase(dt);
		break;
	}
	case Phase::Pick:
	{
		PickPhase(dt);
		break;
	}
	case Phase::Ready:
	{
		ReadyPhase(dt);
		break;
	}
	case Phase::Battle:
	{
		BattlePhase(dt);
		break;
	}
	}
	//sf::Vector2f mousePos = SCENE_MGR.GetCurrScene()->ScreenToWorldPos(INPUT_MGR.GetMousePos());
	//std::cout << "x : " << mousePos.x << std::endl;
	//std::cout << "y : " << mousePos.y << std::endl << std::endl;
	//스테이지로 돌아가기
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Escape))
	{
		SCENE_MGR.ChangeScene(SceneId::Home);
		return;
	}
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Num1))
	{
		mode = Mode::Duo;
		SCENE_MGR.ChangeScene(SceneId::Game);
		return;
	}
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Num2))
	{
		mode = Mode::Trio;
		SCENE_MGR.ChangeScene(SceneId::Game);
		return;
	}
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Num3))
	{
		mode = Mode::Sqaud;
		SCENE_MGR.ChangeScene(SceneId::Game);
		return;
	}
}

void SceneGame::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
}

void SceneGame::ChangePhase(Phase cPhase)
{
	switch (cPhase)
	{
	case Phase::None:
	{
		currentPhase = Phase::None;
		break;
	}
	case Phase::League:
	{
		currentPhase = Phase::League;
		break;
	}
	case Phase::Ban:
	{
		currentPhase = Phase::Ban;
		break;
	}
	case Phase::Pick:
	{
		currentPhase = Phase::Pick;
		break;
	}
	case Phase::Ready:
	{
		currentPhase = Phase::Ready;
		break;
	}
	case Phase::Battle:
	{
		currentPhase = Phase::Battle;
		battleTimer = 30.f;
		break;
	}
	}
}

void SceneGame::ChangeTurn()
{
	switch (currentTurn)
	{
	case Turn::Enemy:
	{
		currentTurn = Turn::Player;
		break;
	}
	case Turn::Player:
	{
		currentTurn = Turn::Enemy;
		break;
	}
	}
}

void SceneGame::ChangeTeam()
{
	switch (team)
	{
	case Team::Red:
	{
		team = Team::Blue;
		break;
	}
	case Team::Blue:
	{
		team = Team::Red;
		break;
	}
	}
}

void SceneGame::LeaguePhase(float dt)
{
	// 노랑 버튼 누르고 Ui 띄우고 플레이어 선택 마우스
	// 선발명단만 하고
	// 스왑페이즈를 만들어라 스왑페이즈에 전술까지 넣어야 하기 때문에
	// 
	// 마우스 좌표 테스트
	sf::Vector2f mousePos = INPUT_MGR.GetMousePos();
	sf::Vector2f uiMousePos = ScreenToUiPos(mousePos);
	std::cout << "마우스x: " << uiMousePos.x << "마우스y: "<< uiMousePos.y << std::endl;


	FindGo("Line Up")->SetActive(true);
	FindGo("Next Button")->SetActive(true);
	FindGo("keyboard Buttons_D")->SetActive(true);
	FindGo("keyboard Buttons_F")->SetActive(true);

	FindGo("Line Up Text1")->SetActive(true);
	FindGo("Line Up Text2")->SetActive(true);
	FindGo("Line Up Text3")->SetActive(true);
	FindGo("Line Up Text4")->SetActive(true);
	FindGo("Line Up Text5")->SetActive(true);
	FindGo("Enemy Team Name Text")->SetActive(true);

	FindGo("Player Member Text1")->SetActive(true);
	FindGo("Player Member Text2")->SetActive(true);
	FindGo("Player Member Text3")->SetActive(true);
	FindGo("Player Member Text4")->SetActive(true);
	FindGo("Player Member Text5")->SetActive(true);
	FindGo("Player Member Text6")->SetActive(true);

	FindGo("Enemy Member Text1")->SetActive(true);
	FindGo("Enemy Member Text2")->SetActive(true);
	FindGo("Enemy Member Text3")->SetActive(true);
	FindGo("Enemy Member Text4")->SetActive(true);


	// 턴과 팀이 바뀌고
	//ChangePhase(Phase::Ban);
}

void SceneGame::BanPhase(float dt)
{
	switch (mode)
	{
	case Mode::Duo:
	{
		ChangeTurn();
		ChangeTeam();

		if (step == 2)
		{
			ChangePhase(Phase::Pick);
			step++;
			return;
		}
		step++;
		break;
	}
	case Mode::Trio:
	{
		ChangeTurn();
		ChangeTeam();
		if (step == 2)
		{
			ChangePhase(Phase::Pick);
			step++;
			return;
		}
		step++;
		break;
	}
	case Mode::Sqaud:
	{
		ChangeTurn();
		ChangeTeam();
		if (step == 4)
		{
			ChangePhase(Phase::Pick);
			step++;
			return;
		}
		else if (step == 10)
		{
			ChangePhase(Phase::Pick);
			step++;
			return;
		}
		step++;
		break;
	}
	}
}

void SceneGame::PickPhase(float dt)
{


	switch (team)
	{



	case Team::Red:
	{

		Champion* redChamp = championPool.Get();
		redChamp->SetState(*CHAMPION_MGR.GetChampion("archer"));
		redChamp->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/archer_0.png"));
		redChamp->SetPosition((Utils::RandomRange(420,520)), (Utils::RandomRange(300, 450)));
		redChamp->SetEnemyTeam(&blueTeam);
		redChamp->SetMyTeam(&redTeam);
		redChamp->SetDieChampion(&cemetery);
		redChamp->ChangeStance(ChampionStance::None);
		redChamp->Reset();
		redChamp->SetSacleX(1);
		redChamp->SetTeamColor(Team::Red);
		redTeam.push_back(redChamp);
		AddGo(redChamp);
		ChangeTeam();
		ChangeTurn();
		break;
	}
	case Team::Blue:
	{
		Champion* blueChamp = championPool.Get();
		blueChamp->SetState(*CHAMPION_MGR.GetChampion("archer"));
		blueChamp->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/archer_0.png"));
		blueChamp->SetPosition((Utils::RandomRange(750, 850)), (Utils::RandomRange(300, 450)));
		blueChamp->SetEnemyTeam(&redTeam);
		blueChamp->SetMyTeam(&blueTeam);
		blueChamp->SetDieChampion(&cemetery);
		blueChamp->ChangeStance(ChampionStance::None);
		blueChamp->Reset();
		blueChamp->SetSacleX(-1);
		blueChamp->SetTeamColor(Team::Blue);
		blueTeam.push_back(blueChamp);
		AddGo(blueChamp);
		ChangeTeam();
		ChangeTurn();
		break;
	}
	}

	if (step == fullStep)
	{
		readyTimer = 1.f;

		ChangePhase(Phase::Ready);
	}
	else if (mode == Mode::Sqaud&&step==8)
	{
		step++;
		ChangePhase(Phase::Ban);
	}
	else
	{
		step++;
	}
}

void SceneGame::ReadyPhase(float dt)
{
	readyTimer -= dt;
	if (readyTimer <= 0.f)
	{
		for (auto unit : championPool.GetUseList())
		{
			unit->ChangeStance(ChampionStance::Idle);
		}
		ChangePhase(Phase::Battle);
	}
}

void SceneGame::BattlePhase(float dt)
{
	battleTimer -= dt;
	if (battleTimer <= 0)
	{
		// 선수들 킬 뎃 어시 정보
		// 챔피언 입힌 피해
		// 당한 피해
		// 힐량
		// 승 패

		if (!redTeam.empty())
		{
			for (auto team : redTeam)
			{
				RemoveGo(team);
				team->SetTeamColor(Team::None);
				championPool.Return(team);
			}
			redTeam.clear();
		}

		if (!blueTeam.empty())
		{
			for (auto team : blueTeam)
			{
				RemoveGo(team);
				team->SetTeamColor(Team::None);
				championPool.Return(team);
			}
			blueTeam.clear();
		}

		if (!cemetery.empty())
		{
			for (auto team : cemetery)
			{
				RemoveGo(team);
				team->SetTeamColor(Team::None);
				championPool.Return(team);
			}
			cemetery.clear();
		}
		ChangePhase(Phase::None);
	}
}

void SceneGame::UiInit()
{
	// 익주꺼 가져옴
	SpriteGo* header = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Sprite/header_bg#43707.png","Header"));
	header->sprite.setScale(2, 2);
	header->sortLayer = 101;
	header->sortOrder = 1;
	header->SetPosition(0, 0);
	header->SetOrigin(Origins::TL);
	header->SetActive(false);
	


	// 리그시스템 선발명단 - 텍스트
	TextGo* lineupText1 = (TextGo*)AddGo(new TextGo("Line Up Text1"));
	lineupText1->sortLayer = 102;
	lineupText1->sortOrder = 1;
	lineupText1->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText1->text.setCharacterSize(37);
	lineupText1->text.setFillColor(sf::Color::White);
	auto stringTable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	lineupText1->text.setString(stringTable->GetW("LineUp1"));
	lineupText1->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f, 118);
	lineupText1->SetOrigin(Origins::MC);
	lineupText1->SetActive(false);

	TextGo* lineupText2 = (TextGo*)AddGo(new TextGo("Line Up Text2"));
	lineupText2->sortLayer = 103;
	lineupText2->sortOrder = 1;
	lineupText2->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText2->text.setCharacterSize(23);
	lineupText2->text.setFillColor(sf::Color::White);
	lineupText2->text.setString(stringTable->GetW("LineUp2"));
	lineupText2->SetPosition(620, 601);
	lineupText2->SetOrigin(Origins::MC);
	lineupText2->SetActive(false);

	TextGo* lineupText3 = (TextGo*)AddGo(new TextGo("Line Up Text3"));
	lineupText3->sortLayer = 103;
	lineupText3->sortOrder = 1;
	lineupText3->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText3->text.setCharacterSize(17);
	lineupText3->text.setFillColor(sf::Color::White);
	lineupText3->text.setString(stringTable->GetW("LineUp3"));
	lineupText3->SetPosition(188, 595);
	lineupText3->SetOrigin(Origins::MC);
	lineupText3->SetActive(false);

	TextGo* lineupText4 = (TextGo*)AddGo(new TextGo("Line Up Text4"));
	lineupText4->sortLayer = 103;
	lineupText4->sortOrder = 1;
	lineupText4->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText4->text.setCharacterSize(17);
	lineupText4->text.setFillColor(sf::Color::White);
	lineupText4->text.setString(stringTable->GetW("LineUp4"));
	lineupText4->SetPosition(284, 198);
	lineupText4->SetOrigin(Origins::MC);
	lineupText4->SetActive(false);

	TextGo* lineupText5 = (TextGo*)AddGo(new TextGo("Line Up Text5"));
	lineupText5->sortLayer = 103;
	lineupText5->sortOrder = 1;
	lineupText5->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText5->text.setCharacterSize(17);
	lineupText5->text.setFillColor(sf::Color::White);
	lineupText5->text.setString(stringTable->GetW("LineUp5"));
	lineupText5->SetPosition(681, 198);
	lineupText5->SetOrigin(Origins::MC);
	lineupText5->SetActive(false);

	TextGo* lineupText6 = (TextGo*)AddGo(new TextGo("Enemy Team Name Text"));
	lineupText6->sortLayer = 103;
	lineupText6->sortOrder = 1;
	lineupText6->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText6->text.setCharacterSize(17);
	lineupText6->text.setFillColor(sf::Color::White);
	lineupText6->text.setString(L"Cheerful Cows");
	lineupText6->SetPosition(988, 198);
	lineupText6->SetOrigin(Origins::MC);
	lineupText6->SetActive(false);

	TextGo* lineupText7 = (TextGo*)AddGo(new TextGo("Player Member Text1"));
	lineupText7->sortLayer = 103;
	lineupText7->sortOrder = 1;
	lineupText7->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText7->text.setCharacterSize(17);
	lineupText7->text.setFillColor(sf::Color::White);
	lineupText7->text.setString(L"내 선수1");
	lineupText7->SetPosition(165, 270);
	lineupText7->SetOrigin(Origins::MC);
	lineupText7->SetActive(false);

	TextGo* lineupText8 = (TextGo*)AddGo(new TextGo("Player Member Text2"));
	lineupText8->sortLayer = 103;
	lineupText8->sortOrder = 1;
	lineupText8->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText8->text.setCharacterSize(17);
	lineupText8->text.setFillColor(sf::Color::White);
	lineupText8->text.setString(L"내 선수2");
	lineupText8->SetPosition(330, 270);
	lineupText8->SetOrigin(Origins::MC);
	lineupText8->SetActive(false);

	TextGo* lineupText9 = (TextGo*)AddGo(new TextGo("Player Member Text3"));
	lineupText9->sortLayer = 103;
	lineupText9->sortOrder = 1;
	lineupText9->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText9->text.setCharacterSize(17);
	lineupText9->text.setFillColor(sf::Color::White);
	lineupText9->text.setString(L"내 선수3");
	lineupText9->SetPosition(165, 416);
	lineupText9->SetOrigin(Origins::MC);
	lineupText9->SetActive(false);

	TextGo* lineupText10 = (TextGo*)AddGo(new TextGo("Player Member Text4"));
	lineupText10->sortLayer = 103;
	lineupText10->sortOrder = 1;
	lineupText10->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText10->text.setCharacterSize(17);
	lineupText10->text.setFillColor(sf::Color::White);
	lineupText10->text.setString(L"내 선수4");
	lineupText10->SetPosition(330, 416);
	lineupText10->SetOrigin(Origins::MC);
	lineupText10->SetActive(false);

	TextGo* lineupText11 = (TextGo*)AddGo(new TextGo("Player Member Text5"));
	lineupText11->sortLayer = 103;
	lineupText11->sortOrder = 1;
	lineupText11->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText11->text.setCharacterSize(17);
	lineupText11->text.setFillColor(sf::Color::White);
	lineupText11->text.setString(L"내 후보1");
	lineupText11->SetPosition(631, 270);
	lineupText11->SetOrigin(Origins::MC);
	lineupText11->SetActive(false);

	TextGo* lineupText12 = (TextGo*)AddGo(new TextGo("Player Member Text6"));
	lineupText12->sortLayer = 103;
	lineupText12->sortOrder = 1;
	lineupText12->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText12->text.setCharacterSize(17);
	lineupText12->text.setFillColor(sf::Color::White);
	lineupText12->text.setString(L"내 후보2");
	lineupText12->SetPosition(631, 416);
	lineupText12->SetOrigin(Origins::MC);
	lineupText12->SetActive(false);

	TextGo* lineupText13 = (TextGo*)AddGo(new TextGo("Enemy Member Text1"));
	lineupText13->sortLayer = 103;
	lineupText13->sortOrder = 1;
	lineupText13->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText13->text.setCharacterSize(17);
	lineupText13->text.setFillColor(sf::Color::White);
	lineupText13->text.setString(L"적 선수1");
	lineupText13->SetPosition(850, 270);
	lineupText13->SetOrigin(Origins::MC);
	lineupText13->SetActive(false);

	TextGo* lineupText14 = (TextGo*)AddGo(new TextGo("Enemy Member Text2"));
	lineupText14->sortLayer = 103;
	lineupText14->sortOrder = 1;
	lineupText14->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText14->text.setCharacterSize(17);
	lineupText14->text.setFillColor(sf::Color::White);
	lineupText14->text.setString(L"적 선수2");
	lineupText14->SetPosition(1045, 270);
	lineupText14->SetOrigin(Origins::MC);
	lineupText14->SetActive(false);

	TextGo* lineupText15 = (TextGo*)AddGo(new TextGo("Enemy Member Text3"));
	lineupText15->sortLayer = 103;
	lineupText15->sortOrder = 1;
	lineupText15->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText15->text.setCharacterSize(17);
	lineupText15->text.setFillColor(sf::Color::White);
	lineupText15->text.setString(L"적 선수3");
	lineupText15->SetPosition(850, 416);
	lineupText15->SetOrigin(Origins::MC);
	lineupText15->SetActive(false);

	TextGo* lineupText16 = (TextGo*)AddGo(new TextGo("Enemy Member Text4"));
	lineupText16->sortLayer = 103;
	lineupText16->sortOrder = 1;
	lineupText16->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	lineupText16->text.setCharacterSize(17);
	lineupText16->text.setFillColor(sf::Color::White);
	lineupText16->text.setString(L"적 선수4");
	lineupText16->SetPosition(1045, 416);
	lineupText16->SetOrigin(Origins::MC);
	lineupText16->SetActive(false);


	// 리그시스템 선발명단 - 라인업
	SpriteGo* pleyerTeamTitle1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_team_title_bg_0.png", "Pleyer Team Title1"));
	pleyerTeamTitle1->sprite.setScale(2, 2);
	pleyerTeamTitle1->sortLayer = 102;
	pleyerTeamTitle1->sortOrder = 1;
	pleyerTeamTitle1->SetPosition(290, 200);
	pleyerTeamTitle1->SetOrigin(Origins::MC);
	pleyerTeamTitle1->SetActive(true); // 왼쪽 테두리

	SpriteGo* enemyTeamTitle1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_team_title_bg_2.png", "Enemy Team Title1"));
	enemyTeamTitle1->sprite.setScale(2, 2);
	enemyTeamTitle1->sortLayer = 102;
	enemyTeamTitle1->sortOrder = 1;
	enemyTeamTitle1->SetPosition(990, 200);
	enemyTeamTitle1->SetOrigin(Origins::MC);
	enemyTeamTitle1->SetActive(true);

	// 리소스 있어서 추가는 했는데 안쓸듯
	SpriteGo* pleyerTeamTitle2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/Black_0.png", "Pleyer Team Title2"));
	pleyerTeamTitle2->sprite.setScale(2, 2);
	pleyerTeamTitle2->sortLayer = 102;
	pleyerTeamTitle2->sortOrder = 1;
	pleyerTeamTitle2->SetPosition(680, 200);
	pleyerTeamTitle2->SetOrigin(Origins::MC);
	pleyerTeamTitle2->SetActive(true); 
	
	SpriteGo* enemyTeamTitle2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_team_title_bg_3.png", "Enemy Team Title2"));
	enemyTeamTitle2->sprite.setScale(2, 2);
	enemyTeamTitle2->sortLayer = 102;
	enemyTeamTitle2->sortOrder = 1;
	enemyTeamTitle2->SetPosition(0, 0);
	enemyTeamTitle2->SetOrigin(Origins::MC);
	enemyTeamTitle2->SetActive(false);

	//       드래프트 슬롯            -------------------------

	SpriteGo* draftSlot1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Draft Slot"));
	draftSlot1->sprite.setScale(2, 2);
	draftSlot1->sortLayer = 102;
	draftSlot1->sortOrder = 1;
	draftSlot1->SetPosition(204, 319);
	draftSlot1->SetOrigin(Origins::MC);
	draftSlot1->SetActive(true);

	SpriteGo* draftSlot2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Draft Slot"));
	draftSlot2->sprite.setScale(2, 2);
	draftSlot2->sortLayer = 102;
	draftSlot2->sortOrder = 1;
	draftSlot2->SetPosition(100, 100);
	draftSlot2->SetOrigin(Origins::MC);
	draftSlot2->SetActive(true);

	SpriteGo* draftSlot3 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Draft Slot"));
	draftSlot3->sprite.setScale(2, 2);
	draftSlot3->sortLayer = 102;
	draftSlot3->sortOrder = 1;
	draftSlot3->SetPosition(204, 487);
	draftSlot3->SetOrigin(Origins::MC);
	draftSlot3->SetActive(true);

	SpriteGo* draftSlot4 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Draft Slot"));
	draftSlot4->sprite.setScale(2, 2);
	draftSlot4->sortLayer = 102;
	draftSlot4->sortOrder = 1;
	draftSlot4->SetPosition(100, 100);
	draftSlot4->SetOrigin(Origins::MC);
	draftSlot4->SetActive(true);

	SpriteGo* draftSlot5 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Draft Slot"));
	draftSlot5->sprite.setScale(2, 2);
	draftSlot5->sortLayer = 102;
	draftSlot5->sortOrder = 1;
	draftSlot5->SetPosition(100, 100);
	draftSlot5->SetOrigin(Origins::MC);
	draftSlot5->SetActive(true);

	SpriteGo* draftSlot6 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Draft Slot"));
	draftSlot6->sprite.setScale(2, 2);
	draftSlot6->sortLayer = 102;
	draftSlot6->sortOrder = 1;
	draftSlot6->SetPosition(100, 100);
	draftSlot6->SetOrigin(Origins::MC);
	draftSlot6->SetActive(true);

	SpriteGo* draftSlot7 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Draft Slot"));
	draftSlot7->sprite.setScale(2, 2);
	draftSlot7->sortLayer = 102;
	draftSlot7->sortOrder = 1;
	draftSlot7->SetPosition(100, 100);
	draftSlot7->SetOrigin(Origins::MC);
	draftSlot7->SetActive(true);

	SpriteGo* draftSlot8 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Draft Slot"));
	draftSlot8->sprite.setScale(2, 2);
	draftSlot8->sortLayer = 102;
	draftSlot8->sortOrder = 1;
	draftSlot8->SetPosition(100, 100);
	draftSlot8->SetOrigin(Origins::MC);
	draftSlot8->SetActive(true);

	SpriteGo* draftSlot9 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Draft Slot"));
	draftSlot9->sprite.setScale(2, 2);
	draftSlot9->sortLayer = 102;
	draftSlot9->sortOrder = 1;
	draftSlot9->SetPosition(100, 100);
	draftSlot9->SetOrigin(Origins::MC);
	draftSlot9->SetActive(true);

	SpriteGo* draftSlot10 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Draft Slot"));
	draftSlot10->sprite.setScale(2, 2);
	draftSlot10->sortLayer = 102;
	draftSlot10->sortOrder = 1;
	draftSlot10->SetPosition(100, 100);
	draftSlot10->SetOrigin(Origins::MC);
	draftSlot10->SetActive(true);








	SpriteGo* lineup = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_bg #43820.png", "Line Up"));
	lineup->sprite.setScale(2, 2);
	lineup->sortLayer = 101;
	lineup->sortOrder = 1;
	lineup->SetPosition(FRAMEWORK.GetWindowSize() / 2.f);
	lineup->SetOrigin(Origins::MC);
	lineup->SetActive(false);

	UiButton* nextButton = (UiButton*)AddGo(new UiButton("graphics/LeagueSystem/Lineup/default_button_0.png", "Next Button"));
	nextButton->sprite.setScale(2, 2);
	nextButton->SetOrigin(Origins::MC);
	nextButton->sortLayer = 102;
	nextButton->sortOrder = 1;
	nextButton->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f, 603);
	nextButton->SetActive(false);

	SpriteGo* keyboardButtons_D = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/keyboard_buttons_d.png", "keyboard Buttons_D"));
	keyboardButtons_D->sprite.setScale(2, 2);
	keyboardButtons_D->sortLayer = 102;
	keyboardButtons_D->sortOrder = 1;
	keyboardButtons_D->SetPosition(130, 600);
	keyboardButtons_D->SetOrigin(Origins::MC);
	keyboardButtons_D->SetActive(false);

	SpriteGo* keyboardButtons_F = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/keyboard_buttons_f.png", "keyboard Buttons_F"));
	keyboardButtons_F->sprite.setScale(2, 2);
	keyboardButtons_F->sortLayer = 102;
	keyboardButtons_F->sortOrder = 1;
	keyboardButtons_F->SetPosition(681, 603);
	keyboardButtons_F->SetOrigin(Origins::MC);
	keyboardButtons_F->SetActive(false);

	SpriteGo* swapLine = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/swap_line.png", "Swap Line"));
	swapLine->sprite.setScale(2, 2);
	swapLine->sortLayer = 101;
	swapLine->sortOrder = 1;
	swapLine->SetPosition(0, 0);
	swapLine->SetOrigin(Origins::TL);
	swapLine->SetActive(false);

	SpriteGo* playerCondition0 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/player_condition_0.png", "Player Condition0"));
	playerCondition0->sprite.setScale(2, 2);
	playerCondition0->sortLayer = 101;
	playerCondition0->sortOrder = 1;
	playerCondition0->SetPosition(0, 0);
	playerCondition0->SetOrigin(Origins::TL);
	playerCondition0->SetActive(false);

	SpriteGo* playerCondition1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/player_condition_1.png", "Player Condition1"));
	playerCondition1->sprite.setScale(2, 2);
	playerCondition1->sortLayer = 101;
	playerCondition1->sortOrder = 1;
	playerCondition1->SetPosition(0, 0);
	playerCondition1->SetOrigin(Origins::TL);
	playerCondition1->SetActive(false);

	SpriteGo* playerCondition2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/player_condition_2.png", "Player Condition2"));
	playerCondition2->sprite.setScale(2, 2);
	playerCondition2->sortLayer = 101;
	playerCondition2->sortOrder = 1;
	playerCondition2->SetPosition(0, 0);
	playerCondition2->SetOrigin(Origins::TL);
	playerCondition2->SetActive(false);

	SpriteGo* playerCondition3 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/player_condition_3.png", "Player Condition3"));
	playerCondition3->sprite.setScale(2, 2);
	playerCondition3->sortLayer = 101;
	playerCondition3->sortOrder = 1;
	playerCondition3->SetPosition(0, 0);
	playerCondition3->SetOrigin(Origins::TL);
	playerCondition3->SetActive(false);

	SpriteGo* playerCondition4 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/player_condition_4.png", "Player Condition4"));
	playerCondition4->sprite.setScale(2, 2);
	playerCondition4->sortLayer = 101;
	playerCondition4->sortOrder = 1;
	playerCondition4->SetPosition(0, 0);
	playerCondition4->SetOrigin(Origins::TL);
	playerCondition4->SetActive(false);

	SpriteGo* playerCondition5 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/player_condition_5.png", "Player Condition5"));
	playerCondition5->sprite.setScale(2, 2);
	playerCondition5->sortLayer = 101;
	playerCondition5->sortOrder = 1;
	playerCondition5->SetPosition(0, 0);
	playerCondition5->SetOrigin(Origins::TL);
	playerCondition5->SetActive(false);

	SpriteGo* playerCondition6 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/player_condition_6.png", "Player Condition6"));
	playerCondition6->sprite.setScale(2, 2);
	playerCondition6->sortLayer = 101;
	playerCondition6->sortOrder = 1;
	playerCondition6->SetPosition(0, 0);
	playerCondition6->SetOrigin(Origins::TL);
	playerCondition6->SetActive(false);

	SpriteGo* playerCondition7 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/player_condition_7.png", "Player Condition7"));
	playerCondition7->sprite.setScale(2, 2);
	playerCondition7->sortLayer = 101;
	playerCondition7->sortOrder = 1;
	playerCondition7->SetPosition(0, 0);
	playerCondition7->SetOrigin(Origins::TL);
	playerCondition7->SetActive(false);

	SpriteGo* playerCondition8 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/player_condition_8.png", "Player Condition8"));
	playerCondition8->sprite.setScale(2, 2);
	playerCondition8->sortLayer = 101;
	playerCondition8->sortOrder = 1;
	playerCondition8->SetPosition(0, 0);
	playerCondition8->SetOrigin(Origins::TL);
	playerCondition8->SetActive(false);

	SpriteGo* playerCondition9 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/player_condition_9.png", "Player Condition9"));
	playerCondition9->sprite.setScale(2, 2);
	playerCondition9->sortLayer = 101;
	playerCondition9->sortOrder = 1;
	playerCondition9->SetPosition(0, 0);
	playerCondition9->SetOrigin(Origins::TL);
	playerCondition9->SetActive(false);

	// 리그시스템 선발명단 직후 라인업넥스트
	SpriteGo* endingStadiumbg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/LineupNext/ending_stadium_bg.png", "Ending Stadium Bg"));
	endingStadiumbg->sprite.setScale(2, 2);
	endingStadiumbg->sortLayer = 101;
	endingStadiumbg->sortOrder = 1;
	endingStadiumbg->SetPosition(0, 0);
	endingStadiumbg->SetOrigin(Origins::TL);
	endingStadiumbg->SetActive(false);

	SpriteGo* framebg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/LineupNext/frame_bg.png", "Frame Bg"));
	framebg->sprite.setScale(2, 2);
	framebg->sortLayer = 101;
	framebg->sortOrder = 1;
	framebg->SetPosition(0, 0);
	framebg->SetOrigin(Origins::TL);
	framebg->SetActive(false);

	SpriteGo* playerSlotBlue = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/LineupNext/player_slot_0.png", "Player Slot Blue"));
	playerSlotBlue->sprite.setScale(2, 2);
	playerSlotBlue->sortLayer = 101;
	playerSlotBlue->sortOrder = 1;
	playerSlotBlue->SetPosition(0, 0);
	playerSlotBlue->SetOrigin(Origins::TL);
	playerSlotBlue->SetActive(false);

	SpriteGo* playerSlotRed = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/LineupNext/player_slot_1.png", "Player Slot Red"));
	playerSlotRed->sprite.setScale(2, 2);
	playerSlotRed->sortLayer = 101;
	playerSlotRed->sortOrder = 1;
	playerSlotRed->SetPosition(0, 0);
	playerSlotRed->SetOrigin(Origins::TL);
	playerSlotRed->SetActive(false);

	SpriteGo* stadiumbg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/LineupNext/stadium_bg.png", "Stadium Bg"));
	stadiumbg->sprite.setScale(2, 2);
	stadiumbg->sortLayer = 101;
	stadiumbg->sortOrder = 1;
	stadiumbg->SetPosition(0, 0);
	stadiumbg->SetOrigin(Origins::TL);
	stadiumbg->SetActive(false);

	SpriteGo* stadiumChair = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/LineupNext/stadium_chair.png", "Stadium Chair"));
	stadiumChair->sprite.setScale(2, 2);
	stadiumChair->sortLayer = 101;
	stadiumChair->sortOrder = 1;
	stadiumChair->SetPosition(0, 0);
	stadiumChair->SetOrigin(Origins::TL);
	stadiumChair->SetActive(false);

	SpriteGo* stadiumChat = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/LineupNext/stadium_chat_bubble.png", "Stadium Chat"));
	stadiumChat->sprite.setScale(2, 2);
	stadiumChat->sortLayer = 101;
	stadiumChat->sortOrder = 1;
	stadiumChat->SetPosition(0, 0);
	stadiumChat->SetOrigin(Origins::TL);
	stadiumChat->SetActive(false);

	SpriteGo* stadiumDesk = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/LineupNext/stadium_desk.png", "Stadium Desk"));
	stadiumDesk->sprite.setScale(2, 2);
	stadiumDesk->sortLayer = 101;
	stadiumDesk->sortOrder = 1;
	stadiumDesk->SetPosition(0, 0);
	stadiumDesk->SetOrigin(Origins::TL);
	stadiumDesk->SetActive(false);

	SpriteGo* stadiumFrame = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/LineupNext/stadium_frame.png", "Stadium Frame"));
	stadiumFrame->sprite.setScale(2, 2);
	stadiumFrame->sortLayer = 101;
	stadiumFrame->sortOrder = 1;
	stadiumFrame->SetPosition(0, 0);
	stadiumFrame->SetOrigin(Origins::TL);
	stadiumFrame->SetActive(false);

	SpriteGo* stadiumFrameFront = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/LineupNext/stadium_frame_front.png", "Stadium Frame Front"));
	stadiumFrameFront->sprite.setScale(2, 2);
	stadiumFrameFront->sortLayer = 101;
	stadiumFrameFront->sortOrder = 1;
	stadiumFrameFront->SetPosition(0, 0);
	stadiumFrameFront->SetOrigin(Origins::TL);
	stadiumFrameFront->SetActive(false);

	// 리그시스템 밴픽
	SpriteGo* banEffectAnimation0 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_effect_animation_0.png", "Ban Effect Animation0"));
	banEffectAnimation0->sprite.setScale(2, 2);
	banEffectAnimation0->sortLayer = 101;
	banEffectAnimation0->sortOrder = 1;
	banEffectAnimation0->SetPosition(0, 0);
	banEffectAnimation0->SetOrigin(Origins::TL);
	banEffectAnimation0->SetActive(false);

	SpriteGo* banEffectAnimation1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_effect_animation_1.png", "Ban Effect Animation1"));
	banEffectAnimation1->sprite.setScale(2, 2);
	banEffectAnimation1->sortLayer = 101;
	banEffectAnimation1->sortOrder = 1;
	banEffectAnimation1->SetPosition(0, 0);
	banEffectAnimation1->SetOrigin(Origins::TL);
	banEffectAnimation1->SetActive(false);

	SpriteGo* banEffectAnimation2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_effect_animation_2.png", "Ban Effect Animation2"));
	banEffectAnimation2->sprite.setScale(2, 2);
	banEffectAnimation2->sortLayer = 101;
	banEffectAnimation2->sortOrder = 1;
	banEffectAnimation2->SetPosition(0, 0);
	banEffectAnimation2->SetOrigin(Origins::TL);
	banEffectAnimation2->SetActive(false);

	SpriteGo* banEffectAnimation3 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_effect_animation_3.png", "Ban Effect Animation3"));
	banEffectAnimation3->sprite.setScale(2, 2);
	banEffectAnimation3->sortLayer = 101;
	banEffectAnimation3->sortOrder = 1;
	banEffectAnimation3->SetPosition(0, 0);
	banEffectAnimation3->SetOrigin(Origins::TL);
	banEffectAnimation3->SetActive(false);

	SpriteGo* banEffectAnimation4 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_effect_animation_4.png", "Ban Effect Animation4"));
	banEffectAnimation4->sprite.setScale(2, 2);
	banEffectAnimation4->sortLayer = 101;
	banEffectAnimation4->sortOrder = 1;
	banEffectAnimation4->SetPosition(0, 0);
	banEffectAnimation4->SetOrigin(Origins::TL);
	banEffectAnimation4->SetActive(false);

	SpriteGo* banFeedbackIcon = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_feedback_icon.png", "Ban Feedback Icon"));
	banFeedbackIcon->sprite.setScale(2, 2);
	banFeedbackIcon->sortLayer = 101;
	banFeedbackIcon->sortOrder = 1;
	banFeedbackIcon->SetPosition(0, 0);
	banFeedbackIcon->SetOrigin(Origins::TL);
	banFeedbackIcon->SetActive(false);

	SpriteGo* banIcon = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_icon.png", "Ban Icon"));
	banIcon->sprite.setScale(2, 2);
	banIcon->sortLayer = 101;
	banIcon->sortOrder = 1;
	banIcon->SetPosition(0, 0);
	banIcon->SetOrigin(Origins::TL);
	banIcon->SetActive(false);

	SpriteGo* banIconLarge = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_icon_large.png", "Ban Icon Large"));
	banIconLarge->sprite.setScale(2, 2);
	banIconLarge->sortLayer = 101;
	banIconLarge->sortOrder = 1;
	banIconLarge->SetPosition(0, 0);
	banIconLarge->SetOrigin(Origins::TL);
	banIconLarge->SetActive(false);

	SpriteGo* banIconSmall = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_icon_small.png", "Ban Icon Small"));
	banIconSmall->sprite.setScale(2, 2);
	banIconSmall->sortLayer = 101;
	banIconSmall->sortOrder = 1;
	banIconSmall->SetPosition(0, 0);
	banIconSmall->SetOrigin(Origins::TL);
	banIconSmall->SetActive(false);

	SpriteGo* banSlotbg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_slot_bg_0.png", "Ban Slot Bg"));
	banSlotbg->sprite.setScale(2, 2);
	banSlotbg->sortLayer = 101;
	banSlotbg->sortOrder = 1;
	banSlotbg->SetPosition(0, 0);
	banSlotbg->SetOrigin(Origins::TL);
	banSlotbg->SetActive(false);

	SpriteGo* banSlotBluebg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_slot_bg_1.png", "Ban Slot Blue Bg"));
	banSlotBluebg->sprite.setScale(2, 2);
	banSlotBluebg->sortLayer = 101;
	banSlotBluebg->sortOrder = 1;
	banSlotBluebg->SetPosition(0, 0);
	banSlotBluebg->SetOrigin(Origins::TL);
	banSlotBluebg->SetActive(false);

	SpriteGo* banSlotRedbg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/ban_slot_bg_2.png", "Ban Slot Red Bg"));
	banSlotRedbg->sprite.setScale(2, 2);
	banSlotRedbg->sortLayer = 101;
	banSlotRedbg->sortOrder = 1;
	banSlotRedbg->SetPosition(0, 0);
	banSlotRedbg->SetOrigin(Origins::TL);
	banSlotRedbg->SetActive(false);

	SpriteGo* bannedSlotbg0 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/banned_slot_bg.png", "Banned Slot Bg0"));
	bannedSlotbg0->sprite.setScale(2, 2);
	bannedSlotbg0->sortLayer = 101;
	bannedSlotbg0->sortOrder = 1;
	bannedSlotbg0->SetPosition(0, 0);
	bannedSlotbg0->SetOrigin(Origins::TL);
	bannedSlotbg0->SetActive(false);

	SpriteGo* bannedSlotbg1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/banned_slot_bg_0.png", "Banned Slot Bg1"));
	bannedSlotbg1->sprite.setScale(2, 2);
	bannedSlotbg1->sortLayer = 101;
	bannedSlotbg1->sortOrder = 1;
	bannedSlotbg1->SetPosition(0, 0);
	bannedSlotbg1->SetOrigin(Origins::TL);
	bannedSlotbg1->SetActive(false);

	SpriteGo* bannedSlotFront = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/banned_slot_front.png", "Banned Slot Front"));
	bannedSlotFront->sprite.setScale(2, 2);
	bannedSlotFront->sortLayer = 101;
	bannedSlotFront->sortOrder = 1;
	bannedSlotFront->SetPosition(0, 0);
	bannedSlotFront->SetOrigin(Origins::TL);
	bannedSlotFront->SetActive(false);

	SpriteGo* banpickMask = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/banpick_mask.png", ""));
	banpickMask->sprite.setScale(2, 2);
	banpickMask->sortLayer = 101;
	banpickMask->sortOrder = 1;
	banpickMask->SetPosition(0, 0);
	banpickMask->SetOrigin(Origins::TL);
	banpickMask->SetActive(false);

	SpriteGo* banpickStatbg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/banpick_stat_bg.png", ""));
	banpickStatbg->sprite.setScale(2, 2);
	banpickStatbg->sortLayer = 101;
	banpickStatbg->sortOrder = 1;
	banpickStatbg->SetPosition(0, 0);
	banpickStatbg->SetOrigin(Origins::TL);
	banpickStatbg->SetActive(false);

	SpriteGo* banpickUibg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/banpick_ui_bg.png", ""));
	banpickUibg->sprite.setScale(2, 2);
	banpickUibg->sortLayer = 101;
	banpickUibg->sortOrder = 1;
	banpickUibg->SetPosition(0, 0);
	banpickUibg->SetOrigin(Origins::TL);
	banpickUibg->SetActive(false);

	SpriteGo* globalBanIcon = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/global_ban_icon.png", ""));
	globalBanIcon->sprite.setScale(2, 2);
	globalBanIcon->sortLayer = 101;
	globalBanIcon->sortOrder = 1;
	globalBanIcon->SetPosition(0, 0);
	globalBanIcon->SetOrigin(Origins::TL);
	globalBanIcon->SetActive(false);

	SpriteGo* middleBanBlueLine = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/middle_ban_blue_line.png", ""));
	middleBanBlueLine->sprite.setScale(2, 2);
	middleBanBlueLine->sortLayer = 101;
	middleBanBlueLine->sortOrder = 1;
	middleBanBlueLine->SetPosition(0, 0);
	middleBanBlueLine->SetOrigin(Origins::TL);
	middleBanBlueLine->SetActive(false);

	SpriteGo* middleBanRedLine = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/middle_ban_red_line.png", ""));
	middleBanRedLine->sprite.setScale(2, 2);
	middleBanRedLine->sortLayer = 101;
	middleBanRedLine->sortOrder = 1;
	middleBanRedLine->SetPosition(0, 0);
	middleBanRedLine->SetOrigin(Origins::TL);
	middleBanRedLine->SetActive(false);

	// 리그 시스템 스왑 / swap_line겸칩 / 위치 라인업
	SpriteGo* swapArrowIcon = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Swap/swap_arrow_icon.png", ""));
	swapArrowIcon->sprite.setScale(2, 2);
	swapArrowIcon->sortLayer = 101;
	swapArrowIcon->sortOrder = 1;
	swapArrowIcon->SetPosition(0, 0);
	swapArrowIcon->SetOrigin(Origins::TL);
	swapArrowIcon->SetActive(false);

	SpriteGo* swapChampCardbg0 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Swap/swap_champ_card_bg_0.png", ""));
	swapChampCardbg0->sprite.setScale(2, 2);
	swapChampCardbg0->sortLayer = 101;
	swapChampCardbg0->sortOrder = 1;
	swapChampCardbg0->SetPosition(0, 0);
	swapChampCardbg0->SetOrigin(Origins::TL);
	swapChampCardbg0->SetActive(false);

	SpriteGo* swapChampCardbg1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Swap/swap_champ_card_bg_1.png", ""));
	swapChampCardbg1->sprite.setScale(2, 2);
	swapChampCardbg1->sortLayer = 101;
	swapChampCardbg1->sortOrder = 1;
	swapChampCardbg1->SetPosition(0, 0);
	swapChampCardbg1->SetOrigin(Origins::TL);
	swapChampCardbg1->SetActive(false);

	SpriteGo* swapChampCardBluebg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Swap/swap_champ_card_bg_2.png", ""));
	swapChampCardBluebg->sprite.setScale(2, 2);
	swapChampCardBluebg->sortLayer = 101;
	swapChampCardBluebg->sortOrder = 1;
	swapChampCardBluebg->SetPosition(0, 0);
	swapChampCardBluebg->SetOrigin(Origins::TL);
	swapChampCardBluebg->SetActive(false);

	SpriteGo* swapChampCardRedbg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Swap/swap_champ_card_bg_4.png", ""));
	swapChampCardRedbg->sprite.setScale(2, 2);
	swapChampCardRedbg->sortLayer = 101;
	swapChampCardRedbg->sortOrder = 1;
	swapChampCardRedbg->SetPosition(0, 0);
	swapChampCardRedbg->SetOrigin(Origins::TL);
	swapChampCardRedbg->SetActive(false);

	SpriteGo* swapUibg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Swap/swap_ui_bg.png", ""));
	swapUibg->sprite.setScale(2, 2);
	swapUibg->sortLayer = 101;
	swapUibg->sortOrder = 1;
	swapUibg->SetPosition(0, 0);
	swapUibg->SetOrigin(Origins::TL);
	swapUibg->SetActive(false);


}
