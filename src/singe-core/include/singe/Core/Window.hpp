#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <memory>
#include <string>
#include <vector>

namespace singe {

    class EventHandler {
    public:
        /**
         * Event callback for a key press event.
         *
         * @param event the key press event
         */
        virtual void onKeyPressed(const sf::Event::KeyEvent & event) {}

        /**
         * Event callback for a key release event
         *
         * @param event the key release event
         */
        virtual void onKeyReleased(const sf::Event::KeyEvent & event) {}

        /**
         * Event callback for a mouse move event.
         *
         * @param event the mouse move event
         */
        virtual void onMouseMove(const sf::Event::MouseMoveEvent & event) {}

        /**
         * Event callback for a mouse button press.
         *
         * @param event the mouse button event
         */
        virtual void onMouseDown(const sf::Event::MouseButtonEvent & event) {}

        /**
         * Event callback for a mouse button release.
         *
         * @param event the mouse button event
         */
        virtual void onMouseUp(const sf::Event::MouseButtonEvent & event) {}

        /**
         * Event callback for a mouse scroll event.
         *
         * @param event the mouse scroll event
         */
        virtual void onMouseScroll(const sf::Event::MouseWheelScrollEvent & event) {
        }

        /**
         * Event callback for a window resize event.
         *
         * @param event the window resize event
         */
        virtual void onResized(const sf::Event::SizeEvent & event) {}
    };

    class Window {
        bool grab;

    protected:
        /// Pointer to the SFML RenderWindow object
        sf::RenderWindow window;

        std::vector<EventHandler *> handlers;

    public:
        Window(const std::string & title,
               unsigned int width = 800,
               unsigned int height = 600,
               bool fullscreen = false);

        /**
         * Default move constructor.
         */
        Window(Window && other) = default;

        /**
         * Default move assign operator.
         */
        Window & operator=(Window && other) = default;

        ~Window();

        /**
         * Grab or release the mouse.
         *
         * @param grab should the mouse be grabbed or released
         */
        void setMouseGrab(bool grab);

        bool isOpen() const;

        void close();

        void poll();

        void display();

        void addEventHandler(EventHandler * handler);
    };
}
