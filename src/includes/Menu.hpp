#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <list>
#include <memory>

namespace game {
    class MenuItem : public sf::Text {
        std::function<void(void)> callback;

    public:
        typedef std::shared_ptr<MenuItem> Ptr;
        typedef std::shared_ptr<const MenuItem> ConstPtr;

        MenuItem(void);
        virtual ~MenuItem();

        void setCallback(std::function<void(void)> callback);

        void click();

        bool contains(sf::Vector2f point);

        static Ptr create(void);
    };

    class Menu : public sf::Drawable, public sf::Transformable {
        sf::Font font;
        sf::Text title;
        std::list<MenuItem> items;
        bool isMouseDown = false;

    public:
        bool isVisible = false;

        typedef std::shared_ptr<Menu> Ptr;
        typedef std::shared_ptr<const Menu> ConstPtr;

        Menu(void);
        Menu(const sf::Font & font);
        Menu(const sf::Font & font, const std::string &title);
        virtual ~Menu();

        void setFont(const sf::Font &font);

        void setTitle(const std::string &text);

        void show();

        void hide();

        void addMenuItem(const std::string &text, std::function<void(void)> callback);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void onMouseMove(sf::Event::MouseMoveEvent e);

        void onMouseDown(sf::Event::MouseButtonEvent e);

        void onMouseUp(sf::Event::MouseButtonEvent e);

        static Ptr create(void);
        static Ptr create(const sf::Font &font);
        static Ptr create(const sf::Font &font, const std::string &title);
    };
}
