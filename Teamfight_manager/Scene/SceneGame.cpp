#include "stdafx.h"
#include "SceneGame.h"

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
	bg->SetPosition(centerPos);
	bg->SetOrigin(Origins::MC);

	championPool.OnCreate = [this](Champion* champion){
		champion->ChangeStance(ChampionStance::None);
	};
	championPool.Init();

	fullStep = 4;
	step = 1;
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
}

void SceneGame::Exit()
{
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
			step = 1;
			// Blue 팀이 될 플레이어 선택
			team = Team::Blue;
			break;
		}
		case Mode::Trio:
		{
			fullStep = 8;
			step = 1;
			team = Team::Blue;
			break;
		}
		case Mode::Sqaud:
		{
			fullStep = 14;
			step = 1;
			team = Team::Blue;
			break;
		}
		}
		ChangePhase(Phase::Ban);
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
}

void SceneGame::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
}

void SceneGame::ChangePhase(Phase cPhase)
{
	switch (cPhase)
	{
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
		battleTimer = 20.f;
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
		redChamp->SetState(100, 10, 10, 10, 10, 25);
		redChamp->SetOrder(TagetingOrder::ShortRange);
		redChamp->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/archer_0.png"));
		redChamp->SetPosition((Utils::RandomRange(400,500)), (Utils::RandomRange(300, 400)));
		redChamp->SetId(std::to_string(step));
		redChamp->SetEnemyTeam(&blueTeam);
		redChamp->SetMyTeam(&redTeam);
		redChamp->ChangeStance(ChampionStance::None);
		redChamp->Reset();
		redChamp->SetSacleX(1);
		if (currentTurn == Turn::Player)
		{
			redChamp->SetPlayerTeam();
		}
		redTeam.push_back(redChamp);
		AddGo(redChamp);
		ChangeTeam();
		ChangeTurn();
		break;
	}
	case Team::Blue:
	{
		Champion* blueChamp = championPool.Get();
		blueChamp->SetState(100, 10, 10, 10, 10, 25);
		blueChamp->SetOrder(TagetingOrder::ShortRange);
		blueChamp->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/archer_0.png"));
		blueChamp->SetPosition((Utils::RandomRange(700, 800)), (Utils::RandomRange(300, 400)));
		blueChamp->SetId(std::to_string(step));
		blueChamp->SetEnemyTeam(&redTeam);
		blueChamp->SetMyTeam(&blueTeam);
		blueChamp->ChangeStance(ChampionStance::None);
		blueChamp->Reset();
		blueChamp->SetSacleX(-1);
		if (currentTurn == Turn::Player)
		{
			blueChamp->SetPlayerTeam();
		}
		blueTeam.push_back(blueChamp);
		AddGo(blueChamp);
		ChangeTeam();
		ChangeTurn();
		break;
	}
	}

	if (step == fullStep)
	{
		readyTimer = 5.f;

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
		if (!redTeam.empty())
		{
			for (auto team : redTeam)
			{
				RemoveGo(team);
				championPool.Return(team);
			}
			redTeam.clear();
		}


		if (!blueTeam.empty())
		{
			for (auto team : blueTeam)
			{
				RemoveGo(team);
				championPool.Return(team);
			}
			blueTeam.clear();
		}
		ChangePhase(Phase::Pick);
	}
}
