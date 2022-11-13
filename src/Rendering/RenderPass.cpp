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
    std::vector< ShaderPermutationParameter> parameters;
    parameters.reserve(3);
    std::vector<Shader_ptr> shaders;
    parameters.push_back(ShaderPermutationParameter::MakePermutationParameterForShader(mainShaderFile, EShaderType::VertexShader, "USE_TEXCOORD"));
    parameters.push_back(ShaderPermutationParameter::MakePermutationParameterForShader(mainShaderFile, EShaderType::VertexShader, "USE_NORMAL"));
    //parameters.push_back(ShaderPermutationParameter::MakePermutationParameterForShader(mainShaderFile, EShaderType::VertexShader, "USE_COLOR"));
    shaders.push_back(std::make_shared<Shader>(mainShaderFile, EShaderType::VertexShader, parameters));

    parameters.clear();
	parameters.push_back(ShaderPermutationParameter::MakePermutationParameterForShader(mainShaderFile, EShaderType::PixelShader, "USE_TEXCOORD"));
	parameters.push_back(ShaderPermutationParameter::MakePermutationParameterForShader(mainShaderFile, EShaderType::PixelShader, "USE_NORMAL"));
	//parameters.push_back(ShaderPermutationParameter::MakePermutationParameterForShader(mainShaderFile, EShaderType::PixelShader, "USE_COLOR"));
    shaders.push_back(std::make_shared<Shader>(mainShaderFile, EShaderType::PixelShader, parameters));

    Program = ShaderProgram_ptr(new ShaderProgram(shaders, true));
    //auto test = new ShaderProgram(shaders, true);
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
    glUseProgram(*Program);
}

void RenderPass::EndPass()
{
    //glUseProgram(0);
}

GLuint RenderPass::GetUniformLocation(const char* uniformName)
{
    GLuint location = glGetUniformLocation(*Program, uniformName);

    if (location == GL_INVALID_VALUE || location == GL_INVALID_OPERATION)
    {
        std::cerr << "Warning! Unable to get the location of uniform" << uniformName << "\n";
    }

    return location;
}

} // end namespace FalconEye