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

#include <algorithm>

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
	isClickBlocker = false;

	// 카메라
	sf::Vector2f windowSize = FRAMEWORK.GetWindowSize();
	sf::Vector2f centerPos = windowSize * 0.5f;

	worldView.setSize(windowSize * 0.7f);
	worldView.setCenter(centerPos.x, centerPos.y - 50.f);
	uiView.setSize(windowSize);
	uiView.setCenter(centerPos);

	SpriteGo* bg = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Texture2D/stadium.png", "1"));
	bg->sortLayer = -1;
	bg->sortOrder = 2;
	bg->SetPosition(centerPos);
	bg->SetOrigin(Origins::MC);
	bg->SetActive(false);

	stadiumSkyBg = (SpriteGo*)AddGo(new SpriteGo("graphics/BackGround/stadium_sky_bg.png", "StadiumSkyBg"));
	stadiumSkyBg->sortLayer = -3;
	stadiumSkyBg->SetPosition(centerPos);
	stadiumSkyBg->SetOrigin(Origins::MC);
	stadiumSkyBg->SetActive(false);

	banBlueLine = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/middle_ban_blue_line.png", "BanBlueLine"));
	banBlueLine->sprite.setScale(15, 29);
	banBlueLine->sortLayer = 102;
	banBlueLine->sortOrder = 1;
	banBlueLine->SetPosition(525, 127);
	banBlueLine->SetOrigin(Origins::MC);
	banBlueLine->SetActive(false);

	banRedLine = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/middle_ban_red_line.png", "BanRedLine"));
	banRedLine->sprite.setScale(15, 29);
	banRedLine->sortLayer = 102;
	banRedLine->sortOrder = 1;
	banRedLine->SetPosition(525, 127);
	banRedLine->SetOrigin(Origins::MC);
	banRedLine->SetActive(false);

	//벡터 임시위치
	championSlot = std::vector<UiButton*>(champCount);
	swapSlot = std::vector<UiButton*>(6);

	banEffectSheet = std::vector<SpriteGo*>(effectCount);
	banEffectAnimation = std::vector<AnimatioControler>(effectAniCount);


	UiInit();
	BanPickInit();
	LineUpInit();

	//애니메이션 추가 임시위치, 정규화하기
	for (int i = 0; i < effectCount; i++)
	{
		banEffectSheet[i] = (SpriteGo*)AddGo(new SpriteGo("", "BanEffectSheet"));
		banEffectSheet[i]->SetOrigin(Origins::MC);
		banEffectSheet[i]->sortLayer = 103;
		banEffectSheet[i]->sortOrder = 1;
		banEffectSheet[i]->SetActive(false);
	}
	for (int i = 0; i < effectCount; i++)
	{
		banEffectAnimation[i].SetTarget(&banEffectSheet[i]->sprite);
		banEffectAnimation[i].AddClip(*RESOURCE_MGR.GetAnimationClip("animations/banEffect.csv"));

	}

	banSheet = new SpriteGo("", "BanSheet");
	AddGo(banSheet);
	banSheet->sortLayer = 102;
	banAnimation.SetTarget(&banSheet->sprite);
	banAnimation.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/banSlotBlue.csv"));

	Waterfall = new SpriteGo("", "WaterFall");
	AddGo(Waterfall);
	Waterfall->sortLayer = -3;
	Waterfall->SetSize(1.5, 1);
	waterfallAnimation.SetTarget(&Waterfall->sprite);
	waterfallAnimation.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/waterfall.csv"));

	Waterfall2 = new SpriteGo("", "WaterFall2");
	AddGo(Waterfall2);
	Waterfall2->sortLayer = -3;
	Waterfall2->SetSize(1.5, 1);
	waterfallAnimation2.SetTarget(&Waterfall2->sprite);
	waterfallAnimation2.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/waterfall.csv"));

	turnArrowBlue = new SpriteGo("", "TurnArrowBlue");
	AddGo(turnArrowBlue);
	turnArrowBlue->SetSize(2, 2);
	turnArrowBlue->sortLayer = 107;
	turnArrowBlueAnimation.SetTarget(&turnArrowBlue->sprite);
	turnArrowBlueAnimation.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/turnArrowBlue.csv"));

	turnArrowRed = new SpriteGo("", "TurnArrowRed");
	AddGo(turnArrowRed);
	turnArrowRed->SetSize(2, 2);
	turnArrowRed->sortLayer = 107;
	turnArrowRedAnimation.SetTarget(&turnArrowRed->sprite);
	turnArrowRedAnimation.AddClip(*RESOURCE_MGR.GetAnimationClip("animations/turnArrowRed.csv"));

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

	// 금지단계 밴픽단계 텍스트 추가
	banText = (TextGo*)AddGo(new TextGo("BanText"));
	banText->sortLayer = 107;
	banText->sortOrder = 1;
	banText->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	banText->text.setCharacterSize(45);
	banText->text.setFillColor(sf::Color::White);
	banText->text.setString(L"금지 단계");
	banText->SetPosition(0, FRAMEWORK.GetWindowSize().y / 2);
	banText->SetOrigin(Origins::MC);
	banText->SetActive(false);

	pickText = (TextGo*)AddGo(new TextGo("PickText"));
	pickText->sortLayer = 107;
	pickText->sortOrder = 1;
	pickText->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	pickText->text.setCharacterSize(45);
	pickText->text.setFillColor(sf::Color::White);
	pickText->text.setString(L"선택 단계");
	pickText->SetPosition(0, FRAMEWORK.GetWindowSize().y / 2);
	pickText->SetOrigin(Origins::MC);
	pickText->SetActive(false);

	grayScreen = (RectGo*)AddGo(new RectGo("GrayScreen"));
	grayScreen->sortLayer = 106;
	grayScreen->sortOrder = 1;
	grayScreen->rectangle.setFillColor(sf::Color(0, 0, 0, 128));
	grayScreen->SetSize(FRAMEWORK.GetWindowSize());
	grayScreen->SetPosition(FRAMEWORK.GetWindowSize() / 2.f);
	grayScreen->SetOrigin(Origins::MC);
	grayScreen->SetActive(false);

	effectPool.OnCreate = [this](ChampionEffect* effect) {
		effect->SetEffectType(0);
		effect->SetPool(&effectPool);
		};
	effectPool.Init(100);

	championPool.OnCreate = [this, field](Champion* champion) {
		champion->ChangeStance(ChampionStance::None);
		champion->SetField(field);
		champion->SetEffectPool(&effectPool);
		champion->sortLayer = 3;
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

	banAnimation.Play("Idle");
	waterfallAnimation.Play("Idle");
	waterfallAnimation2.Play("Idle");
	turnArrowBlueAnimation.Play("Idle");
	turnArrowRedAnimation.Play("Idle");

	banSheet->SetOrigin(Origins::MC);
	Waterfall->SetOrigin(Origins::MC);
	Waterfall2->SetOrigin(Origins::MC);
	turnArrowBlue->SetOrigin(Origins::MC);
	turnArrowRed->SetOrigin(Origins::MC);

	currentPhase = Phase::None;
	banChamps = std::vector<int>(14, -1);

	banSheet->SetActive(false);
	banSheet->SetPosition(-100, -100);
	banSheetBlueTeam->SetActive(false);
	banSheetRedTeam->SetActive(false);

	for (int i = 0; i < champCount; i++)
	{
		std::stringstream ss;
		ss << "ChampionSlotName" << i + 1;
		FindGo(ss.str())->SetActive(false);
	}

	Waterfall->SetActive(false);
	Waterfall2->SetActive(false);
	stadiumSkyBg->SetActive(false);
	turnArrowBlue->SetActive(false);
	turnArrowRed->SetActive(false);

	for (int i = 0; i < 6; i++)
	{
		banEffectSheet[i]->SetOrigin(Origins::MC);
		banEffectSheet[i]->SetActive(false);
	}

	banCount = 0;
	pickCount = 0;
	pickEnemyCount = 0;

	pickSlotCount = (int)mode + 2;
	pickDoingSlotCount = (int)mode + 2;
	pickSlotEnemyCount = (int)mode + 2;
	enemyPickDoingSlotCount = (int)mode + 2;

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

	UiButton* ui = (UiButton*)FindGo("Next Button");
	ui->OnClick = [this, ui]()
		{
			banPickTextClickCount = 0;
			pickDoingSlotClickCount = 0;

			pickDoingSlotCount = 0;
			enemyPickDoingSlotCount = 0;


			FindGo("Draft_Slot_Blue_Clicked")->SetActive(false);
			std::cout << "벤 페이즈!" << std::endl;
			LineUpFalse();
			SwapSlotFalse();		
			ChangePhase(Phase::Ban);
			ChampSlotFalse();
		};

	isPlayerEnough = TEAM_MGR.GetPlayerNum() > 0;
	BanPickTrue(false);
	GetPlayers();
	LineUpFalse();
	UiEnter();
	if (isPlayerEnough)
	{
		LineUpTrue();
	}

	aiBanPick.Init();
	aiBanPick.SetChampions();
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
	ClearObjectPool(effectPool);
	Scene::Exit();
}

