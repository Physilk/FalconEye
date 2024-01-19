#pragma once

#define HAVE_M_PI
#include "gKit/app.h"
#include "LuaInterface/luaObjectInterface.h"
#include "Utils/resourceManager.h"
#include "Rendering/gBuffer.h"
#include "Rendering/RenderPass.h"
#include "Rendering/ShaderManager.h"
#include "Rendering/Texture2D.h"

#include "glcore.h"
#include "orbiter.h"

namespace FalconEye
{

class DefferedShadingApp : public App
{
public:
    //! constructeur, dimensions de la fenetre et version d'openGL.
    DefferedShadingApp( const int width, const int height, const int major= 4, const int minor= 4, bool bResizable = true );
    virtual ~DefferedShadingApp( );

    //! a deriver pour creer les objets openGL.
    virtual int init( ) override;
    //! a deriver pour detruire les objets openGL.
    virtual int quit( ) override;

    //! a deriver et redefinir pour animer les objets en fonction du temps.
    virtual int update( const float time, const float delta ) override;
    //! a deriver pour afficher les objets.
    virtual int render() override;

    void LuaRun();

    //void LuaRun() { Run(); }
    LUA_BEGIN_BIND_METHODS(DefferedShadingApp)
            LUA_BIND_CONSTRUCTOR(const int, const int)
            LUA_BIND_METHOD(DefferedShadingApp, LuaRun)
            LUA_END_BIND_METHODS

protected:
    GBuffer* gBuffer;
    RenderPass* gBufferPass;
    RenderPass* LightingPass;

    Mesh_ptr Exemple_Mesh;
    GLuint Exemple_buffer;
    Image_ptr Exemple_Image;
    GLuint Exemple_Texture;

    std::vector<Image_ptr> CubemapImages;
	Texture2D_ptr Cubemaptexture;
    Mesh_ptr CubemapMesh;
    GLuint CubemapBuffer;
    RenderPass_ptr CubemapRenderPass;
    RenderPass_ptr CubemapRenderPass_deffered;

    ShaderProgram_ptr UnlitLightingShader;

    Orbiter Camera;
    float FOV;

    const int WindowWidth;
    const int WindowHeight;

    GLuint DummyVAO;
private:

    bool bGBufferDisplay = false;
    bool bDepthDisplay = false;
    bool bStencilDisplay = false;

    void SetupCubemap();
};

} // end namespace FalconEye