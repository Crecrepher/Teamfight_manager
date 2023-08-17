#include "stdafx.h"
#include "SceneGame.h"

#include "DataTableMgr.h"
#include "StringTable.h"
#include "SceneMgr.h"
#include "InputMgr.h"
#include "ResourceMgr.h"
#include "ChampionMgr.h"
#include "SkillMgr.h"
#include "Framework.h"

#include "RectGo.h"
#include "SpriteGo.h"
#include "TextGo.h"
#include "SoundGo.h"
#include "UiButton.h"
#include "TeamMgr.h"

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

	worldView.setSize(windowSize*0.7f);
	worldView.setCenter(centerPos.x,centerPos.y-50.f);
	uiView.setSize(windowSize);
	uiView.setCenter(centerPos);

	SpriteGo* bg = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Texture2D/stadium.png", "1"));
	bg->sortLayer = -1;
	bg->sortOrder = 2;
	bg->SetPosition(centerPos);
	bg->SetOrigin(Origins::MC);
	bg->SetActive(false);


	//벡터 임시위치
	championSlot = std::vector<UiButton*>(champCount);

	UiInit();
	ButtonInit();

	//애니메이션 추가 임시위치
	banSheet = new SpriteGo("", "BanSheet");
	AddGo(banSheet);
	banSheet->sortLayer = 102;
	banAnimation.SetTarget(&banSheet->sprite);
	banAnimation.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/banSlotBlue.csv"));


	RectGo* field = (RectGo*)AddGo(new RectGo("field"));
	field->rectangle.setSize({ 544, 308 });
	field->SetPosition({ 361,191 });
	field->sortLayer = -2;

	SpriteGo* header = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Sprite/header_bg#43707.png"));
	header->sprite.setScale(2, 2);
	header->sortLayer = 101;
	header->sortOrder = 1;
	header->SetPosition(0, 0);
	header->SetOrigin(Origins::TL);

	banSheetBlueTeam = new SpriteGo("", "BanSheet_BlueTeam");
	AddGo(banSheetBlueTeam);
	banSheetBlueTeam->sortLayer = 102;
	banAnimation2.SetTarget(&banSheetBlueTeam->sprite);
	banAnimation2.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/banSlotBlueTeam.csv"));

	banSheetRedTeam = new SpriteGo("", "BanSheet_RedTeam");
	AddGo(banSheetRedTeam);
	banSheetRedTeam->sortLayer = 102;
	banAnimation3.SetTarget(&banSheetRedTeam->sprite);
	banAnimation3.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/banSlotRedTeam.csv"));

	banSheet->SetActive(false);
	banSheetBlueTeam->SetActive(false);
	banSheetRedTeam->SetActive(false);

	//FindGo("BanSheet")->SetActive(false);
	//FindGo("BanSheet_BlueTeam")->SetActive(false);
	//FindGo("BanSheet_RedTeam")->SetActive(false);

	championPool.OnCreate = [this, field](Champion* champion) {
		champion->ChangeStance(ChampionStance::None);
		champion->SetField(field);
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
	banAnimation.Play("Idle"); // 무조건 한번만 호출되게
	banSheet->SetOrigin(Origins::MC);
	currentPhase = Phase::None;
	banChamps = std::vector<int>(6,-1);

	redScore = 0;
	blueScore = 0;

	SpriteGo* spr = (SpriteGo*)FindGo("1");
	spr->SetActive(false);

	for (int i = 0; i < champCount; i++)
	{
		std::stringstream ss;
		ss << "Champion Slot" << i + 1;
		UiButton* bt = (UiButton*)FindGo(ss.str());
		bt->sprite.setColor(sf::Color::White);
		ss << "Image";
		bt = (UiButton*)FindGo(ss.str());
		bt->sprite.setColor(sf::Color::White);
	}
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
	if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
	{
		std::cout << INPUT_MGR.GetMousePos().x << "\t"
			<< INPUT_MGR.GetMousePos().y << std::endl;
	}
	Scene::Update(dt);
	banAnimation.Update(dt);

	selectCheck = true;
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
		step = 0;
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
	case Phase::Result:
	{
		ResultPhase(dt);
		break;
	}
	}
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
		BanPickFalse();
		currentPhase = Phase::Battle;
		battleTimer = 60.f;
		break;
	}
	case Phase::Result:
	{
		currentPhase = Phase::Result;
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
	/*sf::Vector2f mousePos = INPUT_MGR.GetMousePos();
	sf::Vector2f uiMousePos = ScreenToUiPos(mousePos);
	std::cout << "마우스x: " << uiMousePos.x << "마우스y: " << uiMousePos.y << std::endl;*/

	LineUpTrue();

	UiButton* ui = (UiButton*)FindGo("Next Button");

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

	FindGo("Pleyer Team Title1")->SetActive(true);
	FindGo("Enemy Team Title1")->SetActive(true);

	FindGo("Player Draft Slot1")->SetActive(true);
	FindGo("Player Draft Slot2")->SetActive(true);
	FindGo("Player Draft Slot3")->SetActive(true);
	FindGo("Player Draft Slot4")->SetActive(true);
	FindGo("Player Draft Slot5")->SetActive(true);
	FindGo("Player Draft Slot6")->SetActive(true);

	FindGo("Enemy Draft Slot1")->SetActive(true);
	FindGo("Enemy Draft Slot2")->SetActive(true);
	FindGo("Enemy Draft Slot3")->SetActive(true);
	FindGo("Enemy Draft Slot4")->SetActive(true);
	FindGo("Draft Arrow1")->SetActive(true);


	ui->OnClick = [this, ui]()

	{
		std::cout << "벤 페이즈!" << std::endl;
		LineUpFalse();
		ChangePhase(Phase::Ban);
	};

}