void SceneGame::Update(float dt)
{
	// 마우스 좌표 테스트
	/*if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
	{
		std::cout << INPUT_MGR.GetMousePos().x << "\t"
			<< INPUT_MGR.GetMousePos().y << std::endl;
	}*/

	// 월드 마우스 좌표 테스트
	/*if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
	{
		std::cout << this->ScreenToWorldPos(INPUT_MGR.GetMousePos()).x << "\t"
			<< this->ScreenToWorldPos(INPUT_MGR.GetMousePos()).y << std::endl;
	}*/



	banAnimation.Update(dt);
	waterfallAnimation.Update(dt);
	waterfallAnimation2.Update(dt);
	turnArrowBlueAnimation.Update(dt);
	turnArrowRedAnimation.Update(dt);

	for (int i = 0; i < 6; i++)
	{
		banEffectAnimation[i].Update(dt);
		if (banEffectAnimation[i].GetLastFrame())
		{
			banEffectAnimation[i].GetTarget()->setScale(76.f / 38.f, 96.f / 52.f);
			banEffectSheet[i]->SetOrigin(Origins::MC);
		}
	}
	
	Scene::Update(dt);

	selectCheck = true;

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
		if (isPlayerEnough)
		{
			ChangePhase(Phase::League);
		}
		else
		{
			LineUpFalse();
			SwapSlotFalse();
			ChangePhase(Phase::Ban);
			FindGo("Draft_Slot_Blue_Clicked")->SetActive(false);
			FindGo("Draft_Slot_White")->SetActive(false);
			FindGo("Draft_Slot_Red_White")->SetActive(false);
		}
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
		BanPickTrue();
		SetBanPick();
		currentPhase = Phase::Ban;
		break;
	}
	case Phase::Pick:
	{
		// 픽페이즈 넘어가기전 타이머 초기화
		textDuration = 1.2f;
		textSpeed = 1000.f;

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
		BanPickToBattleFalse();
		currentPhase = Phase::Battle;
		battleTimer = 60.f;
		SpriteGo* spr = (SpriteGo*)FindGo("1");
		spr->SetActive(true);
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
	isClickBlocker = false;
}

void SceneGame::ChampionPick(int id, Team team)
{
	Champion* champ = championPool.Get();
	champ->SetName(std::to_string((int)team) + "팀 " + std::to_string(step) + "선수");
	champ->SetOrder(TargetingOrder::Default);
	champ->SetDieChampion(&cemetery);
	champ->ChangeStance(ChampionStance::None);
	champ->SetOrigin(Origins::MC);
	champ->SetSacleX(1);
	champ->SetUltiTimer(Utils::RandomRange(10.f, 30.f));

	switch (team)
	{
	case Team::Red:
	{
		champ->SetPosition((Utils::RandomRange(750, 850)), (Utils::RandomRange(300, 450)));
		champ->SetEnemyTeam(&blueTeam);
		champ->SetMyTeam(&redTeam);
		champ->SetTeamScore(&redScore);
		champ->SetTeamColor(Team::Red);
		champ->SetState(*CHAMPION_MGR.GetChampion(id));
		champ->UpdateState(0.f);
		champ->SetSkill(*SKILL_MGR.GetSkill(champ->GetCurretState().skillCode1));
		champ->SetSkill(*SKILL_MGR.GetSkill(champ->GetCurretState().skillCode2));
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
		champ->SetState(*CHAMPION_MGR.GetChampion(id));
		champ->UpdateState(0.f);
		champ->SetSkill(*SKILL_MGR.GetSkill(champ->GetCurretState().skillCode1));
		champ->SetSkill(*SKILL_MGR.GetSkill(champ->GetCurretState().skillCode2));
		blueTeam.push_back(champ);
		break;
	}
	}
	AddGo(champ);

	step++;
}

void SceneGame::BanPhase(float dt)
{
	sf::FloatRect textBounds = banText->text.getLocalBounds();
	sf::Vector2f newPosition = banText->GetPosition() + sf::Vector2f(textSpeed * dt, 0);

	grayScreen->SetActive(true);
	banText->SetActive(true);
	banText->SetPosition(newPosition);

	if (newPosition.x >= 640.f && textDuration >= 0.f)
	{
		banText->SetPosition(FRAMEWORK.GetWindowSize() / 2.f);
		textDuration -= dt;
	}
	else if (textDuration <= 0.f)
	{
		banText->SetPosition(newPosition);
	}
	if (banText->GetPosition().x >= 1350.f)
	{
		grayScreen->SetActive(false);
		banText->SetActive(false);
	}

	aiBanTimer -= dt;

	if (currentTurn == Turn::Enemy && aiBanTimer <= 0.f)
	{
		AiSelect();
	}

	FindGo("Ban Bg 2:2")->SetActive(true);

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
	sf::FloatRect textBounds = pickText->text.getLocalBounds();
	sf::Vector2f newPosition = pickText->GetPosition() + sf::Vector2f(textSpeed * dt, 0);

	grayScreen->SetActive(true);
	pickText->SetActive(true);
	pickText->SetPosition(newPosition);

	if (newPosition.x >= 640.f && textDuration >= 0.f)
	{
		pickText->SetPosition(FRAMEWORK.GetWindowSize() / 2.f);
		textDuration -= dt;
	}
	else if (textDuration <= 0.f)
	{
		pickText->SetPosition(newPosition);
	}
	if (pickText->GetPosition().x >= 1350.f)
	{
		grayScreen->SetActive(false);
		pickText->SetActive(false);
	}

	aiPickTimer -= dt;

	if (currentTurn == Turn::Enemy && aiPickTimer <= 0.f)
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

	FindGo("WaterFall")->SetActive(true);
	FindGo("WaterFall2")->SetActive(true);

	stadiumSkyBg->SetActive(true);

	Waterfall->SetPosition(747.1f, 629.4f);
	Waterfall2->SetPosition(530.1f, 629.4f);

	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Numpad0))
	{
		speedUp = 1.f;
	}
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Numpad1))
	{
		speedUp = 2.f;
	}
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Numpad2))
	{
		speedUp = 0.5f;
	}
	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Numpad3))
	{
		speedUp = 60.f;
	}

	if (!championPool.GetUseList().empty())
	{
		for (auto unit : championPool.GetUseList())
		{
			unit->sortOrder = unit->GetPosition().y;
		}
	}
	battleTimer -= dt * speedUp;
	if (battleTimer > 0.f)
	{
		for (auto champ : championPool.GetUseList())
		{
			champ->BattleUpdate(dt * 1.5f * speedUp);
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
		}
		else if (blueScore < redScore)
		{
			std::cout << "red win" << std::endl;
		}
		else if (blueScore == redScore)
		{
			std::cout << "draw" << std::endl;
		}
		speedUp = 1.f;
		ChangePhase(Phase::Result);
	}
}

