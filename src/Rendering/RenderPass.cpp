#include "Rendering/RenderPass.h"
#include "gKit/program.h"
#include <iostream>

namespace FalconEye
{
RenderPass::RenderPass()
: mainProgram(0)
{}

RenderPass::~RenderPass()
{
    release_program(mainProgram);
}

bool RenderPass::Init(const char* mainShaderFile)
{
    mainProgram = read_program(mainShaderFile);
    // verifie les erreurs
    GLint status;
    glGetProgramiv(mainProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        return false;
    }

    return true;
}

void RenderPass::BeginPass()
{
    glUseProgram(mainProgram);
}

void RenderPass::EndPass()
{
    //glUseProgram(0);
}

GLuint RenderPass::GetUniformLocation(const char* uniformName)
{
    GLuint location = glGetUniformLocation(mainProgram, uniformName);

    if (location == GL_INVALID_VALUE || location == GL_INVALID_OPERATION)
    {
        std::cerr << "Warning! Unable to get the location of uniform" << uniformName << "\n";
    }

    return location;
}

} // end namespace FalconEye