void SceneGame::ChampionPick(int id, Team team)
{
	Champion* champ = championPool.Get();
	champ->SetState(*CHAMPION_MGR.GetChampion(id));
	champ->UpdateState();
	champ->SetName(std::to_string((int)team) + "팀 " + std::to_string(step) + "선수");
	champ->SetOrder(TagetingOrder::Default);
	champ->SetTeamScore(&redScore);
	champ->SetDieChampion(&cemetery);
	champ->SetSkill(*SKILL_MGR.GetSkill(champ->GetCurretState().skillCode1));
	champ->SetSkill(*SKILL_MGR.GetSkill(champ->GetCurretState().skillCode2));
	champ->ChangeStance(ChampionStance::None);
	champ->SetOrigin(Origins::MC);
	champ->SetSacleX(1);

	switch (team)
	{
	case Team::Red:
	{
		champ->SetPosition((Utils::RandomRange(750, 850)), (Utils::RandomRange(300, 450)));
		champ->SetEnemyTeam(&blueTeam);
		champ->SetMyTeam(&redTeam);
		champ->SetTeamColor(Team::Red);
		redTeam.push_back(champ);
		break;
	}
	case Team::Blue:
	{
		champ->SetPosition((Utils::RandomRange(420, 520)), (Utils::RandomRange(300, 450)));
		champ->SetEnemyTeam(&redTeam);
		champ->SetMyTeam(&blueTeam);
		champ->SetTeamScore(&blueScore);
		champ->SetTeamColor(Team::Blue);
		blueTeam.push_back(champ);
		break;
	}
	}
	AddGo(champ);

	step++;
}

void SceneGame::BanPhase(float dt)
{
	if (currentTurn == Turn::Enemy)
	{
		AiSelect();
	}

	FindGo("Ban Bg 2:2")->SetActive(true);
	banAnimation.Update(dt);

	ButtonTrue();

	sf::Vector2f mousePos = INPUT_MGR.GetMousePos();
	sf::Vector2f uiMousePos = ScreenToUiPos(mousePos);

	switch (mode)
	{
	case Mode::Duo:
	{
		if (step == 2)
		{
			ChangePhase(Phase::Pick);
			std::cout << "페이즈 이동" << std::endl;
			return;
		}
		break;
	}
	case Mode::Trio:
	{
		if (step == 2)
		{
			ChangePhase(Phase::Pick);
			return;
		}
		break;
	}
	case Mode::Sqaud:
	{
		if (step == 4)
		{
			ChangePhase(Phase::Pick);
			return;
		}
		else if (step == 10)
		{
			ChangePhase(Phase::Pick);
			return;
		}
		break;
	}
	}

}