void SceneGame::ResultPhase(float dt)
{

	if (INPUT_MGR.GetKeyDown(sf::Keyboard::Enter) ||
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
		if (blueScore > redScore)
		{
			TEAM_MGR.DoPerfectWin();
			TEAM_MGR.EarnMoney(150);
		}
		else if (blueScore < redScore)
		{
			TEAM_MGR.DoLose();
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
		lineupText1->sortLayer = 104;
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
		lineupText2->sortLayer = 104;
		lineupText2->sortOrder = 1;
		lineupText2->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText2->text.setCharacterSize(23);
		lineupText2->text.setFillColor(sf::Color::White);
		lineupText2->text.setString(stringTable->GetW("LineUp2"));
		lineupText2->SetPosition(620, 601);
		lineupText2->SetOrigin(Origins::MC);
		lineupText2->SetActive(false);

		TextGo* lineupText3 = (TextGo*)AddGo(new TextGo("Line Up Text3"));
		lineupText3->sortLayer = 104;
		lineupText3->sortOrder = 1;
		lineupText3->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText3->text.setCharacterSize(17);
		lineupText3->text.setFillColor(sf::Color::White);
		lineupText3->text.setString(stringTable->GetW("LineUp3"));
		lineupText3->SetPosition(188, 595);
		lineupText3->SetOrigin(Origins::MC);
		lineupText3->SetActive(false);

		TextGo* lineupText4 = (TextGo*)AddGo(new TextGo("Line Up Text4"));
		lineupText4->sortLayer = 104;
		lineupText4->sortOrder = 1;
		lineupText4->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText4->text.setCharacterSize(17);
		lineupText4->text.setFillColor(sf::Color::White);
		lineupText4->text.setString(stringTable->GetW("LineUp4"));
		lineupText4->SetPosition(284, 198);
		lineupText4->SetOrigin(Origins::MC);
		lineupText4->SetActive(false);

		TextGo* lineupText5 = (TextGo*)AddGo(new TextGo("Line Up Text5"));
		lineupText5->sortLayer = 104;
		lineupText5->sortOrder = 1;
		lineupText5->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText5->text.setCharacterSize(17);
		lineupText5->text.setFillColor(sf::Color::White);
		lineupText5->text.setString(stringTable->GetW("LineUp5"));
		lineupText5->SetPosition(681, 198);
		lineupText5->SetOrigin(Origins::MC);
		lineupText5->SetActive(false);

		TextGo* lineupText6 = (TextGo*)AddGo(new TextGo("Enemy Team Name Text"));
		lineupText6->sortLayer = 104;
		lineupText6->sortOrder = 1;
		lineupText6->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText6->text.setCharacterSize(17);
		lineupText6->text.setFillColor(sf::Color::White);
		lineupText6->text.setString(L"Cheerful Cows");
		lineupText6->SetPosition(988, 198);
		lineupText6->SetOrigin(Origins::MC);
		lineupText6->SetActive(false);

		TextGo* lineupText7 = (TextGo*)AddGo(new TextGo("Player Member Text1"));
		lineupText7->sortLayer = 105;
		lineupText7->sortOrder = 1;
		lineupText7->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText7->text.setCharacterSize(17);
		lineupText7->text.setFillColor(sf::Color::White);
		lineupText7->text.setString(L"내 선수1");
		lineupText7->SetPosition(165, 245);
		lineupText7->SetOrigin(Origins::MC);
		lineupText7->SetActive(false);

		TextGo* lineupText8 = (TextGo*)AddGo(new TextGo("Player Member Text2"));
		lineupText8->sortLayer = 105;
		lineupText8->sortOrder = 1;
		lineupText8->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText8->text.setCharacterSize(17);
		lineupText8->text.setFillColor(sf::Color::White);
		lineupText8->text.setString(L"내 선수2");
		lineupText8->SetPosition(338, 245);
		lineupText8->SetOrigin(Origins::MC);
		lineupText8->SetActive(false);

		TextGo* lineupText9 = (TextGo*)AddGo(new TextGo("Player Member Text3"));
		lineupText9->sortLayer = 105;
		lineupText9->sortOrder = 1;
		lineupText9->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText9->text.setCharacterSize(17);
		lineupText9->text.setFillColor(sf::Color::White);
		lineupText9->text.setString(L"내 선수3");
		lineupText9->SetPosition(165, 414);
		lineupText9->SetOrigin(Origins::MC);
		lineupText9->SetActive(false);

		TextGo* lineupText10 = (TextGo*)AddGo(new TextGo("Player Member Text4"));
		lineupText10->sortLayer = 105;
		lineupText10->sortOrder = 1;
		lineupText10->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText10->text.setCharacterSize(17);
		lineupText10->text.setFillColor(sf::Color::White);
		lineupText10->text.setString(L"내 선수4");
		lineupText10->SetPosition(338, 414);
		lineupText10->SetOrigin(Origins::MC);
		lineupText10->SetActive(false);

		TextGo* lineupText11 = (TextGo*)AddGo(new TextGo("Player Member Text5"));
		lineupText11->sortLayer = 105;
		lineupText11->sortOrder = 1;
		lineupText11->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText11->text.setCharacterSize(17);
		lineupText11->text.setFillColor(sf::Color::White);
		lineupText11->text.setString(L"내 후보1");
		lineupText11->SetPosition(646, 245);
		lineupText11->SetOrigin(Origins::MC);
		lineupText11->SetActive(false);

		TextGo* lineupText12 = (TextGo*)AddGo(new TextGo("Player Member Text6"));
		lineupText12->sortLayer = 105;
		lineupText12->sortOrder = 1;
		lineupText12->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText12->text.setCharacterSize(17);
		lineupText12->text.setFillColor(sf::Color::White);
		lineupText12->text.setString(L"내 후보2");
		lineupText12->SetPosition(646, 414);
		lineupText12->SetOrigin(Origins::MC);
		lineupText12->SetActive(false);

		TextGo* lineupText13 = (TextGo*)AddGo(new TextGo("Enemy Member Text1"));
		lineupText13->sortLayer = 105;
		lineupText13->sortOrder = 1;
		lineupText13->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText13->text.setCharacterSize(17);
		lineupText13->text.setFillColor(sf::Color::White);
		lineupText13->text.setString(L"적 선수1");
		lineupText13->SetPosition(865, 245);
		lineupText13->SetOrigin(Origins::MC);
		lineupText13->SetActive(false);

		TextGo* lineupText14 = (TextGo*)AddGo(new TextGo("Enemy Member Text2"));
		lineupText14->sortLayer = 105;
		lineupText14->sortOrder = 1;
		lineupText14->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText14->text.setCharacterSize(17);
		lineupText14->text.setFillColor(sf::Color::White);
		lineupText14->text.setString(L"적 선수2");
		lineupText14->SetPosition(1038, 245);
		lineupText14->SetOrigin(Origins::MC);
		lineupText14->SetActive(false);

		TextGo* lineupText15 = (TextGo*)AddGo(new TextGo("Enemy Member Text3"));
		lineupText15->sortLayer = 105;
		lineupText15->sortOrder = 1;
		lineupText15->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		lineupText15->text.setCharacterSize(17);
		lineupText15->text.setFillColor(sf::Color::White);
		lineupText15->text.setString(L"적 선수3");
		lineupText15->SetPosition(865, 414);
		lineupText15->SetOrigin(Origins::MC);
		lineupText15->SetActive(false);

		TextGo* lineupText16 = (TextGo*)AddGo(new TextGo("Enemy Member Text4"));
		lineupText16->sortLayer = 105;
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
		pleyerTeamTitle1->sortLayer = 103;
		pleyerTeamTitle1->sortOrder = 1;
		pleyerTeamTitle1->SetPosition(290, 200);
		pleyerTeamTitle1->SetOrigin(Origins::MC);
		pleyerTeamTitle1->SetActive(false); // 왼쪽 테두리

		SpriteGo* enemyTeamTitle1 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_team_title_bg_2.png", "Enemy Team Title1"));
		enemyTeamTitle1->sprite.setScale(2, 2);
		enemyTeamTitle1->sortLayer = 103;
		enemyTeamTitle1->sortOrder = 1;
		enemyTeamTitle1->SetPosition(990, 200);
		enemyTeamTitle1->SetOrigin(Origins::MC);
		enemyTeamTitle1->SetActive(false);

		SpriteGo* subTitle = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/subTitle.png", "Sub_Title"));
		subTitle->sprite.setScale(1.9, 1.4);
		subTitle->sortLayer = 103;
		subTitle->sortOrder = 1;
		subTitle->SetPosition(684, 200);
		subTitle->SetOrigin(Origins::MC);
		subTitle->SetActive(false);

		// 리소스 있어서 추가는 했는데 안쓸듯
		SpriteGo* pleyerTeamTitle2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/Black_0.png", "Pleyer Team Title2"));
		pleyerTeamTitle2->sprite.setScale(2, 2);
		pleyerTeamTitle2->sortLayer = 103;
		pleyerTeamTitle2->sortOrder = 1;
		pleyerTeamTitle2->SetPosition(680, 200);
		pleyerTeamTitle2->SetOrigin(Origins::MC);
		pleyerTeamTitle2->SetActive(false);

		SpriteGo* enemyTeamTitle2 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/lineup_ui_team_title_bg_3.png", "Enemy Team Title2"));
		enemyTeamTitle2->sprite.setScale(2, 2);
		enemyTeamTitle2->sortLayer = 103;
		enemyTeamTitle2->sortOrder = 1;
		enemyTeamTitle2->SetPosition(0, 0);
		enemyTeamTitle2->SetOrigin(Origins::MC);
		enemyTeamTitle2->SetActive(false);

		SpriteGo* draftSlotBlueWhite = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0_white.png", "Draft_Slot_White"));
		draftSlotBlueWhite->sprite.setScale(2, 2);
		draftSlotBlueWhite->sortLayer = 104;
		draftSlotBlueWhite->sortOrder = 1;
		draftSlotBlueWhite->SetPosition(0, 0);
		draftSlotBlueWhite->SetOrigin(Origins::MC);
		draftSlotBlueWhite->SetActive(false);

		SpriteGo* draftSlotRedWhite = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_1_white.png", "Draft_Slot_Red_White"));
		draftSlotRedWhite->sprite.setScale(2, 2);
		draftSlotRedWhite->sortLayer = 104;
		draftSlotRedWhite->sortOrder = 1;
		draftSlotRedWhite->SetPosition(0, 0);
		draftSlotRedWhite->SetOrigin(Origins::MC);
		draftSlotRedWhite->SetActive(false);

		SpriteGo* draftSlotBlueClicked = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0_clicked.png", "Draft_Slot_Blue_Clicked"));
		draftSlotBlueClicked->sprite.setScale(2, 2);
		draftSlotBlueClicked->sortLayer = 104;
		draftSlotBlueClicked->sortOrder = 1;
		draftSlotBlueClicked->SetPosition(0, 0);
		draftSlotBlueClicked->SetOrigin(Origins::MC);
		draftSlotBlueClicked->SetActive(false);

		SpriteGo* draftSlotRedClicked = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_1_clicked.png", "Draft_Slot_Red_Clicked"));
		draftSlotRedClicked->sprite.setScale(2, 2);
		draftSlotRedClicked->sortLayer = 104;
		draftSlotRedClicked->sortOrder = 1;
		draftSlotRedClicked->SetPosition(0, 0);
		draftSlotRedClicked->SetOrigin(Origins::MC);
		draftSlotRedClicked->SetActive(false);
	}
	// 드래프트 슬롯
	{
		UiButton* draftSlot7 = (UiButton*)AddGo(new UiButton("graphics/LeagueSystem/Lineup/draft_slot_1.png", "Enemy Draft Slot1"));
		draftSlot7->sprite.setScale(2, 2);
		draftSlot7->sortLayer = 103;
		draftSlot7->sortOrder = 1;
		draftSlot7->SetPosition(904, 309);
		draftSlot7->SetOrigin(Origins::MC);
		draftSlot7->SetActive(false);

		UiButton* draftSlot8 = (UiButton*)AddGo(new UiButton("graphics/LeagueSystem/Lineup/draft_slot_1.png", "Enemy Draft Slot2"));
		draftSlot8->sprite.setScale(2, 2);
		draftSlot8->sortLayer = 103;
		draftSlot8->sortOrder = 1;
		draftSlot8->SetPosition(1076, 309);
		draftSlot8->SetOrigin(Origins::MC);
		draftSlot8->SetActive(false);

		UiButton* draftSlot9 = (UiButton*)AddGo(new UiButton("graphics/LeagueSystem/Lineup/draft_slot_1.png", "Enemy Draft Slot3"));
		draftSlot9->sprite.setScale(2, 2);
		draftSlot9->sortLayer = 103;
		draftSlot9->sortOrder = 1;
		draftSlot9->SetPosition(904, 477);
		draftSlot9->SetOrigin(Origins::MC);
		draftSlot9->SetActive(false);

		UiButton* draftSlot10 = (UiButton*)AddGo(new UiButton("graphics/LeagueSystem/Lineup/draft_slot_1.png", "Enemy Draft Slot4"));
		draftSlot10->sprite.setScale(2, 2);
		draftSlot10->sortLayer = 103;
		draftSlot10->sortOrder = 1;
		draftSlot10->SetPosition(1076, 477);
		draftSlot10->SetOrigin(Origins::MC);
		draftSlot10->SetActive(false);

		SpriteGo* Draft_Slot_Red_White = (SpriteGo*)FindGo("Draft_Slot_Red_White");

		draftSlot7->OnEnter = [this, draftSlot7, Draft_Slot_Red_White]() {
			Draft_Slot_Red_White->SetActive(true);
			Draft_Slot_Red_White->SetPosition(draftSlot7->GetPosition());
			};
		draftSlot7->OnExit = [this, Draft_Slot_Red_White]() {
			Draft_Slot_Red_White->SetActive(false);
			};
		draftSlot8->OnEnter = [this, draftSlot8, Draft_Slot_Red_White]() {
			Draft_Slot_Red_White->SetActive(true);
			Draft_Slot_Red_White->SetPosition(draftSlot8->GetPosition());
			};
		draftSlot8->OnExit = [this, Draft_Slot_Red_White]() {
			Draft_Slot_Red_White->SetActive(false);
			};
		draftSlot9->OnEnter = [this, draftSlot9, Draft_Slot_Red_White]() {
			Draft_Slot_Red_White->SetActive(true);
			Draft_Slot_Red_White->SetPosition(draftSlot9->GetPosition());
			};
		draftSlot9->OnExit = [this, Draft_Slot_Red_White]() {
			Draft_Slot_Red_White->SetActive(false);
			};
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
	lineupSwap->sortLayer = 102;
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
	draftArrow2->sortLayer = 103;
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
		SpriteGo* banBg22 = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Banpick/banpick_bg.png", "Ban Bg 2:2"));
		//banBg22->sprite.setScale(2, 2);
		banBg22->sortLayer = 101;
		banBg22->sortOrder = 1;
		banBg22->SetPosition(0, 0);
		banBg22->SetOrigin(Origins::TL);
		banBg22->SetActive(false);

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

		SpriteGo* swapUibg = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Swap/swap_ui_bg.png", "Swap_bg"));
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
		text->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f, 65.f);
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

void SceneGame::UiEnter()
{
	TextGo* text;
	text = (TextGo*)FindGo("BanText");
	text->SetPosition(0, FRAMEWORK.GetWindowSize().y / 2);
	text->SetActive(false);
	text = (TextGo*)FindGo("PickText");
	text->SetPosition(0, FRAMEWORK.GetWindowSize().y / 2);
	text->SetActive(false);

	grayScreen->SetActive(false);
	textDuration = 1.2f;
	BanPickTrue(false);
	SpriteGo* spr = (SpriteGo*)FindGo("1");
	spr->SetActive(false);
	for (int i = 0; i < 6; i++)
	{
		swapSlot[i]->SetActive(true);
	}
	for (int i = 0; i < 2 - (int)mode; i++)
	{
		swapSlot[3 - i]->SetActive(false);
	}
	if (TEAM_MGR.GetPlayerNum() < (int)mode + 4)
	{
		for (int i = 0; i < (int)mode + 4 - TEAM_MGR.GetPlayerNum(); i++)
		{
			swapSlot[5 - i]->SetActive(false);
		}
	}
}

void SceneGame::BanPickInit()
{
	pickSlot = std::vector<UiButton*>(4);
	enemySlot = std::vector<UiButton*>(4);
	pickDoingSlot = std::vector<SpriteGo*>(4);
	enemyPickDoingSlot = std::vector<SpriteGo*>(4);

	SpriteGo* spr;
	TextGo* text;

	championSlotName = std::vector<TextGo*>(champCount);
	banPickText = std::vector<TextGo*>(banPickTextCount); // 일단 3:3기준이라 카운트가 6임

	for (int i = 0; i < banPickTextCount; i++)
	{
		std::stringstream ss;
		ss << "BanPickText" << i + 1;

		banPickText[i] = (TextGo*)AddGo(new TextGo(ss.str()));
		banPickText[i]->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		banPickText[i]->sortLayer = 106;
		banPickText[i]->sortOrder = 1;
		banPickText[i]->text.setCharacterSize(30);
		banPickText[i]->text.setFillColor(sf::Color::White);

		auto stringTable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
		banPickText[i]->text.setString(stringTable->GetW(ss.str()));

		banPickText[i]->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f, 125);
		banPickText[i]->SetOrigin(Origins::MC);
		banPickText[i]->SetActive(false);
	}

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ssPickSlot;
		ssPickSlot << "BanPick Slot" << i + 1;
		pickSlot[i] = (UiButton*)AddGo(new UiButton("graphics/LeagueSystem/Banpick/pickSlot.png", ssPickSlot.str()));
		pickSlot[i]->sprite.setScale(2, 2);
		pickSlot[i]->SetOrigin(Origins::MC);
		pickSlot[i]->sortLayer = 103;
		pickSlot[i]->sortOrder = 1;
		pickSlot[i]->SetPosition(80, 157 + (i * 158));
		pickSlot[i]->SetActive(false);

		ssPickSlot.str("");
		ssPickSlot << "EnemySlot" << i + 1;
		enemySlot[i] = (UiButton*)AddGo(new UiButton("graphics/LeagueSystem/Lineup/draft_slot_1.png", ssPickSlot.str()));
		enemySlot[i]->sprite.setScale(2, 2);
		enemySlot[i]->SetOrigin(Origins::MC);
		enemySlot[i]->sortLayer = 103;
		enemySlot[i]->sortOrder = 1;
		enemySlot[i]->SetPosition(1200, 157 + (i * 158));
		enemySlot[i]->SetActive(false);

		ssPickSlot.str("");
		ssPickSlot << "PickDoingSlot" << i + 1;
		pickDoingSlot[i] = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_0_clicked.png", ssPickSlot.str()));
		pickDoingSlot[i]->sprite.setScale(2, 2);
		pickDoingSlot[i]->SetOrigin(Origins::MC);
		pickDoingSlot[i]->sortLayer = 104;
		pickDoingSlot[i]->sortOrder = 1;
		pickDoingSlot[i]->SetPosition(80, 157 + (i * 158));
		pickDoingSlot[i]->SetActive(false);

		ssPickSlot.str("");
		ssPickSlot << "EnemyPickDoingSlot" << i + 1;
		enemyPickDoingSlot[i] = (SpriteGo*)AddGo(new SpriteGo("graphics/LeagueSystem/Lineup/draft_slot_1_clicked.png", ssPickSlot.str()));
		enemyPickDoingSlot[i]->sprite.setScale(2, 2);
		enemyPickDoingSlot[i]->SetOrigin(Origins::MC);
		enemyPickDoingSlot[i]->sortLayer = 103;
		enemyPickDoingSlot[i]->sortOrder = 1;
		enemyPickDoingSlot[i]->SetPosition(1200, 157 + (i * 158));
		enemyPickDoingSlot[i]->SetActive(false);

		std::stringstream ss;
		ss << "BanPickPlayerName" << i;
		text = (TextGo*)AddGo(new TextGo(ss.str()));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(19);
		text->sortLayer = 105;

		ss.str("");
		ss << "BanPickPlayerAtk" << i;
		text = (TextGo*)AddGo(new TextGo(ss.str()));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(19);
		text->sortLayer = 105;

		ss.str("");
		ss << "BanPickPlayerDef" << i;
		text = (TextGo*)AddGo(new TextGo(ss.str()));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(19);
		text->sortLayer = 105;

		for (int j = 0; j < 4; j++)
		{
			ss.str("");
			ss << "BanPickPlayer" << i << "Champ" << j;
			spr = (SpriteGo*)AddGo(new SpriteGo("graphics/UiFix/character_icons_0.png", ss.str()));
			spr->SetSize(1.3, 1.3);
			spr->sortLayer = 106;

			ss << "Line";
			spr = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Sprite/champion&player_slot_1.png", ss.str()));
			spr->SetSize(1.3, 1.3);
			spr->sortLayer = 107;

			ss << "Back";
			spr = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Sprite/champion_face_bg_0.png", ss.str()));
			spr->SetSize(1.3, 1.3);
			spr->sortLayer = 105;

			ss << "Level";
			text = (TextGo*)AddGo(new TextGo(ss.str()));
			text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
			text->text.setCharacterSize(10);
			text->sortLayer = 108;
		}

		ss.str("");
		ss << "BanPickEnemyName" << i;
		text = (TextGo*)AddGo(new TextGo(ss.str()));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(19);
		text->sortLayer = 105;

		ss.str("");
		ss << "BanPickEnemyAtk" << i;
		text = (TextGo*)AddGo(new TextGo(ss.str()));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(19);
		text->sortLayer = 105;

		ss.str("");
		ss << "BanPickEnemyDef" << i;
		text = (TextGo*)AddGo(new TextGo(ss.str()));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(19);
		text->sortLayer = 105;

		for (int j = 0; j < 4; j++)
		{
			ss.str("");
			ss << "BanPickEnemy" << i << "Champ" << j;
			spr = (SpriteGo*)AddGo(new SpriteGo("graphics/UiFix/character_icons_0.png", ss.str()));
			spr->SetSize(1.3, 1.3);
			spr->sortLayer = 106;

			ss << "Line";
			spr = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Sprite/champion&player_slot_1.png", ss.str()));
			spr->SetSize(1.3, 1.3);
			spr->sortLayer = 107;

			ss << "Back";
			spr = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Sprite/champion_face_bg_0.png", ss.str()));
			spr->SetSize(1.3, 1.3);
			spr->sortLayer = 105;

			ss << "Level";
			text = (TextGo*)AddGo(new TextGo(ss.str()));
			text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
			text->text.setCharacterSize(8);
			text->sortLayer = 108;
		}
	}

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

		std::stringstream iconTextureRoute;
		iconTextureRoute << "graphics/UiFix/character_icons_" << i << ".png";

		std::stringstream pickTexture;
		pickTexture << "graphics/UiFix/character_icons_" << i << ".png";

		ss << "Image";
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

		// 작업중
		ss.str("");
		ss << "PickImage" << i + 1;
		AddGo(new SpriteGo(pickTexture.str(), ss.str()));
		SpriteGo* pick = (SpriteGo*)FindGo(ss.str());
		pick->SetPosition(-100, -100);
		pick->SetOrigin(Origins::MC);
		pick->sortLayer = 104;
		pick->sortOrder = 1;
		pick->SetSize(1.4, 1.4);
		pick->sprite.setColor(sf::Color::White);
		pick->SetActive(false);

		championSlot[i]->OnEnter = [this, i]() {
			FindGo("BanSheet")->SetActive(true);
			banSheet->SetPosition(championSlot[i]->GetPosition());
			
			selectCheck = true;
			};
		championSlot[i]->OnExit = [this]() {
			// if문 앞에 있으면
			FindGo("BanSheet")->SetActive(false);
			if (selectCheck)
			{
				return;
			}
			selectCheck = false;

			};
		championSlot[i]->OnClick = [this, i]() {
			if (grayScreen->GetActive())
			{
				return;
			}

			std::stringstream ss;
			ss << "Champion Slot" << i + 1 << "Image";
			if (currentPhase == Phase::Ban)
			{
				// 중복벤 방지
				for (int j = 0; j < banChamps.size(); j++)
				{
					if (banChamps[j] == i)
					{
						return;
					}
				}
				
				if (aiBanTimer >= 0 && currentTurn == Turn::Enemy)
				{
					std::cout << "상대방의 턴입니다." << std::endl;
					return;
				}

				if (currentTurn == Turn::Player && banPickCheck)
				{
					FindGo("TurnArrowRed")->SetActive(true);
					FindGo("BanRedLine")->SetActive(true);

					FindGo("TurnArrowBlue")->SetActive(false);
					FindGo("BanBlueLine")->SetActive(false);

					banPickText[banPickTextClickCount]->SetActive(false);
					banPickTextClickCount++;
					banPickText[banPickTextClickCount]->SetActive(true);
					
					turnArrowRed->SetPosition(1048, 125);
					banPickCheck = false;
				}
				else if (currentTurn == Turn::Enemy && !banPickCheck)
				{

					FindGo("TurnArrowBlue")->SetActive(true);
					FindGo("BanBlueLine")->SetActive(true);

					std::cout << "banPickTextClickCount" << banPickTextClickCount << std::endl;
					if (banPickTextClickCount >= 8)
					{
						FindGo("TurnArrowBlue")->SetActive(false);
						FindGo("BanBlueLine")->SetActive(false);
					}

					FindGo("TurnArrowRed")->SetActive(false);
					FindGo("BanRedLine")->SetActive(false);

					pickDoingSlot[pickDoingSlotCount]->SetActive(true);

					banPickText[banPickTextClickCount]->SetActive(false);
					banPickTextClickCount++;
					banPickText[banPickTextClickCount]->SetActive(true);

					turnArrowBlue->SetPosition(232, 125);
					banPickCheck = true;
				}
				
				aiBanTimer = 2.f;

				banEffectAnimation[banCount].Play("Idle");
				banEffectAnimation[banCount].GetTarget()->setScale(1.f, 1.f);
				banEffectSheet[banCount]->SetOrigin(Origins::MC);

				// 밴 이펙트 애니메이션
				banEffectSheet[banCount]->SetActive(true);
				banEffectSheet[banCount]->SetPosition(championSlot[i]->GetPosition());

				SpriteGo* spr = (SpriteGo*)FindGo(ss.str());
				spr->sprite.setColor(sf::Color(40, 40, 40));

				banChamps[step] = i;
				banCount++;
				step++;
				ChangeTeam();
				ChangeTurn();
			}
			else if (currentPhase == Phase::Pick)
			{
				// 중복 방지
				for (int j = 0; j < banChamps.size(); j++)
				{
					if (banChamps[j] == i)
					{
						return;
					}
				}

				// 본인의 턴이 아닐때 클릭방지
				if (aiPickTimer >= 0 && currentTurn == Turn::Enemy)
				{
					std::cout << "상대방의 턴입니다." << std::endl;
					return;
				}

				if (currentTurn == Turn::Player && banPickCheck)
				{
					FindGo("TurnArrowRed")->SetActive(true);
					FindGo("BanRedLine")->SetActive(true);

					FindGo("TurnArrowBlue")->SetActive(false);
					FindGo("BanBlueLine")->SetActive(false);

					enemyPickDoingSlot[enemyPickDoingSlotCount]->SetActive(true);

					pickDoingSlot[pickDoingSlotCount]->SetActive(false);
					if (pickDoingSlotCount < 3)
					{
						pickDoingSlotCount++;
					}

					banPickText[banPickTextClickCount]->SetActive(false);
					banPickTextClickCount++;
					banPickText[banPickTextClickCount]->SetActive(true);
					banPickCheck = false;
				}
				else if (currentTurn == Turn::Enemy && !banPickCheck)
				{
					FindGo("TurnArrowBlue")->SetActive(true);
					FindGo("BanBlueLine")->SetActive(true);

					FindGo("TurnArrowRed")->SetActive(false);
					FindGo("BanRedLine")->SetActive(false);

					pickDoingSlot[pickDoingSlotCount]->SetActive(true);
					//pickDoingSlotCount++;

					enemyPickDoingSlot[enemyPickDoingSlotCount]->SetActive(false);
					enemyPickDoingSlotCount++;

					banPickText[banPickTextClickCount]->SetActive(false);
					banPickTextClickCount++;
					banPickText[banPickTextClickCount]->SetActive(true);

					banPickCheck = true;
				}

				aiPickTimer = 2.f;

				if (team == Team::Red)
				{
					championSlot[i]->sprite.setColor(sf::Color(250, 0, 0));
					ss.str("");
					ss << "PickImage" << i + 1;
					SpriteGo* enemyPickIcon = (SpriteGo*)FindGo(ss.str());
					if (pickEnemyCount < 3)
					{
						enemyPickIcon->SetPosition(1150, 137 + (pickEnemyCount * 158));
						pickEnemyCount++;
						std::cout << "적의 픽카운트: " << pickEnemyCount << std::endl;
					}
					enemyPickIcon->SetActive(true);
				}
				else
				{
					championSlot[i]->sprite.setColor(sf::Color(0, 0, 250));

					ss.str("");
					ss << "PickImage" << i + 1;
					SpriteGo* pickIcon = (SpriteGo*)FindGo(ss.str());
					if (pickCount < 3)
					{
						pickIcon->SetPosition(29, 137 + (pickCount * 158));
						pickCount++;
						std::cout << "나의 픽카운트: " << pickCount << std::endl;
					}
					pickIcon->SetActive(true);
				}
				banChamps[step] = i;
				ChampionPick(i, team);
				ChangeTeam();
				ChangeTurn();
			}
			};
	}

	for (int i = 0; i < champCount; i++)
	{
		std::stringstream ss;
		ss << "ChampionSlotName" << i + 1;
		text = (TextGo*)AddGo(new TextGo(ss.str()));

		text->sortLayer = 107;
		text->sortOrder = 1;
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(12);
		text->text.setFillColor(sf::Color::White);
		text->text.setString(CHAMPION_MGR.GetChampion(i)->charId);

		text->SetPosition(championSlot[i]->GetPosition().x, championSlot[i]->GetPosition().y + 30);

		text->SetOrigin(Origins::MC);
		text->SetActive(true);

	}

}

