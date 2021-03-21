#include "Game.hpp"
#include <exception>
#include <glm/gtc/noise.hpp>
#include <spdlog/spdlog.h>


static void getGlError() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        SPDLOG_ERROR("glGetError() returned {}", err);
    }
}

Game::Game(const sf::String & resPath) : GameBase(), resManager(resPath) { }

Game::~Game() { }

void GLAPIENTRY MessageCallback( GLenum source,
                                 GLenum type,
                                 GLuint id,
                                 GLenum severity,
                                 GLsizei length,
                                 const GLchar *message,
                                 const void *userParam ) {
    SPDLOG_ERROR("GL CALLBACK: {} type = 0x{:x}, severity = 0x{:x}, message = {}",
                 ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
                 type, severity, message );
}

bool Game::onCreate() {
    // defautFont loaded from memory by GameBase
    fps = std::make_shared<FPSDisplay>();
    fps->setFont(uiFont);
    fps->setRate(0.1f);

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // Add menu buttons
    menu->addMenuItem("Resume", [&]() {
        menu->hide();
        SetMouseGrab(true);
    });
    menu->addMenuItem("Exit", [&]() {
        window->close();
    });

    // Initialize camera to look at origin
    camera->move({-3, 2, -1});
    camera->rotate({0, 110});
    camera->setFov(70);

    shader = resManager.loadShader("shader/shader.vert", "shader/shader.frag");
    if (!shader)
        return false;

    devTexture = resManager.loadTexture("devTexture", "img/uv.png");
    if (!devTexture)
        return false;
    devTexture->setFilter(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);

    for (int x = 0; x < SubChunk::N; x++) {
        for (int z = 0; z < SubChunk::N; z++) {
            float u1 = x / (float)SubChunk::N;
            float u2 = (x + 1) / (float)SubChunk::N;
            float v1 = z / (float)SubChunk::N;
            float v2 = (z + 1) / (float)SubChunk::N;
            UV uv(u1, v1, u2, v2);
            styles.push_back(std::make_shared<BlockStyle>(uv, uv, uv, uv, uv, uv));
        }
    }

    auto style = std::make_shared<BlockStyle>();
    Cube cube ({0, 0, 0}, style);

    chunk = std::make_shared<Chunk>(glm::vec3(0, 0, 0), 5);
    for (int x = 0; x < SubChunk::N; x++) {
        for (int z = 0; z < SubChunk::N; z++) {
            auto height = 1 + glm::simplex(glm::vec2(x * 0.05, z * 0.05)) * 4;
            SPDLOG_DEBUG("Height {}", height);
            for (int y = 0; y < height; y++) {
                chunk->set(x, y, z, styles[x * 8 + z]);
            }
        }
    }
    model = std::make_shared<Model>();
    //bool res = model->loadFromPoints(cube.toPoints({0, 0, 0}));
    bool res = model->loadFromPoints(chunk->toPoints());
    if (!res)
        return false;

    SetMouseGrab(true);
    getGlError();
    return true;
}

void Game::onDestroy() { }

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

    if (e.button == sf::Mouse::Left) {
        if (step > 1)
            step -= 1;
    }
    else
        step += 1;

    for (int x = 0; x < SubChunk::N; x++) {
        for (int y = 0; y < SubChunk::N * chunk->subchunks.size(); y++) {
            for (int z = 0; z < SubChunk::N; z++) {
                chunk->get(x, y, z).enabled = x % step == 0 && z % step == 0 && y % step == 0;
            }
        }
    }
    model = std::make_shared<Model>();
    model->loadFromPoints(chunk->toPoints());
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
    glPointSize(10);

    glm::mat4 vp = camera->projMatrix() * camera->viewMatrix();

    shader->bind();
    devTexture->bind();
    shader->setMat4("mvp", vp);
    shader->setMat4("model", model->modelMatrix());
    {
        model->draw();

        glBegin(GL_POINTS);
        {
            glVertex3d(0, 0, 0);
            glVertex3d(1, 0, 0);
            glVertex3d(0, 1, 0);
            glVertex3d(0, 0, 1);
            glVertex3d(-0.5, 0, 0);
            //glVertex3d(0, 0, -0.5);
        }
        glEnd();
    }
    shader->unbind();
    devTexture->unbind();


    window->pushGLStates();
    window->draw(*fps);
    window->popGLStates();
}

