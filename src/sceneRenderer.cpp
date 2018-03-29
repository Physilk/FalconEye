#include "sceneRenderer.h"

namespace FalconEye {

    SceneRenderOption& SceneRenderOption::defaultRenderOptions = SceneRenderOption();

    SceneRenderOption& SceneRenderOption::getDefaultOptions()
    {
        return defaultRenderOptions;
    }
} // end namespace FalconEye