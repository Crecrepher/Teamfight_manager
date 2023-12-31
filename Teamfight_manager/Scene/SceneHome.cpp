#include "stdafx.h"
#include "SceneHome.h"

#include "DataTableMgr.h"
#include "StringTable.h"
#include "ItemTable.h"
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

#include "AiBanPick.h"// 임시 추가

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
	cloudSpeed = std::vector<float>(7);

	AddGoSprites();
	AddGoUiButton();
	AddGoText();



	AddGo(new RectGo("UiShade"));
	AddGo(new RectGo("PopupUiShade"));
	AddGo(new RectGo("PopupTextShade"));
	AddGo(new SoundGo("sound/InfiniteDoors.wav", "HomeBgm"));
	AddGo(new SoundGo("sound/Cash register 5.wav", "CashSound"));
	AddGo(new SoundGo("sound/Revive heal 5.wav", "CraftSound"));

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

	SoundGo* sound = (SoundGo*)FindGo("HomeBgm");
	sound->sound.setLoop(true);
	sound->Play();

	RESOURCE_MGR.LoadFromCsv("tables/HomeResourceList.csv");
	equipCraftParts = std::vector<int>(4);
	selectedSponsorIndex = -1;
	backClick = true;
	isMenuOn = false;
	isPopupOn = false;
	isInnerPopOn = false;
	popTextFade = maxPopTextFade;
	gainTrainingInfo = TEAM_MGR.GetTrainingInfo();

	if (TEAM_MGR.GetTodayDate() == 0 && TEAM_MGR.GetPlayerNum() == 0)
	{
		NewYear();
		if (TEAM_MGR.GetTodayYear() == 2021)
		{
			for (int i = 0; i < 4; i++)
			{
				Recruit(-1, 0);
				TEAM_MGR.Employ(0);
				gainTrainingInfo = TEAM_MGR.GetTrainingInfo();
			}
		}
	}
	else if (TEAM_MGR.GetTodayDate() == 4)
	{
		UiSponsorContractOpen(true, true);
	}

	MakeMainUi();
	MakeSubUi();
	ResetSky();
	SetClouds();

	TEAM_MGR.Save();
}

void SceneHome::Exit()
{
	SoundGo* sound = (SoundGo*)FindGo("HomeBgm");
	sound->sound.stop();
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
		selectedMenu = -1;
	}

	if (!isPopupOn && popTextFade >= 0.f)
	{
		PopTextUpdate(dt);
	}

	TestingCheats();
	UpdateClouds(dt);

	if (isInnerPopOn)
	{
		ScrollEquip(dt);
	}
}

void SceneHome::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
}

void SceneHome::AddGoSprites()
{
	//Main
	AddGo(new SpriteGo("graphics/Origin/Sprite/ground.png", "Ground"));
	AddGo(new SpriteGo("graphics/UiFix/house_bg.png", "House"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/sky_day.png", "Sky"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_bg.png", "HeadUi"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_slot_bg.png", "HeadSlot0"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_slot_bg.png", "HeadSlot1"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_gold_icon.png", "GoldUi"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_calendar_icon.png", "CalendarUi"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/header_league_bg.png", "LeagueUi"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/training_ui_bg.png", "SubUiBack"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/proceed_button_2.png", "PlayBArrow"));

	AddGo(new SpriteGo("graphics/Origin/Sprite/sponser_logo_new_3.png", "TeamIco"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/league_icon_0.png", "LeagueIco"));

	for (int i = 0; i < 7; i++)
	{
		std::stringstream ss;
		ss << "Clouds" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/clouds_0.png", ss.str()));
	}

	//Recruit
	AddGo(new SpriteGo("graphics/UiFix/player_recruit_card.png", "RecruitBg"));
	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "RecruitChamp" << i;
		AddGo(new SpriteGo("graphics/UiFix/character_icons_0.png", ss.str()));
	}

	//Training
	AddGo(new SpriteGo("graphics/Origin/Sprite/scroll_bar.png", "ScrollBar"));
	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "TrainingSlotUi" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/training_slot_bg.png", ss.str()));
		ss << "Name";
		AddGo(new TextGo(ss.str()));
		ss << "Point";
		AddGo(new TextGo(ss.str()));
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
		ss.str("");
		ss << "CharacterStatLevel" << i;
		AddGo(new TextGo(ss.str()));
	}
	AddGo(new SpriteGo("graphics/Origin/Sprite/attack_icon.png", "IconAtk0"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/attack_icon.png", "IconAtk1"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/defense_icon.png", "IconDef0"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/defense_icon.png", "IconDef1"));

	// PlayerInfo
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::stringstream ss;
			ss << "Player" << i << "InfoCharacterIcons" << j;
			AddGo(new SpriteGo("graphics/Origin/Sprite/character_icons_0.png", ss.str()));
			ss << "Back";
			AddGo(new SpriteGo("graphics/Origin/Sprite/champion&player_slot_0.png", ss.str()));
			ss << "Line";
			AddGo(new SpriteGo("graphics/Origin/Sprite/champion&player_slot_1.png", ss.str()));
		}
	}

	//Sponsor
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

	for (size_t i = 0; i < 3; i++)
	{
		std::stringstream ss;
		ss << "ListSponsorLogo" << i;
		AddGo(new SpriteGo("graphics/Origin/Sprite/sponser_logo_0.png", ss.str()));
		ss << "back";
		AddGo(new SpriteGo("graphics/Origin/Sprite/champ_bg #43858.png", ss.str()));
	}
	AddGo(new SpriteGo("graphics/Origin/Sprite/lock_icon.png", "LockIcon0"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/lock_icon.png", "LockIcon1"));

	//Item

	AddGo(new SpriteGo("graphics/Origin/Sprite/items_headset_28.png", "ItemEquipedIcon0"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/items_controller_36.png", "ItemEquipedIcon1"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/items_chair_1.png", "ItemEquipedIcon2"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/items_uniform_31.png", "ItemEquipedIcon3"));

	for (int i = 0; i < 3; i++)
	{
		std::stringstream ss;
		ss << "Equipmake" << i;
		if (i == 0)
		{
			AddGo(new SpriteGo("graphics/UiFix/equipment_making_slot_bg_0.png", ss.str()));
		}
		else
		{
			AddGo(new SpriteGo("graphics/Origin/Sprite/equipment_making_slot_locked_bg.png", ss.str()));
		}
	}
	AddGo(new SpriteGo("graphics/Origin/Sprite/lock_icon.png", "ItemMakeLock0"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/lock_icon.png", "ItemMakeLock1"));

	// Change Equip
	AddGo(new SpriteGo("graphics/UiFix/equipment_change_popup_bg.png", "EquipPopup"));

	// CraftResult
	AddGo(new SpriteGo("graphics/UiFix/equipment_result_bg.png", "CraftResultBack"));
	AddGo(new SpriteGo("graphics/Origin/Sprite/items_uniform_0.png", "CraftResultIcon"));
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

	//Recruit
	AddGo(new UiButton("graphics/Origin/Sprite/important_button_0.png", "RecruitB"));
	AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", "RecruitCloseB"));
	AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", "RecruitCanceleB"));

	// PlayerInfo
	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "PlayerInfoB" << i;
		AddGo(new UiButton("graphics/UiFix/slot_normal&hover_0.png", ss.str()));
	}
	AddGo(new UiButton("graphics/Origin/Sprite/important_button_0.png", "PlayerFireB"));


	// Training
	for (int i = 0; i < 12; i++)
	{
		std::stringstream ss;
		ss << "PlayerList" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/player_list_bg_0.png", ss.str()));
		ss << "Name";
		AddGo(new TextGo(ss.str()));
		ss << "Point";
		AddGo(new TextGo(ss.str()));
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

	for (int i = 0; i < 3; i++)
	{
		std::stringstream ss;
		ss << "UseSponlist" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/type_ui_0.png", ss.str()));
	}

	//Item
	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "EquipSlotB" << i;
		AddGo(new UiButton("graphics/UiFix/equipment_slot_bg_0.png", ss.str()));
	}
	for (int i = 0; i < 3; i++)
	{
		std::stringstream ss;
		ss << "EquipMakeB" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/important_button_0.png", ss.str()));
	}

	//ChangeEquip
	AddGo(new SpriteGo("graphics/Origin/Sprite/equipment_change_popup_bg_mask1.png", "EquipMask"));
	for (int i = 0; i < 38; i++)
	{
		std::stringstream ss;
		ss << "EquipItemB" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/equipment_icon_bg_0.png", ss.str()));
		ss << "Sprite";
		AddGo(new SpriteGo("graphics/Origin/Sprite/items_controller_0.png", ss.str()));
		ss << "Lock";
		AddGo(new SpriteGo("graphics/Origin/Sprite/lock_icon.png", ss.str()));
	}
	AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", "ExitEquipPopB"));

	//Craft
	AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", "PartsResetB"));
	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "CraftSlot" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/equipment_making_slot_bg_0.png", ss.str()));
		ss << "Icon";
		switch (i)
		{
		case 0:
			AddGo(new SpriteGo("graphics/Origin/Sprite/sound_chip.png", ss.str()));
			break;
		case 1:
			AddGo(new SpriteGo("graphics/Origin/Sprite/keyboard_switch.png", ss.str()));
			break;
		case 2:
			AddGo(new SpriteGo("graphics/Origin/Sprite/screw.png", ss.str()));
			break;
		case 3:
			AddGo(new SpriteGo("graphics/Origin/Sprite/fabric_piece.png", ss.str()));
			break;
		}
		ss << "Bg";
		AddGo(new SpriteGo("graphics/Origin/Sprite/material_item_bg_0.png", ss.str()));
		ss.str("");
		ss << "PartDecrease" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", ss.str()));
		ss << "x5";
		AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", ss.str()));
		ss.str("");
		ss << "PartIncrease" << i;
		AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", ss.str()));
		ss << "x5";
		AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", ss.str()));
	}
	AddGo(new UiButton("graphics/Origin/Sprite/important_button_0.png", "OnCraftB"));

	// CraftResult
	AddGo(new UiButton("graphics/Origin/Sprite/default_button_0.png", "CraftResultB"));
}

void SceneHome::AddGoText()
{
	AddGo(new TextGo("Calinder"));
	AddGo(new TextGo("MoneyInfoT"));

	AddGo(new TextGo("UiMenuTitleText"));
	AddGo(new TextGo("UiMenuCloseText"));

	AddGo(new TextGo("ProgressPlayText"));
	AddGo(new TextGo("DoProgressPlayText"));

	AddGo(new TextGo("TeamNameText"));
	AddGo(new TextGo("TeamScoreText"));
	AddGo(new TextGo("LeagueGradeText"));

	AddGo(new TextGo("PopupText"));

	//Recruit
	AddGo(new TextGo("RecruitName"));
	AddGo(new TextGo("RecruitAge"));
	AddGo(new TextGo("RecruitCost"));
	AddGo(new TextGo("RecruitAtk"));
	AddGo(new TextGo("RecruitDef"));
	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "RecruitChampText" << i;
		AddGo(new TextGo(ss.str()));
	}
	AddGo(new TextGo("RecruitBText"));
	AddGo(new TextGo("RecruitCancelBText"));
	AddGo(new TextGo("RecruitExitBText"));

	// PlayerInfo
	for (int i = 0; i < 6; i++)
	{
		AddGo(new TextGo("PlayerInfoTName" + std::to_string(i)));
		AddGo(new TextGo("PlayerInfoTAge" + std::to_string(i)));
		AddGo(new TextGo("PlayerInfoTAtk" + std::to_string(i)));
		AddGo(new TextGo("PlayerInfoTDef" + std::to_string(i)));
		for (int j = 0; j < 4; j++)
		{
			AddGo(new TextGo("PlayerInfoT" + std::to_string(i)
				+ "ChampLV" + std::to_string(j)));
		}
		AddGo(new TextGo("PlayerInfoTCost" + std::to_string(i)));
	}
	AddGo(new TextGo("PlayerInfoCount"));
	AddGo(new TextGo("FireBText"));

	//Training
	AddGo(new TextGo("TrainPlayerName"));
	AddGo(new TextGo("TrainPlayerAge"));
	AddGo(new TextGo("TrainPlayerAtk"));
	AddGo(new TextGo("TrainPlayerDef"));
	AddGo(new TextGo("TrainPlayerListName"));
	AddGo(new TextGo("TrainPlayerListPoint"));
	AddGo(new TextGo("TrainByUsePoint"));
	AddGo(new TextGo("TrainPlayerReturnBText"));
	AddGo(new TextGo("TrainPlayerLeftPointText"));

	// Contract
	AddGo(new TextGo("SponsorContractDescribe"));
	AddGo(new TextGo("SelectedSponsorCountText"));
	AddGo(new TextGo("SponsorContractAcceptText"));
	AddGo(new TextGo("SponsorContractLeftWarnText"));
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
	AddGo(new TextGo("SponsorNameUi"));
	AddGo(new TextGo("SponsorStateUi"));
	for (int i = 0; i < 3; i++)
	{
		std::stringstream ss;
		ss << "SponsorSlotText" << i;
		AddGo(new TextGo(ss.str()));
		ss << "State";
		AddGo(new TextGo(ss.str()));
	}
	AddGo(new TextGo("SponsorLockText0"));
	AddGo(new TextGo("SponsorLockText1"));
	AddGo(new TextGo("SponsorQuestUiText"));
	AddGo(new TextGo("SponsorQuestNowText"));
	AddGo(new TextGo("SponsorRewardUiText"));
	AddGo(new TextGo("SponsorRewardMoneyUiText"));


	//Equip
	AddGo(new TextGo("UsingEquipUiText"));
	AddGo(new TextGo("MakeEquipUiText"));
	AddGo(new TextGo("CurPartsEquipUiText"));
	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "EquipItemNameSlot" << i;
		AddGo(new TextGo(ss.str()));
		ss << "Stat";
		AddGo(new TextGo(ss.str()));
		ss.str("");
		ss << "EquipItemType" << i;
		AddGo(new TextGo(ss.str()));
		ss.str("");
		ss << "HavePartsText" << i;
		AddGo(new TextGo(ss.str()));
	}
	for (int i = 0; i < 3; i++)
	{
		std::stringstream ss;
		ss << "MakeItemPartsText" << i;
		AddGo(new TextGo(ss.str()));
		ss.str("");
		ss << "MakeItemBText" << i;
		AddGo(new TextGo(ss.str()));
		ss.str("");
		ss << "MakingItemText" << i;
		AddGo(new TextGo(ss.str()));
		ss << "Time";
		AddGo(new TextGo(ss.str()));
	}
	for (int i = 0; i < 2; i++)
	{
		std::stringstream ss;
		ss << "CanCraftExText" << i;
		AddGo(new TextGo(ss.str()));
	}

	// EquipChange
	AddGo(new TextGo("PopupUiText"));
	AddGo(new TextGo("EquipItemText"));
	AddGo(new TextGo("EquipItemValText"));
	AddGo(new TextGo("EquipingText"));
	AddGo(new TextGo("LookItemText"));
	AddGo(new TextGo("LookItemValText"));
	AddGo(new TextGo("ExitEquipPopBText"));

	// EquipMake
	AddGo(new TextGo("SelectPartsText"));
	AddGo(new TextGo("SelectPartsResetBText"));
	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "UsingCraftPartsText" << i;
		AddGo(new TextGo(ss.str()));
		ss << "Cur";
		AddGo(new TextGo(ss.str()));
		ss << "Text";
		AddGo(new TextGo(ss.str()));

		for (int j = 0; j < 4; j++)
		{
			ss.str("");
			ss << "InputPartsBt" << i << "Text" << j;
			AddGo(new TextGo(ss.str()));
		}
		ss.str("");
		ss << "CraftUiUsingCount" << i;
		AddGo(new TextGo(ss.str()));

		ss.str("");
		ss << "CraftUiUsingPartsText" << i;
		AddGo(new TextGo(ss.str()));
	}
	AddGo(new TextGo("CraftUiCurMoneyText"));
	AddGo(new TextGo("CraftUiPartsHeadText"));
	AddGo(new TextGo("CraftUiUseDateText"));
	AddGo(new TextGo("CraftUiUseCostText"));
	AddGo(new TextGo("CraftingBText"));

	//CraftResult
	AddGo(new TextGo("CraftResultTitle"));
	AddGo(new TextGo("CraftResultName"));
	AddGo(new TextGo("CraftResultVal"));
	AddGo(new TextGo("CraftResultOkBText"));
}


void SceneHome::MakeMainUi()
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);

	RectGo* popTextShade = (RectGo*)FindGo("PopupTextShade");
	popTextShade->SetSize(FRAMEWORK.GetWindowSize());
	popTextShade->rectangle.setFillColor(sf::Color(56, 58, 64, 200));
	popTextShade->sortLayer = 100;
	popTextShade->SetPosition(FRAMEWORK.GetWindowSize() * 0.5f);
	popTextShade->SetOrigin(Origins::MC);


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

	spr = (SpriteGo*)FindGo("TeamIco");
	spr->SetPosition(5, 5);
	spr->SetOrigin(Origins::TL);
	spr->SetSize(2, 2);
	spr->sortLayer = 107;

	spr = (SpriteGo*)FindGo("LeagueIco");
	spr->SetPosition(20, 95);
	spr->SetOrigin(Origins::MC);
	spr->SetSize(2, 2);
	spr->sortLayer = 107;

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
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/main_menu_button_0.png"));
		bt->OnClick = [this, i]() {
			if (isMenuOn)
			{
				return;
			}
			MainUiClose();
			MainUiOpen((MainMenuType)i);
			backClick = false;
			for (int j = 0; j < 5; j++)
			{
				std::stringstream bName;
				bName << "MainB" << i;
				UiButton* mainBt = (UiButton*)FindGo(bName.str());
				if (i != j)
				{
					mainBt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/main_menu_button_2.png"));
					bName << "Text";
					TextGo* tx = (TextGo*)FindGo(bName.str());
					tx->text.setFillColor(sf::Color::Black);
				}
				else
				{
					mainBt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/main_menu_button_0.png"));
					bName << "Text";
					TextGo* tx = (TextGo*)FindGo(bName.str());
					tx->text.setFillColor(sf::Color::White);
				}
			}
			selectedMenu = i;
			};
		bt->OnEnter = [this, i, bt]() {
			if (i != selectedMenu)
			{
				bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/main_menu_button_1.png"));
				std::stringstream bName;
				bName << "MainB" << i << "Text";
				TextGo* tx = (TextGo*)FindGo(bName.str());
				tx->text.setFillColor(sf::Color(204, 255, 58));
			}
			};
		bt->OnExit = [this, i, bt]() {
			if (i != selectedMenu)
			{
				bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/main_menu_button_0.png"));
				std::stringstream bName;
				bName << "MainB" << i << "Text";
				TextGo* tx = (TextGo*)FindGo(bName.str());
				tx->text.setFillColor(sf::Color::White);
			}
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

			bt2->OnEnter = [bt2, j, this]() {
				bt2->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/main_menu_button_1.png"));
				std::stringstream bName;
				bName << "MainB" << j << "Text";
				TextGo* tx = (TextGo*)FindGo(bName.str());
				tx->text.setFillColor(sf::Color(204, 255, 58));
				};
			bt2->OnExit = [bt2, j, this]() {
				bt2->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/main_menu_button_0.png"));
				std::stringstream bName;
				bName << "MainB" << j << "Text";
				TextGo* tx = (TextGo*)FindGo(bName.str());
				tx->text.setFillColor(sf::Color::White);
				};

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
	bt->OnClick = [this, stringtable]() {
		if (isMenuOn)
		{
			return;
		}
		TEAM_MGR.DayPass();

		UpdateMoney();
		if (TEAM_MGR.GetTodayDate() == 0)
		{
			NewYear();
		}
		else if (TEAM_MGR.GetTodayDate() == 4)
		{
			UiSponsorContractOpen(true, true);
		}
		TeamMgr::Schedule schedule = TEAM_MGR.GetSchedule(TEAM_MGR.GetTodayDate() - 1);
		if (schedule != TeamMgr::Schedule::Recruit &&
			schedule != TeamMgr::Schedule::Vacation)
		{
			SCENE_MGR.ChangeScene(SceneId::Game);
		}
		TextGo* text = (TextGo*)FindGo("Calinder");
		text->text.setString(std::to_wstring(TEAM_MGR.GetTodayYear()) + L" / "
			+ std::to_wstring(TEAM_MGR.GetTodayDate() / 4 + 1) + stringtable->GetW("Month") + L" "
			+ std::to_wstring(TEAM_MGR.GetTodayDate() % 4 + 1) + stringtable->GetW("Weekday"));
		UpdatePopDate();
		popTextFade = maxPopTextFade;
		ResetSky();
		};
	bt->OnStay = [bt, this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/proceed_button_3.png"));
			TextGo* tx = (TextGo*)FindGo("ProgressPlayText");
			tx->text.setFillColor(sf::Color::Black);
			tx = (TextGo*)FindGo("DoProgressPlayText");
			tx->text.setFillColor(sf::Color::Black);
		}
		else if (INPUT_MGR.GetMouseButtonUp(sf::Mouse::Left))
		{
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/proceed_button_1.png"));
			TextGo* tx = (TextGo*)FindGo("ProgressPlayText");
			tx->text.setFillColor(sf::Color::White);
			tx = (TextGo*)FindGo("DoProgressPlayText");
			tx->text.setFillColor(sf::Color::White);
		}
		};
	bt->OnEnter = [bt, this]() {
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/proceed_button_1.png"));
		TextGo* tx = (TextGo*)FindGo("ProgressPlayText");
		tx->text.setFillColor(sf::Color::White);
		tx = (TextGo*)FindGo("DoProgressPlayText");
		tx->text.setFillColor(sf::Color::White);
		};
	bt->OnExit = [bt, this]() {
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/proceed_button_0.png"));
		TextGo* tx = (TextGo*)FindGo("ProgressPlayText");
		tx->text.setFillColor(sf::Color::Black);
		tx = (TextGo*)FindGo("DoProgressPlayText");
		tx->text.setFillColor(sf::Color::Black);
		};

	TextGo* text = (TextGo*)FindGo("ProgressPlayText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Progress"));
	text->text.setFillColor(sf::Color::Black);
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::TL);
	text->SetPosition(bt->GetPosition().x - 240, bt->GetPosition().y - 70);
	text->sortLayer = 103;

	text = (TextGo*)FindGo("DoProgressPlayText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("DoNextProgress"));
	text->text.setFillColor(sf::Color::Black);
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::TL);
	text->SetPosition(bt->GetPosition().x - 240, bt->GetPosition().y - 35);
	text->sortLayer = 103;

	spr = (SpriteGo*)FindGo("PlayBArrow");
	spr->SetPosition(bt->GetPosition());
	spr->SetOrigin(Origins::BR);
	spr->SetSize(uiSizeStart, uiSizeStart);
	spr->sortLayer = 102;

	text = (TextGo*)FindGo("MoneyInfoT");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(std::to_string(TEAM_MGR.GetMoney()));
	text->text.setFillColor(sf::Color::White);
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::TL);
	text->SetPosition(FRAMEWORK.GetWindowSize().x * 0.85f, 22.f);
	text->sortLayer = 107;

	text = (TextGo*)FindGo("Calinder");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(std::to_wstring(TEAM_MGR.GetTodayYear()) + L" / "
		+ std::to_wstring(TEAM_MGR.GetTodayDate() / 4 + 1) + stringtable->GetW("Month") + L" "
		+ std::to_wstring(TEAM_MGR.GetTodayDate() % 4 + 1) + stringtable->GetW("Weekday"));
	text->text.setFillColor(sf::Color::White);
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::TL);
	text->SetPosition(845, 22);
	text->sortLayer = 107;

	text = (TextGo*)FindGo("PopupText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Silver.ttf"));
	UpdatePopDate();
	text->text.setFillColor(sf::Color::White);
	text->text.setOutlineColor(sf::Color::Black);
	text->text.setOutlineThickness(5);
	text->text.setCharacterSize(120);
	text->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f, 100.f);
	text->SetOrigin(Origins::TC);
	text->sortLayer = 101;

	text = (TextGo*)FindGo("TeamNameText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("Smart Burger");
	text->text.setFillColor(sf::Color::White);
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::ML);
	text->SetPosition(71, 18);
	text->sortLayer = 107;

	text = (TextGo*)FindGo("TeamScoreText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	std::wstringstream wss;
	wss << TEAM_MGR.GetWin() << stringtable->GetW("Win") << " "
		<< TEAM_MGR.GetLose() << stringtable->GetW("Lose");
	text->text.setString(wss.str());
	text->text.setFillColor(sf::Color::White);
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::ML);
	text->SetPosition(71, 53);
	text->sortLayer = 107;

	text = (TextGo*)FindGo("LeagueGradeText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(std::to_wstring(TEAM_MGR.GetTodayYear()) + L" " + stringtable->GetW("AmatureLeague"));
	text->text.setFillColor(sf::Color::White);
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::ML);
	text->SetPosition(42, 93);
	text->sortLayer = 107;
}


