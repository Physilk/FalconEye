#version 440

#ifdef VERTEX_SHADER


layout (location = 0) in vec3 aPos;

uniform mat4 gWVP;
uniform mat4 gWorld;
uniform mat4 gView;
uniform mat4 gProj;

out vec3 TexCoords;

void main()
{
    vec4 pos = gProj * gView * vec4((aPos / 1000.0f) * 10, 1.0);
    TexCoords = aPos / 1000.0f;
    gl_Position = pos.xyww;
    //gl_Position = pos;
}

#endif

#ifdef FRAGMENT_SHADER

out vec4 FragColor;

in vec3 TexCoords; // direction vector representing a 3D texture coordinate
uniform samplerCube cubemap; // cubemap texture sampler

void main()
{
    FragColor = texture(cubemap, TexCoords);
}

#endif