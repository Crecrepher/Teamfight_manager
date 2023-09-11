#include "stdafx.h"
#include "InputMgr.h"
#include "Framework.h"
#include "Utils.h"

InputMgr::InputMgr()
{
	{
		AxisInfo infoH;
		infoH.axis = Axis::Horizontal;
		infoH.positivies.push_back((int)sf::Keyboard::Key::D);
		infoH.positivies.push_back((int)sf::Keyboard::Key::Right);
		infoH.negatives.push_back((int)sf::Keyboard::Key::A);
		infoH.negatives.push_back((int)sf::Keyboard::Key::Left);
		axisInfoMap.insert({ infoH.axis,infoH });
	}
	{
		AxisInfo infoV;
		infoV.axis = Axis::Vertical;
		infoV.positivies.push_back((int)sf::Keyboard::Key::S);
		infoV.positivies.push_back((int)sf::Keyboard::Key::Down);
		infoV.negatives.push_back((int)sf::Keyboard::Key::W);
		infoV.negatives.push_back((int)sf::Keyboard::Key::Up);
		axisInfoMap.insert({ infoV.axis,infoV });
	}
}

void InputMgr::Update(float dt)
{
	downList.clear();
	upList.clear();

	for (auto& it : axisInfoMap)
	{
		auto& axisInfo = it.second;
		float raw = GetAxisRaw(axisInfo.axis); // -1.0 , 0, 1.0
		if (raw == 0.f && axisInfo.value != 0.f)
		{
			raw = axisInfo.value > 0.f ? -1.f : 1.f;
		}
		float diff = axisInfo.sensi * dt;
		axisInfo.value = Utils::Clamp(axisInfo.value + raw * diff, -1.0f, 1.0f);
		if (abs(axisInfo.value) < diff * 0.5) //Áß¾Ó°ª º¸Á¤
		{
			axisInfo.value = 0.f;
		}
	}
}

void InputMgr::UpdateEvent(const sf::Event& ev)
{
	switch (ev.type)
	{
	case sf::Event::KeyPressed:
		if (std::find(ingList.begin(), ingList.end(), ev.key.code) == ingList.end())
		{
			downList.push_back(ev.key.code);
			ingList.push_back(ev.key.code);
		}
		break;
	case sf::Event::KeyReleased:
		ingList.remove(ev.key.code);
		upList.push_back(ev.key.code);
		break;
	case sf::Event::MouseButtonPressed:
	{
		int code = sf::Keyboard::KeyCount + ev.mouseButton.button;
		if (std::find(ingList.begin(), ingList.end(), code) == ingList.end())
		{
			downList.push_back(code);
			ingList.push_back(code);
		}
		break;
	}

	case sf::Event::MouseButtonReleased:
	{
		int code = sf::Keyboard::KeyCount + ev.mouseButton.button;
		ingList.remove(code);
		upList.push_back(code);
		break;
	}

	case sf::Event::MouseWheelScrolled:
		if (ev.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
		{
			float delta = ev.mouseWheelScroll.delta;
			if (delta > 0) {
				mouseWheell = MouseWheelDir::WheelUp;
			}
			else if (delta < 0) {
				mouseWheell = MouseWheelDir::WheelDown;
			}
		}
		break;
	}
	mousePos = (sf::Vector2f)sf::Mouse::getPosition(FRAMEWORK.GetWindow());
}

bool InputMgr::GetKeyDown(sf::Keyboard::Key key)
{
	return std::find(downList.begin(), downList.end(), key) != downList.end();
}

bool InputMgr::GetKey(sf::Keyboard::Key key)
{
	return std::find(ingList.begin(), ingList.end(), key) != ingList.end();
}

bool InputMgr::GetKeyUp(sf::Keyboard::Key key)
{
	return std::find(upList.begin(), upList.end(), key) != upList.end();
}

const sf::Vector2f& InputMgr::GetMousePos()
{
	return mousePos;
}

bool InputMgr::GetMouseButtonDown(sf::Mouse::Button button)
{
	int code = sf::Keyboard::KeyCount + (int)button;

	return std::find(downList.begin(), downList.end(), code) != downList.end();
}

bool InputMgr::GetMouseButton(sf::Mouse::Button button)
{
	int code = sf::Keyboard::KeyCount + (int)button;
	return std::find(ingList.begin(), ingList.end(), code) != ingList.end();
}

bool InputMgr::GetMouseButtonUp(sf::Mouse::Button button)
{
	int code = sf::Keyboard::KeyCount + (int)button;
	return std::find(upList.begin(), upList.end(), code) != upList.end();
}

bool InputMgr::GetMouseWheel(MouseWheelDir wheelDir)
{
	if (mouseWheell == wheelDir)
	{
		mouseWheell = MouseWheelDir::None;
		return true;
	}
	return false;
}

float InputMgr::GetAxis(Axis axis)
{
	auto it = axisInfoMap.find(axis);
	if (it == axisInfoMap.end())
	{
		return 0.0f;
	}
	return it->second.value;
}


float InputMgr::GetAxisRaw(Axis axis)
{
	auto it = axisInfoMap.find(axis);
	if (it == axisInfoMap.end())
	{
		return 0.0f;
	}

	const AxisInfo& info = it->second;

	auto rit = ingList.rbegin();
	while (rit != ingList.rend())
	{
		int code = *rit;
		if (std::find(info.positivies.begin(), info.positivies.end(), code) != info.positivies.end())
		{
			return 1.f;
		}
		if (std::find(info.negatives.begin(), info.negatives.end(), code) != info.negatives.end())
		{
			return -1.f;
		}
		++rit;
	}
	return 0.0f;
}
