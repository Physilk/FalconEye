#include "Rendering/DefferedShadingApp.h"
#include "gKit/program.h"
#include "texture.h"
#include "uniforms.h"

namespace FalconEye
{
    
DefferedShadingApp::DefferedShadingApp( const int width, const int height, const int major, const int minor, bool bResizable)
    : App(width, height, major, minor, bResizable)
    , gBuffer(nullptr)
    , gBufferPass(nullptr)
    , Exemple_Mesh(nullptr)
    , Exemple_buffer(0)
    , Exemple_Image(nullptr)
    , Exemple_Texture(0)
    , Camera()
    , FOV(60.0f)
    , WindowWidth(width)
    , WindowHeight(height)
{
    Camera.lookat(Point(0.0f ,0.0f, 0.0f), 5.0f);
}

DefferedShadingApp::~DefferedShadingApp( )
{
    delete gBufferPass;
    delete gBuffer;
}

int DefferedShadingApp::init( )
{
	glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

    gBuffer = new GBuffer();
    if (gBuffer != nullptr)
    {
        if (!gBuffer->Init(WindowWidth, WindowHeight))
        {
            return -1;
        }
    }

    gBufferPass = new RenderPass();
    if (gBufferPass != nullptr)
    {
        if (!gBufferPass->Init("data/shaders/defered_shading_gbuffer.glsl"))
        {
            return -1;
        }
    }

    Exemple_Mesh = ResourceManager::Instance().requestMesh("data/capsule/capsule.obj");

    if (Exemple_Mesh != nullptr)
    {
        Exemple_buffer = Exemple_Mesh->create_buffers(true, true, false);
        if (!Exemple_buffer)
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }

    Exemple_Image = ResourceManager::Instance().requestImage("data/capsule/capsule0.jpg");
    if (Exemple_Image != nullptr)
    {
        Exemple_Texture = make_texture(0, *Exemple_Image);
        if (!Exemple_Texture)
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }

    Camera.rotation(90.0f, 0.0f);
    return 0;
}
int DefferedShadingApp::quit( )
{
    delete gBufferPass;
    gBufferPass  = nullptr;

    delete gBuffer;
    gBuffer = nullptr;

    return 0;
}

int DefferedShadingApp::update( const float time, const float delta )
{
    Camera.rotation(delta * 15.0f / 1000.0f, 0.0f);
    return 0;
}

int DefferedShadingApp::render()
{
	gBufferPass->BeginPass();
    gBuffer->BindForWriting();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint program = gBufferPass->GetProgram();

	glBindVertexArray(Exemple_buffer);

    Transform ViewTransform = Camera.view();

	Transform mv = ViewTransform;
	Transform mvp = Transform::Perspective(FOV, WindowWidth / WindowHeight, 0.00001f, 1000000.0f) * ViewTransform;

	program_uniform(program, "gWVP", mvp);
	program_uniform(program, "gWorld", mv);
    program_use_texture(program, "gColorMap", 0, Exemple_Texture);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)Exemple_Mesh->positions().size());
    gBufferPass->EndPass();

    // AFTER GBUFFER RENDERING
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gBuffer->BindForReading();

	GLsizei HalfWidth = (GLsizei)(WindowWidth / 2.0f);
	GLsizei HalfHeight = (GLsizei)(WindowHeight / 2.0f);

    gBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
	glBlitFramebuffer(0, 0, WindowWidth, WindowHeight,
		0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    gBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE);
	glBlitFramebuffer(0, 0, WindowWidth, WindowHeight,
		0, HalfHeight, HalfWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    gBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
	glBlitFramebuffer(0, 0, WindowWidth, WindowHeight,
		HalfWidth, HalfHeight, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    gBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD);
	glBlitFramebuffer(0, 0, WindowWidth, WindowHeight,
		HalfWidth, 0, WindowWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    return 1;
}

void DefferedShadingApp::LuaRun()
{
    run();
}

} // end namespace FalconEye