void SceneHome::MakeSubUi()
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	UiButton* bt = (UiButton*)FindGo("TrainCloseB");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(1175, 555);
	bt->SetSize(2, 2);
	bt->sortLayer = 111;
	bt->OnClick = [this]() {
		isMenuOn = false;
		UiTrainingOpen(false);
		UiSponsorContractOpen(true, false);
		UiSponsorContractOpen(false, false);
		};
	bt->OnStay = [bt, this]() {
		if (INPUT_MGR.GetMouseButtonDown(sf::Mouse::Left))
		{
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/default_button_2.png"));
		}
		else if (INPUT_MGR.GetMouseButtonUp(sf::Mouse::Left))
		{
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/default_button_1.png"));
		}
		};
	bt->OnEnter = [bt, this]() {
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/default_button_1.png"));
		};
	bt->OnExit = [bt, this]() {
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/default_button_0.png"));
		};

	TextGo* text = (TextGo*)FindGo("UiMenuTitleText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"메뉴 제목");
	text->text.setCharacterSize(35);
	text->SetOrigin(Origins::MC);
	text->SetPosition(FRAMEWORK.GetWindowSize().x / 2.f, 80);
	text->sortLayer = 112;
	text->SetActive(false);

	text = (TextGo*)FindGo("UiMenuCloseText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Close"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition().x, bt->GetPosition().y - 3);
	text->sortLayer = 112;
	text->SetActive(false);


	RectGo* rect = (RectGo*)FindGo("UiShade");
	rect->SetSize(FRAMEWORK.GetWindowSize());
	rect->sortLayer = 110;
	rect->sortOrder = -1;
	rect->rectangle.setFillColor(sf::Color(0, 0, 0, 150));

	MakeSubUiTraining();
	MakeSubUiPlayerInfo();
	MakeSubUiSponsorContract();
	MakeSubUiEquip();
	MakeSubUiRecruit();
	UiTrainingOpen(false);
	UiEquipOpen(false);
	UiCraftFinish(0, false);
	UiRecruitOpen(false);
	UiPlayerInfoOpen(false);
}

void SceneHome::MakeSubUiRecruit()
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;
	spr = (SpriteGo*)FindGo("RecruitBg");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f,
		FRAMEWORK.GetWindowSize().y * 0.44f);
	spr->SetSize(2, 2);
	spr->sortLayer = 110;

	spr = (SpriteGo*)FindGo("RecruitChamp0");
	spr->sprite.setTextureRect({ 0,0,25,25 });
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(590, 300);
	spr->SetSize(2, 2);
	spr->sortLayer = 111;

	text = (TextGo*)FindGo("RecruitChampText0");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"23");
	text->text.setCharacterSize(16);
	text->text.setOutlineColor(sf::Color::Black);
	text->text.setOutlineThickness(2);
	text->SetOrigin(Origins::MC);
	text->SetPosition(spr->GetPosition().x + 12,
		spr->GetPosition().y - 16);
	text->sortLayer = 112;

	for (int i = 1; i < 4; i++)
	{
		std::stringstream ss;
		ss << "RecruitChamp" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->sprite.setTextureRect({ 0,0,25,25 });
		spr->SetPosition(580 + (43 * (i - 1)), 370);
		spr->SetOrigin(Origins::MC);
		spr->SetSize(1.7, 1.7);
		spr->sortLayer = 111;

		ss.str("");
		ss << "RecruitChampText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(L"23");
		text->text.setCharacterSize(12);
		text->text.setOutlineColor(sf::Color::Black);
		text->text.setOutlineThickness(2);
		text->SetOrigin(Origins::MC);
		text->SetPosition(spr->GetPosition().x + 12,
			spr->GetPosition().y - 10);
		text->sortLayer = 112;
	}

	bt = (UiButton*)FindGo("RecruitB");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f,
		FRAMEWORK.GetWindowSize().y * 0.59f);
	bt->SetSize(2, 1.5);
	bt->sortLayer = 111;

	text = (TextGo*)FindGo("RecruitBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("PlayerSearch"));
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 112;

	bt = (UiButton*)FindGo("RecruitCanceleB");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f,
		FRAMEWORK.GetWindowSize().y * 0.65f);
	bt->SetSize(2, 1.5);
	bt->sortLayer = 111;

	text = (TextGo*)FindGo("RecruitCancelBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Cancel"));
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 112;

	bt = (UiButton*)FindGo("RecruitCloseB");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.62f,
		FRAMEWORK.GetWindowSize().y * 0.53f);
	bt->SetSize(1.5, 1.5);
	bt->sortLayer = 111;

	text = (TextGo*)FindGo("RecruitExitBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Close"));
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 112;

	text = (TextGo*)FindGo("RecruitName");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"Name");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::ML);
	text->SetPosition(565, 253);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("RecruitAge");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"20 Age");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::MC);
	text->SetPosition(665, 285);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("RecruitCost");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"123");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::ML);
	text->SetPosition(660, 311);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("RecruitAtk");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"12");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::ML);
	text->SetPosition(590, 338);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("RecruitDef");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"23");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::ML);
	text->SetPosition(661, 338);
	text->sortLayer = 112;
}

void SceneHome::MakeSubUiPlayerInfo()
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;

	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "PlayerInfoB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::MC);
		bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f,
			FRAMEWORK.GetWindowSize().y * 0.227f + (64.f * i));
		bt->SetSize(2, 2);
		bt->sortLayer = 111;
		bt->OnClick = [this, i]() {
			selectedSponsorIndex = i;
			for (int j = 0; j < 6; j++)
			{
				std::stringstream name;
				name << "PlayerInfoB" << j;
				UiButton* slot = (UiButton*)FindGo(name.str());
				if (i != j)
				{
					slot->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/slot_normal&hover_0.png"));
				}
				else
				{
					slot->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/slot_normal&hover_1.png"));
				}
			}
			};

		for (int j = 0; j < 4; j++)
		{
			SpriteGo* icon;
			std::stringstream ss2;
			ss2 << "Player" << i << "InfoCharacterIcons" << j;
			icon = (SpriteGo*)FindGo(ss2.str());
			sf::Vector2u texSize = icon->sprite.getTexture()->getSize();
			icon->sprite.setTextureRect({ std::max(0,(int)texSize.x / 2 - 12), 0,25,25 });
			icon->SetPosition(bt->GetPosition().x + 200 + (j * 60), bt->GetPosition().y);
			icon->SetOrigin(Origins::MC);
			icon->SetSize(2, 2);
			icon->sortLayer = 113;

			ss2 << "Back";
			spr = (SpriteGo*)FindGo(ss2.str());
			spr->SetOrigin(Origins::MC);
			spr->SetPosition(icon->GetPosition());
			spr->SetSize(2, 2);
			spr->sortLayer = 112;

			ss2 << "Line";
			spr = (SpriteGo*)FindGo(ss2.str());
			spr->SetOrigin(Origins::MC);
			spr->SetPosition(icon->GetPosition());
			spr->SetSize(2, 2);
			spr->sortLayer = 114;



			text = (TextGo*)FindGo("PlayerInfoT" + std::to_string(i)
				+ "ChampLV" + std::to_string(j));
			text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
			text->text.setString(L"7");
			text->text.setCharacterSize(14);
			text->SetOrigin(Origins::MC);
			text->SetPosition(icon->GetPosition().x + 12, icon->GetPosition().y - 17);
			text->sortLayer = 115;
		}


		text = (TextGo*)FindGo(("PlayerInfoTName" + std::to_string(i)));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(L"Name");
		text->text.setCharacterSize(19);
		text->SetOrigin(Origins::ML);
		text->SetPosition(bt->GetPosition().x - 500, bt->GetPosition().y - 1);
		text->sortLayer = 112;

		text = (TextGo*)FindGo(("PlayerInfoTAge" + std::to_string(i)));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(L"20 Age");
		text->text.setCharacterSize(19);
		text->SetOrigin(Origins::ML);
		text->SetPosition(bt->GetPosition().x - 300, bt->GetPosition().y - 1);
		text->sortLayer = 112;

		text = (TextGo*)FindGo(("PlayerInfoTAtk" + std::to_string(i)));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(L"20");
		text->text.setCharacterSize(19);
		text->SetOrigin(Origins::ML);
		text->SetPosition(bt->GetPosition().x - 60.f, bt->GetPosition().y - 1);
		text->sortLayer = 112;

		text = (TextGo*)FindGo(("PlayerInfoTDef" + std::to_string(i)));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(L"20");
		text->text.setCharacterSize(19);
		text->SetOrigin(Origins::ML);
		text->SetPosition(bt->GetPosition().x + 60.f, bt->GetPosition().y - 1);
		text->sortLayer = 112;

		text = (TextGo*)FindGo(("PlayerInfoTCost" + std::to_string(i)));
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(L"20");
		text->text.setCharacterSize(19);
		text->SetOrigin(Origins::MR);
		text->SetPosition(bt->GetPosition().x + 470.f, bt->GetPosition().y - 1);
		text->sortLayer = 112;
	}

	bt = (UiButton*)FindGo("PlayerFireB");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f, 555);
	bt->SetSize(2, 2);
	bt->sortLayer = 111;
	bt->OnClick = [this]() {
		if (selectedSponsorIndex >= 0 && TEAM_MGR.GetPlayerNum() > 4)
		{
			SoundGo* so = (SoundGo*)FindGo("CashSound");
			so->Play();
			TEAM_MGR.FirePlayer(selectedSponsorIndex);
			UiPlayerInfoOpen(true);
		}
		};

	text = (TextGo*)FindGo("PlayerInfoCount");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"6/6");
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::ML);
	text->SetPosition(35, 555);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("FireBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("FirePlayer"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 112;
}

