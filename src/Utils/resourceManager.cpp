#include "resourceManager.h"

#ifdef GK_MACOS
#include <SDL2_image/SDL_image.h>
#else
#ifdef WIN32
#include <SDL.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#endif
#endif

#include "gKit/wavefront.h"

namespace FalconEye {
    ResourceManager ResourceManager::instance = ResourceManager();

    ResourceManager::ResourceManager() : image_map() {

    }

    ResourceManager::~ResourceManager() {

    }

    Image_ptr ResourceManager::requestImage(const string & path)
    {
        auto it = image_map.find(path);
        if (it != image_map.end())
        {
            return it->second;
        }
        Image img = read_image(path.c_str());
        if (img == Image::error())
        {
            std::cerr << "Error: image " << path << " could not be load\n";
            return Image_ptr(nullptr);
        }
        Image_ptr ptr = Image_ptr(new Image(img));
        image_map.insert(std::pair<string, Image_ptr>(path, ptr));

        return ptr;
    }

    Mesh_ptr ResourceManager::requestMesh(const string & path)
    {
        auto it = mesh_map.find(path);
        if (it != mesh_map.end())
        {
            return it->second;
        }
        Mesh_ptr ptr = Mesh_ptr(new Mesh(read_mesh(path.c_str())));
        mesh_map.insert(std::pair<string, Mesh_ptr>(path, ptr));

        return ptr;
    }
} // end namespace FalconEye