void SceneGame::BanPickTrue(bool on)
{
	SpriteGo* spr;
	banSheet->SetActive(on);
	banSheetBlueTeam->SetActive(on);
	banSheetRedTeam->SetActive(on);

	FindGo("TurnArrowBlue")->SetActive(on);
	turnArrowBlue->SetPosition(232, 125);

	banPickText[banPickTextClickCount]->SetActive(on);

	spr = (SpriteGo*)FindGo("Ban Bg 2:2");
	spr->SetActive(on);

	for (int i = 0; i < champCount; i++)
	{
		std::stringstream ss;
		championSlot[i]->SetActive(on);
		FindGo("BanSheet")->SetActive(on);

		ss << "Champion Slot" << i + 1 << "Image";
		FindGo(ss.str())->SetActive(on);

		ss.str("");
		ss << "PickImage" << i + 1;
		FindGo(ss.str())->SetActive(on);
	}

	for (int i = 0; i < pickSlotCount; i++)
	{
		BanPickPlayersOn(i, on);
	}
	for (int i = pickSlotCount; i < 4; i++)
	{
		BanPickPlayersOn(i, false);
	}
}

void SceneGame::LineUpInit()
{

	for (int i = 0; i < 4; i++)
	{
		pickSlot[i]->SetActive(true);
	}
	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "Swap Slot" << i + 1;
		swapSlot[i] = (UiButton*)AddGo(new UiButton("graphics/LeagueSystem/Lineup/draft_slot_0.png", ss.str()));
		swapSlot[i]->sprite.setScale(2, 2);
		swapSlot[i]->SetOrigin(Origins::MC);
		swapSlot[i]->sortLayer = 103;
		swapSlot[i]->sortOrder = 1;
		swapSlot[i]->SetActive(true);

		if (i < 4)
		{
			swapSlot[i]->SetPosition(204 + ((i % 2) * 171), 309 + ((i / 2) * 168));
		}
		else
		{
			swapSlot[i]->SetPosition(684, 309 + ((i - 4) * 168));
		}

		swapSlot[i]->OnEnter = [this, i]() {
			SpriteGo* draftSlotBlueWhite = (SpriteGo*)FindGo("Draft_Slot_White");
			draftSlotBlueWhite->SetActive(true);
			draftSlotBlueWhite->SetPosition(swapSlot[i]->GetPosition());
			};
		swapSlot[i]->OnExit = [this]() {
			FindGo("Draft_Slot_White")->SetActive(false);
			};
		swapSlot[i]->OnClick = [this, i]() {
			SpriteGo* draftSlotBlueClicked = (SpriteGo*)FindGo("Draft_Slot_Blue_Clicked");

			// 스왑단계 클릭 중복

			if (!isSwapCheck1 && !isClickBlocker)
			{
				FindGo("Line Up Swap")->SetActive(true);
				FindGo("Draft Arrow2")->SetActive(true);
				draftSlotBlueClicked->SetActive(true);
				draftSlotBlueClicked->SetPosition(swapSlot[i]->GetPosition());

				selectedButton = swapSlot[i];
				isSwapCheck1 = true;
				if (i > (int)mode + 1)
				{
					swapIndex = i - (2 - (int)mode);
				}
				else
				{
					swapIndex = i;
				}


				std::cout << "챔피언 슬롯 클릭" << i << std::endl;
				return;
			}
			else if (isSwapCheck1 && selectedButton != nullptr)
			{
				FindGo("Line Up Swap")->SetActive(false);
				FindGo("Draft Arrow2")->SetActive(false);
				draftSlotBlueClicked->SetActive(false);

				isSwapCheck1 = false;
				selectedButton = nullptr;
				if (i > (int)mode + 1)
				{
					std::swap(playerInfo[i - (2 - (int)mode)], playerInfo[swapIndex]);
				}
				else
				{
					std::swap(playerInfo[i], playerInfo[swapIndex]);
				}
				SetLineUp();

				isClickBlocker = true;
				std::cout << "스왑 완료" << std::endl;
			}

			// 스왑체크1이 처음에 트루라 작동, 컴플릿이 처음에 트루라 작동
			// 처음 클릭시 스왑컴플릿이 펄스로 바뀜
			};
	}

	SpriteGo* spr;
	TextGo* text;
	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "PlayerAtk" << i;
		text = (TextGo*)AddGo(new TextGo(ss.str()));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(19);
		text->sortLayer = 105;

		ss.str("");
		ss << "PlayerDef" << i;
		text = (TextGo*)AddGo(new TextGo(ss.str()));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(19);
		text->sortLayer = 106;

		for (int j = 0; j < 4; j++)
		{
			ss.str("");
			ss << "Player" << i << "Champ" << j;
			spr = (SpriteGo*)AddGo(new SpriteGo("graphics/UiFix/character_icons_0.png", ss.str()));
			if (j == 0)
			{
				spr->SetSize(2, 2);
			}
			else
			{
				spr->SetSize(1.3, 1.3);
			}
			spr->sortLayer = 106;

			ss << "Line";
			spr = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Sprite/champion&player_slot_1.png", ss.str()));
			if (j == 0)
			{
				spr->SetSize(2, 2);
			}
			else
			{
				spr->SetSize(1.3, 1.3);
			}
			spr->sortLayer = 107;

			ss << "Level";
			text = (TextGo*)AddGo(new TextGo(ss.str()));
			text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
			if (j == 0)
			{
				text->text.setCharacterSize(14);
			}
			else
			{
				text->text.setCharacterSize(8);
			}
			text->sortLayer = 108;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "EnemyAtk" << i;
		text = (TextGo*)AddGo(new TextGo(ss.str()));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(19);
		text->sortLayer = 105;

		ss.str("");
		ss << "EnemyDef" << i;
		text = (TextGo*)AddGo(new TextGo(ss.str()));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setCharacterSize(19);
		text->sortLayer = 105;

		for (int j = 0; j < 4; j++)
		{
			ss.str("");
			ss << "Enemy" << i << "Champ" << j;
			spr = (SpriteGo*)AddGo(new SpriteGo("graphics/UiFix/character_icons_0.png", ss.str()));
			if (j == 0)
			{
				spr->SetSize(2, 2);
			}
			else
			{
				spr->SetSize(1.3, 1.3);
			}
			spr->sortLayer = 106;

			ss << "Line";
			spr = (SpriteGo*)AddGo(new SpriteGo("graphics/Origin/Sprite/champion&player_slot_1.png", ss.str()));
			if (j == 0)
			{
				spr->SetSize(2, 2);
			}
			else
			{
				spr->SetSize(1.3, 1.3);
			}
			spr->sortLayer = 107;

			ss << "Level";
			text = (TextGo*)AddGo(new TextGo(ss.str()));
			text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
			if (j == 0)
			{
				text->text.setCharacterSize(14);
			}
			else
			{
				text->text.setCharacterSize(8);
			}
			text->sortLayer = 108;
		}
	}
}