void SceneHome::MakeSubUiTraining()
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;
	spr = (SpriteGo*)FindGo("SubUiBack");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5f,
		FRAMEWORK.GetWindowSize().y * 0.44f);
	spr->SetSize((FRAMEWORK.GetWindowSize().x / spr->GetSize().x) - 0.05f,
		(FRAMEWORK.GetWindowSize().x / spr->GetSize().x) - 0.05f);
	spr->sortLayer = 110;

	text = (TextGo*)FindGo("TrainPlayerName");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"선수 이름");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::ML);
	text->SetPosition(50, 150);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("TrainPlayerAge");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(L"0세");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::MR);
	text->SetPosition(450, 150);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("TrainPlayerAtk");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("0");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::ML);
	text->SetPosition(75, 215);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("TrainPlayerDef");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("0");
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::ML);
	text->SetPosition(165, 215);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("TrainPlayerListName");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Name"));
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::MC);
	text->SetPosition(185, 275);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("TrainPlayerListPoint");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("UnusedPoint"));
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::MC);
	text->SetPosition(370, 275);
	text->sortLayer = 112;

	text = (TextGo*)FindGo("TrainByUsePoint");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("UsePointTo"));
	text->text.setCharacterSize(30);
	text->SetOrigin(Origins::MC);
	text->SetPosition(860, 175);
	text->sortLayer = 112;

	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "TrainingSlotUi" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::TL);
		spr->SetSize(2, 2);
		spr->SetPosition(483 + (spr->GetSize().x * 2.f + 10.f) * (i % 2), 275 + (spr->GetSize().y * 2.f + 10.f) * (i / 2));
		spr->sortLayer = 111;

		ss << "Name";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(L"특성 이름");
		text->text.setCharacterSize(16);
		text->SetOrigin(Origins::ML);
		text->SetPosition(spr->GetPosition().x + 58,
			spr->GetPosition().y + 25);
		text->sortLayer = 112;

		ss << "Point";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("0");
		text->text.setCharacterSize(16);
		text->SetOrigin(Origins::ML);
		text->SetPosition(spr->GetPosition().x + 220,
			spr->GetPosition().y + 25);
		text->sortLayer = 112;

		ss.str("");
		ss << "XpBack" << i;
		SpriteGo* xpBar = (SpriteGo*)FindGo(ss.str());
		xpBar->SetOrigin(Origins::TL);
		xpBar->SetSize(2, 2);
		xpBar->SetPosition(spr->GetPosition().x + 55.f, spr->GetPosition().y + 56.f);
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

		for (int j = i * 8; j < i * 8 + 8; j++)
		{
			std::stringstream ss;
			ss << "TrainingPointGuage" << j;
			SpriteGo* trainPoint = (SpriteGo*)FindGo(ss.str());
			trainPoint->SetOrigin(Origins::MC);
			trainPoint->SetSize(2, 2);
			trainPoint->SetPosition(spr->GetPosition().x + 295.f - (j % 8 * 6), spr->GetPosition().y + 27.f);
			trainPoint->sortLayer = 112;
		}
		if (i > 1)
		{
			ss.str("");
			ss << "CharacterStatIcons" << i - 2;
			SpriteGo* charIcon = (SpriteGo*)FindGo(ss.str());
			charIcon->SetOrigin(Origins::TL);
			charIcon->SetSize(2, 2);
			charIcon->sprite.setTextureRect({ 0,0,25,25 });
			charIcon->SetPosition(spr->GetPosition().x, spr->GetPosition().y);
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
			statIcon->SetPosition(spr->GetPosition().x + 25.f, spr->GetPosition().y + 25.f);
			statIcon->SetSize(2, 2);
			statIcon->sortLayer = 111;
		}
	}

	SpriteGo* leftTrainPoint;
	for (int i = 0; i < 8; i++)
	{
		std::stringstream ss;
		ss << "LeftTrainingPointGuage" << i;
		leftTrainPoint = (SpriteGo*)FindGo(ss.str());
		leftTrainPoint->SetOrigin(Origins::MC);
		leftTrainPoint->SetPosition(1155 - (i * 18), 246);
		leftTrainPoint->SetSize(2, 2);
		leftTrainPoint->sortLayer = 113;
	}
	text = (TextGo*)FindGo("TrainPlayerLeftPointText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("LeftTrainPoint"));
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::MR);
	text->SetPosition(leftTrainPoint->GetPosition().x - 10,
		leftTrainPoint->GetPosition().y);
	text->sortLayer = 112;

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "CharacterIcons" << i;
		SpriteGo* characterIcons = (SpriteGo*)FindGo(ss.str());
		characterIcons->SetOrigin(Origins::TL);
		characterIcons->sprite.setTextureRect({ 0,0,25,25 });
		characterIcons->SetPosition(250 + (55 * i), 190);
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

		ss.str("");
		ss << "CharacterStatLevel" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("0");
		text->text.setCharacterSize(11);
		text->SetOrigin(Origins::MC);
		text->SetPosition(spr->GetPosition().x + 38,
			spr->GetPosition().y + 7);
		text->sortLayer = 114;
	}
	spr = (SpriteGo*)FindGo("IconAtk0");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(60, 215);
	spr->SetSize(2, 2);
	spr->sortLayer = 111;

	spr = (SpriteGo*)FindGo("IconDef0");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(150, 215);
	spr->SetSize(2, 2);
	spr->sortLayer = 111;

	bt = (UiButton*)FindGo("TrainPointReturnB");
	bt->SetOrigin(Origins::ML);
	bt->SetPosition(495, 245);
	bt->SetSize(1.5, 1.2);
	bt->sortLayer = 111;

	text = (TextGo*)FindGo("TrainPlayerReturnBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Reset"));
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition().x + 52, bt->GetPosition().y);
	text->sortLayer = 112;

	for (int i = 0; i < 12; i++)
	{
		std::stringstream ss;
		ss << "PlayerList" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::TL);
		bt->SetPosition(42, 308 + (i * 35));
		bt->SetSize(2, 2);
		bt->sortLayer = 111;
		bt->OnClick = [this, i]() {
			UiTrainingPlayerSelect(i);
			};
		ss << "Name";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(L"선수 이름");
		text->text.setCharacterSize(16);
		text->SetOrigin(Origins::ML);
		text->SetPosition(bt->GetPosition().x + 10,
			bt->GetPosition().y + 17);
		text->sortLayer = 112;

		ss << "Point";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("0");
		text->text.setCharacterSize(16);
		text->SetOrigin(Origins::ML);
		text->SetPosition(bt->GetPosition().x + 320,
			bt->GetPosition().y + 17);
		text->sortLayer = 112;
	}
}

void SceneHome::MakeSubUiSponsorContract()
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	SpriteGo* spr;

	for (int i = 0; i < 9; i++)
	{
		std::stringstream ss;
		ss << "SponSlot" << i;
		UiButton* bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::TL);
		bt->SetPosition(65 + ((bt->GetSize().x * 2 + 5) * (i % 3)), 175 + ((bt->GetSize().y * 2 + 5) * (i / 3)));
		bt->SetSize(2, 2);
		bt->sortLayer = 111;
		bt->SetActive(false);
		bt->OnEnter = [bt, i, this]() {
			for (int j = 0; j < 3; j++)
			{
				if (i == selectedSponsorIndex)
				{
					return;
				}
			}
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/sponsor_card_bg_1.png"));
			};
		bt->OnExit = [bt, i, this]() {
			for (int j = 0; j < 3; j++)
			{
				if (i == selectedSponsorIndex)
				{
					return;
				}
			}
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/sponsor_card_bg_0.png"));
			};

		ss.str("");
		ss << "SponLogo" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::TL);
		spr->SetPosition(bt->GetPosition().x + 6, bt->GetPosition().y + 8);
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
	spr->SetPosition(950, 195);
	spr->SetSize(2.3, 2.3);
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
		spr->SetPosition(950 + 72 * i, 425);
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
	bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/default_button_0.png"));
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(485, 470);
	bt->SetSize(2, 2);
	bt->sortLayer = 111;
	bt->OnClick = [this]() {
		if (selectedSponsorIndex != -1)
		{
			SponsorContract(selectedSponsorIndex);
		}
		};
	bt->SetActive(false);

	for (int i = 0; i < 3; i++)
	{
		std::stringstream ss;
		ss << "UseSponlist" << i;
		UiButton* SponBt = (UiButton*)FindGo(ss.str());
		SponBt->SetOrigin(Origins::TL);
		SponBt->SetPosition(45, 190 + 100 * i);
		SponBt->SetSize(5.85f, 4.f);
		SponBt->sortLayer = 111;
		SponBt->OnClick = [this]() {

			};
		SponBt->SetActive(false);

		ss.str("");
		ss << "ListSponsorLogo" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::TL);
		spr->SetPosition(SponBt->GetPosition().x + 15, SponBt->GetPosition().y + 15);
		spr->SetSize(2, 2);
		spr->sortLayer = 113;
		spr->SetActive(false);

		ss << "back";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::TL);
		spr->SetPosition(SponBt->GetPosition().x + 15, SponBt->GetPosition().y + 15);
		spr->SetSize(2.4, 2.4);
		spr->sortLayer = 112;
		spr->SetActive(false);
		if (i > 0)
		{
			ss.str("");
			ss << "LockIcon" << i - 1;
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetOrigin(Origins::MC);
			spr->SetPosition(SponBt->GetPosition().x + 430, SponBt->GetPosition().y + 40);
			spr->SetSize(4, 4);
			spr->sortLayer = 113;
			spr->SetActive(false);
			ss.str("");
			ss << "SponsorLockText" << i - 1;
			TextGo* text = (TextGo*)FindGo(ss.str());
			text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
			text->text.setString(stringtable->GetW("CanSponsorExpend"));
			text->text.setCharacterSize(17);
			text->SetOrigin(Origins::MC);
			text->SetPosition(spr->GetPosition().x, spr->GetPosition().y + 35);
			text->sortLayer = 112;
			text->SetActive(false);
		}

		ss.str("");
		ss << "SponsorSlotText" << i;
		TextGo* text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(stringtable->GetW("SponsorName0"));
		text->text.setCharacterSize(20);
		text->SetOrigin(Origins::ML);
		text->SetPosition(SponBt->GetPosition().x + 90, SponBt->GetPosition().y + 47);
		text->sortLayer = 112;
		text->SetActive(false);
		ss << "State";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(stringtable->GetW("Processing"));
		text->text.setCharacterSize(20);
		text->SetOrigin(Origins::MC);
		text->SetPosition(SponBt->GetPosition().x + 740, SponBt->GetPosition().y + 47);
		text->sortLayer = 112;
		text->SetActive(false);
	}

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
	text->SetPosition(953, 270);
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

	text = (TextGo*)FindGo("SponsorNameUi");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("SponsorName"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MC);
	text->SetPosition(335, 160);
	text->sortLayer = 112;
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorStateUi");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("State"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MC);
	text->SetPosition(780, 160);
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
		text->SetPosition(987 + 72 * i, 455);
		text->sortLayer = 112;
		text->SetActive(false);
	}

	text = (TextGo*)FindGo("SponsorQuestUiText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("ContractConditions"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MC);
	text->SetPosition(1090, 160);
	text->sortLayer = 112;
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorQuestNowText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Now"));
	text->text.setCharacterSize(20);
	text->text.setFillColor(sf::Color::Yellow);
	text->SetOrigin(Origins::ML);
	text->SetPosition(950, 320);
	text->sortLayer = 112;
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorRewardUiText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("ContractReward"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MC);
	text->SetPosition(1090, 360);
	text->sortLayer = 112;
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorRewardMoneyUiText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("SupportMoney"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::ML);
	text->SetPosition(950, 405);
	text->sortLayer = 112;
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorContractDescribe");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("SponContractDiscribe"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::ML);
	text->SetPosition(65, 155);
	text->sortLayer = 112;
	text->SetActive(false);

	text = (TextGo*)FindGo("SelectedSponsorCountText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("SelectedSponsor"));
	text->text.setString(text->text.getString() + " "
		+ std::to_string(TEAM_MGR.GetContractedSponsor()) + "/"
		+ std::to_string(TEAM_MGR.GetMaxSponsor()));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::TL);
	text->SetPosition(77, 470);
	text->sortLayer = 112;
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorContractAcceptText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("DoContract"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MC);
	text->SetPosition(485, 470);
	text->sortLayer = 114;
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorContractLeftWarnText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("IsLeftSponContract"));
	text->text.setCharacterSize(18);
	text->text.setFillColor(sf::Color::Red);
	text->SetOrigin(Origins::TR);
	text->SetPosition(883, 470);
	text->sortLayer = 112;
	text->SetActive(false);
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
	for (int i = 0; i < 5; i++)
	{
		std::stringstream ss;
		ss << "MainB" << i;
		UiButton* bt = (UiButton*)FindGo(ss.str());
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/main_menu_button_0.png"));
		ss << "Text";
		TextGo* btText = (TextGo*)FindGo(ss.str());
		btText->text.setFillColor(sf::Color::White);
	}
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

