#pragma once

#include "glcore.h"

namespace FalconEye
{

class RenderPass
{
public:
    RenderPass();
    virtual ~RenderPass();

    RenderPass(const RenderPass&) = delete;
    RenderPass(RenderPass&&) = default;

    virtual bool Init(const char* mainShaderFile);
    virtual void BeginPass();
    virtual void EndPass();

    GLuint GetProgram() const { return mainProgram; }
protected:
    GLuint GetUniformLocation(const char* uniformName);

    GLuint mainProgram;
private:

};

} // en namespace FalconEye