void SceneGame::LineUpTrue()
{
	FindGo("Line Up")->SetActive(true);
	FindGo("Next Button")->SetActive(true);
	//FindGo("keyboard Buttons_D")->SetActive(true);
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

	if (swapChampCount >= 5)
	{
		FindGo("Player Member Text5")->SetActive(true);
		FindGo("Player Member Text6")->SetActive(true);
	}
	else if (swapChampCount >= 4)
	{
		FindGo("Player Member Text5")->SetActive(true);
	}
	else
	{
		FindGo("Player Member Text5")->SetActive(false);
		FindGo("Player Member Text6")->SetActive(false);
	}

	FindGo("Enemy Member Text1")->SetActive(true);
	FindGo("Enemy Member Text2")->SetActive(true);
	FindGo("Enemy Member Text3")->SetActive(true);

	FindGo("Pleyer Team Title1")->SetActive(true);
	FindGo("Enemy Team Title1")->SetActive(true);
	FindGo("Sub_Title")->SetActive(true);

	FindGo("Enemy Draft Slot1")->SetActive(true);
	FindGo("Enemy Draft Slot2")->SetActive(true);
	FindGo("Enemy Draft Slot3")->SetActive(true);

	FindGo("Draft Arrow1")->SetActive(true);

	// FindGo("Player Member Text4")->SetActive(true); // 출전, 후보 선수 1명씩 제외
	// FindGo("Player Draft Slot4")->SetActive(true);
	// FindGo("Player Draft Slot6")->SetActive(true);
	// FindGo("Enemy Member Text4")->SetActive(true); // 기본 3:3이라 적 선수 1명 제외
	// FindGo("Enemy Draft Slot4")->SetActive(true);

	playerInfo = TEAM_MGR.GetPlayerInfo();
	enemyInfo = TEAM_MGR.GetAiTeam(TEAM_MGR.GetAiTeamFightNum());
	SetLineUp();
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
	FindGo("Sub_Title")->SetActive(false);

	FindGo("Enemy Draft Slot1")->SetActive(false);
	FindGo("Enemy Draft Slot2")->SetActive(false);
	FindGo("Enemy Draft Slot3")->SetActive(false);
	FindGo("Enemy Draft Slot4")->SetActive(false);
	//FindGo("Enemy Draft Slot5")->SetActive(false);

	FindGo("Draft Arrow1")->SetActive(false);
	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "PlayerAtk" << i;
		FindGo(ss.str())->SetActive(false);
		ss.str("");
		ss << "PlayerDef" << i;
		FindGo(ss.str())->SetActive(false);

		for (int j = 0; j < 4; j++)
		{
			ss.str("");
			ss << "Player" << i << "Champ" << j;
			FindGo(ss.str())->SetActive(false);
			ss << "Line";
			FindGo(ss.str())->SetActive(false);
			ss << "Level";
			FindGo(ss.str())->SetActive(false);
		}
	}
	for (size_t i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "EnemyAtk" << i;
		FindGo(ss.str())->SetActive(false);
		ss.str("");
		ss << "EnemyDef" << i;
		FindGo(ss.str())->SetActive(false);
		for (int j = 0; j < 4; j++)
		{
			ss.str("");
			ss << "Enemy" << i << "Champ" << j;
			FindGo(ss.str())->SetActive(false);
			ss << "Line";
			FindGo(ss.str())->SetActive(false);
			ss << "Level";
			FindGo(ss.str())->SetActive(false);
		}
	}
}

