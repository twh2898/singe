#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <list>
#include <memory>
#include <string>

#include "singe/Core/Window.hpp"

namespace singe {
    using std::string;
    using std::shared_ptr;
    using std::function;

    /**
     * A single item in a Menu. Currently this can be used as a label or a
     * button.
     */
    class MenuItem : public sf::Text {
    public:
        using Ptr = shared_ptr<MenuItem>;
        using ConstPtr = shared_ptr<const MenuItem>;

    private:
        function<void()> callback;

    public:
        /**
         * Create a new MenuItem with a callback for click events.
         *
         * @param callback the callback for click events
         */
        MenuItem(function<void()> callback);

        /// Destructor
        virtual ~MenuItem();

        /**
         * Set the lambda to be called when onClick() is called.
         *
         * @param callback the calback for click events
         */
        void setCallback(function<void()> callback);

        /**
         * Handle a click event for this item by calling the callback lambda.
         */
        void onClick() const;

        /**
         * Check if the bounds of this MenuItem contain a point.
         *
         * @param point the point to check
         *
         * @return is the point within the bounds of this
         */
        bool contains(const sf::Vector2f & point) const;
    };

    /**
     * A menu system for the title screen and the in game pause menu.
     */
    class Menu : public sf::Drawable, public sf::Transformable, public EventHandler {
    public:
        using Ptr = shared_ptr<Menu>;
        using ConstPtr = shared_ptr<const Menu>;

    private:
        sf::Font font;
        sf::Text title;
        std::list<MenuItem::Ptr> items;
        bool isMouseDown;
        bool visible;

    public:
        /**
         * Construct a Menu.
         */
        Menu();

        /**
         * Construct a Menu and use `font` for the title and menu items.
         *
         * @param font the default font for the Menu
         */
        Menu(const sf::Font & font);

        /**
         * Construct a Menu, use `font` for the title and menu items and set
         * the title to `title`.
         *
         * @param font the Font to use for the title and menu items
         * @param title the menu title
         */
        Menu(const sf::Font & font, const string & title);

        /**
         * Destruct the Menu.
         */
        virtual ~Menu();

        /**
         * Set the Font to be used by the menu title and menu items.
         *
         * @param font the Font to be used for the title and menu items
         */
        void setFont(const sf::Font & font);

        /**
         * Set the menu title. This is displayed above the menu items.
         *
         * @param text the new title
         */
        void setTitle(const string & text);

        /**
         * Make the Menu visible and start accepting events.
         */
        void show();

        /**
         * Hide the Menu and stop responding to events.
         */
        void hide();

        /**
         * Is the Menu currently visible.
         *
         * @return is the Menu visible
         */
        bool isVisible() const;

        /// TODO: Document addMenuItem once it's fate is decided
        bool addMenuItem(const string & text, function<void()> callback);

        /**
         * Override for sf::Drawable::draw. If the menu is visible, this method
         * will draw a background, the title and the menu items.
         *
         * @param target the sf::RenderTarget to draw to
         * @param states the current sf::RenderStates used to draw the title and
         *               menu items
         */
        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        /**
         * Event callback for the mouse move event. If the menu is not visible,
         * this method will ignore the event.
         *
         * @param e the mouse move event
         */
        void onMouseMove(const sf::Event::MouseMoveEvent & e) override;

        /**
         * Event callback for the mouse down event. If the menu is not visible,
         * this method will ignore the event.
         *
         * @param e the mouse press event
         */
        void onMouseDown(const sf::Event::MouseButtonEvent & e) override;

        /**
         * Event callback for the mouse up event. If the menu is not visible,
         * this method will ignore the event.
         *
         * @param e the mouse up event
         */
        void onMouseUp(const sf::Event::MouseButtonEvent & e) override;
    };
}
