#version 440

#ifdef VERTEX_SHADER

layout (location = 0) in vec3 Position;
out vec3 WorldPos0;

#ifdef USE_TEXCOORD
layout (location = 1) in vec2 TexCoord; 
out vec2 TexCoord0;
#endif

#ifdef USE_NORMAL
layout(location= 2) in vec3 Normal;
out vec3 Normal0;
#endif

#ifdef USE_COLOR
layout(location= 3) in vec4 Color;
out vec4 VertexColor0;
#endif

uniform mat4 gWVP;
uniform mat4 gWorld;

void main()
{       
    gl_Position    = gWVP * vec4(Position, 1.0);
    WorldPos0      = (gWorld * vec4(Position, 1.0)).xyz;
#ifdef USE_TEXCOORD
    TexCoord0      = TexCoord;
#endif
#ifdef USE_NORMAL
    Normal0        = (gWorld * vec4(Normal, 0.0)).xyz;
#endif
#ifdef USE_COLOR
    VertexColor0   = Color;
#endif
}

#endif

#ifdef FRAGMENT_SHADER

in vec3 WorldPos0;

#ifdef USE_TEXCOORD
in vec2 TexCoord0;
#endif

#ifdef USE_NORMAL
in vec3 Normal0;
#endif

#ifdef USE_COLOR
in vec4 VertexColor0;
#endif

layout (location = 0) out vec3 WorldPosOut;
layout (location = 1) out vec3 DiffuseOut;
layout (location = 2) out vec3 NormalOut;
layout (location = 3) out vec3 TexCoordOut;

#ifdef USE_TEXCOORD
uniform sampler2D gColorMap;                
#endif

void main()
{

	WorldPosOut = WorldPos0;
    TexCoordOut = vec3(0.0, 0.0, 0.0);
    vec4 color = vec4(1.0, 1.0, 1.0, 0.0);
#ifdef USE_COLOR
    color = VertexColor0;
#endif
    DiffuseOut = color.xyz;
#ifdef USE_TEXCOORD
	DiffuseOut = texture(gColorMap, TexCoord0).xyz * color.xyz;	
	TexCoordOut = vec3(TexCoord0, 0.0);
#else
    TexCoordOut = vec3(0.0, 0.0, 0.0);
#endif

#ifdef USE_NORMAL
    NormalOut       = normalize(Normal0);
#else
    vec3 t = normalize(dFdx(WorldPos0));
    vec3 b = normalize(dFdy(WorldPos0));
    NormalOut = normalize(cross(t, b));
#endif
}

#endif