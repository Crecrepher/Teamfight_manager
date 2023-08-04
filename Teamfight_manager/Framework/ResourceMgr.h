#pragma once
#include "Singleton.h"

class ResourceMgr : public Singleton<ResourceMgr>
{
	friend Singleton<ResourceMgr>;
private:
	ResourceMgr() = default;
	virtual ~ResourceMgr() override;
	
	std::map<std::string, std::tuple<sf::Texture*,bool>> mapTexture;
	std::map<std::string, std::tuple<sf::Font*, bool>> mapFont;
	std::map<std::string, std::tuple<sf::SoundBuffer*, bool>> mapSoundBuffer;
	std::map<std::string, std::tuple<AnimationClip*, bool>> mapAnimationClip;
	//map은 tree구조를 가지고 있다

public:
	void UnloadAll();

	void Init();
	void Load(ResourceTypes t,const std::string path, bool isDefault = false);
	void LoadFromCsv(const std::string path, bool isDefault = false);

	void UnLoad(ResourceTypes t, const std::string path);

	sf::Texture* GetTexture(const std::string& id);
	sf::Font* GetFont(const std::string& id);
	sf::SoundBuffer* GetSoundBuffer(const std::string& id);
	AnimationClip* GetAnimationClip(const std::string& id);

};

#define RESOURCE_MGR (ResourceMgr::Instance())