void SceneGame::BanPickToBattleFalse()
{
	for (int i = 0; i < 4; i++)
	{
		pickDoingSlot[i]->SetActive(false);
	}

	for (int i = 0; i < 6; i++)
	{
		banEffectSheet[i]->SetActive(false);
	}

	SpriteGo* spr;
	banSheet->SetActive(false);
	banSheetBlueTeam->SetActive(false);
	banSheetRedTeam->SetActive(false);

	FindGo("TurnArrowRed")->SetActive(false);
	FindGo("BanRedLine")->SetActive(false);
	FindGo("TurnArrowBlue")->SetActive(false);
	FindGo("BanBlueLine")->SetActive(false);
	banPickText[banPickTextClickCount]->SetActive(false);

	for (int i = 0; i < championSlot.size(); i++)
	{
		championSlot[i]->SetActive(false);
		std::stringstream ss;
		ss << "Champion Slot" << i + 1 << "Image";
		FindGo(ss.str())->SetActive(false);
	}

	spr = (SpriteGo*)FindGo("Ban Bg 2:2");
	spr->SetActive(false);
	spr = (SpriteGo*)FindGo("1");
	//spr->sortLayer = 101;
	spr->SetActive(true);
}

void SceneGame::BanPickPlayersOn(int index, bool on)
{
	int i = index;
	pickSlot[i]->SetActive(on);
	enemySlot[i]->SetActive(on);

	std::stringstream ss;
	ss << "BanPickPlayerName" << i;
	FindGo(ss.str())->SetActive(on);
	ss.str("");
	ss << "BanPickPlayerAtk" << i;
	FindGo(ss.str())->SetActive(on);
	ss.str("");
	ss << "BanPickPlayerDef" << i;
	FindGo(ss.str())->SetActive(on);
	for (int j = 0; j < 4; j++)
	{
		ss.str("");
		ss << "BanPickPlayer" << i << "Champ" << j;
		FindGo(ss.str())->SetActive(on);
		ss << "Line";
		FindGo(ss.str())->SetActive(on);
		ss << "Back";
		FindGo(ss.str())->SetActive(on);
		ss << "Level";
		FindGo(ss.str())->SetActive(on);
	}
	ss.str("");
	ss << "BanPickEnemyName" << i;
	FindGo(ss.str())->SetActive(on);
	ss.str("");
	ss << "BanPickEnemyAtk" << i;
	FindGo(ss.str())->SetActive(on);
	ss.str("");
	ss << "BanPickEnemyDef" << i;
	FindGo(ss.str())->SetActive(on);
	for (int j = 0; j < 4; j++)
	{
		ss.str("");
		ss << "BanPickEnemy" << i << "Champ" << j;
		FindGo(ss.str())->SetActive(on);
		ss << "Line";
		FindGo(ss.str())->SetActive(on);
		ss << "Back";
		FindGo(ss.str())->SetActive(on);
		ss << "Level";
		FindGo(ss.str())->SetActive(on);
	}

}

