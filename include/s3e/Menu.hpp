#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <list>
#include <memory>

namespace Tom::s3e {

    class MenuItem : public sf::Text {
        std::function<void(void)> callback;

    public:
        typedef std::shared_ptr<MenuItem> Ptr;
        typedef std::shared_ptr<const MenuItem> ConstPtr;

        MenuItem(void);
        virtual ~MenuItem();

        void setCallback(std::function<void(void)> callback);

        void click() const;

        bool contains(sf::Vector2f point) const;

        static Ptr create(void);
    };

    /**
     * A menu system for the title screen and the in game pause menu.
     */
    class Menu : public sf::Drawable, public sf::Transformable {
        sf::Font font;
        sf::Text title;
        std::list<MenuItem::Ptr> items;
        bool isMouseDown = false;
        bool visible = false;

    public:

        typedef std::shared_ptr<Menu> Ptr;
        typedef std::shared_ptr<const Menu> ConstPtr;

        Menu(void);
        Menu(const sf::Font & font);
        Menu(const sf::Font & font, const std::string & title);
        virtual ~Menu();

        void setFont(const sf::Font & font);

        void setTitle(const std::string & text);

        void show(void);

        void hide(void);

        bool isVisible(void);

        void addMenuItem(const std::string & text, std::function<void(void)> callback);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        void onMouseMove(sf::Event::MouseMoveEvent e);

        void onMouseDown(sf::Event::MouseButtonEvent e);

        void onMouseUp(sf::Event::MouseButtonEvent e);

        static Ptr create(void);
        static Ptr create(const sf::Font & font);
        static Ptr create(const sf::Font & font, const std::string & title);
    };
}