void SceneHome::MakeSubUiEquip()
{
	// Base
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;
	text = (TextGo*)FindGo("UsingEquipUiText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("UsingEquip"));
	text->text.setCharacterSize(30);
	text->SetOrigin(Origins::MC);
	text->SetPosition(250, 160);
	text->sortLayer = 111;

	text = (TextGo*)FindGo("MakeEquipUiText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("MakeEquip"));
	text->text.setCharacterSize(30);
	text->SetOrigin(Origins::MC);
	text->SetPosition(720, 160);
	text->sortLayer = 111;

	text = (TextGo*)FindGo("CurPartsEquipUiText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("HaveParts"));
	text->text.setCharacterSize(30);
	text->SetOrigin(Origins::MC);
	text->SetPosition(1095, 160);
	text->sortLayer = 111;

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "EquipSlotB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::TL);
		bt->SetPosition(42, 210 + (i * 75));
		bt->SetSize(2, 2);
		bt->sortLayer = 111;
		bt->OnClick = [this, i]() {
			if (isInnerPopOn)
			{
				return;
			}
			UiEquipChangeOpen(i);
			};

		ss.str("");
		ss << "EquipItemNameSlot" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(stringtable->GetW("Headset0"));
		text->text.setCharacterSize(20);
		text->SetOrigin(Origins::TL);
		text->SetPosition(bt->GetPosition().x + 75, bt->GetPosition().y + 10);
		text->sortLayer = 112;

		ss << "Stat";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(L"장비 스텟");
		text->text.setCharacterSize(11);
		text->SetOrigin(Origins::TL);
		text->SetPosition(bt->GetPosition().x + 75, bt->GetPosition().y + 35);
		text->sortLayer = 112;

		ss.str("");
		ss << "ItemEquipedIcon" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::MC);
		spr->SetPosition(bt->GetPosition().x + 36, bt->GetPosition().y + 36);
		spr->SetSize(2, 2);
		spr->sortLayer = 112;

		ss.str("");
		ss << "EquipItemType" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		switch (i)
		{
		case 0:
			text->text.setString(stringtable->GetW("Headset"));
			break;
		case 1:
			text->text.setString(stringtable->GetW("Controller"));
			break;
		case 2:
			text->text.setString(stringtable->GetW("Chair"));
			break;
		case 3:
			text->text.setString(stringtable->GetW("Uniform"));
			break;
		}
		text->text.setCharacterSize(14);
		text->SetOrigin(Origins::TR);
		text->SetPosition(bt->GetPosition().x + 400, bt->GetPosition().y + 10);
		text->sortLayer = 112;
		text->text.setFillColor(sf::Color(100, 100, 100));

		ss.str("");
		ss << "HavePartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		switch (i)
		{
		case 0:
			text->text.setString(stringtable->GetW("SoundChip"));
			break;
		case 1:
			text->text.setString(stringtable->GetW("Switch"));
			break;
		case 2:
			text->text.setString(stringtable->GetW("Screw"));
			break;
		case 3:
			text->text.setString(stringtable->GetW("Fabric"));
			break;
		}
		text->text.setCharacterSize(20);
		text->SetOrigin(Origins::TL);
		text->SetPosition(1036, 220 + (i * 75));
		text->sortLayer = 112;

	}

	for (int i = 0; i < 3; i++)
	{
		std::stringstream ss;
		ss << "Equipmake" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::MC);
		spr->SetPosition(710, 255 + (i * 100));
		spr->SetSize(2, 2);
		spr->sortLayer = 111;

		ss.str("");
		ss << "EquipMakeB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::MC);
		bt->SetPosition(spr->GetPosition().x + 140, spr->GetPosition().y);
		bt->SetSize(1.5, 1.5);
		bt->sortLayer = 111;

		ss.str("");
		ss << "MakeItemPartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("0");
		text->text.setCharacterSize(17);
		text->SetOrigin(Origins::MR);
		text->SetPosition(spr->GetPosition().x - 150, spr->GetPosition().y + 15);
		text->sortLayer = 113;

		ss.str("");
		ss << "MakeItemBText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(stringtable->GetW("Craft"));
		text->text.setCharacterSize(17);
		text->SetOrigin(Origins::MC);
		text->SetPosition(bt->GetPosition());
		text->sortLayer = 113;

		ss.str("");
		ss << "MakingItemText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(stringtable->GetW("Crafting"));
		text->text.setCharacterSize(17);
		text->SetOrigin(Origins::MC);
		text->SetPosition(spr->GetPosition().x + 140, spr->GetPosition().y - 25);
		text->sortLayer = 113;

		ss << "Time";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(stringtable->GetW("TimeRemaining"));
		text->text.setCharacterSize(17);
		text->SetOrigin(Origins::MC);
		text->SetPosition(spr->GetPosition().x + 140, spr->GetPosition().y);
		text->sortLayer = 113;

		if (i > 0)
		{
			ss.str("");
			ss << "ItemMakeLock" << i - 1;
			SpriteGo* lockIco = (SpriteGo*)FindGo(ss.str());
			lockIco->SetOrigin(Origins::MC);
			lockIco->SetPosition(spr->GetPosition().x, spr->GetPosition().y - 12.f);
			lockIco->SetSize(5, 5);
			lockIco->sortLayer = 111;

			ss.str("");
			ss << "CanCraftExText" << i - 1;
			text = (TextGo*)FindGo(ss.str());
			text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
			text->text.setString(stringtable->GetW("CanCraftExpend"));
			text->text.setCharacterSize(17);
			text->SetOrigin(Origins::MC);
			text->SetPosition(lockIco->GetPosition().x, lockIco->GetPosition().y + 37);
			text->sortLayer = 113;
		}

	}

	// ChangeEquip

	RectGo* rect = (RectGo*)FindGo("PopupUiShade");
	rect->SetSize(FRAMEWORK.GetWindowSize());
	rect->sortLayer = 114;
	rect->sortOrder = -1;
	rect->rectangle.setFillColor(sf::Color(0, 0, 0, 150));

	for (int i = 0; i < 38; i++)
	{
		std::stringstream ss;
		ss.str("");
		ss << "EquipItemB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::MC);
		bt->SetPosition(330 + (63 * (i % 5)), 210 + (63 * (i / 5)));
		bt->SetSize(2, 2);
		bt->sortLayer = 115;

		ss << "Sprite";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::MC);
		spr->SetPosition(bt->GetPosition());
		spr->SetSize(2, 2);
		spr->sortLayer = 115;
		spr->sortOrder = 1;

		ss << "Lock";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::MC);
		spr->SetPosition(bt->GetPosition().x + 15, bt->GetPosition().y - 15);
		spr->SetSize(2, 2);
		spr->sortLayer = 115;
		spr->sortOrder = 2;
	}

	spr = (SpriteGo*)FindGo("EquipPopup");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5, FRAMEWORK.GetWindowSize().y * 0.5 - 30);
	spr->SetSize(2, 2);
	spr->sortLayer = 114;

	spr = (SpriteGo*)FindGo("EquipMask");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5, FRAMEWORK.GetWindowSize().y * 0.5 - 30);
	spr->SetSize(2, 2);
	spr->sortLayer = 116;


	text = (TextGo*)FindGo("PopupUiText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("ChangeEquip"));
	text->text.setCharacterSize(30);
	text->SetOrigin(Origins::MC);
	text->SetPosition(spr->GetPosition().x, spr->GetPosition().y - 215);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("EquipItemText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Headset0"));
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::ML);
	text->SetPosition(665, 190);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("EquipItemValText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Headset0"));
	text->text.setCharacterSize(14);
	text->SetOrigin(Origins::ML);
	text->SetPosition(665, 220);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("EquipingText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Equiping"));
	text->text.setCharacterSize(14);
	text->SetOrigin(Origins::MR);
	text->SetPosition(980, 190);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("LookItemText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Headset0"));
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::ML);
	text->SetPosition(665, 376);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("LookItemValText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Headset0"));
	text->text.setCharacterSize(14);
	text->SetOrigin(Origins::ML);
	text->SetPosition(665, 405);
	text->sortLayer = 117;

	spr = (SpriteGo*)FindGo("ExitEquipPopB");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(930, 540);
	spr->SetSize(2, 2);
	spr->sortLayer = 117;

	text = (TextGo*)FindGo("ExitEquipPopBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Close"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MC);
	text->SetPosition(spr->GetPosition());
	text->sortLayer = 118;

	// Craft

	text = (TextGo*)FindGo("SelectPartsText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("SelectParts"));
	text->text.setCharacterSize(25);
	text->SetOrigin(Origins::MC);
	text->SetPosition(530, 180);
	text->sortLayer = 116;

	bt = (UiButton*)FindGo("PartsResetB");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(700, 185);
	bt->SetSize(1.3, 1);
	bt->sortLayer = 116;
	bt->OnClick = [this]() {
		equipCraftParts = std::vector<int>(4);
		UpdateCraftVal();
		};

	text = (TextGo*)FindGo("SelectPartsResetBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Reset"));
	text->text.setCharacterSize(12);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 117;

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "CraftSlot" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::TL);
		bt->SetPosition(356, 225 + (65 * i));
		bt->SetSize(1.8, 1.4);
		bt->sortLayer = 116;


		ss << "Icon";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::MC);
		spr->SetPosition(320, 255 + (65 * i));
		spr->SetSize(2, 2);
		spr->sortLayer = 117;

		ss << "Bg";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetOrigin(Origins::MC);
		spr->SetPosition(320, 255 + (65 * i));
		spr->SetSize(2, 2);
		spr->sortLayer = 116;

		bt->OnEnter = [bt, spr]() {
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/equipment_making_slot_bg_1.png"));
			spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/material_item_bg_2.png"));
			};
		bt->OnExit = [bt, spr]() {
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/equipment_making_slot_bg_0.png"));
			spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/material_item_bg_0.png"));
			};

		ss.str("");
		ss << "UsingCraftPartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		switch (i)
		{
		case 0:
			text->text.setString(stringtable->GetW("SoundChip"));
			break;
		case 1:
			text->text.setString(stringtable->GetW("Switch"));
			break;
		case 2:
			text->text.setString(stringtable->GetW("Screw"));
			break;
		case 3:
			text->text.setString(stringtable->GetW("Fabric"));
			break;
		}
		text->text.setCharacterSize(16);
		text->SetOrigin(Origins::TL);
		text->SetPosition(bt->GetPosition().x + 10, bt->GetPosition().y + 10);
		text->sortLayer = 117;

		ss << "Cur";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("0");
		text->text.setCharacterSize(18);
		text->text.setOutlineColor(sf::Color::Black);
		text->text.setOutlineThickness(2);
		text->SetOrigin(Origins::TR);
		text->SetPosition(350, 263 + (65 * i));
		text->sortLayer = 117;

		ss << "Text";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString(stringtable->GetW("HavePartsNGold"));
		text->text.setCharacterSize(16);
		text->SetOrigin(Origins::TL);
		text->SetPosition(bt->GetPosition().x + 10, bt->GetPosition().y + 32);
		text->sortLayer = 117;

		ss.str("");
		ss << "PartDecrease" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::MC);
		bt->SetPosition(600, 255 + (65 * i));
		bt->SetSize(0.45, 1);
		bt->sortLayer = 116;
		bt->OnClick = [this, i]() {
			equipCraftParts[i] = std::max(equipCraftParts[i] - 1, 0);
			UpdateCraftVal();
			};

		ss.str("");
		ss << "InputPartsBt" << i << "Text1";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("-1");
		text->text.setCharacterSize(16);
		text->SetOrigin(Origins::MC);
		text->SetPosition(bt->GetPosition());
		text->sortLayer = 117;

		ss.str("");
		ss << "PartDecrease" << i << "x5";
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::MC);
		bt->SetPosition(567, 255 + (65 * i));
		bt->SetSize(0.45, 1);
		bt->sortLayer = 116;
		bt->OnClick = [this, i]() {
			equipCraftParts[i] = std::max(equipCraftParts[i] - 5, 0);
			UpdateCraftVal();
			};

		ss.str("");
		ss << "InputPartsBt" << i << "Text0";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("-5");
		text->text.setCharacterSize(16);
		text->SetOrigin(Origins::MC);
		text->SetPosition(bt->GetPosition());
		text->sortLayer = 117;

		ss.str("");
		ss << "PartIncrease" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::MC);
		bt->SetPosition(680, 255 + (65 * i));
		bt->SetSize(0.45, 1);
		bt->sortLayer = 116;
		bt->OnClick = [this, i]() {
			equipCraftParts[i] = std::min(equipCraftParts[i] + 1, 20);
			UpdateCraftVal();
			};

		ss.str("");
		ss << "InputPartsBt" << i << "Text2";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("+1");
		text->text.setCharacterSize(16);
		text->SetOrigin(Origins::MC);
		text->SetPosition(bt->GetPosition());
		text->sortLayer = 117;

		ss.str("");
		ss << "PartIncrease" << i << "x5";
		bt = (UiButton*)FindGo(ss.str());
		bt->SetOrigin(Origins::MC);
		bt->SetPosition(713, 255 + (65 * i));
		bt->SetSize(0.45, 1);
		bt->sortLayer = 116;
		bt->OnClick = [this, i]() {
			equipCraftParts[i] = std::min(equipCraftParts[i] + 5, 20);
			UpdateCraftVal();
			};

		ss.str("");
		ss << "InputPartsBt" << i << "Text3";
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("+5");
		text->text.setCharacterSize(16);
		text->SetOrigin(Origins::MC);
		text->SetPosition(bt->GetPosition());
		text->sortLayer = 117;

		ss.str("");
		ss << "CraftUiUsingCount" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		text->text.setString("0");
		text->text.setCharacterSize(20);
		text->SetOrigin(Origins::MC);
		text->SetPosition(640, 255 + (65 * i));
		text->sortLayer = 117;

		ss.str("");
		ss << "CraftUiUsingPartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
		switch (i)
		{
		case 0:
			text->text.setString(stringtable->GetW("SoundChip"));
			break;
		case 1:
			text->text.setString(stringtable->GetW("Switch"));
			break;
		case 2:
			text->text.setString(stringtable->GetW("Screw"));
			break;
		case 3:
			text->text.setString(stringtable->GetW("Fabric"));
			break;
		}
		text->text.setCharacterSize(15);
		text->SetOrigin(Origins::ML);
		text->SetPosition(813, 286 + (36 * i));
		text->sortLayer = 117;
	}

	text = (TextGo*)FindGo("CraftUiCurMoneyText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString("0");
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MR);
	text->SetPosition(988, 184);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("CraftUiPartsHeadText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("UseParts"));
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::MC);
	text->SetPosition(880, 245);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("CraftUiUseDateText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("RemainingTime"));
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::ML);
	text->SetPosition(780, 428);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("CraftUiUseCostText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("PartsCost"));
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::ML);
	text->SetPosition(780, 455);
	text->sortLayer = 117;

	bt = (UiButton*)FindGo("OnCraftB");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5, 540);
	bt->SetSize(2, 2);
	bt->sortLayer = 116;

	text = (TextGo*)FindGo("CraftingBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("CraftGo"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 117;

	//Craft Result
	spr = (SpriteGo*)FindGo("CraftResultBack");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(FRAMEWORK.GetWindowSize() * 0.5f);
	spr->SetSize(2, 2);
	spr->sortLayer = 116;

	spr = (SpriteGo*)FindGo("CraftResultIcon");
	spr->SetOrigin(Origins::MC);
	spr->SetPosition(515, 337);
	spr->SetSize(2, 2);
	spr->sortLayer = 116;

	bt = (UiButton*)FindGo("CraftResultB");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5, 460);
	bt->SetSize(2, 2);
	bt->sortLayer = 116;

	text = (TextGo*)FindGo("CraftResultTitle");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("CheckResult"));
	text->text.setCharacterSize(30);
	text->SetOrigin(Origins::MC);
	text->SetPosition(FRAMEWORK.GetWindowSize().x * 0.5, 250);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("CraftResultName");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Headset0"));
	text->text.setCharacterSize(18);
	text->SetOrigin(Origins::ML);
	text->SetPosition(550, 315);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("CraftResultVal");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("Headset0"));
	text->text.setCharacterSize(16);
	text->SetOrigin(Origins::ML);
	text->SetPosition(550, 335);
	text->sortLayer = 117;

	text = (TextGo*)FindGo("CraftResultOkBText");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(stringtable->GetW("OK"));
	text->text.setCharacterSize(20);
	text->SetOrigin(Origins::MC);
	text->SetPosition(bt->GetPosition());
	text->sortLayer = 117;
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
		UiRecruitOpen();
		break;
	case 6:
		UiTrainingOpen();
		isMenuOn = true;//임시코드
		break;
	case 7:
		UiPlayerInfoOpen();
		break;
	case 8:
		UiSponsorContractOpen(false, true);
		break;
	case 9:
		UiEquipOpen();
		break;
	case 13:
		UpdatePopDate();
		popTextFade = maxPopTextFade;
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
	case 16:
	{
		// 작업중 AiBanPick바꿔서 많은 오류

		//AiBanPick banPick;

		//banPick.printStats(banPick.setChampions());

		//Champion highStatChampion = banPick.CompareHighStatChampion(banPick.getChampions()[0], banPick.getChampions()[1]);

		//Champion secondStatChampion = banPick.CompareSecondStatChampion(banPick.getChampions()[0], banPick.getChampions()[1]);


		//banPick.isBan(highStatChampion, true);
		//banPick.isPick(secondStatChampion, false);

	}
	break;
	case 19:
		SCENE_MGR.ChangeScene(SceneId::Title);
		break;
	case 20:
	{
		auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
		TEAM_MGR.Save();
		popTextFade = maxPopTextFade;
		TextGo* text = (TextGo*)FindGo("PopupText");
		text->text.setString(stringtable->GetW("IsSaved"));
	}
	break;
	default:
		break;
	}
	//isMenuOn = true;
	//나중에 주석풀기
}

void SceneHome::SubUiBaseOpen(int index, bool on)
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	isPopupOn = on;

	SpriteGo* spr = (SpriteGo*)FindGo("SubUiBack");
	spr->SetActive(on);

	UiButton* bt = (UiButton*)FindGo("TrainCloseB");
	bt->SetOrigin(Origins::MC);
	bt->SetPosition(1175, 555);
	bt->SetSize(2, 2);
	bt->sortLayer = 111;
	bt->OnClick = [this]() {
		isMenuOn = false;
		UiTrainingOpen(false);
		UiSponsorContractOpen(true, false);
		UiSponsorContractOpen(false, false);
		UiEquipOpen(false);
		UiPlayerInfoOpen(false);
		};
	bt->SetActive(on);

	TextGo* text = (TextGo*)FindGo("UiMenuTitleText");
	text->SetActive(on);
	std::stringstream ss;
	ss << "MainB" << index << "Text";
	text->text.setString(stringtable->GetW(ss.str()));

	text = (TextGo*)FindGo("UiMenuCloseText");
	text->SetActive(on);

	RectGo* rect = (RectGo*)FindGo("UiShade");
	rect->SetActive(on);
}

