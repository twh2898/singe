#include "singe/Core/Model.hpp"

#include "glpp/VBO.hpp"
#include "singe/Support/Util.hpp"
#include "singe/Support/log.hpp"

namespace singe {

    Model::Model(const std::string & name) : Transform3d(), name(name) {}

    Model::~Model() {}

    void Model::send() {
        for (auto & geom : geometry) {
            geom.mesh->send();
        }
    }

    void Model::draw(RenderState state) const {
        state.transform *= toMatrix();
        state.sendMVP();
        for (auto & geom : geometry) {
            materials[geom.materialId]->bind(state.shader);
            geom.mesh->draw();
        }
    }
}
