#include "Game.hpp"

#include <stdexcept>

using std::make_shared;
using glpp::extra::Grid;

Game::Game(Window::Ptr & window) : GameBase(window), res("../../examples/res") {

    drawGrid = true;
    drawMarker = false;
    move = true;

    step = 0.1;

    shader = res.getMVPShader("shader/default.vert", "shader/default.frag");

    camera.setPosition({3, 2, 3});
    camera.setRotation({0.2, -0.75, 0});

    scene.grid = make_shared<Grid>(10, vec4(1, 1, 1, 1), true);

    marker = make_shared<Diamond>(vec2(0.03), vec4(1.0, 0.0, 0.0, 1.0), &camera);

    box.aabb = AABB({0, 0, 0}, {1, 1, 1});
    box.scene = scene.addChild();
    box.scene->models = res.loadModel("model/corner_cube.obj");
    box.scene->models[0]->material->shader = shader;

    ball.sphere.p = glm::vec3(0.5, 0.5, 1.5);
    ball.sphere.r = 0.2;
    ball.scene = scene.addChild();
    ball.scene->models = res.loadModel("model/sphere.obj");
    ball.scene->models[0]->material->shader = shader;

    reset();

    window->setMouseGrab(true);
}

Game::~Game() {}

void Game::reset() {
    ball.sphere.p.z = 1.5;
}

void Game::onKeyReleased(const sf::Event::KeyEvent & event) {
    GameBase::onKeyReleased(event);
    switch (event.code) {
        case sf::Keyboard::G:
            drawGrid = !drawGrid;
            break;
        case sf::Keyboard::M:
            drawMarker = !drawMarker;
            break;
        case sf::Keyboard::R:
            reset();
            break;
        case sf::Keyboard::Space:
            move = !move;
            break;
        case sf::Keyboard::Up:
            ball.sphere.p.y += step;
            break;
        case sf::Keyboard::Down:
            ball.sphere.p.y -= step;
            break;
        case sf::Keyboard::Left:
            ball.sphere.p.x -= step;
            break;
        case sf::Keyboard::Right:
            ball.sphere.p.x += step;
            break;
        case sf::Keyboard::PageUp:
            step *= 1.1;
            break;
        case sf::Keyboard::PageDown:
            step *= 0.9;
            break;
        default:
            GameBase::onKeyReleased(event);
            break;
    }
}

static glm::vec3 aabb_center(const AABB & aabb) {
    return glm::min(aabb.a, aabb.b);
}

static glm::vec3 aabb_scale(const AABB & aabb) {
    auto max = glm::max(aabb.a, aabb.b);
    auto min = glm::min(aabb.a, aabb.b);
    return max - min;
}

void Game::onUpdate(const sf::Time & delta) {
    float s = delta.asSeconds();

    if (move) {
        ball.sphere.p += glm::vec3(0, 0, -s * step);
    }

    bool fullCollide = true;
    if (fullCollide) {
        drawMarker = collides(ball.sphere, box.aabb);
    }
    else {
        drawMarker = false;
        for (auto & p : box.aabb.points()) {
            drawMarker = drawMarker || collides(p, ball.sphere);
        }
    }

    move = !drawMarker && move;

    box.scene->transform.setPosition(aabb_center(box.aabb));
    box.scene->transform.setScale(aabb_scale(box.aabb));
    ball.scene->transform.setPosition(ball.sphere.p);
    ball.scene->transform.setScale(glm::vec3(ball.sphere.r));

    marker->setPos(ball.sphere.p);
}

inline void setupGl() {
    glClearColor(0.25, 0.25, 0.25, 1.0);
    glDisable(GL_CULL_FACE);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::onDraw() const {
    setupGl();

    RenderState state(camera);
    state.setGridEnable(drawGrid);
    scene.draw(state);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    if (drawMarker)
        marker->draw();

    glpp::BufferArray::unbind();
}
