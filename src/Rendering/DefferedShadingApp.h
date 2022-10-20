#pragma once

#include "gKit/app.h"
#include "LuaInterface/luaObjectInterface.h"
#include "Utils/resourceManager.h"
#include "Rendering/gBuffer.h"
#include "Rendering/RenderPass.h"

#include "glcore.h"

namespace FalconEye
{

class DefferedShadingApp : public App
{
public:
    //! constructeur, dimensions de la fenetre et version d'openGL.
    DefferedShadingApp( const int width, const int height, const int major= 3, const int minor= 3, bool bResizable = true );
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

    Mesh_ptr Exemple_Mesh;
};

} // end namespace FalconEye