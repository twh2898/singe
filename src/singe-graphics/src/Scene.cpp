#include "singe/Graphics/Scene.hpp"

#include <memory>

namespace singe {
    using std::make_shared;
    using std::move;

    Scene::Scene() {}

    Scene::Scene(Scene && other)
        : children(move(other.children)),
          models(move(other.models)),
          transform(move(other.transform)),
          grid(move(other.grid)) {}

    Scene & Scene::operator=(Scene && other) {
        children = move(other.children);
        models = move(other.models);
        transform = move(other.transform);
        grid = move(other.grid);
        return *this;
    }

    Scene::~Scene() {}

    Scene::Ptr & Scene::addChild() {
        return children.emplace_back(make_shared<Scene>());
    }

    Model::Ptr & Scene::addModel() {
        return models.emplace_back(make_shared<Model>());
    }

    void Scene::draw(RenderState state) const {
        state.pushTransform(transform);
        if (grid && state.getGridEnable())
            grid->draw(state.getMVP());
        for (auto & model : models) model->draw(state);
        for (auto & child : children) child->draw(state);
    }
}
