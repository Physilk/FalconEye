#include "gBuffer.h"
#include <cstring> // memset
#include <cstdio>

namespace FalconEye
{

GBuffer::GBuffer()
{
    Fbo = 0;
    DepthTexture = 0;
    StencilTextureStorage = 0;
    StencilTextureView = 0;
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

    //I am not sure as to how to delete texture storage and texture view
    if (StencilTextureStorage != 0) {
        glDeleteTextures(1, &StencilTextureStorage);
    }
    
    if (StencilTextureView != 0) 
    {
        glDeleteTextures(1, &StencilTextureView);
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
    glGenTextures(1, &StencilTextureStorage);
    glGenTextures(1, &StencilTextureView);

    for (unsigned int i = 0; i < (sizeof(Textures) / sizeof(Textures[0])); i++) {
        glBindTexture(GL_TEXTURE_2D, Textures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, Textures[i], 0);
    }

    // depth
    glBindTexture(GL_TEXTURE_2D, DepthTexture);
    //glTexStorage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WindowWidth, WindowHeight);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WindowWidth, WindowHeight, 0,  GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);

    // stencil
    //glBindTexture(GL_TEXTURE_2D, StencilTextureStorage);
    
    //glBindTexture(GL_TEXTURE_2D, StencilTextureView);
    glTextureView(StencilTextureView, GL_TEXTURE_2D, DepthTexture, GL_DEPTH24_STENCIL8, 0, 1, 0, 1);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX8, WindowWidth, WindowHeight, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);


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
	//glBindFramebuffer(GL_READ_FRAMEBUFFER, Fbo);
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    unsigned int i = 0;
	for (i = 0; i < (sizeof(Textures) / sizeof(Textures[0])); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, Textures[static_cast<uint8_t>(GBufferTextureType_RGB32F::EType::Position) + i]);
	}

    glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, DepthTexture);
    glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
	++i;

    glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, StencilTextureView);
    glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_INDEX);
	++i;
}

void GBuffer::SetReadBuffer(GBufferTextureType_RGB32F TextureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + static_cast<uint8_t>(TextureType));
}

void GBuffer::SetReadBuffer_Depth()
{
    glReadBuffer(GL_DEPTH_ATTACHMENT);
}

void GBuffer::SetReadBuffer_Stencil()
{
    glReadBuffer(GL_STENCIL_ATTACHMENT);
}


} // end namespace FalconEye