void SceneGame::SetBanPick()
{
	TextGo* text;
	SpriteGo* spr;
	for (int i = 0; i < pickSlotCount; i++)
	{
		std::stringstream ss;
		ss << "BanPickPlayerName" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);
		text->text.setString(playerInfo[i].name);
		text->SetOrigin(Origins::ML);
		text->SetPosition(pickSlot[i]->GetPosition().x - 70
			, pickSlot[i]->GetPosition().y - 65);

		ss.str("");
		ss << "BanPickPlayerAtk" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);
		text->text.setString(std::to_string(playerInfo[i].attack));
		text->SetOrigin(Origins::ML);
		text->SetPosition(pickSlot[i]->GetPosition().x - 50
			, pickSlot[i]->GetPosition().y + 20);

		ss.str("");
		ss << "BanPickPlayerDef" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setString(std::to_string(playerInfo[i].defence));
		text->SetOrigin(Origins::ML);
		text->SetPosition(pickSlot[i]->GetPosition().x + 30
			, pickSlot[i]->GetPosition().y + 20);
		text->SetActive(true);

		for (int j = 0; j < playerInfo[i].knownChamp; j++)
		{
			ss.str("");
			ss << "BanPickPlayer" << i << "Champ" << j;
			SpriteGo* icon = (SpriteGo*)FindGo(ss.str());
			icon->SetPosition(pickSlot[i]->GetPosition().x - 58 + (j * 38)
				, pickSlot[i]->GetPosition().y + 54);
			std::stringstream champ;
			champ << "graphics/UiFix/character_icons_" <<
				playerInfo[i].proficiencyCode[j] << ".png";
			icon->sprite.setTexture((*RESOURCE_MGR.GetTexture(champ.str())));
			sf::Vector2u texSize = icon->sprite.getTexture()->getSize();
			icon->sprite.setTextureRect({ std::max(0,(int)texSize.x / 2 - 12), 0,25,25 });
			icon->SetOrigin(Origins::MC);
			icon->SetActive(true);

			ss << "Line";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetPosition(icon->GetPosition());
			spr->SetOrigin(Origins::MC);
			spr->SetActive(true);

			ss << "Back";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetPosition(icon->GetPosition());
			spr->SetOrigin(Origins::MC);
			spr->SetActive(true);

			ss << "Level";
			text = (TextGo*)FindGo(ss.str());
			text->text.setString(std::to_string(playerInfo[i].proficiencyLevel[j]));
			text->SetOrigin(Origins::MC);
			text->SetPosition(icon->GetPosition().x + 8
				, icon->GetPosition().y - 13);
			text->SetActive(true);
		}

		ss.str("");
		ss << "BanPickEnemyName" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);
		text->text.setString(enemyInfo.player[i].name);
		text->SetOrigin(Origins::ML);
		text->SetPosition(enemySlot[i]->GetPosition().x - 70
			, enemySlot[i]->GetPosition().y - 65);

		ss.str("");
		ss << "BanPickEnemyAtk" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);
		text->text.setString(std::to_string(enemyInfo.player[i].attack));
		text->SetOrigin(Origins::ML);
		text->SetPosition(enemySlot[i]->GetPosition().x - 50
			, enemySlot[i]->GetPosition().y + 20);

		ss.str("");
		ss << "BanPickEnemyDef" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setString(std::to_string(enemyInfo.player[i].defence));
		text->SetOrigin(Origins::ML);
		text->SetPosition(enemySlot[i]->GetPosition().x + 30
			, enemySlot[i]->GetPosition().y + 20);
		text->SetActive(true);

		for (int j = 0; j < enemyInfo.player[i].knownChamp; j++)
		{
			ss.str("");
			ss << "BanPickEnemy" << i << "Champ" << j;
			SpriteGo* icon = (SpriteGo*)FindGo(ss.str());
			icon->SetPosition(enemySlot[i]->GetPosition().x - 58 + (j * 38)
				, enemySlot[i]->GetPosition().y + 54);
			std::stringstream champ;
			champ << "graphics/UiFix/character_icons_" <<
				enemyInfo.player[i].proficiencyCode[j] << ".png";
			icon->sprite.setTexture((*RESOURCE_MGR.GetTexture(champ.str())));
			sf::Vector2u texSize = icon->sprite.getTexture()->getSize();
			icon->sprite.setTextureRect({ std::max(0,(int)texSize.x / 2 - 12), 0,25,25 });
			icon->SetOrigin(Origins::MC);
			icon->SetActive(true);

			ss << "Line";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetPosition(icon->GetPosition());
			spr->SetOrigin(Origins::MC);
			spr->SetActive(true);

			ss << "Back";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetPosition(icon->GetPosition());
			spr->SetOrigin(Origins::MC);
			spr->SetActive(true);

			ss << "Level";
			text = (TextGo*)FindGo(ss.str());
			text->text.setString(std::to_string(enemyInfo.player[i].proficiencyLevel[j]));
			text->SetOrigin(Origins::MC);
			text->SetPosition(icon->GetPosition().x + 8
				, icon->GetPosition().y - 13);
			text->SetActive(true);
		}
	}

}

