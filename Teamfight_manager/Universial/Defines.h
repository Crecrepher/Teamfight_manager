#pragma once


enum class Languages
{
	KOR,
	ENG,
	JP,
	COUNT,
};

enum class Direction
{
	Left,
	Right,
	None
};

enum class SceneId
{
	None = -1,
	Title,
	Home,
	Game,
	ChampEdit,
	Count
};

enum class ResourceTypes
{
	Texture,
	Font,
	Sound,
	AnimationClip,
};

enum class Origins
{
	TL,
	TC,
	TR,
	ML,
	MC,
	MR,
	BL,
	BC,
	BR,
	CUSTOM,
};

#define SCREEN_WIDTH 1280;
#define SCREEN_HEIGHT 720;

#include "AnimatonClip.h"