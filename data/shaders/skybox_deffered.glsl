#version 440

#ifdef VERTEX_SHADER

layout (location = 0) in vec3 Position;
out vec3 TexCoords;

uniform mat4 gView;
uniform mat4 gProj;

void main()
{
    vec4 pos = gProj * gView * vec4((Position / 1000.0f) * 10, 1.0);
    TexCoords = Position / 1000.0f;
    gl_Position = pos.xyww;
}

#endif

#ifdef FRAGMENT_SHADER

in vec3 TexCoords;

layout (location = 0) out vec3 WorldPosOut;
layout (location = 1) out vec3 DiffuseOut;
layout (location = 2) out vec3 NormalOut;
layout (location = 3) out vec3 TexCoordOut;

uniform samplerCube cubemap; // cubemap texture sampler

void main()
{

	WorldPosOut = vec3(0.0, 0.0, 0.0);
    TexCoordOut = TexCoords;
    DiffuseOut = texture(cubemap, TexCoords).xyz;
    NormalOut = vec3(0.0, 0.0, 0.0);
}

#endif