void SceneGame::PickPhase(float dt)
{
	if (currentTurn == Turn::Enemy)
	{
		AiSelect();
	}

	if (step == fullStep)
	{
		readyTimer = 1.f;

		for (auto unit : championPool.GetUseList())
		{
			unit->Reset();
			unit->sortOrder = unit->GetPosition().y;
		}

		ChangePhase(Phase::Ready);

	}
	else if (mode == Mode::Sqaud && step == 8)
	{
		ChangePhase(Phase::Ban);
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
	TextGo* text = (TextGo*)FindGo("GameTimeCounter");
	text->text.setString(std::to_string((int)floor(battleTimer)));
	text->SetOrigin(Origins::MC);
	text = (TextGo*)FindGo("RedScoreCounter");
	text->text.setString(std::to_string(redScore));
	text = (TextGo*)FindGo("BlueScoreCounter");
	text->text.setString(std::to_string(blueScore));


	if (!championPool.GetUseList().empty())
	{
		for (auto unit : championPool.GetUseList())
		{
			unit->sortOrder = unit->GetPosition().y;
		}
	}
	battleTimer -= dt;
	if (battleTimer > 0.f)
	{
		for (auto champ : championPool.GetUseList())
		{
			champ->BattleUpdate(dt);
		}
	}
	if (battleTimer <= 0)
	{
		// 선수들 킬 뎃 어시 정보
		// 챔피언 입힌 피해
		// 당한 피해
		// 힐량
		// 승 패
		std::cout << "blue Kill : " << blueScore << std::endl;
		std::cout << "red Kill : " << redScore << std::endl;
		if (blueScore > redScore)
		{
			std::cout << "blue win" << std::endl;
			TEAM_MGR.EarnMoney(150);
		}
		else if (blueScore < redScore)
		{
			std::cout << "red win" << std::endl;
		}
		else if (blueScore == redScore)
		{
			std::cout << "draw" << std::endl;
		}

		ChangePhase(Phase::Result);
	}
}

void SceneGame::ResultPhase(float dt)
{
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Enter)||
		INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
	{
		if (!redTeam.empty())
		{
			for (auto team : redTeam)
			{
				std::cout << team->GetName() << " 킬 : " << team->GetKillScore() << std::endl;
				std::cout << team->GetName() << " 데스 : " << team->GetDeathScore() << std::endl;
				std::cout << team->GetName() << " 입힌 피해 : " << team->GetTotalDamage() << std::endl;
				std::cout << team->GetName() << " 당한 피해 : " << team->GetTotalOnHit() << std::endl;
				team->ReleaseSkill();
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
				std::cout << team->GetName() << " 킬 : " << team->GetKillScore() << std::endl;
				std::cout << team->GetName() << " 데스 : " << team->GetDeathScore() << std::endl;
				std::cout << team->GetName() << " 입힌 피해 : " << team->GetTotalDamage() << std::endl;
				std::cout << team->GetName() << " 당한 피해 : " << team->GetTotalOnHit() << std::endl;
				team->ReleaseSkill();
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
				std::cout << team->GetName() << " 킬 : " << team->GetKillScore() << std::endl;
				std::cout << team->GetName() << " 데스 : " << team->GetDeathScore() << std::endl;
				std::cout << team->GetName() << " 입힌 피해 : " << team->GetTotalDamage() << std::endl;
				std::cout << team->GetName() << " 당한 피해 : " << team->GetTotalOnHit() << std::endl;
				team->ReleaseSkill();
				RemoveGo(team);
				team->SetTeamColor(Team::None);
				championPool.Return(team);
			}
			cemetery.clear();
		}

		SCENE_MGR.ChangeScene(SceneId::Home);
	}
}

