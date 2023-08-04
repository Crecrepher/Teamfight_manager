#pragma once
#include "Singleton.h"
#include "SoundGo.h"
#include "Scene.h"

class Scene;

class SceneMgr : public Singleton<SceneMgr>
{
	friend Singleton<SceneMgr>;

protected:
	SceneMgr() = default;
	virtual	~SceneMgr() = default;

	std::vector<Scene*> scenes;
	SceneId currentSceneId = SceneId::None;
	Scene* currentScene = nullptr;

	SceneId startSceneId = SceneId::Game;
public:
	int hiScore = 0;

	void ChangeScene(SceneId id);

	void Init();
	void Release();

	void Update(float dt);
	void Draw(sf::RenderWindow& window);

	Scene* GetCurrScene()const;

	void SaveLoad();
};

#define SCENE_MGR (SceneMgr::Instance())
