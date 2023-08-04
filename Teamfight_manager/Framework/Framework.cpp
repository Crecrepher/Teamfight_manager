#include "stdafx.h"
#include "Framework.h"
#include "InputMgr.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "ResourceMgr.h"
#include "DataTableMgr.h"

Framework::Framework(int w, int h, const std::string& t)
    :screenWidth(w), screenHeight(h), title(t)
{
}

void Framework::Init(int width, int height, const std::string& title)
{
	window.create(sf::VideoMode(width, height), title);
    RESOURCE_MGR.Init();
    DATATABLE_MGR.LoadAll();
    SCENE_MGR.Init();
}

void Framework::Release()
{
    DATATABLE_MGR.ReleaseAll();
    SCENE_MGR.Release();
}

void Framework::Update(float dt)
{
    SCENE_MGR.Update(dt);
}

void Framework::Draw()
{
    SCENE_MGR.Draw(window);
}

void Framework::Run()
{
    Init(screenWidth, screenHeight,title);
    clock.restart();

    sf::Image cursorImg;
    cursorImg.loadFromFile("graphics/cursor_bright.png");

    sf::Cursor cursor;
    if (cursor.loadFromPixels(cursorImg.getPixelsPtr(), cursorImg.getSize(), {0,0 /*cursorImg.getSize().x / 2, cursorImg.getSize().y / 2*/ }))
    {
        window.setMouseCursor(cursor);
    }

    while (window.isOpen())
    {
        sf::Time deltaTime = clock.restart();
        float dt = deltaTime.asSeconds();

        INPUT_MGR.Update(dt);

        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                SCENE_MGR.GetCurrScene()->Exit();
                window.close();
                break;
            case sf::Event::GainedFocus:
                window.setMouseCursor(cursor);
                break;
            }
            INPUT_MGR.UpdateEvent(event);
        }

        if (window.isOpen())
		{
			Update(dt);

			window.clear();
			Draw();
			window.display();
        }
       
    }
    Release();
}

sf::Vector2f Framework::GetWindowSize()
{
    return sf::Vector2f((float)screenWidth, (float)screenHeight);
}

sf::RenderWindow& Framework::GetWindow()
{
    return window;
}
