#pragma once
#include "Scene.h"
#include "SpriteGo.h"
#include "AnimatioControler.h"

class Champion;

struct FrameInfo
{
	int left = 0;
	int top = 0;
	int width = 0;
	int height = 0;

	int disX = 0;
	int disY = 0;
};

class SceneChampEdit : public Scene
{
protected:
	sf::Vector2f mouseMove;
	float zoom = 0.5f;

	int maxFrame = 0;
	int curFrame = 0;
	int fps = 0;
	int loopType = 0;
	std::vector<FrameInfo> frameInfo;
	std::vector<FrameInfo> freeset;

	AnimatioControler animation;
	AnimatioControler skillAni;
	SpriteGo champ;
	SpriteGo skillEffect;
	int curChampCode = 0;
	bool isClipWorks = false;

	bool isCSVMakerOn = false;
	bool isDragMode = false;
	bool isButtonInDrag = false;
	int curChampAni = 0;
	int aniPosType = 0;
	sf::Vector2f csvPickPoint;

	float csvEffectPreviewSize = 1.f;
public:
	SceneChampEdit();
	virtual ~SceneChampEdit() override;

	virtual void Init() override;
	virtual void Release() override;

	virtual void Enter() override;
	virtual void Exit() override;

	virtual void Update(float dt) override;
	virtual void Draw(sf::RenderWindow& window) override;

	void InitDefaultUi();
	void InitCSVMaker();

	void EnterDefaultUi();
	void ActivceDefaultUi(bool on = true);
	void LoadEffectPreview();
	void UpdateEffectPreviewAni();

	void EnterCSVMaker();
	void ActiveCSVMaker(bool on = true);
	void LoadCsvMaker();
	void ResetFrameInfo();
	void ResetCsvEffectPreview();
	void UpdateCsvMaker(float dt);
	void LoadCSVDistanceCalculator();
	void CSVDistanceCalculator();

	void LoadCsv();
	void SaveCsv();

	int ReturnSkillCode(int champCode, bool isUlt = false);
};

