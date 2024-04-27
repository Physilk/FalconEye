#pragma once
#include <memory>

#include "SceneComponent.h"

namespace FalconEye
{

class SceneActor
{

private:
	// Root component, other scene components will be children of this root
	SceneComponent_ptr RootComponent;

	// Non-scene components
	std::vector<ActorComponent_ptr> Components;
};

using SceneActor_ptr = std::shared_ptr<SceneActor>;
} // End Nam