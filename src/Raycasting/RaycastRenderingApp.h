#pragma once

#include "gKit/app.h"
#include "scene.h"
#include "sceneRenderer.h"
#include "renderingContext.h"

namespace FalconEye
{

class RaycastRenderingApp : public App
{
public:
    RaycastRenderingApp( const int width, const int height, Scene_ptr inScene = nullptr, const int major= 3, const int minor= 3 )
    : App(width, height, major, minor)
    , scene(inScene)
    {}

    virtual ~RaycastRenderingApp() {}

    virtual int init() override;
    virtual int quit() override;

    virtual int update(const float time, const float delta) override;
    virtual int render() override;

    void LuaRun();
    void DebugRender() { sceneRenderer->renderScene(renderOption); }

    Scene_ptr getScene() const { return scene; }
    SceneRenderer_ptr getSceneRenderer() const { return sceneRenderer; }

    SceneRenderOption_ptr getRenderOption() const { return renderOption; }
    void setRenderOption(const SceneRenderOption_ptr& InRenderOption) { renderOption = InRenderOption; }

    LUA_BEGIN_BIND_METHODS(RaycastRenderingApp)
            LUA_BIND_CONSTRUCTOR(const int, const int, _opt<Scene_ptr>, _opt<const int>, _opt<const int>)
            LUA_BIND_METHOD(RaycastRenderingApp, getScene)
            LUA_BIND_METHOD(RaycastRenderingApp, getSceneRenderer)
            LUA_BIND_PROPERTY(RaycastRenderingApp, renderOption, getRenderOption, setRenderOption)
            LUA_BIND_METHOD(RaycastRenderingApp, LuaRun)
            LUA_BIND_METHOD(RaycastRenderingApp, DebugRender)
            LUA_END_BIND_METHODS

protected:
    Scene_ptr scene;
    SceneRenderer_ptr sceneRenderer;
    RenderingContext_ptr renderingContext;
    SceneRenderOption_ptr renderOption;
}; // end class RaycastRenderingApp

} // end namespace FalconEye