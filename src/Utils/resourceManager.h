#pragma once

#include <map>
#include <memory>
#include <string>

#include "gKit/image.h"
#include "gKit/image_io.h"
#include "gKit/mesh.h"

using std::map;
using Image_ptr = std::shared_ptr<Image>;
using Mesh_ptr = std::shared_ptr<Mesh>;
using std::string;

namespace FalconEye {
    class ResourceManager {
    public:
        static ResourceManager& Instance() { return instance; }

    private:
        static ResourceManager instance;

        map<string, Image_ptr> image_map;
        map<string, Mesh_ptr> mesh_map;

        ResourceManager();
        ResourceManager(const ResourceManager&) = default;
        ~ResourceManager();

    public:
        Image_ptr requestImage(const string &path);
        Image_ptr requestImage(const char *path) { return requestImage(string(path)); }

        Mesh_ptr requestMesh(const string &path);
        Mesh_ptr requestMesh(const char *path) { return requestMesh(string(path)); }
    };
} // end namespace FalconEye
