#pragma once

#include <singe/Core/FPSDisplay.hpp>
#include <singe/Core/GameBase.hpp>
#include <singe/Core/ResourceManager.hpp>
#include <singe/Core/Window.hpp>
#include <singe/Graphics/Model.hpp>
#include <singe/Graphics/Scene.hpp>
#include <singe/Graphics/Shader.hpp>
#include <singe/Support/log.hpp>
using namespace singe;

#include <glpp/extra/Line.hpp>
#include <memory>

#include "Diamond.hpp"

class Game : public GameBase {
    ResourceManager res;
    FPSDisplay fps;
    std::shared_ptr<singe::MVPShader> shader;
    std::shared_ptr<singe::Shader> circle_shader;
    Scene scene;
    shared_ptr<Diamond> circle;
    glpp::extra::Line::Ptr line;

public:
    Game(Window::Ptr & window);
    virtual ~Game();

    void onUpdate(const sf::Time & delta) override;
    void onDraw(void) const override;
};
