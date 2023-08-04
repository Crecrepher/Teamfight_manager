#include "stdafx.h"
#include "ResourceMgr.h"
#include "ErrorPrinter.h"
#include "rapidcsv.h"
ResourceMgr::~ResourceMgr()
{
	UnloadAll();
}

void ResourceMgr::UnloadAll()
{
	{
		auto it = mapTexture.begin();
		while (it != mapTexture.end())
		{
			if (std::get<1>(it->second))
			{
				delete std::get<0>(it->second);
				it = mapTexture.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	{
		auto it = mapFont.begin();
		while (it != mapFont.end())
		{
			if (std::get<1>(it->second))
			{
				delete std::get<0>(it->second);
				it = mapFont.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	{
		auto it = mapSoundBuffer.begin();
		while (it != mapSoundBuffer.end())
		{
			if (std::get<1>(it->second))
			{
				delete std::get<0>(it->second);
				it = mapSoundBuffer.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	{
		auto it = mapAnimationClip.begin();
		while (it != mapAnimationClip.end())
		{
			if (std::get<1>(it->second))
			{
				delete std::get<0>(it->second);
				it = mapAnimationClip.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void ResourceMgr::Init()
{
	LoadFromCsv("tables/DefaultResourceList.csv",true);
}

void ResourceMgr::Load(ResourceTypes t, const std::string path, bool isDefault)
{
    //mapTexture["ID"] = new sf::Texture();
    //강제로 넣어 덮어씀, 메모리 릭 위험있음
    switch (t)
	{
	case ResourceTypes::Texture:
	{
		auto it = mapTexture.find(path);
		//키를 찾아 검사한다.
		//이터레이터에서 ->first 키값과 ->second벨류 이렇게 두가지를 가진다

		//mapTexture.insert({ "ID", new sf::Texture() });
		//키를 검사하고 넣음
		if (mapTexture.end() == it)
		{
			auto texture = new sf::Texture();
			texture->loadFromFile(path);
			mapTexture.insert({ path, {texture,isDefault} });
		}
	}
	break;
	case ResourceTypes::Font:
	{
		auto it = mapFont.find(path);
		if (mapFont.end() == it)
		{
			auto font = new sf::Font();
			font->loadFromFile(path);
			mapFont.insert({ path, {font,isDefault} });
		}
	}
	break;
	case ResourceTypes::Sound:
	{
		auto it = mapSoundBuffer.find(path);
		if (mapSoundBuffer.end() == it)
		{
			auto sb = new sf::SoundBuffer();
			sb->loadFromFile(path);
			mapSoundBuffer.insert({ path, {sb,isDefault} });
		}
	}
	break;
	case ResourceTypes::AnimationClip:
	{
		auto it = mapAnimationClip.find(path);
		if (mapAnimationClip.end() == it)
		{
			auto clip = new AnimationClip();
			clip->LoadFromFile(path);
			mapAnimationClip.insert({ path, {clip,isDefault} });
		}
	}
	break;
	}

   
}

void ResourceMgr::LoadFromCsv(const std::string path, bool isDefault)
{
	rapidcsv::Document doc(path);
	std::vector<int> types = doc.GetColumn<int>(0);
	std::vector<std::string> paths = doc.GetColumn<std::string>(1);
	for (size_t i = 0; i < types.size(); ++i)
	{
		Load((ResourceTypes)types[i], paths[i]);
	}
}

void ResourceMgr::UnLoad(ResourceTypes t, const std::string path)
{
	switch (t)
	{
	case ResourceTypes::Texture:
	{
		auto it = mapTexture.find(path);
		if (it != mapTexture.end())
		{
			if (!std::get<1>(it->second))
			{
				delete std::get<0>(it->second);
				mapTexture.erase(it);

			}
			else
			{
				std::cout << "ERR: ID DEFAULT RESOURCE" << std::endl;
			}
			ERROR_PRINTER.isUnload(path);
		}
	}
	break;
	case ResourceTypes::Font:
	{
		auto it = mapFont.find(path);
		if (it != mapFont.end())
		{
			if (!std::get<1>(it->second))
			{
				delete std::get<0>(it->second);
				mapFont.erase(it);

			}
			else
			{
				std::cout << "ERR: ID DEFAULT RESOURCE" << std::endl;
			}
			ERROR_PRINTER.isUnload(path);
		}
	}
	break;
	case ResourceTypes::Sound:
	{
		auto it = mapSoundBuffer.find(path);
		if (it != mapSoundBuffer.end())
		{
			if (!std::get<1>(it->second))
			{
				delete std::get<0>(it->second);
				mapSoundBuffer.erase(it);

			}
			else
			{
				std::cout << "ERR: ID DEFAULT RESOURCE" << std::endl;
			}
		}
	}
	case ResourceTypes::AnimationClip:
	{
		auto it = mapAnimationClip.find(path);
		if (it != mapAnimationClip.end())
		{
			if (!std::get<1>(it->second))
			{
				delete std::get<0>(it->second);
				mapAnimationClip.erase(it);
			}
			else
			{
				std::cout << "ERR: ID DEFAULT RESOURCE" << std::endl;
			}
		}
	}
	break;
	}
}

sf::Texture* ResourceMgr::GetTexture(const std::string& id)
{
	
	auto it = mapTexture.find(id);
	if (it != mapTexture.end())
	{
		return std::get<0>(it->second);
	}
	ERROR_PRINTER.GetFailCheck(id);
    return nullptr;
}

sf::Font* ResourceMgr::GetFont(const std::string& id)
{
	auto it = mapFont.find(id);
	if (it != mapFont.end())
	{
		return std::get<0>(it->second);
	}
	ERROR_PRINTER.GetFailCheck(id);
    return nullptr;
}

sf::SoundBuffer* ResourceMgr::GetSoundBuffer(const std::string& id)
{
	auto it = mapSoundBuffer.find(id);
	if (it != mapSoundBuffer.end())
	{
		return std::get<0>(it->second);
	}
	ERROR_PRINTER.GetFailCheck(id);
    return nullptr;
}

AnimationClip* ResourceMgr::GetAnimationClip(const std::string& id)
{
	auto it = mapAnimationClip.find(id);
	if (it != mapAnimationClip.end())
	{
		return std::get<0>(it->second); it->second;
	}
	return nullptr;

}