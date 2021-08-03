#include "s3e/Graphics/Model.hpp"

#include "s3e/Graphics/VBO.hpp"
#include "s3e/Support/Util.hpp"
#include "s3e/Support/log.hpp"

namespace Tom::s3e {

    Model::Model(const std::string & name, Mode mode, Usage usage)
        : Mesh(mode, usage), name(name) {}

    Model::~Model() {}

    void Model::draw() const {
        for (int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            textures[i]->bind();
        }
        Mesh::draw();
    }
}
