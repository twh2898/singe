#include "singe/Graphics/Model.hpp"

#include <singe/Support/Util.hpp>
#include <singe/Support/log.hpp>

namespace singe {

    Model::Model(const std::string & name) : Transform(), name(name) {}

    Model::~Model() {}

    void Model::draw(glpp::Buffer::Mode mode, RenderState state) const {
        state.transform *= toMatrix();
        state.sendMVP();
        for (auto & geom : geometry) {
            materials[geom.materialId]->bind(state.shader);
            geom.mesh->draw(mode);
        }
    }
}
