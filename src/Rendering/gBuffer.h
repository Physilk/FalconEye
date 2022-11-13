#pragma once

#include "glcore.h"

#define GBUFFER_POSITION_TEXTURE_UNIT 0
#define GBUFFER_DIFFUSE_TEXTURE_UNIT  1
#define GBUFFER_NORMAL_TEXTURE_UNIT   2
#define GBUFFER_TEXCOORD_TEXTURE_UNIT 3

namespace FalconEye
{

class GBuffer
{
public:

    enum GBufferTextureType {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_TEXTURE_TYPE_TEXCOORD,
        GBUFFER_NUM_TEXTURES
    };

    GBuffer();

    ~GBuffer();

    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading();

    void SetReadBuffer(GBufferTextureType TextureType);

    GLuint GetFBO() const { return Fbo; }
    GLuint* GetTextures() { return Textures; }
private:

    GLuint Fbo;
    GLuint Textures[GBUFFER_NUM_TEXTURES];
    GLuint DepthTexture;
};

} // end namespace FalconEye