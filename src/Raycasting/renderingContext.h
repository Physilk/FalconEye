#pragma once

namespace FalconEye {

class Scene;
class SceneRenderer;

class RenderingContext
{
private:
// I don't think I'll need those
	RenderingContext() = delete;
	RenderingContext(const RenderingContext&) = delete;
	RenderingContext(RenderingContext&&) = delete;
	
public:
	RenderingContext(const FalconEye::Scene* InSceneRef, const SceneRenderer* InRendererRef)
	: SceneRef(InSceneRef)
	, RendererRef(InRendererRef)
	{}
	
public:
	const Scene* SceneRef;
	const SceneRenderer* RendererRef;
};

using RenderingContext_ptr = std::shared_ptr<RenderingContext>;
} // end namespace FalconEye