void SceneHome::UiRecruitOpen(bool on)
{
	isMenuOn = on;
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;
	bool recruitAble = on && TEAM_MGR.CheckRecruitSlot(0);
	PlayerInfo player = TEAM_MGR.GetRecruitSlotInfo(0);

	spr = (SpriteGo*)FindGo("RecruitBg");
	spr->SetActive(on);

	RectGo* rect = (RectGo*)FindGo("UiShade");
	rect->SetActive(on);

	spr = (SpriteGo*)FindGo("RecruitChamp0");
	spr->SetActive(recruitAble);
	if (recruitAble)
	{
		std::stringstream ss;
		ss << "graphics/UiFix/character_icons_" << player.proficiencyCode[0] << ".png";
		spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(ss.str()));
	}

	text = (TextGo*)FindGo("RecruitChampText0");
	text->SetActive(recruitAble);
	if (recruitAble)
	{
		text->text.setString(std::to_string(player.proficiencyLevel[0]));
		text->SetOrigin(Origins::MC);
	}

	for (int i = 1; i < 4; i++)
	{
		std::stringstream ss;
		ss << "RecruitChamp" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(recruitAble);
		if (recruitAble && i < player.knownChamp)
		{
			std::stringstream ss;
			ss << "graphics/UiFix/character_icons_" << player.proficiencyCode[i] << ".png";
			spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(ss.str()));
		}
		else
		{
			spr->SetActive(false);
		}

		ss.str("");
		ss << "RecruitChampText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(recruitAble);
		if (recruitAble && i < player.knownChamp)
		{
			text->text.setString(std::to_string(player.proficiencyLevel[i]));
			text->SetOrigin(Origins::MC);
		}
		else
		{
			text->SetActive(false);
		}
	}

	bt = (UiButton*)FindGo("RecruitB");
	bt->SetActive(on);
	bt->OnClick = [this, stringtable]() {
		if (!TEAM_MGR.CheckRecruitSlot(0))
		{
			if (TEAM_MGR.GetMoney() < 10)
			{
				return;
			}
			else
			{
				SoundGo* so = (SoundGo*)FindGo("CashSound");
				so->Play();
				Recruit(0, 0);
				UpdateMoney();
				UiRecruitOpen();
			}
		}
		else
		{
			if (TEAM_MGR.GetRecruitSlotInfo(0).contract_cost > TEAM_MGR.GetMoney())
			{
				return;
			}
			else if (!TEAM_MGR.IsCanRecruit())
			{
				UiRecruitOpen(false);
				TextGo* text = (TextGo*)FindGo("PopupText");
				text->text.setString(L"\n" + stringtable->GetW("PlayerIsMax"));
				popTextFade = maxPopTextFade;
				text->SetOrigin(Origins::MC);
			}
			else
			{
				SoundGo* so = (SoundGo*)FindGo("CashSound");
				so->Play();
				TEAM_MGR.Employ(0);
				UpdateMoney();
				UiRecruitOpen();
			}
		}
		};
	if (TEAM_MGR.GetMoney() < 10)
	{
		bt->sprite.setColor(sf::Color(100, 100, 100));
	}
	else if (recruitAble)
	{
		if (TEAM_MGR.GetRecruitSlotInfo(0).contract_cost > TEAM_MGR.GetMoney())
		{
			bt->sprite.setColor(sf::Color(100, 100, 100));
		}
		else
		{
			bt->sprite.setColor(sf::Color::White);
		}
	}
	else
	{
		bt->sprite.setColor(sf::Color::White);
	}


	text = (TextGo*)FindGo("RecruitBText");
	text->SetActive(on);
	if (recruitAble)
	{
		text->text.setString(stringtable->GetW("Recruit"));
		text->SetOrigin(Origins::MC);
	}
	else
	{
		text->text.setString(stringtable->GetW("PlayerSearch"));
		text->SetOrigin(Origins::MC);
	}


	bt = (UiButton*)FindGo("RecruitCanceleB");
	bt->SetActive(recruitAble);
	bt->OnClick = [this]() {
		TEAM_MGR.DeleteRecruitSlot(0);
		UiRecruitOpen(true);
		};

	text = (TextGo*)FindGo("RecruitCancelBText");
	text->SetActive(recruitAble);

	bt = (UiButton*)FindGo("RecruitCloseB");
	bt->SetActive(on);
	bt->OnClick = [this]() {
		UiRecruitOpen(false);
		};

	text = (TextGo*)FindGo("RecruitExitBText");
	text->SetActive(on);

	text = (TextGo*)FindGo("RecruitName");
	text->SetActive(recruitAble);
	if (recruitAble)
	{
		text->text.setString(player.name);
		text->SetOrigin(Origins::ML);
	}

	text = (TextGo*)FindGo("RecruitAge");
	text->SetActive(recruitAble);
	if (recruitAble)
	{
		text->text.setString(std::to_wstring(player.age) + stringtable->GetW("AgeCount"));
		text->SetOrigin(Origins::MC);
	}

	text = (TextGo*)FindGo("RecruitCost");
	text->SetActive(recruitAble);
	if (recruitAble)
	{
		text->text.setString(std::to_wstring(player.contract_cost));
		text->SetOrigin(Origins::ML);
	}

	text = (TextGo*)FindGo("RecruitAtk");
	text->SetActive(recruitAble);
	if (recruitAble)
	{
		text->text.setString(std::to_wstring(player.attack));
		text->SetOrigin(Origins::ML);
	}

	text = (TextGo*)FindGo("RecruitDef");
	text->SetActive(recruitAble);
	if (recruitAble)
	{
		text->text.setString(std::to_wstring(player.defence));
		text->SetOrigin(Origins::ML);
	}
}

void SceneHome::UiPlayerInfoOpen(bool on)
{
	selectedSponsorIndex = -1;
	SubUiBaseOpen(7, on);
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	int playerCount = TEAM_MGR.GetPlayerNum();
	std::vector<PlayerInfo> playerInfo = TEAM_MGR.GetPlayerInfo();
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;

	spr = (SpriteGo*)FindGo("SubUiBack");
	spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/squad_ui_bg.png"));


	for (int i = 0; i < playerCount; i++)
	{
		std::stringstream ss;
		ss << "PlayerInfoB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(on);
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/slot_normal&hover_0.png"));

		for (int j = 0; j < playerInfo[i].knownChamp; j++)
		{
			SpriteGo* icon;
			std::stringstream ss2;
			ss2 << "Player" << i << "InfoCharacterIcons" << j;
			icon = (SpriteGo*)FindGo(ss2.str());
			icon->SetActive(on);
			std::stringstream champ;
			champ << "graphics/UiFix/character_icons_" <<
				playerInfo[i].proficiencyCode[j] << ".png";
			icon->sprite.setTexture((*RESOURCE_MGR.GetTexture(champ.str())));
			sf::Vector2u texSize = icon->sprite.getTexture()->getSize();
			icon->sprite.setTextureRect({ std::max(0,(int)texSize.x / 2 - 12), 0,25,25 });

			ss2 << "Back";
			spr = (SpriteGo*)FindGo(ss2.str());
			spr->SetActive(on);

			ss2 << "Line";
			spr = (SpriteGo*)FindGo(ss2.str());
			spr->SetActive(on);

			text = (TextGo*)FindGo("PlayerInfoT" + std::to_string(i)
				+ "ChampLV" + std::to_string(j));
			text->SetActive(on);
			text->text.setString(std::to_string(playerInfo[i].proficiencyLevel[j]));
			text->SetOrigin(Origins::MC);
		}
		for (int j = playerInfo[i].knownChamp; j < 4; j++)
		{
			SpriteGo* icon;
			std::stringstream ss2;
			ss2 << "Player" << i << "InfoCharacterIcons" << j;
			icon = (SpriteGo*)FindGo(ss2.str());
			icon->SetActive(false);

			ss2 << "Back";
			spr = (SpriteGo*)FindGo(ss2.str());
			spr->SetActive(false);

			ss2 << "Line";
			spr = (SpriteGo*)FindGo(ss2.str());
			spr->SetActive(false);

			text = (TextGo*)FindGo("PlayerInfoT" + std::to_string(i)
				+ "ChampLV" + std::to_string(j));
			text->SetActive(false);
		}


		text = (TextGo*)FindGo(("PlayerInfoTName" + std::to_string(i)));
		text->SetActive(on);
		text->text.setString(playerInfo[i].name);
		text->SetOrigin(Origins::ML);

		text = (TextGo*)FindGo(("PlayerInfoTAge" + std::to_string(i)));
		text->SetActive(on);
		text->text.setString(std::to_string(playerInfo[i].age));
		text->SetOrigin(Origins::ML);

		text = (TextGo*)FindGo(("PlayerInfoTAtk" + std::to_string(i)));
		text->SetActive(on);
		text->text.setString(std::to_string(playerInfo[i].attack));
		text->SetOrigin(Origins::ML);

		text = (TextGo*)FindGo(("PlayerInfoTDef" + std::to_string(i)));
		text->SetActive(on);
		text->text.setString(std::to_string(playerInfo[i].defence));
		text->SetOrigin(Origins::ML);

		text = (TextGo*)FindGo(("PlayerInfoTCost" + std::to_string(i)));
		text->SetActive(on);
		text->text.setString(std::to_string(playerInfo[i].contract_cost));
		text->SetOrigin(Origins::MR);
	}

	for (int i = playerCount; i < 6; i++)
	{
		std::stringstream ss;
		ss << "PlayerInfoB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);

		for (int j = 0; j < 4; j++)
		{
			SpriteGo* icon;
			std::stringstream ss2;
			ss2 << "Player" << i << "InfoCharacterIcons" << j;
			icon = (SpriteGo*)FindGo(ss2.str());
			icon->SetActive(false);

			ss2 << "Back";
			spr = (SpriteGo*)FindGo(ss2.str());
			spr->SetActive(false);

			ss2 << "Line";
			spr = (SpriteGo*)FindGo(ss2.str());
			spr->SetActive(false);

			text = (TextGo*)FindGo("PlayerInfoT" + std::to_string(i)
				+ "ChampLV" + std::to_string(j));
			text->SetActive(false);
		}

		text = (TextGo*)FindGo(("PlayerInfoTName" + std::to_string(i)));
		text->SetActive(false);

		text = (TextGo*)FindGo(("PlayerInfoTAge" + std::to_string(i)));
		text->SetActive(false);

		text = (TextGo*)FindGo(("PlayerInfoTAtk" + std::to_string(i)));
		text->SetActive(false);

		text = (TextGo*)FindGo(("PlayerInfoTDef" + std::to_string(i)));
		text->SetActive(false);

		text = (TextGo*)FindGo(("PlayerInfoTCost" + std::to_string(i)));
		text->SetActive(false);
	}

	bt = (UiButton*)FindGo("PlayerFireB");
	bt->SetActive(on);
	if (TEAM_MGR.GetPlayerNum() > 4)
	{
		bt->sprite.setColor(sf::Color::White);
	}
	else
	{
		bt->sprite.setColor(sf::Color(100, 100, 100));
	}

	text = (TextGo*)FindGo("PlayerInfoCount");
	text->SetActive(on);
	std::wstringstream wss;
	wss << stringtable->GetW("HavePlayers") << TEAM_MGR.GetPlayerNum() << L"/"
		<< TEAM_MGR.MaxPlayerNum();
	text->text.setString(wss.str());

	text = (TextGo*)FindGo("FireBText");
	text->SetActive(on);
}

void SceneHome::UiTrainingOpen(bool on)
{
	SubUiBaseOpen(6, on);
	TextGo* text;
	SpriteGo* spr = (SpriteGo*)FindGo("SubUiBack");
	spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/training_ui_bg.png"));

	text = (TextGo*)FindGo("TrainPlayerListName");
	text->SetActive(on);

	text = (TextGo*)FindGo("TrainPlayerListPoint");
	text->SetActive(on);

	text = (TextGo*)FindGo("TrainByUsePoint");
	text->SetActive(false);

	text = (TextGo*)FindGo("TrainPlayerName");
	text->SetActive(false);

	text = (TextGo*)FindGo("TrainPlayerAge");
	text->SetActive(false);

	text = (TextGo*)FindGo("TrainPlayerDef");
	text->SetActive(false);

	text = (TextGo*)FindGo("TrainPlayerAtk");
	text->SetActive(false);

	text = (TextGo*)FindGo("TrainPlayerLeftPointText");
	text->SetActive(false);

	text = (TextGo*)FindGo("TrainPlayerReturnBText");
	text->SetActive(false);

	for (int i = 0; i < 6; i++)
	{
		std::stringstream ss;
		ss << "TrainingSlotUi" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);

		ss << "Name";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss << "Point";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

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
		ss.str("");
		ss << "CharacterStatLevel" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);
	}
	spr = (SpriteGo*)FindGo("IconAtk0");
	spr->SetActive(on);

	spr = (SpriteGo*)FindGo("IconDef0");
	spr->SetActive(on);

	UiButton* bt = (UiButton*)FindGo("TrainPointReturnB");
	bt->SetActive(false);

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

		ss << "Name";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);
		text->text.setString(TEAM_MGR.GetPlayerInfo()[i].name);

		ss << "Point";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);
		text->text.setString(std::to_string(TEAM_MGR.GetMaxTrainingPoint()
			- TEAM_MGR.GetTrainingInfo()[i].usedTrainingPoint));
	}
	for (int i = playerNum; i < 12; i++)
	{
		std::stringstream ss;
		ss << "PlayerList" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);

		ss << "Name";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss << "Point";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);
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
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);

	PlayerInfo selectedPlayer = TEAM_MGR.GetPlayerInfo()[index];
	TrainingInfo* tInfo = &gainTrainingInfo[index];
	SpriteGo* spr;
	TextGo* text;

	text = (TextGo*)FindGo("TrainByUsePoint");
	text->SetActive(true);

	text = (TextGo*)FindGo("TrainPlayerName");
	text->SetActive(true);
	text->text.setString(selectedPlayer.name);

	text = (TextGo*)FindGo("TrainPlayerAge");
	text->SetActive(true);
	text->text.setString(std::to_wstring(selectedPlayer.age)
		+ stringtable->GetW("AgeCount"));

	text = (TextGo*)FindGo("TrainPlayerDef");
	text->SetActive(true);
	text->text.setString(std::to_string(selectedPlayer.defence));

	text = (TextGo*)FindGo("TrainPlayerAtk");
	text->SetActive(true);
	text->text.setString(std::to_string(selectedPlayer.attack));

	text = (TextGo*)FindGo("TrainPlayerLeftPointText");
	text->SetActive(true);

	text = (TextGo*)FindGo("TrainPlayerReturnBText");
	text->SetActive(true);

	for (int i = 0; i < 2 + selectedPlayer.knownChamp; i++)
	{
		std::stringstream ss;
		ss << "TrainingSlotUi" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(true);

		ss << "Name";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);
		switch (i)
		{
		case 0:
			text->text.setString(stringtable->GetW("Attack"));
			break;
		case 1:
			text->text.setString(stringtable->GetW("Defence"));
			break;
		default:
		{
			std::stringstream champName;
			champName << "ChampName" << selectedPlayer.proficiencyCode[i - 2];
			text->text.setString(stringtable->GetW(champName.str()));
		}
		break;
		}


		ss << "Point";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);

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
				/ (1300 + (selectedPlayer.attack * 10)), 2.f);
			break;
		case 1:
			xpBar->SetSize(tInfo->xpDef * 2.f
				/ (1300 + (selectedPlayer.defence * 10)), 2.f);
			break;
		default:
			xpBar->SetSize(tInfo->xpChamp[i - 2] * 2.f
				/ (1300 + (selectedPlayer.proficiencyLevel[i - 2] * 10)), 2.f);
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
				/ (1300 + (selectedPlayer.defence * 10)), 2.f), 2.f);
			break;
		default:
			xpBar->SetSize(std::min(futureTrainingInfo.xpChamp[i - 2] * 2.f
				/ (1300 + (selectedPlayer.proficiencyLevel[i - 2] * 10)), 2.f)
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
			trainPointB->OnClick = [this, index, i]() {
				if (0 < gainTrainingInfo[index].trainingChamp[i - 2])
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
					< maxTrainingPoint)
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
			champ << "graphics/UiFix/character_icons_" <<
				selectedPlayer.proficiencyCode[i - 2] << ".png";
			charIcon->sprite.setTexture((*RESOURCE_MGR.GetTexture(champ.str())));
			sf::Vector2u texSize = charIcon->sprite.getTexture()->getSize();
			charIcon->sprite.setTextureRect({ std::max(0,(int)texSize.x / 2 - 12), 0,25,25 });
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

		ss << "Name";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss << "Point";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

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
		if (i < maxTrainingPoint)
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
			champ << "graphics/UiFix/character_icons_" <<
				selectedPlayer.proficiencyCode[i] << ".png";
			characterIcons->sprite.setTexture((*RESOURCE_MGR.GetTexture(champ.str())));
			sf::Vector2u texSize = characterIcons->sprite.getTexture()->getSize();
			characterIcons->sprite.setTextureRect({ std::max(0,(int)texSize.x / 2 - 12), 0,25,25 });

			ss << "Back";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetActive(true);
			ss << "Line";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetActive(true);
			ss.str("");
			ss << "CharacterStatLevel" << i;
			text = (TextGo*)FindGo(ss.str());
			text->text.setString(std::to_string(selectedPlayer.proficiencyLevel[i]));
			text->SetActive(true);
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
			ss.str("");
			ss << "CharacterStatLevel" << i;
			text = (TextGo*)FindGo(ss.str());
			text->SetActive(false);
		}
	}

	UiButton* bt = (UiButton*)FindGo("TrainPointReturnB");
	bt->SetActive(true);
	bt->OnClick = [this, index]() {
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
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	TextGo* text;
	PlayerInfo selectedPlayer = TEAM_MGR.GetPlayerInfo()[index];
	TrainingInfo* tInfo = &gainTrainingInfo[index];

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
		if (i < maxTrainingPoint - gainTrainingInfo[index].usedTrainingPoint)
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
		if (i - 8 < gainTrainingInfo[index].trainingDef)
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

	std::stringstream ss;
	ss << "PlayerList" << index << "Name" << "Point";
	text = (TextGo*)FindGo(ss.str());
	text->text.setString(std::to_string(TEAM_MGR.GetMaxTrainingPoint() -
		tInfo->usedTrainingPoint));


	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "TrainingSlotUi" << i << "Name" << "Point";
		text = (TextGo*)FindGo(ss.str());
		switch (i)
		{
		case 0:
			text->text.setString(std::to_string(tInfo->trainingAtk));
			break;
		case 1:
			text->text.setString(std::to_string(tInfo->trainingDef));
			break;
		default:
			text->text.setString(std::to_string(tInfo->trainingChamp[i - 2]));
			break;
		}
	}
}

