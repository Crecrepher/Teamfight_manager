#pragma once
#include <functional>
enum class AnimationLoopTypes
{
	Single,
	Loop,
};

struct AnimationFrame
{
	std::string textureId;
	sf::IntRect texCoord;
	std::function<void()> action;
};

struct AnimationClip
{
	std::string id;
	AnimationLoopTypes loopType;
	int fps;

	std::vector<AnimationFrame> frames;
	bool LoadFromFile(const std::string path);
};