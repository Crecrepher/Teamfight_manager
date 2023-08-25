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
};

class SceneChampEdit : public Scene
{
protected:
	
	int maxFrame;
	int curFrame;
	int fps;
	int loopType;
	std::vector<FrameInfo> frameInfo;

	AnimatioControler animation;
	SpriteGo champ;
	int curChampCode = 0;

	bool isCSVMakerOn = false;
	int curChampAni = 0;

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
	void EnterCSVMaker(); 
	void ActiveCSVMaker(bool on = true);
	void LoadCsvMaker();
	void ResetFrameInfo();
	void ResetCsvEffectPreview();

	void LoadCsv();
	void SaveCsv();

	int ReturnSkillCode(int champCode, bool isUlt = false);
};