void SceneGame::UiInit()
{
	// 익주꺼 가져옴
	SpriteGo* header = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Sprite/header_bg#43707.png", "Header"));
	header->sprite.setScale(2, 2);
	header->sortLayer = 101;
	header->sortOrder = 1;
	header->SetPosition(0, 0);
	header->SetOrigin(Origins::TL);
	header->SetActive(false);
	// 리그시스템 선발명단 - 텍스트
	//LineUp1,2 등 스트링 테이블 추가해야할 사항 있음
	{
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
		lineupText7->SetPosition(165, 245);
		lineupText7->SetOrigin(Origins::MC);
		lineupText7->SetActive(false);

		TextGo* lineupText8 = (TextGo*)AddGo(new TextGo("Player Member Text2"));
		lineupText8->sortLayer = 103;
		lineupText8->sortOrder = 1;
		lineupText8->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText8->text.setCharacterSize(17);
		lineupText8->text.setFillColor(sf::Color::White);
		lineupText8->text.setString(L"내 선수2");
		lineupText8->SetPosition(338, 245);
		lineupText8->SetOrigin(Origins::MC);
		lineupText8->SetActive(false);

		TextGo* lineupText9 = (TextGo*)AddGo(new TextGo("Player Member Text3"));
		lineupText9->sortLayer = 103;
		lineupText9->sortOrder = 1;
		lineupText9->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText9->text.setCharacterSize(17);
		lineupText9->text.setFillColor(sf::Color::White);
		lineupText9->text.setString(L"내 선수3");
		lineupText9->SetPosition(165, 414);
		lineupText9->SetOrigin(Origins::MC);
		lineupText9->SetActive(false);

		TextGo* lineupText10 = (TextGo*)AddGo(new TextGo("Player Member Text4"));
		lineupText10->sortLayer = 103;
		lineupText10->sortOrder = 1;
		lineupText10->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText10->text.setCharacterSize(17);
		lineupText10->text.setFillColor(sf::Color::White);
		lineupText10->text.setString(L"내 선수4");
		lineupText10->SetPosition(338, 414);
		lineupText10->SetOrigin(Origins::MC);
		lineupText10->SetActive(false);

		TextGo* lineupText11 = (TextGo*)AddGo(new TextGo("Player Member Text5"));
		lineupText11->sortLayer = 103;
		lineupText11->sortOrder = 1;
		lineupText11->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText11->text.setCharacterSize(17);
		lineupText11->text.setFillColor(sf::Color::White);
		lineupText11->text.setString(L"내 후보1");
		lineupText11->SetPosition(646, 245);
		lineupText11->SetOrigin(Origins::MC);
		lineupText11->SetActive(false);

		TextGo* lineupText12 = (TextGo*)AddGo(new TextGo("Player Member Text6"));
		lineupText12->sortLayer = 103;
		lineupText12->sortOrder = 1;
		lineupText12->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText12->text.setCharacterSize(17);
		lineupText12->text.setFillColor(sf::Color::White);
		lineupText12->text.setString(L"내 후보2");
		lineupText12->SetPosition(646, 414);
		lineupText12->SetOrigin(Origins::MC);
		lineupText12->SetActive(false);

		TextGo* lineupText13 = (TextGo*)AddGo(new TextGo("Enemy Member Text1"));
		lineupText13->sortLayer = 103;
		lineupText13->sortOrder = 1;
		lineupText13->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText13->text.setCharacterSize(17);
		lineupText13->text.setFillColor(sf::Color::White);
		lineupText13->text.setString(L"적 선수1");
		lineupText13->SetPosition(865, 245);
		lineupText13->SetOrigin(Origins::MC);
		lineupText13->SetActive(false);

		TextGo* lineupText14 = (TextGo*)AddGo(new TextGo("Enemy Member Text2"));
		lineupText14->sortLayer = 103;
		lineupText14->sortOrder = 1;
		lineupText14->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText14->text.setCharacterSize(17);
		lineupText14->text.setFillColor(sf::Color::White);
		lineupText14->text.setString(L"적 선수2");
		lineupText14->SetPosition(1038, 245);
		lineupText14->SetOrigin(Origins::MC);
		lineupText14->SetActive(false);

		TextGo* lineupText15 = (TextGo*)AddGo(new TextGo("Enemy Member Text3"));
		lineupText15->sortLayer = 103;
		lineupText15->sortOrder = 1;
		lineupText15->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText15->text.setCharacterSize(17);
		lineupText15->text.setFillColor(sf::Color::White);
		lineupText15->text.setString(L"적 선수3");
		lineupText15->SetPosition(865, 414);
		lineupText15->SetOrigin(Origins::MC);
		lineupText15->SetActive(false);

		TextGo* lineupText16 = (TextGo*)AddGo(new TextGo("Enemy Member Text4"));
		lineupText16->sortLayer = 103;
		lineupText16->sortOrder = 1;
		lineupText16->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText16->text.setCharacterSize(17);
		lineupText16->text.setFillColor(sf::Color::White);
		lineupText16->text.setString(L"적 선수4");
		lineupText16->SetPosition(1038, 414);
		lineupText16->SetOrigin(Origins::MC);
		lineupText16->SetActive(false);
	}
	// 리그시스템 선발명단 - 라인업
	{
		SpriteGo* pleyerTeamTitle1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_team_title_bg_0.png", "Pleyer Team Title1"));
		pleyerTeamTitle1->sprite.setScale(2, 2);
		pleyerTeamTitle1->sortLayer = 102;
		pleyerTeamTitle1->sortOrder = 1;
		pleyerTeamTitle1->SetPosition(290, 200);
		pleyerTeamTitle1->SetOrigin(Origins::MC);
		pleyerTeamTitle1->SetActive(false); // 왼쪽 테두리

		SpriteGo* enemyTeamTitle1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_team_title_bg_2.png", "Enemy Team Title1"));
		enemyTeamTitle1->sprite.setScale(2, 2);
		enemyTeamTitle1->sortLayer = 102;
		enemyTeamTitle1->sortOrder = 1;
		enemyTeamTitle1->SetPosition(990, 200);
		enemyTeamTitle1->SetOrigin(Origins::MC);
		enemyTeamTitle1->SetActive(false);

		// 리소스 있어서 추가는 했는데 안쓸듯
		SpriteGo* pleyerTeamTitle2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/Black_0.png", "Pleyer Team Title2"));
		pleyerTeamTitle2->sprite.setScale(2, 2);
		pleyerTeamTitle2->sortLayer = 102;
		pleyerTeamTitle2->sortOrder = 1;
		pleyerTeamTitle2->SetPosition(680, 200);
		pleyerTeamTitle2->SetOrigin(Origins::MC);
		pleyerTeamTitle2->SetActive(false);

		SpriteGo* enemyTeamTitle2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_team_title_bg_3.png", "Enemy Team Title2"));
		enemyTeamTitle2->sprite.setScale(2, 2);
		enemyTeamTitle2->sortLayer = 102;
		enemyTeamTitle2->sortOrder = 1;
		enemyTeamTitle2->SetPosition(0, 0);
		enemyTeamTitle2->SetOrigin(Origins::MC);
		enemyTeamTitle2->SetActive(false);
	}
	// 드래프트 슬롯  
	{
		SpriteGo* draftSlot1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Player Draft Slot1"));
		draftSlot1->sprite.setScale(2, 2);
		draftSlot1->sortLayer = 102;
		draftSlot1->sortOrder = 1;
		draftSlot1->SetPosition(204, 309);
		draftSlot1->SetOrigin(Origins::MC);
		draftSlot1->SetActive(false);

		SpriteGo* draftSlot2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Player Draft Slot2"));
		draftSlot2->sprite.setScale(2, 2);
		draftSlot2->sortLayer = 102;
		draftSlot2->sortOrder = 1;
		draftSlot2->SetPosition(375, 309);
		draftSlot2->SetOrigin(Origins::MC);
		draftSlot2->SetActive(false);

		SpriteGo* draftSlot3 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Player Draft Slot3"));
		draftSlot3->sprite.setScale(2, 2);
		draftSlot3->sortLayer = 102;
		draftSlot3->sortOrder = 1;
		draftSlot3->SetPosition(204, 477);
		draftSlot3->SetOrigin(Origins::MC);
		draftSlot3->SetActive(false);

		SpriteGo* draftSlot4 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Player Draft Slot4"));
		draftSlot4->sprite.setScale(2, 2);
		draftSlot4->sortLayer = 102;
		draftSlot4->sortOrder = 1;
		draftSlot4->SetPosition(375, 477);
		draftSlot4->SetOrigin(Origins::MC);
		draftSlot4->SetActive(false);

		SpriteGo* draftSlot5 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Player Draft Slot5"));
		draftSlot5->sprite.setScale(2, 2);
		draftSlot5->sortLayer = 102;
		draftSlot5->sortOrder = 1;
		draftSlot5->SetPosition(684, 309);
		draftSlot5->SetOrigin(Origins::MC);
		draftSlot5->SetActive(false);

		SpriteGo* draftSlot6 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Player Draft Slot6"));
		draftSlot6->sprite.setScale(2, 2);
		draftSlot6->sortLayer = 102;
		draftSlot6->sortOrder = 1;
		draftSlot6->SetPosition(684, 477);
		draftSlot6->SetOrigin(Origins::MC);
		draftSlot6->SetActive(false);

		SpriteGo* draftSlot7 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Enemy Draft Slot1"));
		draftSlot7->sprite.setScale(2, 2);
		draftSlot7->sortLayer = 102;
		draftSlot7->sortOrder = 1;
		draftSlot7->SetPosition(904, 309);
		draftSlot7->SetOrigin(Origins::MC);
		draftSlot7->SetActive(false);

		SpriteGo* draftSlot8 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Enemy Draft Slot2"));
		draftSlot8->sprite.setScale(2, 2);
		draftSlot8->sortLayer = 102;
		draftSlot8->sortOrder = 1;
		draftSlot8->SetPosition(1076, 309);
		draftSlot8->SetOrigin(Origins::MC);
		draftSlot8->SetActive(false);

		SpriteGo* draftSlot9 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Enemy Draft Slot3"));
		draftSlot9->sprite.setScale(2, 2);
		draftSlot9->sortLayer = 102;
		draftSlot9->sortOrder = 1;
		draftSlot9->SetPosition(904, 477);
		draftSlot9->SetOrigin(Origins::MC);
		draftSlot9->SetActive(false);

		SpriteGo* draftSlot10 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0.png", "Enemy Draft Slot4"));
		draftSlot10->sprite.setScale(2, 2);
		draftSlot10->sortLayer = 102;
		draftSlot10->sortOrder = 1;
		draftSlot10->SetPosition(1076, 477);
		draftSlot10->SetOrigin(Origins::MC);
		draftSlot10->SetActive(false);
	}

	SpriteGo* lineup = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_bg #43820.png", "Line Up"));
	lineup->sprite.setScale(2, 2);
	lineup->sortLayer = 101;
	lineup->sortOrder = 1;
	lineup->SetPosition(FRAMEWORK.GetWindowSize() / 2.f);
	lineup->SetOrigin(Origins::MC);
	lineup->SetActive(false);

	SpriteGo* lineupSwap = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_bg_swap.png", "Line Up Swap"));
	lineupSwap->sprite.setScale(2, 2);
	lineupSwap->sortLayer = 101;
	lineupSwap->sortOrder = 1;
	lineupSwap->SetPosition(FRAMEWORK.GetWindowSize() / 2.f);
	lineupSwap->SetOrigin(Origins::MC);
	lineupSwap->SetActive(false);

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

	SpriteGo* draftArrow1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_arrow1.png", "Draft Arrow1"));
	draftArrow1->sprite.setScale(0.9, 0.9);
	draftArrow1->sortLayer = 102;
	draftArrow1->sortOrder = 1;
	draftArrow1->SetPosition(530, FRAMEWORK.GetWindowSize().y / 2.f);
	draftArrow1->SetOrigin(Origins::MC);
	draftArrow1->SetActive(false);

	SpriteGo* draftArrow2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_arrow2.png", "Draft Arrow2"));
	draftArrow2->sprite.setScale(0.9, 0.9);
	draftArrow2->sortLayer = 102;
	draftArrow2->sortOrder = 1;
	draftArrow2->SetPosition(530, FRAMEWORK.GetWindowSize().y / 2.f);
	draftArrow2->SetOrigin(Origins::MC);
	draftArrow2->SetActive(false);

	SpriteGo* swapLine = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/swap_line.png", "Swap Line"));
	swapLine->sprite.setScale(2, 2);
	swapLine->sortLayer = 101;
	swapLine->sortOrder = 1;
	swapLine->SetPosition(0, 0);
	swapLine->SetOrigin(Origins::TL);
	swapLine->SetActive(false);

	// 플레이어 컨디션 //
	{
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
	}

	// 리그시스템 선발명단 직후 라인업넥스트
	{
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
	}

	// 리그시스템 밴픽
	{
		SpriteGo* banBg22 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/banpick_22_bg.png", "Ban Bg 2:2"));
		//banBg22->sprite.setScale(2, 2);
		banBg22->sortLayer = 101;
		banBg22->sortOrder = 1;
		banBg22->SetPosition(0, 0);
		banBg22->SetOrigin(Origins::TL);
		banBg22->SetActive(false);

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
	}

	// 리그 시스템 스왑 / swap_line겸칩 / 위치 라인업
	{

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

	//경기중 상단 UI - 위치 필요한곳에 배치하여 조정하세요-김1
	{
		AddGo(new TextGo("GameTimeCounter"));
		TextGo* text = (TextGo*)FindGo("GameTimeCounter");
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("60");
		text->text.setCharacterSize(20);
		text->SetOrigin(Origins::MC);
		text->SetPosition(FRAMEWORK.GetWindowSize().x/2.f,65.f);
		text->sortLayer = 110;

		AddGo(new TextGo("RedScoreCounter"));
		text = (TextGo*)FindGo("RedScoreCounter");
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("0");
		text->text.setCharacterSize(30);
		text->SetOrigin(Origins::MC);
		text->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f + 90.f, 25.f);
		text->sortLayer = 110;

		AddGo(new TextGo("BlueScoreCounter"));
		text = (TextGo*)FindGo("BlueScoreCounter");
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("0");
		text->text.setCharacterSize(30);
		text->SetOrigin(Origins::MC);
		text->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f - 90.f, 25.f);
		text->sortLayer = 110;
	}
}

