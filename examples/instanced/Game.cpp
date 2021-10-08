#include "Game.hpp"

#include <exception>
#include <glm/gtc/noise.hpp>


Game::Game(const sf::String & resPath) : GameBase(), resManager(resPath) {}

Game::~Game() {}

void GLAPIENTRY MessageCallback(GLenum source,
                                GLenum type,
                                GLuint id,
                                GLenum severity,
                                GLsizei length,
                                const GLchar * message,
                                const void * userParam) {
    Logging::Game->error(
        "GL CALLBACK: {} type = 0x{:x}, severity = 0x{:x}, message = {}",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
        message);
}

void Game::onCreate() {
    // defautFont loaded from memory by GameBase
    fps = std::make_shared<FPSDisplay>();
    fps->setFont(uiFont);
    fps->setRate(0.1f);

#ifdef DEBUG
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif

    // Add menu buttons
    menu->addMenuItem("Resume", [&]() {
        menu->hide();
        SetMouseGrab(true);
    });
    menu->addMenuItem("Exit", [&]() {
        window->close();
    });

    // Initialize camera to look at origin
    camera->move({5, 2, 5});
    camera->rotateEuler({0, -1, 0});
    camera->setFov(70);

    instanceShader =
        resManager.loadShader("shader/instance.vert", "shader/instance.frag");
    if (!instanceShader)
        throw std::runtime_error("Failed to load shader/instance");

    scene = std::make_shared<Scene>("Root");

    auto objectScene = resManager.loadScene("model/sphere.obj");
    if (!objectScene)
        throw std::runtime_error("Failed to load model/sphere.obj");
    objectScene->move({0, 2, 0});
    scene->children.push_back(objectScene);

    auto floorScene = resManager.loadScene("model/cube_plane.obj");
    if (!floorScene)
        throw std::runtime_error("Failed to load model/cube_plane.obj");
    scene->children.push_back(floorScene);

    scene->send();

    SetMouseGrab(true);
}

void Game::onDestroy() {}

void Game::onKeyPressed(const sf::Event::KeyEvent & e) {
    GameBase::onKeyPressed(e);
}

void Game::onKeyReleased(const sf::Event::KeyEvent & e) {
    GameBase::onKeyReleased(e);
}

void Game::onMouseMove(const sf::Event::MouseMoveEvent & e) {
    GameBase::onMouseMove(e);
}

void Game::onMouseDown(const sf::Event::MouseButtonEvent & e) {
    GameBase::onMouseDown(e);
}

void Game::onMouseUp(const sf::Event::MouseButtonEvent & e) {
    GameBase::onMouseUp(e);
}

void Game::onResized(const sf::Event::SizeEvent & e) {
    GameBase::onResized(e);
}

void Game::onUpdate(const sf::Time & delta) {
    float deltaS = delta.asSeconds();
    fps->update(delta);
}

void Game::onDraw() const {

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glm::mat4 vp = camera->projMatrix() * camera->toMatrix();

    instanceShader->setMat4("mvp", vp);
    scene->draw(instanceShader);

    window->pushGLStates();
    window->draw(*fps);
    window->popGLStates();
}
