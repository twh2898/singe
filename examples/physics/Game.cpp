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
    camera->move({-3, 2, -1});
    camera->rotateEuler({0, 1, 0});
    camera->setFov(70);

    devTexture = resManager.loadTexture("img/uv.png");
    if (!devTexture)
        throw std::runtime_error("Failed to load img/uv.png");

    physics = std::make_shared<Physics>();
    // physics->loadObjects();

    floorModel = resManager.loadScene("model/ground.obj");
    if (!floorModel)
        throw std::runtime_error("Failed to load model/ground.obj");
    floorModel->send();
    // auto *mesh = new btTriangleIndexVertexArray(/* TODO */);
    // btCollisionShape *collisionShape = new btBvhTriangleMeshShape(mesh, false);
    btCollisionShape * collisionShape = new btStaticPlaneShape({0, 1, 0}, 0);
    physics->makeRigidBody(collisionShape, 0, 1);


    objectModel = resManager.loadScene("model/sphere.obj");
    if (!objectModel)
        throw std::runtime_error("Failed to load model/sphere.obj");
    objectModel->send();
    collisionShape = new btSphereShape(2);
    physics->makeRigidBody(collisionShape, 1, 1);

    SetMouseGrab(true);
}

void Game::loadObjects() {
    {
        btCollisionShape * groundShape =
            new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

        btRigidBody * body = physics->makeRigidBody(groundShape, 0, 1);

        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0, -56, 0));
        groundTransform.setRotation(btQuaternion(btVector3(1, 0, 0), 3.14 * 0.03));
        body->getMotionState()->setWorldTransform(groundTransform);
    }

    {
        btCollisionShape * colShape = new btSphereShape(btScalar(1.));

        btRigidBody * body = physics->makeRigidBody(colShape, 1, 1);

        /// Create Dynamic Objects
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(2, 0, 0));
        body->getMotionState()->setWorldTransform(startTransform);
        body->setLinearVelocity(btVector3(0, 0, 0));
    }
}


void Game::onDestroy() {}

void Game::onKeyPressed(const sf::Event::KeyEvent & e) {
    GameBase::onKeyPressed(e);

    if (e.code == sf::Keyboard::Space) {
        physics->removeObjects();
        loadObjects();
    }
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

    physics->step(delta);
    // physics->printObjectsLocations();

    // camera->setPosition(objectModel->getPosition() + glm::vec3(-3, 2, -1));
}

void Game::onDraw() const {

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glPointSize(10);

    glm::mat4 vp = camera->projMatrix() * camera->toMatrix();

    defaultShader->bind();
    devTexture->bind();
    defaultShader->setMat4("mvp", vp);
    {
        // shader->setMat4("model", floorModel->modelMatrix());
        btTransform trans;
        physics->getTransform(0, trans);
        glm::mat4 model;
        trans.getOpenGLMatrix(&model[0][0]);
        floorModel->draw(defaultShader, model);

        // shader->setMat4("model", objectModel->modelMatrix());
        physics->getTransform(1, trans);
        trans.getOpenGLMatrix(&model[0][0]);
        objectModel->draw(defaultShader, model);
    }
    defaultShader->unbind();
    devTexture->unbind();


    window->pushGLStates();
    window->draw(*fps);
    window->popGLStates();
}
