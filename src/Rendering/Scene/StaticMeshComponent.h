#pragma once
#include <memory>

#include "SceneComponent.h"
#include "mat.h"
#include "Utils\resourceManager.h"


namespace FalconEye
{

	class StaticMeshComponent : SceneComponent
	{

	private:
		Mesh_ptr StaticMesh;
	};

	using StaticMeshComponent_ptr = std::shared_ptr<StaticMeshComponent>;
} // End Namespace FalconEye