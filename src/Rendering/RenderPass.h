#pragma once

#include "glcore.h"
#include "ShaderManager.h"

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

    GLuint GetProgram() const { return Program->GetProgram(); }
protected:
    GLuint GetUniformLocation(const char* uniformName);

    GLuint mainProgram;
    ShaderProgram_ptr Program;
private:

};

} // en namespace FalconEye