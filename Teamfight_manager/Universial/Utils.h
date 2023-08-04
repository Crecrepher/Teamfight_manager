#pragma once
#include <iostream>
#include <random>
#include <SFML/Graphics.hpp>
#include "Defines.h"

class Utils
{
private:
	static std::random_device rd;
	static std::mt19937 gen;

public:
	static int RandomRange(int min, int maxExclude);
	static float RandomRange(float min, float max);

	static sf::Vector2f RandomOnCircle(float radius);
	static sf::Vector2f RandomInCircle(float radius);
	static float RandomValue();//0.0f ~1.0f

	static void SetOrigin(sf::Sprite& sprite, Origins origin);
	static void SetOrigin(sf::Text& text, Origins origin);
	static void SetOrigin(sf::Shape& shape, Origins origin);

	static void SetOrigin(sf::Transformable& obj, Origins origin, const sf::FloatRect& rect);

	/*static int Clamp(int v, int min, int max);*/
	static float Clamp(float v, float min, float max);
	static sf::Vector2f Clamp(const sf::Vector2f& v, const sf::Vector2f& min, const sf::Vector2f& max);
	static float Lerp(float a, float b, float t, bool clamping = true);
	static sf::Vector2f Lerp(const sf::Vector2f a, const sf::Vector2f b, float t, bool clamping = true);
	static sf::Color Lerp(const sf::Color& a, const sf::Color& b, float t, bool clamping = true);

	// Vector2
	static float SqrMagnitude(const sf::Vector2f& vec);
	static float Magnitude(const sf::Vector2f& vec);
	static sf::Vector2f Normalize(const sf::Vector2f& vec);
	static float Distance(const sf::Vector2f& a, const sf::Vector2f& b); //거리 구하는 함수

	static float Angle(const sf::Vector2f& start, const sf::Vector2f& end);
	static float Angle(const sf::Vector2f& dir);
	int FixSign(float number); //-1,1 clamp
};

