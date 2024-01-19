#pragma once
#include <stdint.h>
#include "glcore.h"

namespace FalconEye
{
	class EGLType
	{
	public:
		enum class EType : uint32_t
		{
			Byte = GL_BYTE,
			Byte_Unsigned = GL_UNSIGNED_BYTE,
			Short = GL_SHORT,
			Short_Unsigned = GL_UNSIGNED_SHORT,
			Int = GL_INT,
			Int_Unsigned = GL_UNSIGNED_INT,
			HalfFloat = GL_HALF_FLOAT,
			Float = GL_FLOAT
			//TODO: add the other weird types
			/*
			GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV,
			GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV,
			GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV
			*/
		} Type;

		EGLType(EGLType::EType t) : Type(t) {}

		//operator GLint() const { return static_cast<GLint>(Type); }
		operator GLenum() const { return static_cast<GLenum>(Type); }
	};

	class ETextureTarget
	{
	public:
		enum class EType : uint32_t
		{
			Texture_2D = GL_TEXTURE_2D,
			//Texture_1D_Array = GL_TEXTURE_1D_ARRAY, //not supported yet
			//Texture_3D = GL_TEXTURE_3D, // not supported yet
			Rectangle = GL_TEXTURE_RECTANGLE,
			Cubemap = GL_TEXTURE_CUBE_MAP,
			Cubemap_Positive_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			Cubemap_Negative_X = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			Cubemap_Positive_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			Cubemap_Negative_Y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			Cubemap_Positive_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			Cubemap_Negative_Z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		} Type;
		ETextureTarget(ETextureTarget::EType t) : Type(t) {}
		ETextureTarget(GLenum e) : Type(static_cast<EType>(e)) {}

		//operator GLint() const { return static_cast<GLint>(Type); }
		operator GLenum() const { return static_cast<GLenum>(Type); }
		operator ETextureTarget::EType() const { return Type; }
		//operator uint8_t() const { return static_cast<uint32_t>(Type); }

		static ETextureTarget CubemapAtIndex(int i)
		{
			static ETextureTarget arr[] = {
			EType::Cubemap_Positive_X, EType::Cubemap_Negative_X,
			EType::Cubemap_Positive_Y, EType::Cubemap_Negative_Y,
			EType::Cubemap_Positive_Z , EType::Cubemap_Negative_Z
			};

			if (i >= 0 && i < 6)
			{
				return arr[i];
			}

			return EType::Cubemap_Positive_X;
		}
	};

	class ETextureFormat
	{
	public:
		enum class EType : uint32_t
		{
			Red = GL_RED,
			RG = GL_RG,
			RGB = GL_RGB,
			BGR = GL_BGR,
			RGBA = GL_RGBA,
			BGRA = GL_BGRA,
			Red_Int = GL_RED_INTEGER,
			RG_Int = GL_RG_INTEGER,
			RGB_Int = GL_RGB_INTEGER,
			BGR_Int = GL_BGR_INTEGER,
			RGBA_Int = GL_RGBA_INTEGER,
			BGRA_Int = GL_BGRA_INTEGER,
			Stencil_Index = GL_STENCIL_INDEX,
			DepthComponent = GL_DEPTH_COMPONENT,
			DepthStencil = GL_DEPTH_STENCIL
		} Type;
		ETextureFormat(ETextureFormat::EType t) : Type(t) {}

		//operator GLint() const { return static_cast<GLint>(Type); }
		operator GLenum() const { return static_cast<GLenum>(Type); }
	};

	class ETextureInternalFormat
	{
	public:
		enum class EType : int32_t
		{
			// Base internal formats
			DepthComponent = GL_DEPTH_COMPONENT,
			DepthStencilComponent = GL_DEPTH_STENCIL,
			Red = GL_RED,
			RG = GL_RG,
			RGB = GL_RGB,
			RGBA = GL_RGBA,

			// Sized Internal Formats
			R8 = GL_R8,
			RG8 = GL_RG8,
			RGB8 = GL_RGB8,
			RGBA8 = GL_RGBA8,
			R16 = GL_R16,
			RG16 = GL_RG16,
			RGB16 = GL_RGB16,
			RGBA16 = GL_RGBA16,
			R8i = GL_R8I,
			RG8i = GL_RG8I,
			RGB8i = GL_RGB8I,
			RGBA8i = GL_RGBA8I,
			R16i = GL_R16I,
			RG16i = GL_RG16I,
			RGB16i = GL_RGB16I,
			RGBA16i = GL_RGBA16I,
			R32i = GL_R32I,
			RG32i = GL_RG32I,
			RGB32i = GL_RGB32I,
			RGBA32i = GL_RGBA32I,
			R8ui = GL_R8UI,
			RG8ui = GL_RG8UI,
			RGB8ui = GL_RGB8UI,
			RGBA8ui = GL_RGBA8UI,
			R16ui = GL_R16UI,
			RG16ui = GL_RG16UI,
			RGB16ui = GL_RGB16UI,
			RGBA16ui = GL_RGBA16UI,
			R32ui = GL_R32UI,
			RG32ui = GL_RG32UI,
			RGB32ui = GL_RGB32UI,
			RGBA32ui = GL_RGBA32UI,
			R16f = GL_R16F,
			RG16f = GL_RG16F,
			RGB16f = GL_RGB16F,
			RGBA16f = GL_RGBA16F,
			R32f = GL_RGBA32F,
			RG32f = GL_RG32F,
			RGB32f = GL_RGB32F,
			RGBA32f = GL_RGBA32F,
			// TODO: add signed normalized and other weird formats

			// Compressed Internal Formats
			Red_Compressed = GL_COMPRESSED_RED,
			RG_Compressed = GL_COMPRESSED_RG,
			RGB_Compressed = GL_COMPRESSED_RGB,
			RGBA_Compressed = GL_COMPRESSED_RGBA,
		} Type;
		ETextureInternalFormat(ETextureInternalFormat::EType t) : Type(t) {}

		operator GLint() const { return static_cast<GLint>(Type); }
		//operator GLenum() const { return static_cast<GLenum>(Type); }
	};
}