void SceneGame::ButtonInit()
{
	//// 밴픽단계 챔피언 슬롯 18개
	for (int i = 0; i < champCount; i++)
	{
		std::stringstream ss;
		ss << "Champion Slot" << i + 1;
		championSlot[i] = (UiButton*)AddGo(new UiButton("graphics/LeagueSystem/Banpick/championSlot1.png", ss.str()));
		championSlot[i]->sprite.setScale(0.98f, 0.93f);
		championSlot[i]->SetOrigin(Origins::MC);
		championSlot[i]->sortLayer = 103;
		championSlot[i]->sortOrder = 1;
		championSlot[i]->SetPosition(278 + ((i % 10) * 78), 233 + ((i / 10) * 113));
		championSlot[i]->SetActive(false);

		championSlot[i]->OnEnter = [this, i]() {
			FindGo("BanSheet")->SetActive(true);
			banSheet->SetPosition(championSlot[i]->GetPosition());
			selectCheck = true;
		};
		championSlot[i]->OnExit = [this]() {
			if (selectCheck)
			{
				return;
			}
			FindGo("BanSheet")->SetActive(false);
			selectCheck = false;
		};
		championSlot[i]->OnClick = [this,i]() {
			std::stringstream ss;
			ss << "Champion Slot" << i + 1 << "Image";
			if (currentPhase == Phase::Ban)
			{
				for (int j = 0; j < banChamps.size(); j++)
				{
					if (banChamps[j] == i)
					{
						return;
					}
				}

				SpriteGo* spr = (SpriteGo*)FindGo(ss.str());
				spr->sprite.setColor(sf::Color(50, 50, 50));
				banChamps[step] = i;
				step++;
				ChangeTeam();
				ChangeTurn();
			}
			else if(currentPhase == Phase::Pick)
			{
				for (int j = 0; j < banChamps.size(); j++)
				{
					if (banChamps[j] == i)
					{
						return;
					}
				}
				if (team == Team::Red)
				{
					championSlot[i]->sprite.setColor(sf::Color(250, 0, 0));
				}
				else
				{
					championSlot[i]->sprite.setColor(sf::Color(0, 0, 250));
				}
				banChamps[step] = i;
				ChampionPick(i,team);
				ChangeTeam();
				ChangeTurn();	
			}
		};
		ss << "Image";
		std::stringstream iconTextureRoute;
		iconTextureRoute << "graphics/UiFix/character_icons_" << i << ".png";
		AddGo(new UiButton(iconTextureRoute.str(), ss.str()));
		UiButton* spr = (UiButton*)FindGo(ss.str());
		spr->SetOrigin(Origins::MC);
		spr->SetPosition(championSlot[i]->GetPosition().x,
			championSlot[i]->GetPosition().y - 12.f);
		spr->sortLayer = 103;
		spr->sortOrder = 1;
		spr->SetSize(1.5, 1.5);
		spr->sprite.setColor(sf::Color::White);
		spr->SetActive(false);
	}
}

