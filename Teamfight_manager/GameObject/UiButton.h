#pragma once
#include "SpriteGo.h"
class UiButton : public SpriteGo
{
protected:
    bool isHover = false;
public:
	UiButton(const std::string id = "", const std::string n = "") :SpriteGo(id,n) {}
    virtual ~UiButton() override;

    virtual void Init()override;
    virtual void Reset()override;
    virtual void Release() override;
    virtual void Draw(sf::RenderWindow& window)override;
    virtual void Update(float dt)override;

    sf::Text text;

    std::function<void()> OnClick;
    std::function<void()> OnEnter;
    std::function<void()> OnExit;
};

