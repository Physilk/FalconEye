#version 440

#ifdef VERTEX_SHADER

layout(location = 0) out vec2 texCoord;
void main()
{
    vec2 position = vec2(gl_VertexID % 2, gl_VertexID / 2) * 4.0 - 1;
    texCoord = (position + 1) * 0.5;
    gl_Position = vec4(position, 0, 1);
}

#endif

#ifdef FRAGMENT_SHADER

uniform sampler2D gWorldPos0;
uniform sampler2D gDiffuse0;
uniform sampler2D gNormal0;

out vec4 FragColor;

in vec2 texCoord;


vec2 CalcTexCoord()
{
    //return gl_FragCoord.xy / gScreenSize;
    return gl_FragCoord.xy / vec2(1920, 1080);
}

vec4 CalcDirectionalLight(//BaseLight Light,
					   vec3 LightDirection,
					   vec3 WorldPos,
					   vec3 Normal)
{
    vec4 AmbientColor = vec4(0.1, 0.1, 0.1, 1.0);
    float DiffuseFactor = dot(Normal, -LightDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0.0) {
        DiffuseColor = vec4(vec3(0.8, 0.8, 0.8) * DiffuseFactor, 1.0);
    }

    return (AmbientColor + DiffuseColor);
}

void main()
{
    vec2 TexCoord = CalcTexCoord();
    vec3 WorldPos = texture(gWorldPos0, TexCoord).xyz;
    vec3 Color = texture(gDiffuse0, TexCoord).xyz;
    vec3 Normal = texture(gNormal0, TexCoord).xyz;
    Normal = normalize(Normal);

    FragColor = vec4(Color, 1.0)  * CalcDirectionalLight(normalize(vec3(-1, -1,  - 1)),  WorldPos, Normal);
}

#endif