void SceneHome::UiSponsorContractOpen(bool contract, bool on)
{
	isMenuOn = on;
	isPopupOn = on;
	selectedSponsorIndex = -1;
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	SubUiBaseOpen(8, on);

	SpriteGo* spr = (SpriteGo*)FindGo("SubUiBack");
	spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/sponsor_ui_bg.png"));

	spr = (SpriteGo*)FindGo("SponLogoBack");
	spr->SetActive(false);

	spr = (SpriteGo*)FindGo("SelectedLogo");
	spr->SetActive(false);

	spr = (SpriteGo*)FindGo("GoldUiEx");
	spr->SetActive(false);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "PartBack" << i;
		spr = (UiButton*)FindGo(ss.str());
		spr->SetActive(false);
	}

	spr = (SpriteGo*)FindGo("SoundChip");
	spr->SetActive(false);

	spr = (SpriteGo*)FindGo("KeyboardSwitch");
	spr->SetActive(false);

	spr = (SpriteGo*)FindGo("Screw");
	spr->SetActive(false);

	spr = (SpriteGo*)FindGo("FabricPiece");
	spr->SetActive(false);

	TextGo* text = (TextGo*)FindGo("SelectedSponsorName");
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorQuest");
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorMoney");
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorQuestUiText");
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorRewardUiText");
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorRewardMoneyUiText");
	text->SetActive(false);

	text = (TextGo*)FindGo("SponsorQuestNowText");
	text->SetActive(false);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "SponsorParts" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);
	}

	UiButton* bt = (UiButton*)FindGo("TrainCloseB");
	bt->SetActive(on);

	RectGo* rect = (RectGo*)FindGo("UiShade");
	rect->SetActive(on);

	if (contract)
	{

		text = (TextGo*)FindGo("UiMenuTitleText");
		text->text.setString(stringtable->GetW("SponContract"));

		newSponsor = std::vector<Sponsor>(9);

		for (int i = 0; i < 9; i++)
		{
			int sponsorType = 0;
			while (true)
			{
				bool sameCheck = true;
				sponsorType = Utils::RandomRange(0, 18);
				for (int j = 0; j < i; j++)
				{
					if (sponsorType == newSponsor[j].sponsorType)
					{
						sameCheck = false;
						break;
					}
				}
				if (sameCheck)
				{
					break;
				}
			}
			newSponsor[i].sponsorType = sponsorType;
			std::stringstream ss;
			ss << "SponsorName" << sponsorType;
			newSponsor[i].sponsorName = stringtable->Get(ss.str());
			ss.str("");
			ss << "graphics/Origin/Sprite/sponser_logo_" << sponsorType << ".png";
			newSponsor[i].sponsorTextureId = ss.str();
			newSponsor[i].questCode = Utils::RandomRange(0, TEAM_MGR.GetQuestCount() - 1);
			newSponsor[i].questDifficulty = Utils::RandomRange(0, 2);
			newSponsor[i].rewardMoney = Utils::RandomRange(1000, 1500) + (newSponsor[i].questDifficulty * 500) + (int)TEAM_MGR.GetLeagueGrade() * 1000;
			for (int j = 0; j < 4; j++)
			{
				newSponsor[i].rewardParts[j] = Utils::RandomRange(7, 17) + (newSponsor[i].questDifficulty * 5) + (int)TEAM_MGR.GetLeagueGrade() * 5;
			}
		}

		for (int i = 0; i < 9; i++)
		{
			std::stringstream ss;
			ss << "SponSlot" << i;
			UiButton* bt = (UiButton*)FindGo(ss.str());
			bt->SetActive(on);
			bt->OnClick = [this, i]() {
				UiSponsorContractSelect(newSponsor[i], i);
				};

			ss.str("");
			ss << "SponLogo" << i;
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetActive(on);
			spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(newSponsor[i].sponsorTextureId));

			ss.str("");
			ss << "SponsorName" << i;
			TextGo* text = (TextGo*)FindGo(ss.str());
			text->SetActive(on);
			{
				std::stringstream ss2;
				ss2 << "SponsorName" << newSponsor[i].sponsorType;
				text->text.setString(stringtable->GetW(ss2.str()));
			}
		}
		bt = (UiButton*)FindGo("SponsorContract");
		bt->SetOrigin(Origins::MC);
		bt->SetPosition(485, 470);
		bt->SetSize(2, 2);
		bt->sortLayer = 111;
		bt->SetActive(on);

		text = (TextGo*)FindGo("SponsorContractDescribe");
		text->SetActive(on);

		text = (TextGo*)FindGo("SelectedSponsorCountText");
		text->text.setString(stringtable->GetW("SelectedSponsor"));
		text->text.setString(text->text.getString() + " "
			+ std::to_string(TEAM_MGR.GetContractedSponsor()) + "/"
			+ std::to_string(TEAM_MGR.GetMaxSponsor()));
		text->SetActive(on);

		text = (TextGo*)FindGo("SponsorContractAcceptText");
		text->text.setFillColor(sf::Color::White);
		text->SetActive(on);

		text = (TextGo*)FindGo("SponsorContractLeftWarnText");
		text->SetActive(on);
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			std::stringstream ss;
			ss << "UseSponlist" << i;
			UiButton* SponBt = (UiButton*)FindGo(ss.str());
			SponBt->SetActive(on);
			SponBt->OnClick = [this, i]() {
				if (TEAM_MGR.GetMaxSponsor() <= i)
				{
					return;
				}
				UiSponsorSelect(i);
				for (int j = 0; j < 3; j++)
				{
					std::stringstream ss;
					ss << "UseSponlist" << i;
					UiButton* SponBt = (UiButton*)FindGo(ss.str());
					if (j != selectedSponsorIndex)
					{
						SponBt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/type_ui_2.png"));
					}
					else
					{
						SponBt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/type_ui_0.png"));
					}
				}
				};
			SponBt->OnEnter = [SponBt, this, i]() {
				if (TEAM_MGR.GetMaxSponsor() <= i ||
					selectedSponsorIndex == i)
				{
					return;
				}
				SponBt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/type_ui_1.png"));
				};
			SponBt->OnExit = [SponBt, this, i]() {
				if (TEAM_MGR.GetMaxSponsor() <= i ||
					selectedSponsorIndex == i)
				{
					return;
				}
				SponBt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/type_ui_0.png"));
				};

			if (i > 0 && i >= TEAM_MGR.GetMaxSponsor())
			{
				ss.str("");
				ss << "LockIcon" << i - 1;
				spr = (SpriteGo*)FindGo(ss.str());
				spr->SetActive(on);
				ss.str("");
				ss << "SponsorLockText" << i - 1;
				TextGo* text = (TextGo*)FindGo(ss.str());
				text->SetActive(on);
			}
			if (i < TEAM_MGR.GetMaxSponsor() && TEAM_MGR.GetSponsor(i).sponsorType != -1)
			{
				ss.str("");
				ss << "ListSponsorLogo" << i;
				spr = (SpriteGo*)FindGo(ss.str());
				spr->SetActive(on);
				spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(TEAM_MGR.GetSponsor(i).sponsorTextureId));

				ss << "back";
				spr = (SpriteGo*)FindGo(ss.str());
				spr->SetActive(on);

				ss.str("");
				ss << "SponsorSlotText" << i;
				TextGo* text = (TextGo*)FindGo(ss.str());
				text->SetActive(on);
				{
					std::stringstream ss;
					ss << "SponsorName" << TEAM_MGR.GetSponsor(i).sponsorType;
					text->text.setString(stringtable->GetW(ss.str()));
				}

				ss << "State";
				text = (TextGo*)FindGo(ss.str());
				text->SetActive(on);
				if (TEAM_MGR.GetSponsor(i).success)
				{
					text->text.setString(stringtable->GetW("Success"));
				}
				else
				{
					text->text.setString(stringtable->GetW("Processing"));
				}
			}
		}

		text = (TextGo*)FindGo("SponsorNameUi");
		text->SetActive(on);

		text = (TextGo*)FindGo("SponsorStateUi");
		text->SetActive(on);
	}
}

void SceneHome::UiSponsorContractSelect(Sponsor sponsor, int index)
{
	selectedSponsorIndex = index;
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	for (int i = 0; i < 9; i++)
	{
		std::stringstream ss;
		ss << "SponSlot" << i;
		UiButton* bt = (UiButton*)FindGo(ss.str());
		if (i == index)
		{
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/sponsor_card_bg_2.png"));
		}
		else
		{
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/sponsor_card_bg_0.png"));
		}
	}
	SpriteGo* spr = (SpriteGo*)FindGo("SelectedLogo");
	spr->SetActive(true);
	spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(sponsor.sponsorTextureId));

	TextGo* text = (TextGo*)FindGo("SelectedSponsorName");
	text->SetActive(true);
	{
		std::stringstream ss;
		ss << "SponsorName" << sponsor.sponsorType;
		text->text.setString(stringtable->GetW(ss.str()));
	}

	text = (TextGo*)FindGo("SponsorQuest");
	text->SetActive(true);
	std::stringstream ss;
	ss << "ContractConditions" << sponsor.questCode;
	text->text.setString(stringtable->GetW(ss.str()));
	switch (sponsor.questCode)
	{
	case 0:
		text->text.setString(text->text.getString() + " " + std::to_string(3 - sponsor.questDifficulty));
		break;
	case 1:
	case 2:
		text->text.setString(text->text.getString() + " " + std::to_string(5 + sponsor.questDifficulty * 2));
		break;
	case 3:
	case 4:
		text->text.setString(text->text.getString() + " " + std::to_string(300 + sponsor.questDifficulty * 50));
	default:
		break;
	}

	text = (TextGo*)FindGo("SponsorMoney");
	text->SetActive(true);
	text->text.setString(std::to_string(sponsor.rewardMoney));

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "SponsorParts" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);
		text->text.setString("x" + std::to_string(sponsor.rewardParts[i]));
	}

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "PartBack" << i;
		spr = (UiButton*)FindGo(ss.str());
		spr->SetActive(true);
	}

	spr = (SpriteGo*)FindGo("SoundChip");
	spr->SetActive(true);

	spr = (SpriteGo*)FindGo("KeyboardSwitch");
	spr->SetActive(true);

	spr = (SpriteGo*)FindGo("Screw");
	spr->SetActive(true);

	spr = (SpriteGo*)FindGo("FabricPiece");
	spr->SetActive(true);

	spr = (SpriteGo*)FindGo("SponLogoBack");
	spr->SetActive(true);

	spr = (SpriteGo*)FindGo("GoldUiEx");
	spr->SetActive(true);

	text = (TextGo*)FindGo("SponsorRewardMoneyUiText");
	text->SetActive(true);

	text = (TextGo*)FindGo("SponsorQuestUiText");
	text->SetActive(true);

	text = (TextGo*)FindGo("SponsorRewardUiText");
	text->SetActive(true);
}

void SceneHome::UpdateMoney()
{
	TextGo* text = (TextGo*)FindGo("MoneyInfoT");
	text->text.setString(std::to_string(TEAM_MGR.GetMoney()));
	text->SetOrigin(Origins::TL);
}

void SceneHome::Recruit(int grade, int slotNum)
{
	switch (grade)
	{
	case -1:
		TEAM_MGR.Recruit(slotNum, MakeDefaultPlayer());
	case 0:
		if (TEAM_MGR.GetMoney() < 10 || TEAM_MGR.CheckRecruitSlot(slotNum) == true)
		{
			return;
		}
		TEAM_MGR.Recruit(slotNum, MakeLocalPlayer());
		TEAM_MGR.UseMoney(10);
		break;
	default:
		break;
	}

}

void SceneHome::NewYear()
{
	std::vector<AiTeam> newAiTeams = std::vector<AiTeam>(7);
	for (int i = 0; i < 7; i++)
	{
		std::stringstream ss;
		ss << "Team" << i + 1;
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
				newAiTeams[i].player[j] = MakeLocalPlayer();
			}
		}
		break;
	default:
		break;
	}
	TEAM_MGR.SetAiTeams(newAiTeams);
}

PlayerInfo SceneHome::MakeDefaultPlayer()
{
	PlayerInfo player;
	{
		std::stringstream ss;
		ss << "PlayerName" << Utils::RandomRange(0, 9);
		auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
		player.name = stringtable->Get(ss.str());
	}
	player.age = Utils::RandomRange(19, 22);
	player.attack = Utils::RandomRange(6, 9);
	player.defence = 15 - player.attack;
	player.knownChamp = Utils::RandomRange(1, 3);
	for (int i = 0; i < player.knownChamp; i++)
	{
		while (true)
		{
			int champCode = Utils::RandomRange(0, TEAM_MGR.GetAbleChamps() - 1);
			bool checker = true;
			for (int j = 0; j < i; j++)
			{
				if (player.proficiencyCode[j] == champCode)
				{
					checker = false;
				}
			}
			if (checker)
			{
				player.proficiencyCode.push_back(champCode);
				player.proficiencyLevel.push_back(Utils::RandomRange(2, 4));
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
	player.contract_cost = 0;
	player.potential = 0;

	return player;
}

void SceneHome::SponsorContract(int index)
{
	for (size_t i = 0; i < 3; i++)
	{
		if (TEAM_MGR.GetSponsor(i).sponsorType == newSponsor[index].sponsorType)
		{
			return;
		}
	}

	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	TEAM_MGR.ContractSponsor(newSponsor[index]);

	if (TEAM_MGR.GetMaxSponsor() == TEAM_MGR.GetContractedSponsor())
	{
		TextGo* text = (TextGo*)FindGo("SponsorContractLeftWarnText");
		text->SetActive(false);
		text = (TextGo*)FindGo("SponsorContractAcceptText");
		text->text.setFillColor(sf::Color(100, 100, 100));
		UiButton* bt = (UiButton*)FindGo("SponsorContract");
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/default_button_3.png"));
	}
	TextGo* text = (TextGo*)FindGo("SelectedSponsorCountText");
	text->text.setString(stringtable->GetW("SelectedSponsor"));
	text->text.setString(text->text.getString() + " "
		+ std::to_string(TEAM_MGR.GetContractedSponsor()) + "/"
		+ std::to_string(TEAM_MGR.GetMaxSponsor()));
}

void SceneHome::UiSponsorSelect(int index)
{
	Sponsor sponsor = TEAM_MGR.GetSponsor(index);
	if (sponsor.sponsorType == -1)
	{
		return;
	}
	selectedSponsorIndex = index;
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	SpriteGo* spr = (SpriteGo*)FindGo("SelectedLogo");
	spr->SetActive(true);
	spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(sponsor.sponsorTextureId));

	TextGo* text = (TextGo*)FindGo("SelectedSponsorName");
	text->SetActive(true);
	{
		std::stringstream ss;
		ss << "SponsorName" << sponsor.sponsorType;
		text->text.setString(stringtable->GetW(ss.str()));
	}

	text = (TextGo*)FindGo("SponsorQuest");
	text->SetActive(true);
	std::stringstream ss;
	ss << "ContractConditions" << sponsor.questCode;
	text->text.setString(stringtable->GetW(ss.str()));
	switch (sponsor.questCode)
	{
	case 0:
		text->text.setString(text->text.getString() + " " + std::to_string(3 - sponsor.questDifficulty));
		break;
	case 1:
	case 2:
		text->text.setString(text->text.getString() + " " + std::to_string(5 + sponsor.questDifficulty * 2));
		break;
	case 3:
	case 4:
		text->text.setString(text->text.getString() + " " + std::to_string(300 + sponsor.questDifficulty * 50));
	default:
		break;
	}


	text = (TextGo*)FindGo("SponsorMoney");
	text->SetActive(true);
	text->text.setString(std::to_string(sponsor.rewardMoney));

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "SponsorParts" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(true);
		text->text.setString("x" + std::to_string(sponsor.rewardParts[i]));
	}

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "PartBack" << i;
		spr = (UiButton*)FindGo(ss.str());
		spr->SetActive(true);
	}

	spr = (SpriteGo*)FindGo("SoundChip");
	spr->SetActive(true);

	spr = (SpriteGo*)FindGo("KeyboardSwitch");
	spr->SetActive(true);

	spr = (SpriteGo*)FindGo("Screw");
	spr->SetActive(true);

	spr = (SpriteGo*)FindGo("FabricPiece");
	spr->SetActive(true);

	spr = (SpriteGo*)FindGo("SponLogoBack");
	spr->SetActive(true);

	spr = (SpriteGo*)FindGo("GoldUiEx");
	spr->SetActive(true);

	text = (TextGo*)FindGo("SponsorRewardMoneyUiText");
	text->SetActive(true);

	text = (TextGo*)FindGo("SponsorQuestUiText");
	text->SetActive(true);

	text = (TextGo*)FindGo("SponsorRewardUiText");
	text->SetActive(true);

	text = (TextGo*)FindGo("SponsorQuestNowText");
	text->SetActive(true);
	text->text.setString(stringtable->GetW("Now"));
	text->text.setString(text->text.getString() + " " + std::to_string(TEAM_MGR.GetSponsor(index).currentProcess));
}


PlayerInfo SceneHome::MakeLocalPlayer()
{
	PlayerInfo player;
	{
		std::stringstream ss;
		ss << "PlayerName" << Utils::RandomRange(0, 9);
		auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
		player.name = stringtable->Get(ss.str());
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
				if (player.proficiencyCode[j] == champCode)
				{
					checker = false;
				}
			}
			if (checker)
			{
				player.proficiencyCode.push_back(champCode);
				player.proficiencyLevel.push_back(Utils::RandomRange(5, 7));
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

	return player;
}

void SceneHome::UpdatePopDate()
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	TextGo* text = (TextGo*)FindGo("PopupText");
	std::wstringstream wss;
	wss << TEAM_MGR.GetTodayYear() << stringtable->GetW("Year") << " " <<
		TEAM_MGR.GetTodayDate() / 4 + 1 << stringtable->GetW("Month") << " " <<
		TEAM_MGR.GetTodayDate() % 4 + 1 << stringtable->GetW("Weekday") << std::endl;
	switch (TEAM_MGR.GetSchedule(TEAM_MGR.GetTodayDate()))
	{
	case TeamMgr::Schedule::Recruit:
		wss << stringtable->GetW("RecruitDay");
		break;
	case TeamMgr::Schedule::League:
		wss << stringtable->GetW("LeagueDay");
		break;
	case TeamMgr::Schedule::Vacation:
		wss << stringtable->GetW("VacationDay");
		break;
	}
	text->text.setString(wss.str());
	text->SetOrigin(Origins::TC);
}

