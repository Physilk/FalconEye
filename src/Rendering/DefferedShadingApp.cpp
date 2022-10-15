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

    gBufferProgram = read_program("data/shaders/defered_shading_gbuffer.glsl");
    if (!(gBufferProgram > 0))
    {
        return -1;
    }

    Exemple_Mesh = ResourceManager::Instance().requestMesh("data/bodyparts/torse.obj");
    return 0;
}
int DefferedShadingApp::quit( )
{
    release_program(gBufferProgram);
    gBufferProgram = 0;

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