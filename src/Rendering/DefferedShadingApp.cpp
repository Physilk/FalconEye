#include "Rendering/DefferedShadingApp.h"
#include "gKit/program.h"

namespace FalconEye
{
    
DefferedShadingApp::DefferedShadingApp( const int width, const int height, const int major, const int minor, bool bResizable)
    :App(width, height, major, minor, bResizable)
{
}

DefferedShadingApp::~DefferedShadingApp( )
{
    delete gBufferPass;
    delete gBuffer;
}

int DefferedShadingApp::init( )
{
    gBuffer = new GBuffer();
    if (gBuffer != nullptr)
    {
        if (!gBuffer->Init(window_width(), window_height()))
        {
            return -1;
        }
    }

    gBufferPass = new RenderPass();
    if (gBufferPass != nullptr)
    {
        if (!gBufferPass->Init("data/shaders/defered_shading_gbuffer.glsl"))
        {
            return -1;
        }
    }

    Exemple_Mesh = ResourceManager::Instance().requestMesh("data/bodyparts/torse.obj");
    return 0;
}
int DefferedShadingApp::quit( )
{
    delete gBufferPass;
    gBufferPass  = nullptr;

    delete gBuffer;
    gBuffer = nullptr;

    return 0;
}

int DefferedShadingApp::update( const float time, const float delta )
{
    return 0;
}

int DefferedShadingApp::render()
{
    return 1;
}

void DefferedShadingApp::LuaRun()
{
    run();
}

} // end namespace FalconEye