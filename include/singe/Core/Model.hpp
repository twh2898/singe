#pragma once

#include <Transform3d.hpp>
#include <memory>
#include <string>

#include "singe/Core/Material.hpp"
#include "singe/Core/Mesh.hpp"
#include "singe/Graphics/VBO.hpp"

namespace singe {
    /**
     * A Model which has a mesh and model transforms.
     */
    struct Model : public Mesh {

        using Ptr = std::shared_ptr<Model>;
        using ConstPtr = std::shared_ptr<const Model>;

        std::vector<Material::Ptr> materials;
        std::string name;

        /**
         * Create a new Model with name and Mesh parameters.
         *
         * @param name the model name
         * @param mode the Mesh draw mode
         * @param usage the Mesh update usage
         */
        Model(const std::string & name);

        /**
         * Destruct the Model.
         */
        virtual ~Model();

        /**
         * Send the Mesh and any materials to the OpenGL context.
         */
        void send() override;

        /**
         * Bind textures and draw the mesh.
         */
        void draw() const override;
    };
}
