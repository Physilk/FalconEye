#include "gBuffer.h"
#include <cstring> // memset
#include <cstdio>

namespace FalconEye
{

GBuffer::GBuffer()
{
    Fbo = 0;
    DepthTexture = 0;
    std::memset(Textures, 0, sizeof(Textures));
}

GBuffer::~GBuffer()
{
    if (Fbo != 0) {
        glDeleteFramebuffers(1, &Fbo);
    }

    if (Textures[0] != 0) {
        glDeleteTextures((sizeof(Textures) / sizeof(Textures[0])), Textures);
    }

    if (DepthTexture != 0) {
        glDeleteTextures(1, &DepthTexture);
    }
}

bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &Fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, Fbo);

    // Create the gbuffer textures
    glGenTextures((sizeof(Textures) / sizeof(Textures[0])), Textures);
    glGenTextures(1, &DepthTexture);

    for (unsigned int i = 0; i < (sizeof(Textures) / sizeof(Textures[0])); i++) {
        glBindTexture(GL_TEXTURE_2D, Textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, Textures[i], 0);
    }

    // depth
    glBindTexture(GL_TEXTURE_2D, DepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3 };

    glDrawBuffers((sizeof(DrawBuffers) / sizeof(DrawBuffers[0])), DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        std::printf("FB error, status: 0x%x\n", Status);
        return false;
    }

    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void GBuffer::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Fbo);
}

void GBuffer::BindForReading()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, Fbo);
}

void GBuffer::SetReadBuffer(GBufferTextureType TextureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}

} // end namespace FalconEye