void SceneHome::PopTextUpdate(float dt)
{
	popTextFade = std::max(popTextFade - dt, 0.f);
	float popColor = 255 * (popTextFade * 2 / maxPopTextFade);
	TextGo* text = (TextGo*)FindGo("PopupText");
	RectGo* rect = (RectGo*)FindGo("PopupTextShade");
	if (popTextFade > maxPopTextFade / 2.f)
	{
		text->text.setFillColor(sf::Color::White);
		text->text.setOutlineColor(sf::Color::Black);
		rect->rectangle.setFillColor(sf::Color(56, 58, 64, 200));
	}
	else
	{
		text->text.setFillColor(sf::Color(popColor, popColor, popColor, popColor));
		text->text.setOutlineColor((sf::Color(0, 0, 0, popColor)));
		rect->rectangle.setFillColor(sf::Color(56, 58, 64, std::max(popColor - 55.f, 0.f)));
	}

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

void SceneHome::ScrollEquip(float dt)
{
	if (INPUT_MGR.GetMouseWheel(MouseWheelDir::WheelUp))
	{
		SpriteGo* spr;
		UiButton* bt;
		
		bt = (UiButton*)FindGo("EquipItemB0");
		if (bt->GetPosition().y > 200)
		{
			return;
		}

		for (int i = 0; i < 38; i++)
		{
			std::stringstream ss;
			ss.str("");
			ss << "EquipItemB" << i;
			bt = (UiButton*)FindGo(ss.str());
			bt->SetPosition(bt->GetPosition() + sf::Vector2f{ 0,1 }*50.f);

			ss << "Sprite";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetPosition(bt->GetPosition());

			ss << "Lock";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetPosition(bt->GetPosition().x + 15, bt->GetPosition().y - 15);
		}
	}
	else if (INPUT_MGR.GetMouseWheel(MouseWheelDir::WheelDown))
	{
		SpriteGo* spr;
		UiButton* bt;

		bt = (UiButton*)FindGo("EquipItemB37");
		if (bt->GetPosition().y < 465)
		{
			return;
		}

		for (int i = 0; i < 38; i++)
		{
			std::stringstream ss;
			ss.str("");
			ss << "EquipItemB" << i;
			bt = (UiButton*)FindGo(ss.str());
			bt->SetPosition(bt->GetPosition() + sf::Vector2f{ 0,-1 }*50.f);

			ss << "Sprite";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetPosition(bt->GetPosition());

			ss << "Lock";
			spr = (SpriteGo*)FindGo(ss.str());
			spr->SetPosition(bt->GetPosition().x + 15, bt->GetPosition().y - 15);
		}
	}
	EquipListShower();
}

void SceneHome::EquipListShower()
{
	SpriteGo* spr;
	UiButton* bt;
	for (int i = 0; i < useItemCount; i++)
	{
		std::stringstream ss;
		ss.str("");
		ss << "EquipItemB" << i;
		bt = (UiButton*)FindGo(ss.str());
		float pos = bt->GetPosition().y;
		bool available = pos > 110 && pos < 530;

		bt->SetActive(available);

		ss << "Sprite";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(available);

		ss << "Lock";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(available);

	}

}

void SceneHome::ResetSky()
{
	sky = (sky + 1) % 3;
	SpriteGo* spr = (SpriteGo*)FindGo("Sky");
	switch (sky)
	{
	case 0:
		spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/sky_day.png"));
		break;
	case 1:
		spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/sky_sunset.png"));
		break;
	case 2:
		spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/sky_night.png"));
		break;
	}

	SetClouds();
}

void SceneHome::SetClouds()
{
	for (int i = 0; i < 7; i++)
	{
		std::stringstream ss;
		ss << "Clouds" << i;
		SpriteGo* spr = (SpriteGo*)FindGo(ss.str());
		ss.str("");
		ss << "graphics/Origin/Sprite/clouds_" << Utils::RandomRange(0, 7) * 3 + sky
			<< ".png";
		spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(ss.str()));
		spr->sprite.setTextureRect({ 0,0,(int)spr->sprite.getTexture()->getSize().x,(int)spr->sprite.getTexture()->getSize().y });
		spr->SetOrigin(Origins::MC);
		spr->SetPosition(Utils::RandomRange(0.f, FRAMEWORK.GetWindowSize().x),
			Utils::RandomRange(75.f, 460.f));
		spr->SetSize(2, 2);
		cloudSpeed[i] = Utils::RandomRange(0.5f, 1.f);
	}
}

void SceneHome::UpdateClouds(float dt)
{
	for (int i = 0; i < 7; i++)
	{
		std::stringstream ss;
		ss << "Clouds" << i;
		SpriteGo* spr = (SpriteGo*)FindGo(ss.str());
		spr->SetPosition(spr->GetPosition().x - (dt * 100.f * cloudSpeed[i]), spr->GetPosition().y);
		if (spr->GetPosition().x < -100.f)
		{
			ss.str("");
			ss << "graphics/Origin/Sprite/clouds_" << Utils::RandomRange(0, 7) * 3 + sky
				<< ".png";
			spr->sprite.setTexture(*RESOURCE_MGR.GetTexture(ss.str()));
			spr->sprite.setTextureRect({ 0,0,(int)spr->sprite.getTexture()->getSize().x,(int)spr->sprite.getTexture()->getSize().y });
			spr->SetOrigin(Origins::MC);
			spr->SetPosition(FRAMEWORK.GetWindowSize().x + 100,
				Utils::RandomRange(75.f, 460.f));
			spr->SetSize(2, 2);
			cloudSpeed[i] = Utils::RandomRange(0.5f, 1.f);
		}
	}
}

bool SceneHome::isPartsEmpty()
{
	for (int i = 0; i < 4; i++)
	{
		if (equipCraftParts[i] != 0)
		{
			return false;
		}
	}
	return true;
}

void SceneHome::UiEquipOpen(bool on)
{
	SubUiBaseOpen(9, on);
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	auto itemTable = DATATABLE_MGR.Get<ItemTable>(DataTable::Ids::Item);
	std::vector<int> equipedGear = TEAM_MGR.GetEquipedGear();
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;

	spr = (SpriteGo*)FindGo("SubUiBack");
	spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/equipment_bg.png"));

	// Base
	text = (TextGo*)FindGo("UsingEquipUiText");
	text->SetActive(on);

	text = (TextGo*)FindGo("MakeEquipUiText");
	text->SetActive(on);

	text = (TextGo*)FindGo("CurPartsEquipUiText");
	text->SetActive(on);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "EquipSlotB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(on);
		bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/equipment_slot_bg_0.png"));
		bt->OnEnter = [bt, this]() {
			if (isInnerPopOn)
			{
				return;
			}
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/equipment_slot_bg_1.png"));
			};
		bt->OnExit = [bt, this]() {
			if (isInnerPopOn)
			{
				return;
			}
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/equipment_slot_bg_0.png"));
			};

		ss.str("");
		ss << "EquipItemNameSlot" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);
		ReturnItemName(*text, i, equipedGear[i]);

		ss << "Stat";
		text = (TextGo*)FindGo(ss.str());
		text->text.setString(ReturnEquipStats(i, equipedGear[i]));
		text->SetActive(on);

		ss.str("");
		ss << "ItemEquipedIcon" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(on);
		sf::Texture* tex = RESOURCE_MGR.GetTexture(itemTable->Get(i, equipedGear[i]).textureId);
		spr->sprite.setTexture(*tex);
		spr->sprite.setTextureRect({ 0,0,(int)tex->getSize().x,(int)tex->getSize().y });
		spr->SetOrigin(Origins::MC);

		ss.str("");
		ss << "EquipItemType" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);

		ss.str("");
		ss << "HavePartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		switch (i)
		{
		case 0:
		{
			std::wstringstream wss;
			wss << stringtable->GetW("SoundChip") << std::endl
				<< TEAM_MGR.GetParts(i) << stringtable->GetW("Count");
			text->text.setString(wss.str());
		}
		break;
		case 1:
		{
			std::wstringstream wss;
			wss << stringtable->GetW("Switch") << std::endl
				<< TEAM_MGR.GetParts(i) << stringtable->GetW("Count");
			text->text.setString(wss.str());
		}
		break;
		case 2:
		{
			std::wstringstream wss;
			wss << stringtable->GetW("Screw") << std::endl
				<< TEAM_MGR.GetParts(i) << stringtable->GetW("Count");
			text->text.setString(wss.str());
		}
		break;
		case 3:
		{
			std::wstringstream wss;
			wss << stringtable->GetW("Fabric") << std::endl
				<< TEAM_MGR.GetParts(i) << stringtable->GetW("Count");
			text->text.setString(wss.str());
		}
		break;
		}
		text->SetActive(on);
	}

	std::vector<ItemMakeSlot> craftSlot = TEAM_MGR.GetCraftSlot();

	for (int i = 0; i < 3; i++)
	{
		bool slotOpen = on && i < TEAM_MGR.GetCraftSlotCount();
		bool slotClose = on && i >= TEAM_MGR.GetCraftSlotCount();

		std::stringstream ss;
		ss << "Equipmake" << i;
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(on);
		if (slotOpen)
		{
			if (craftSlot[i].inCrafting)
			{
				spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/equipment_making_slot_bg_2.png"));
			}
			else
			{
				spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/equipment_making_slot_bg_0.png"));
			}
		}


		ss.str("");
		ss << "EquipMakeB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(slotOpen);
		if (craftSlot[i].inCrafting && craftSlot[i].leftDate > 0)
		{
			bt->SetActive(false);
		}
		bt->OnClick = [i, this]() {
			if (!TEAM_MGR.GetCraftSlot()[i].inCrafting)
			{
				UiEquipMakeOpen(i);
			}
			else if (TEAM_MGR.GetCraftSlot()[i].leftDate == 0)
			{
				SoundGo* so = (SoundGo*)FindGo("CraftSound");
				so->Play();
				UiCraftFinish(i);
			}
			};

		ss.str("");
		ss << "MakeItemPartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(slotOpen);
		ss.str("");
		ss << craftSlot[i].usedParts[0] << " \t\t"
			<< craftSlot[i].usedParts[1] << "\t\t"
			<< craftSlot[i].usedParts[2] << "\t\t"
			<< craftSlot[i].usedParts[3];
		text->text.setString(ss.str());
		if (!craftSlot[i].inCrafting)
		{
			text->SetActive(false);
		}

		ss.str("");
		ss << "MakeItemBText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(slotOpen);
		if (!craftSlot[i].inCrafting)
		{
			text->text.setString(stringtable->GetW("Craft"));
			text->SetOrigin(Origins::MC);
		}
		else if (craftSlot[i].leftDate == 0)
		{
			text->text.setString(stringtable->GetW("CheckResult"));
			text->SetOrigin(Origins::MC);
		}
		if (craftSlot[i].inCrafting && craftSlot[i].leftDate > 0)
		{
			text->SetActive(false);
		}

		ss.str("");
		ss << "MakingItemText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(slotOpen);
		if (!craftSlot[i].inCrafting || craftSlot[i].leftDate < 1)
		{
			text->SetActive(false);
		}

		ss << "Time";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(slotOpen);
		std::wstringstream wss;
		wss << stringtable->GetW("TimeRemaining") << " " << craftSlot[i].leftDate << stringtable->GetW("Week");
		text->text.setString(wss.str());
		text->SetOrigin(Origins::MC);
		if (!craftSlot[i].inCrafting || craftSlot[i].leftDate < 1)
		{
			text->SetActive(false);
		}


		if (i > 0)
		{
			ss.str("");
			ss << "ItemMakeLock" << i - 1;
			SpriteGo* lockIco = (SpriteGo*)FindGo(ss.str());
			lockIco->SetActive(slotClose);

			ss.str("");
			ss << "CanCraftExText" << i - 1;
			text = (TextGo*)FindGo(ss.str());
			text->SetActive(slotClose);
		}

	}

	// ChangeEquip
	FindGo("EquipMask")->SetActive(false);

	RectGo* rect = (RectGo*)FindGo("PopupUiShade");
	rect->SetActive(false);

	for (int i = 0; i < 38; i++)
	{
		std::stringstream ss;
		ss.str("");
		ss << "EquipItemB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);
		ss << "Sprite";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);

		ss << "Lock";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);
	}

	spr = (SpriteGo*)FindGo("EquipPopup");
	spr->SetActive(false);

	text = (TextGo*)FindGo("PopupUiText");
	text->SetActive(false);

	text = (TextGo*)FindGo("EquipItemText");
	text->SetActive(false);

	text = (TextGo*)FindGo("EquipItemValText");
	text->SetActive(false);

	text = (TextGo*)FindGo("EquipingText");
	text->SetActive(false);

	text = (TextGo*)FindGo("LookItemText");
	text->SetActive(false);

	text = (TextGo*)FindGo("LookItemValText");
	text->SetActive(false);

	spr = (SpriteGo*)FindGo("ExitEquipPopB");
	spr->SetActive(false);

	text = (TextGo*)FindGo("ExitEquipPopBText");
	text->SetActive(false);

	// Craft

	spr = (SpriteGo*)FindGo("EquipPopup");
	spr->SetActive(false);

	text = (TextGo*)FindGo("PopupUiText");
	text->SetActive(false);

	spr = (SpriteGo*)FindGo("ExitEquipPopB");
	spr->SetActive(false);

	text = (TextGo*)FindGo("ExitEquipPopBText");
	text->SetActive(false);

	text = (TextGo*)FindGo("SelectPartsText");
	text->SetActive(false);

	bt = (UiButton*)FindGo("PartsResetB");
	bt->SetActive(false);
	text = (TextGo*)FindGo("SelectPartsResetBText");
	text->SetActive(false);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "CraftSlot" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);

		ss << "Icon";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);

		ss << "Bg";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);

		ss.str("");
		ss << "UsingCraftPartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss << "Cur";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss << "Text";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss.str("");
		ss << "PartDecrease" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);

		ss.str("");
		ss << "InputPartsBt" << i << "Text1";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss.str("");
		ss << "PartDecrease" << i << "x5";
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);

		ss.str("");
		ss << "InputPartsBt" << i << "Text0";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss.str("");
		ss << "PartIncrease" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);

		ss.str("");
		ss << "InputPartsBt" << i << "Text2";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss.str("");
		ss << "PartIncrease" << i << "x5";
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);
		ss.str("");
		ss << "InputPartsBt" << i << "Text3";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss.str("");
		ss << "CraftUiUsingCount" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);

		ss.str("");
		ss << "CraftUiUsingPartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(false);
	}

	text = (TextGo*)FindGo("CraftUiCurMoneyText");
	text->SetActive(false);

	text = (TextGo*)FindGo("CraftUiPartsHeadText");
	text->SetActive(false);

	text = (TextGo*)FindGo("CraftUiUseDateText");
	text->SetActive(false);

	text = (TextGo*)FindGo("CraftUiUseCostText");
	text->SetActive(false);

	bt = (UiButton*)FindGo("OnCraftB");
	bt->SetActive(false);

	text = (TextGo*)FindGo("CraftingBText");
	text->SetActive(false);
}