void SceneGame::ButtonTrue(bool on)
{
	for (int i = 0; i < champCount; i++)
	{
		championSlot[i]->SetActive(on);
		std::stringstream ss;
		ss  << "Champion Slot" << i + 1 << "Image";
		UiButton* spr = (UiButton*)FindGo(ss.str());
		spr->SetActive(on);
		FindGo("BanSheet")->SetActive(on);
	}
}

void SceneGame::LineUpTrue()
{
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

	FindGo("Pleyer Team Title1")->SetActive(true);
	FindGo("Enemy Team Title1")->SetActive(true);

	FindGo("Player Draft Slot1")->SetActive(true);
	FindGo("Player Draft Slot2")->SetActive(true);
	FindGo("Player Draft Slot3")->SetActive(true);
	FindGo("Player Draft Slot4")->SetActive(true);
	FindGo("Player Draft Slot5")->SetActive(true);
	FindGo("Player Draft Slot6")->SetActive(true);

	FindGo("Enemy Draft Slot1")->SetActive(true);
	FindGo("Enemy Draft Slot2")->SetActive(true);
	FindGo("Enemy Draft Slot3")->SetActive(true);
	FindGo("Enemy Draft Slot4")->SetActive(true);
	FindGo("Draft Arrow1")->SetActive(true);
}

