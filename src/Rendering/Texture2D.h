#pragma once
#include <stdint.h>
#include "glcore.h"
#include "Rendering/GLTypes.h"
#include "Utils/resourceManager.h"

namespace FalconEye
{

	class Texture2D
	{
	public:
		

	public:
		Texture2D();
		virtual ~Texture2D();

		void Init2D(Image_ptr image, ETextureInternalFormat internalFormat = ETextureInternalFormat::EType::RGBA);
		void InitRectangle(Image_ptr image, ETextureInternalFormat internalFormat = ETextureInternalFormat::EType::RGBA);
		void InitCubemap(std::vector<Image_ptr> images, ETextureInternalFormat internalFormat = ETextureInternalFormat::EType::RGBA_Compressed);

		operator GLint() const { return static_cast<GLint>(GLTexture); }

		void UseTexture(int unit);
	private:
		void SetData_Internal(int32_t width, int32_t height, const void* data, ETextureInternalFormat internalFormat, ETextureFormat format = ETextureFormat::EType::RGBA, ETextureTarget target = ETextureTarget::EType::Texture_2D, EGLType Type = EGLType::EType::Float, bool bGenerateMipmap = false);
	protected:
		ETextureTarget GLTarget;
		GLuint GLTexture;
	};

	using Texture2D_ptr = std::shared_ptr<Texture2D>;
} // end namespace FalconEye