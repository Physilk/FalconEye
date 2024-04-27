#pragma once

#include <stdint.h>

#include "glcore.h"

namespace FalconEye
{

class GBufferTextureType_RGB32F
{
public:
    enum class EType : uint8_t {
        Position,
        Diffuse,
        Normal,
        TexCoord,
        Num
    } Type;

    GBufferTextureType_RGB32F(GBufferTextureType_RGB32F::EType t) : Type(t){}
    operator uint8_t() const { return static_cast<uint8_t>(Type); }
};

class GBufferTextureType_R8
{
public:
    enum class EType : uint8_t {
        CustomStencil,
        Num
    } Type;

    GBufferTextureType_R8(GBufferTextureType_R8::EType t) : Type(t){}
    operator uint8_t() const { return static_cast<uint8_t>(Type); }

    
};

class GBuffer
{
public:

    GBuffer();

    ~GBuffer();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading();

    void SetReadBuffer(GBufferTextureType_RGB32F TextureType);
    void SetReadBuffer_Depth();
    void SetReadBuffer_Stencil();

    GLuint GetFBO() const { return Fbo; }
    GLuint* GetTextures() { return Textures; }
    GLuint GetDepthTexture() const { return DepthTexture; }
    GLuint GetStencilTexture() const { return StencilTextureView; }
private:

    GLuint Fbo;
    GLuint Textures[static_cast<uint8_t>(GBufferTextureType_RGB32F::EType::Num)];
    GLuint DepthTexture;
    GLuint StencilTextureStorage;
    GLuint StencilTextureView;
};

} // end namespace FalconEye