void SceneGame::ChampSlotFalse()
{
	for (int i = 0; i < champCount; i++)
	{
		std::stringstream ss;
		ss << "PickImage" << i + 1;
		FindGo(ss.str())->SetActive(false);
	}
}

void SceneGame::AiSelect()
{
	int highStatIndex = aiBanPick.CompareHighStatChampionIndex(banChamps);

	for (int j = 0; j < banChamps.size(); j++)
	{
		if (banChamps[j] == highStatIndex)
		{
			std::cout << "넥스트 스탯 호출!"<< std::endl;
			championSlot[aiBanPick.CompareNextStatChampion(banChamps)]->OnClick();
			return;
		}
		else if (highStatIndex >= 0 && highStatIndex < 14 && banChamps[j] != highStatIndex)
		{
			championSlot[highStatIndex]->OnClick();
			return;
		}
	}
	std::cout << "제일 높은 스탯의 챔피언 슬롯 번호 " << highStatIndex + 1 << std::endl;
}

void SceneGame::SwapSlotFalse()
{
	FindGo("Line Up Swap")->SetActive(false);
	FindGo("Draft Arrow2")->SetActive(false);
	FindGo("Draft_Slot_White")->SetActive(false);
	for (int i = 0; i < 6; i++)
	{
		swapSlot[i]->SetActive(false);
	}
}

void SceneGame::GetPlayers()
{
	if (!isPlayerEnough)
	{
		swapChampCount = (int)mode + 2;
		return;
	}
	swapChampCount = Utils::Clamp(TEAM_MGR.GetPlayerNum(), (int)mode + 2, (int)mode + 4);
}

void SceneGame::SetLineUp()
{
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;

	text = (TextGo*)FindGo("Enemy Team Name Text");
	text->text.setString(enemyInfo.name);
	text->SetOrigin(Origins::MC);
	text->SetActive(true);

	for (int i = 0; i < swapChampCount; i++)
	{
		int index = i;
		if (i > (int)mode + 1)
		{
			index += (2 - (int)mode);
		}

		std::stringstream ss;
		ss << "PlayerAtk" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);
		text->text.setString(std::to_string(playerInfo[i].attack));
		text->SetOrigin(Origins::ML);
		text->SetPosition(swapSlot[index]->GetPosition().x - 50
			, swapSlot[index]->GetPosition().y + 20);

		ss.str("");
		ss << "PlayerDef" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);
		text->text.setString(std::to_string(playerInfo[i].defence));
		text->SetOrigin(Origins::ML);
		text->SetPosition(swapSlot[index]->GetPosition().x + 32
			, swapSlot[index]->GetPosition().y + 20);


		ss.str("");
		if (i > (int)mode + 1)
		{
			ss << "Player Member Text" << i + 1 + (2 - (int)mode);
		}
		else
		{
			ss << "Player Member Text" << i + 1;
		}
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);
		text->text.setString(playerInfo[i].name);
		text->SetOrigin(Origins::ML);
		text->SetPosition(swapSlot[index]->GetPosition().x - 70
			, swapSlot[index]->GetPosition().y - 65);

		for (int j = 0; j < playerInfo[i].knownChamp; j++)
		{
			ss.str("");
			ss << "Player" << i << "Champ" << j;
			SpriteGo* icon = (SpriteGo*)FindGo(ss.str());
			if (j == 0)
			{
				icon->SetPosition(swapSlot[index]->GetPosition().x - 49
					, swapSlot[index]->GetPosition().y - 18);
			}
			else
			{
				icon->SetPosition(swapSlot[index]->GetPosition().x - 95.5 + (j * 38)
					, swapSlot[index]->GetPosition().y + 54);

			}
			std::stringstream champ;
			champ << "graphics/UiFix/character_icons_" <<
				playerInfo[i].proficiencyCode[j] << ".png";
			icon->sprite.setTexture((*RESOURCE_MGR.GetTexture(champ.str())));
			sf::Vector2u texSize = icon->sprite.getTexture()->getSize();
			icon->sprite.setTextureRect({ std::max(0,(int)texSize.x / 2 - 12), 0,25,25 });
			icon->SetOrigin(Origins::MC);
			icon->SetActive(true);

			ss << "Line";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetPosition(icon->GetPosition());
			spr->SetOrigin(Origins::MC);
			spr->SetActive(true);

			ss << "Level";
			text = (TextGo*)FindGo(ss.str());
			text->text.setString(std::to_string(playerInfo[i].proficiencyLevel[j]));
			text->SetOrigin(Origins::MC);
			if (j == 0)
			{
				text->SetPosition(icon->GetPosition().x + 13
					, icon->GetPosition().y - 19);
			}
			else
			{
				text->SetPosition(icon->GetPosition().x + 8
					, icon->GetPosition().y - 13);

			}
			text->SetActive(true);
		}
	}

	for (int i = 0; i < (int)mode + 2; i++)
	{
		SpriteGo* enemySlot = (SpriteGo*)FindGo("Enemy Draft Slot" + std::to_string(i + 1));
		std::stringstream ss;
		ss << "EnemyAtk" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setString(std::to_string(enemyInfo.player[i].attack));
		text->SetOrigin(Origins::ML);
		text->SetPosition(enemySlot->GetPosition().x - 50
			, enemySlot->GetPosition().y + 20);
		text->SetActive(true);

		ss.str("");
		ss << "EnemyDef" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setString(std::to_string(enemyInfo.player[i].defence));
		text->SetOrigin(Origins::ML);
		text->SetPosition(enemySlot->GetPosition().x + 30
			, enemySlot->GetPosition().y + 20);
		text->SetActive(true);

		ss.str("");
		ss << "Enemy Member Text" << i + 1;
		text = (TextGo*)FindGo(ss.str());
		text->text.setString(enemyInfo.player[i].name);
		text->SetOrigin(Origins::ML);
		text->SetPosition(enemySlot->GetPosition().x - 70
			, enemySlot->GetPosition().y - 65);
		text->SetActive(true);

		for (int j = 0; j < enemyInfo.player[i].knownChamp; j++)
		{
			ss.str("");
			ss << "Enemy" << i << "Champ" << j;
			SpriteGo* icon = (SpriteGo*)FindGo(ss.str());
			if (j == 0)
			{
				icon->SetPosition(enemySlot->GetPosition().x - 49
					, enemySlot->GetPosition().y - 18);
			}
			else
			{
				icon->SetPosition(enemySlot->GetPosition().x - 95.5 + (j * 38)
					, enemySlot->GetPosition().y + 54);

			}
			std::stringstream champ;
			champ << "graphics/UiFix/character_icons_" <<
				enemyInfo.player[i].proficiencyCode[j] << ".png";
			icon->sprite.setTexture((*RESOURCE_MGR.GetTexture(champ.str())));
			sf::Vector2u texSize = icon->sprite.getTexture()->getSize();
			icon->sprite.setTextureRect({ std::max(0,(int)texSize.x / 2 - 12), 0,25,25 });
			icon->SetOrigin(Origins::MC);
			icon->SetActive(true);

			ss << "Line";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetPosition(icon->GetPosition());
			spr->SetOrigin(Origins::MC);
			spr->SetActive(true);

			ss << "Level";
			text = (TextGo*)FindGo(ss.str());
			text->text.setString(std::to_string(enemyInfo.player[i].proficiencyLevel[j]));
			text->SetOrigin(Origins::MC);
			if (j == 0)
			{
				text->SetPosition(icon->GetPosition().x + 13
					, icon->GetPosition().y - 19);
			}
			else
			{
				text->SetPosition(icon->GetPosition().x + 8
					, icon->GetPosition().y - 13);
			}
			text->SetActive(true);
		}
	}
}