void SceneHome::UiEquipChangeOpen(int type, bool on)
{
	isInnerPopOn = on;
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	auto itemTable = DATATABLE_MGR.Get<ItemTable>(DataTable::Ids::Item);
	std::vector<int> equipedGear = TEAM_MGR.GetEquipedGear();

	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;
	RectGo* rect = (RectGo*)FindGo("PopupUiShade");
	rect->SetActive(on);
	int itemCount = 0;

	switch (type)
	{
	case 0:
		itemCount = 31;
		break;
	case 1:
		itemCount = 37;
		break;
	case 2:
		itemCount = 29;
		break;
	case 3:
		itemCount = 32;
		break;
	}
	useItemCount = itemCount;

	for (int i = 0; i < 38; i++)
	{
		std::stringstream ss;
		ss << "EquipItemB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetPosition(330 + (63 * (i % 5)), 210 + (63 * (i / 5)));
		bt->SetOrigin(Origins::MC);
		std::stringstream ss2;
		ss2 << "EquipItemB" << i << "Sprite";
		spr = (SpriteGo*)FindGo(ss2.str());
		spr->SetPosition(bt->GetPosition());
		spr->SetOrigin(Origins::MC);

		ss2 << "Lock";
		spr = (SpriteGo*)FindGo(ss2.str());
		spr->SetOrigin(Origins::MC);
		spr->SetPosition(bt->GetPosition().x + 15, bt->GetPosition().y - 15);
	}

	for (int i = 0; i < itemCount; i++)
	{
		std::stringstream ss;
		ss.str("");
		ss << "EquipItemB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(on);
		
		if (equipedGear[type] == i)
		{
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/equipment_icon_bg_2.png"));
		}
		else
		{
			bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/equipment_icon_bg_0.png"));
		}
		if (!TEAM_MGR.GetGearOpen(type, i))
		{
			std::stringstream ss2;
			ss2 << "EquipItemB" << i << "Sprite";
			SpriteGo* spr = (SpriteGo*)FindGo(ss2.str());
			spr->sprite.setColor(sf::Color(50, 50, 50));

			ss2 << "Lock";
			spr = (SpriteGo*)FindGo(ss2.str());
			spr->SetActive(on);

		}
		else
		{
			std::stringstream ss2;
			ss2 << "EquipItemB" << i << "Sprite";
			SpriteGo* spr = (SpriteGo*)FindGo(ss2.str());
			spr->sprite.setColor(sf::Color::White);
		}
		bt->OnClick = [this, type, i, itemCount]() {
			if (TEAM_MGR.GetGearOpen(type, i))
			{
				TEAM_MGR.SetEquipedGear(type, i);

				TextGo* text = (TextGo*)FindGo("EquipItemText");
				ReturnItemName(*text, type, i);

				text = (TextGo*)FindGo("EquipItemValText");
				text->text.setString(ReturnEquipStats(type, i));
				for (int j = 0; j < itemCount; j++)
				{
					std::stringstream ss;
					ss.str("");
					ss << "EquipItemB" << j;
					UiButton* bt = (UiButton*)FindGo(ss.str());
					if (j == i)
					{
						bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/equipment_icon_bg_2.png"));
					}
					else
					{
						bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/equipment_icon_bg_0.png"));
					}
				}
			}
			};

		bt->OnEnter = [bt, this, type, i]() {
			TextGo* text = (TextGo*)FindGo("LookItemText");
			ReturnItemName(*text, type, i);

			text = (TextGo*)FindGo("LookItemValText");
			text->text.setString(ReturnEquipStats(type, i));

			if (TEAM_MGR.GetEquipedGear()[type] != i)
			{
				bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/equipment_icon_bg_1.png"));
			}
			};
		bt->OnExit = [bt, type, i]() {
			if (TEAM_MGR.GetEquipedGear()[type] != i)
			{
				bt->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/Origin/Sprite/equipment_icon_bg_0.png"));
			}
			};

		ss << "Sprite";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(on);
		sf::Texture* tex = RESOURCE_MGR.GetTexture(itemTable->Get(type, i).textureId);
		spr->sprite.setTexture(*tex);
		spr->sprite.setTextureRect({ 0,0,(int)tex->getSize().x,(int)tex->getSize().y });
		spr->SetOrigin(Origins::MC);
	}
	for (int i = itemCount; i < 37; i++)
	{
		std::stringstream ss;
		ss.str("");
		ss << "EquipItemB" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(false);

		ss << "Sprite";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);

		ss << "Lock";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(false);
	}

	spr = (SpriteGo*)FindGo("EquipPopup");
	spr->SetActive(on);
	spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/equipment_change_popup_bg.png"));

	text = (TextGo*)FindGo("PopupUiText");
	text->SetActive(on);
	text->text.setString(stringtable->GetW("ChangeEquip"));

	text = (TextGo*)FindGo("EquipItemText");
	text->SetActive(on);

	text = (TextGo*)FindGo("EquipItemValText");
	text->SetActive(on);

	text = (TextGo*)FindGo("EquipingText");
	text->SetActive(on);

	text = (TextGo*)FindGo("LookItemText");
	text->SetActive(on);

	text = (TextGo*)FindGo("LookItemValText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("ExitEquipPopB");
	bt->SetActive(on);
	bt->OnClick = [this, type]() {
		UiEquipChangeOpen(type, false);
		UiEquipOpen();
		isPopupOn = false;
		};

	text = (TextGo*)FindGo("ExitEquipPopBText");
	text->SetActive(on);

	spr = (SpriteGo*)FindGo("EquipMask");
	spr->SetActive(on);

	if (on)
	{
		EquipListShower();
	}
}


void SceneHome::UiEquipMakeOpen(int index, bool on)
{
	isInnerPopOn = on;
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;
	RectGo* rect = (RectGo*)FindGo("PopupUiShade");
	rect->SetActive(on);

	spr = (SpriteGo*)FindGo("EquipPopup");
	spr->SetActive(on);
	spr->sprite.setTexture(*RESOURCE_MGR.GetTexture("graphics/UiFix/equipment_make_popup_ui_bg.png"));

	text = (TextGo*)FindGo("PopupUiText");
	text->SetActive(on);
	text->text.setString(stringtable->GetW("CraftEquip"));

	bt = (UiButton*)FindGo("ExitEquipPopB");
	bt->SetActive(on);
	bt->OnClick = [this, index]() {
		UiEquipMakeOpen(index, false);
		UiEquipOpen();
		isPopupOn = false;
		};

	text = (TextGo*)FindGo("ExitEquipPopBText");
	text->SetActive(on);

	text = (TextGo*)FindGo("SelectPartsText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("PartsResetB");
	bt->SetActive(on);
	text = (TextGo*)FindGo("SelectPartsResetBText");
	text->SetActive(on);

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "CraftSlot" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(on);

		ss << "Icon";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(on);

		ss << "Bg";
		spr = (SpriteGo*)FindGo(ss.str());
		spr->SetActive(on);

		ss.str("");
		ss << "UsingCraftPartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);

		ss << "Cur";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);

		ss << "Text";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);

		ss.str("");
		ss << "PartDecrease" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(on);

		ss.str("");
		ss << "InputPartsBt" << i << "Text1";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);

		ss.str("");
		ss << "PartDecrease" << i << "x5";
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(on);

		ss.str("");
		ss << "InputPartsBt" << i << "Text0";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);

		ss.str("");
		ss << "PartIncrease" << i;
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(on);

		ss.str("");
		ss << "InputPartsBt" << i << "Text2";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);

		ss.str("");
		ss << "PartIncrease" << i << "x5";
		bt = (UiButton*)FindGo(ss.str());
		bt->SetActive(on);
		ss.str("");
		ss << "InputPartsBt" << i << "Text3";
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);

		ss.str("");
		ss << "CraftUiUsingCount" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);

		ss.str("");
		ss << "CraftUiUsingPartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		text->SetActive(on);
	}

	text = (TextGo*)FindGo("CraftUiCurMoneyText");
	text->SetActive(on);
	text->text.setString(std::to_string(TEAM_MGR.GetMoney()));
	text->SetOrigin(Origins::MR);

	text = (TextGo*)FindGo("CraftUiPartsHeadText");
	text->SetActive(on);

	text = (TextGo*)FindGo("CraftUiUseDateText");
	text->SetActive(on);

	text = (TextGo*)FindGo("CraftUiUseCostText");
	text->SetActive(on);

	bt = (UiButton*)FindGo("OnCraftB");
	bt->SetActive(on);
	bt->OnClick = [this, index]() {
		if (craftCost > TEAM_MGR.GetMoney() || isPartsEmpty())
		{
			return;
		}
		ItemMakeSlot craftSlot;
		craftSlot.inCrafting = true;
		craftSlot.itemType = Utils::RandomRange(0, 3);
		craftSlot.itemCode = CraftRoll(craftSlot.itemType);
		craftSlot.leftDate = craftTime;
		craftSlot.usedParts = equipCraftParts;
		TEAM_MGR.SetCraftSlot(index, craftSlot);
		for (int j = 0; j < 4; j++)
		{
			TEAM_MGR.UseParts(j, equipCraftParts[j]);
		}
		TEAM_MGR.UseMoney(craftCost);
		UiEquipMakeOpen(index, false);
		UiEquipOpen();
		UpdateMoney();
		};

	text = (TextGo*)FindGo("CraftingBText");
	text->SetActive(on);

	UpdateCraftVal();
}

int SceneHome::CraftRoll(int type)
{
	int result = 0;
	int grade = Utils::RandomRange(0, 100);
	if (grade >= 100 - (equipCraftParts[type] / 5))
	{
		grade = 4;
	}
	else if (grade >= 90 - (equipCraftParts[type] / 2))
	{
		grade = 3;
	}
	else if (grade >= 70 - (equipCraftParts[type]))
	{
		grade = 2;
	}
	else if (grade >= 40 - (equipCraftParts[type]))
	{
		grade = 1;
	}
	else
	{
		grade = 0;
	}

	switch (type)
	{
	case 0:
		if (grade == 0)
		{
			result = Utils::RandomRange(0, 3);
		}
		else if (grade == 1)
		{
			result = Utils::RandomRange(4, 10);
		}
		else if (grade == 2)
		{
			result = Utils::RandomRange(11, 20);
		}
		else if (grade == 3)
		{
			result = Utils::RandomRange(21, 27);
		}
		else
		{
			result = Utils::RandomRange(28, 30);
		}
		break;
	case 1:
		if (grade == 0)
		{
			result = Utils::RandomRange(0, 5);
		}
		else if (grade == 1)
		{
			result = Utils::RandomRange(6, 13);
		}
		else if (grade == 2)
		{
			result = Utils::RandomRange(14, 25);
		}
		else if (grade == 3)
		{
			result = Utils::RandomRange(26, 32);
		}
		else
		{
			result = Utils::RandomRange(33, 36);
		}
		break;
	case 2:
		if (grade == 0)
		{
			result = Utils::RandomRange(0, 3);
		}
		else if (grade == 1)
		{
			result = Utils::RandomRange(4, 11);
		}
		else if (grade == 2)
		{
			result = Utils::RandomRange(12, 19);
		}
		else if (grade == 3)
		{
			result = Utils::RandomRange(20, 25);
		}
		else
		{
			result = Utils::RandomRange(26, 28);
		}
		break;
	case 3:
		if (grade == 0)
		{
			result = Utils::RandomRange(0, 3);
		}
		else if (grade == 1)
		{
			result = Utils::RandomRange(4, 12);
		}
		else if (grade == 2)
		{
			result = Utils::RandomRange(13, 21);
		}
		else if (grade == 3)
		{
			result = Utils::RandomRange(22, 28);
		}
		else
		{
			result = Utils::RandomRange(29, 31);
		}
		break;
	}
	return result;
}

void SceneHome::UiCraftFinish(int index, bool on)
{
	isInnerPopOn = on;
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	auto itemTable = DATATABLE_MGR.Get<ItemTable>(DataTable::Ids::Item);
	ItemMakeSlot slotInfo;
	if (on)
	{
		slotInfo = TEAM_MGR.GetCraftSlot()[index];
	}
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;
	RectGo* rect = (RectGo*)FindGo("PopupUiShade");
	rect->SetActive(on);

	spr = (SpriteGo*)FindGo("CraftResultBack");
	spr->SetActive(on);

	spr = (SpriteGo*)FindGo("CraftResultIcon");
	sf::Texture* tex = RESOURCE_MGR.GetTexture(itemTable->Get(slotInfo.itemType, slotInfo.itemCode).textureId);
	spr->sprite.setTexture(*tex);
	spr->sprite.setTextureRect({ 0,0,(int)tex->getSize().x,(int)tex->getSize().y });
	spr->SetOrigin(Origins::MC);
	spr->SetActive(on);

	bt = (UiButton*)FindGo("CraftResultB");
	bt->OnClick = [this, index]() {
		UiCraftFinish(index, false);
		UiEquipOpen();
		};
	bt->SetActive(on);

	text = (TextGo*)FindGo("CraftResultTitle");
	text->SetActive(on);

	text = (TextGo*)FindGo("CraftResultName");
	ReturnItemName(*text, slotInfo.itemType, slotInfo.itemCode);
	text->SetOrigin(Origins::TL);
	text->SetActive(on);

	text = (TextGo*)FindGo("CraftResultVal");
	text->text.setFont(*RESOURCE_MGR.GetFont("fonts/Galmuri14.ttf"));
	text->text.setString(ReturnEquipStats(slotInfo.itemType, slotInfo.itemCode));
	text->SetOrigin(Origins::TL);
	text->SetActive(on);

	text = (TextGo*)FindGo("CraftResultOkBText");
	text->SetActive(on);

	if (on)
	{
		TEAM_MGR.OpenItem(slotInfo.itemType, slotInfo.itemCode);
		TEAM_MGR.SetCraftSlot(index, ItemMakeSlot());
	}
}

void SceneHome::UpdateCraftVal()
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	SpriteGo* spr;
	UiButton* bt;
	TextGo* text;

	for (int i = 0; i < 4; i++)
	{
		std::stringstream ss;
		ss << "UsingCraftPartsText" << i << "Cur";
		text = (TextGo*)FindGo(ss.str());
		text->text.setString(std::to_string(TEAM_MGR.GetParts(i)));

		ss << "Text";
		text = (TextGo*)FindGo(ss.str());
		text->text.setString(std::to_wstring(TEAM_MGR.GetParts(i)) + stringtable->GetW("HavePartsNGold"));

		ss.str("");
		ss << "CraftUiUsingCount" << i;
		text = (TextGo*)FindGo(ss.str());
		text->text.setString(std::to_string(equipCraftParts[i]));

		ss.str("");
		ss << "CraftUiUsingPartsText" << i;
		text = (TextGo*)FindGo(ss.str());
		std::wstringstream wss;
		switch (i)
		{
		case 0:
			wss << stringtable->GetW("SoundChip");
			break;
		case 1:
			wss << stringtable->GetW("Switch");
			break;
		case 2:
			wss << stringtable->GetW("Screw");
			break;
		case 3:
			wss << stringtable->GetW("Fabric");
			break;
		}
		wss << " " << equipCraftParts[i] << stringtable->GetW("Count");
		if (equipCraftParts[i] > TEAM_MGR.GetParts(i))
		{
			wss << "(" << equipCraftParts[i] - TEAM_MGR.GetParts(i) <<
				stringtable->GetW("BuyCount") << ")";
		}
		text->text.setString(wss.str());
	}

	text = (TextGo*)FindGo("CraftUiUseDateText");
	{
		int partsSum = 0;
		for (int i = 0; i < 4; i++)
		{
			partsSum += equipCraftParts[i];
		}
		std::wstringstream wss;
		wss << stringtable->GetW("RemainingTime") << "\t" <<
			partsSum / 10 + 1 << stringtable->GetW("Week");
		text->text.setString(wss.str());
		craftTime = partsSum / 10 + 1;
	}
	text = (TextGo*)FindGo("CraftUiUseCostText");
	{
		int partsCost = 0;
		for (int i = 0; i < 4; i++)
		{
			if (equipCraftParts[i] > TEAM_MGR.GetParts(i))
			{
				partsCost += 10 * (equipCraftParts[i] - TEAM_MGR.GetParts(i));
			}
		}
		std::wstringstream wss;
		wss << stringtable->GetW("PartsCost") << "\t" <<
			partsCost;
		text->text.setString(wss.str());
		craftCost = partsCost;
	}

	bt = (UiButton*)FindGo("OnCraftB");
	if (craftCost > TEAM_MGR.GetMoney() || isPartsEmpty())
	{
		bt->sprite.setColor(sf::Color(100, 100, 100));
	}
	else
	{
		bt->sprite.setColor(sf::Color::White);
	}
}

std::wstring SceneHome::ReturnEquipStats(int itemType, int itemNum)
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	auto itemTable = DATATABLE_MGR.Get<ItemTable>(DataTable::Ids::Item);
	ItemInfo item = itemTable->Get(itemType, itemNum);

	std::wstringstream wss;

	if (item.atk != 0)
	{
		wss << stringtable->GetW("Attack") << " +" << item.atk << std::endl;
	}
	if (item.def != 0)
	{
		wss << stringtable->GetW("Defence") << " +" << item.def << std::endl;
	}
	if (item.atkSpeed != 0)
	{
		wss << stringtable->GetW("AtkSpeed") << " +" << item.atkSpeed << std::endl;
	}
	if (item.coolDown != 0)
	{
		wss << stringtable->GetW("SkillCooldown") << " +" << item.coolDown << std::endl;
	}
	if (item.hpDrain != 0)
	{
		wss << stringtable->GetW("HpDrain") << " +" << item.hpDrain << std::endl;
	}
	if (item.proficiencyType != 0)
	{
		std::stringstream ss;
		ss << "ChampType" << (item.proficiencyType - 1);
		wss << stringtable->GetW(ss.str()) << stringtable->GetW("ChampTypePro") << " +" << item.proficiencyValue << std::endl;
	}
	if (item.champProficiency != 0)
	{
		std::stringstream ss;
		ss << "ChampName" << (item.champProficiency - 1);
		wss << stringtable->GetW(ss.str()) << stringtable->GetW("ChampPro") << " +" << item.champProficiencyValue << std::endl;
	}


	return wss.str();
}

void SceneHome::ReturnItemName(TextGo& text, int type, int num)
{
	auto stringtable = DATATABLE_MGR.Get<StringTable>(DataTable::Ids::String);
	auto itemTable = DATATABLE_MGR.Get<ItemTable>(DataTable::Ids::Item);
	std::stringstream ss;
	switch (type)
	{
	case 0:
		ss << "Headset";
		break;
	case 1:
		ss << "Controller";
		break;
	case 2:
		ss << "Chair";
		break;
	case 3:
		ss << "Uniform";
		break;
	}
	ss << num;
	text.text.setString(stringtable->GetW(ss.str()));
	switch (itemTable->Get(type, num).grade)
	{
	case 0:
		text.text.setFillColor(sf::Color::White);
		break;
	case 1:
		text.text.setFillColor(sf::Color(0, 128, 64));
		break;
	case 2:
		text.text.setFillColor(sf::Color(0, 128, 255));
		break;
	case 3:
		text.text.setFillColor(sf::Color(163, 73, 164));
		break;
	case 4:
		text.text.setFillColor(sf::Color(255, 128, 0));
		break;
	}
}

