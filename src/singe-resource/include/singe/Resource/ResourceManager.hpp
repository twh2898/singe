#pragma once

#include <stb_image.h>

#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <future>
#include <glpp/Texture.hpp>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "singe/Graphics/Material.hpp"
#include "singe/Graphics/Model.hpp"
#include "singe/Graphics/Scene.hpp"
#include "singe/Graphics/Shader.hpp"
#include "singe/Support/log.hpp"

namespace singe::Logging {
    extern Logger::Ptr Resource;
}

namespace singe {
    using std::map;
    using std::string;
    using std::vector;
    using std::shared_ptr;
    using glpp::Texture;

    using std::atomic_bool;
    using std::condition_variable;
    using std::future;
    using std::mutex;
    using std::pair;
    using std::promise;
    using std::queue;
    using std::thread;

    namespace fs = std::filesystem;

    class ResourceLoadException : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    template<typename T>
    class ResourceCache {
        map<string, T> cache;
        mutex m;

    public:
        ResourceCache();

        void add(const string & key, const T & item) {
            unique_lock lk(m);
            cache.emplace(key, item);
        }

        void add(const string & key, T && item) {
            unique_lock lk(m);
            cache.emplace(key, move(item));
        }

        bool contains(const string & key) {
            unique_lock lk(m);
            return cache.find(key) != cache.end();
        }

        // FIXME: reference breaks on next add()
        T & get(const string & key) {
            unique_lock lk(m);
            return cache[key];
        }
    };

    class ResourceLoader {
        ResourceCache<void> textureCache;
        ResourceCache<void> modelCache;
        thread t;
        mutex m;
        condition_variable cv;


        vector<thread> workers;

    public:
        ResourceLoader();

        virtual ~ResourceLoader();

        future<void> loadScene(const string & path);
    };

    /**
     * Manage path resolution, resource loading and resource caching for re-use.
     */
    class ResourceManager {
        fs::path root;
        map<string, shared_ptr<Texture>> textures;
        map<string, shared_ptr<Shader>> shaders;
        map<string, shared_ptr<MVPShader>> mvpShaders;

        atomic_bool sceneLoadRunning;
        mutex sceneLoadMux;
        condition_variable sceneLoadCV;
        thread sceneLoadThread;
        queue<pair<promise<shared_ptr<Scene>>, string>> sceneLoadQueue;

        static void sceneLoadWorker(ResourceManager * manager);

    public:
        /**
         * Create a ResourceManager with all resources located at root.
         *
         * @param root the resource directory
         */
        ResourceManager(const fs::path & root);

        ResourceManager(ResourceManager && other);

        ResourceManager & operator=(ResourceManager && other);

        ResourceManager(const ResourceManager &) = delete;
        ResourceManager & operator=(const ResourceManager &) = delete;

        virtual ~ResourceManager();

        /**
         * Update the path to the resource directory
         *
         * @param root the new resource directory
         */
        void setRoot(const fs::path & root);

        /**
         * Get the path to the current resource directory.
         *
         * @return the current resource directory
         */
        const fs::path & getRoot() const;

        /**
         * Resolve a relative path to an absolute path.
         *
         * @param subPath relative path to the resource
         *
         * @return the absolute path to the resource
         */
        fs::path resourceAt(const fs::path & subPath) const;

        /**
         * Load a glpp::Texture or return the cached texture if it exists.
         *
         * If useCached is false, the loaded texture will not be added to the
         * cache.
         *
         * @param path the texture path relative to resource root
         * @param useCached should a cached version be returned if present
         *
         * @return shared_ptr to the glpp::Texture
         */
        shared_ptr<Texture> getTexture(const string & path, bool useCached = true);

        /**
         * Load a Shader or return the cached shader if it exists.
         *
         * If useCached is false, the loaded shader will not be added to the
         * cache.
         *
         * @param vertPath the vertex shader path relative to resource root
         * @param fragPath the fragment shader path relative to resource root
         * @param useCached should a cached version be returned if present
         *
         * @return shared_ptr to the Shader
         */
        shared_ptr<Shader> getShader(const string & vertPath,
                                     const string & fragPath,
                                     bool useCached = true);

        /**
         * Load an MVPShader or return the cached shader if it exists.
         *
         * If useCached is false, the loaded shader will not be added to the
         * cache.
         *
         * @param vertPath the vertex shader path relative to resource root
         * @param fragPath the fragment shader path relative to resource root
         * @param useCached should a cached version be returned if present
         *
         * @return shared_ptr to the MVPShader
         */
        shared_ptr<MVPShader> getMVPShader(const string & vertPath,
                                           const string & fragPath,
                                           bool useCached = true);

        /**
         * Load a model.
         *
         * @param path the model path relative to resource root
         * @param useCached parameter is passed to calls of getTexture and
         *                  getShader
         *
         * @return vector of models
         */
        vector<shared_ptr<Model>> loadModel(const string & path,
                                            bool useCached = true);

        /**
         * Load a scene.
         *
         * @param path the scene path relative to resource root
         *
         * @return shared_ptr to the Scene
         */
        shared_ptr<Scene> loadScene(const string & path);

        future<shared_ptr<Scene>> aLoadScene(const string & path);
    };
}