void SceneGame::LineUpFalse()
{
	FindGo("Line Up")->SetActive(false);
	FindGo("Next Button")->SetActive(false);
	FindGo("keyboard Buttons_D")->SetActive(false);
	FindGo("keyboard Buttons_F")->SetActive(false);

	FindGo("Line Up Text1")->SetActive(false);
	FindGo("Line Up Text2")->SetActive(false);
	FindGo("Line Up Text3")->SetActive(false);
	FindGo("Line Up Text4")->SetActive(false);
	FindGo("Line Up Text5")->SetActive(false);
	FindGo("Enemy Team Name Text")->SetActive(false);

	FindGo("Player Member Text1")->SetActive(false);
	FindGo("Player Member Text2")->SetActive(false);
	FindGo("Player Member Text3")->SetActive(false);
	FindGo("Player Member Text4")->SetActive(false);
	FindGo("Player Member Text5")->SetActive(false);
	FindGo("Player Member Text6")->SetActive(false);

	FindGo("Enemy Member Text1")->SetActive(false);
	FindGo("Enemy Member Text2")->SetActive(false);
	FindGo("Enemy Member Text3")->SetActive(false);
	FindGo("Enemy Member Text4")->SetActive(false);

	FindGo("Pleyer Team Title1")->SetActive(false);
	FindGo("Enemy Team Title1")->SetActive(false);

	FindGo("Player Draft Slot1")->SetActive(false);
	FindGo("Player Draft Slot2")->SetActive(false);
	FindGo("Player Draft Slot3")->SetActive(false);
	FindGo("Player Draft Slot4")->SetActive(false);
	FindGo("Player Draft Slot5")->SetActive(false);
	FindGo("Player Draft Slot6")->SetActive(false);

	FindGo("Enemy Draft Slot1")->SetActive(false);
	FindGo("Enemy Draft Slot2")->SetActive(false);
	FindGo("Enemy Draft Slot3")->SetActive(false);
	FindGo("Enemy Draft Slot4")->SetActive(false);
	FindGo("Draft Arrow1")->SetActive(false);
}

void SceneGame::BanPickFalse()
{
	SpriteGo* spr;
	banSheet->SetActive(false);
	banSheetBlueTeam->SetActive(false);
	banSheetRedTeam->SetActive(false);
	for (int i = 0; i < championSlot.size(); i++)
	{
		championSlot[i]->SetActive(false);
	}
	
	spr = (SpriteGo*)FindGo("Ban Bg 2:2");
	spr->SetActive(false);
	ButtonTrue(false);
	spr = (SpriteGo*)FindGo("1");
	spr->SetActive(true);
}

void SceneGame::AiSelect()
{
	championSlot[Utils::RandomRange(0, champCount - 1)]->OnClick();
}
