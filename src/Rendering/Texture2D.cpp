#include "Rendering/Texture2D.h"

namespace FalconEye
{

	Texture2D::Texture2D()
		: GLTarget(ETextureTarget::EType::Texture_2D)
		, GLTexture(0)
	{
		glGenTextures(1, &GLTexture);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &GLTexture);
	}

	void Texture2D::Init2D(Image_ptr image, ETextureInternalFormat internalFormat /*= TextureInternalFormat::EType::RGBA*/)
	{
		GLTarget = ETextureTarget::EType::Texture_2D;
		SetData_Internal(image->width(), image->height(), image->buffer(), internalFormat, ETextureFormat::EType::RGBA, GLTarget);
	}

	void Texture2D::InitRectangle(Image_ptr image, ETextureInternalFormat internalFormat /*= ETextureInternalFormat::EType::RGBA*/)
	{
		GLTarget = ETextureTarget::EType::Rectangle;
		SetData_Internal(image->width(), image->height(), image->buffer(), internalFormat, ETextureFormat::EType::RGBA, GLTarget);
	}

	void Texture2D::InitCubemap(std::vector<Image_ptr> images, ETextureInternalFormat internalFormat /*= ETextureInternalFormat::EType::RGBA_Compressed*/)
	{
		GLTarget = ETextureTarget::EType::Cubemap;
		glBindTexture(GLTarget, GLTexture);
		if (images.size() == 6)
		{
			for (int i = 0; i < images.size(); ++i)
			{
				//SetData_Internal(images[i]->width(), images[i]->height(), images[i]->buffer(), internalFormat, ETextureFormat::EType::RGBA, ETextureTarget::CubemapAtIndex(i));
				//SetData_Internal(images[i]->width(), images[i]->height(), images[i]->buffer(), internalFormat, ETextureFormat::EType::RGB, static_cast<GLenum>(static_cast<uint8_t>(ETextureTarget::EType::Cubemap_Positive_X) + i));
				glTexImage2D(
					ETextureTarget::CubemapAtIndex(i)
					, 0
					, internalFormat
					, images[i]->width()
					, images[i]->height()
					, 0
					, static_cast<GLenum>(ETextureFormat::EType::RGBA)
					, static_cast<GLenum>(EGLType::EType::Float)
					, images[i]->buffer());
			}
		}
		else
		{
			//TODO: error
			std::cout << "Wrong number of texture to initialize cubemap\n";
		}

		// fixe les parametres de filtrage par defaut
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void Texture2D::UseTexture(int unit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GLTarget, GLTexture);
	}

	void Texture2D::SetData_Internal(int32_t width, int32_t height, const void* data, ETextureInternalFormat internalFormat, ETextureFormat format /*= TextureFormat::EType::RGBA*/, ETextureTarget target /*= TextureTarget::EType::Texture_2D*/, EGLType Type/* = GLType::EType::Float*/, bool bGenerateMipmap/* = false*/)
	{
		glBindTexture(GLTarget, GLTexture);
		GLint clamp = GL_CLAMP_TO_BORDER;
		if (GLTarget == ETextureTarget::EType::Cubemap)
		{
			clamp = GL_CLAMP_TO_EDGE;
		}
		// fixe les parametres de filtrage par defaut
		glTexParameteri(GLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GLTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GLTarget, GL_TEXTURE_WRAP_S, clamp);
		glTexParameteri(GLTarget, GL_TEXTURE_WRAP_T, clamp);
		
		glTexImage2D(target, 0, internalFormat, width, height, 0, format, Type, data);


		
		if (bGenerateMipmap)
		{
			glGenerateMipmap(GLTarget);
		}
	}

} // end namespace FalconEye

