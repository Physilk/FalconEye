#pragma once
#include <memory>

#include "ActorComponent.h"
#include "mat.h"


namespace FalconEye
{

class SceneComponent : ActorComponent
{
	
private:
	Transform RelativeTrnasform;

	std::vector<SceneComponent_ptr> ChildComponents;
};

using SceneComponent_ptr = std::shared_ptr<SceneComponent>;

} // End Namespace FalconEye