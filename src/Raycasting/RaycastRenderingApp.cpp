#include "RaycastRenderingApp.h"

namespace FalconEye
{
    int RaycastRenderingApp::init()
    {
        if (scene == nullptr)
        {
            scene = std::make_shared<Scene>();
        }
        
        sceneRenderer = std::make_shared<SceneRenderer>();
        renderingContext = std::make_shared<RenderingContext>(scene.get(), sceneRenderer.get());
        sceneRenderer->SetContext(renderingContext);

        return 0;
    }

    int RaycastRenderingApp::quit()
    {
        return 0;
    }

    int RaycastRenderingApp::update(const float time, const float delta)
    {
        return 0;
    }

    int RaycastRenderingApp::render()
    {
        return 1;
    }

    void RaycastRenderingApp::LuaRun()
    {
        run();
    }
} // en namespace FalconEye