#include "Rendering/DefferedShadingApp.h"
#include "gKit/program.h"
#include "texture.h"
#include "uniforms.h"
#include "gKit/texture.h"
#include "Rendering/Texture2D.h"

namespace FalconEye
{
    
DefferedShadingApp::DefferedShadingApp( const int width, const int height, const int major, const int minor, bool bResizable)
    : App(width, height, major, minor, bResizable)
    , gBuffer(nullptr)
    , gBufferPass(nullptr)
    , LightingPass(nullptr)
    , Exemple_Mesh(nullptr)
    , Exemple_buffer(0)
    , Exemple_Image(nullptr)
    , Exemple_Texture(0)
    , Camera()
    , FOV(60.0f)
    , WindowWidth(width)
    , WindowHeight(height)
{
    //SDL_Init (SDL_INIT_VIDEO);
    //SDL_SetVideoMode (width, height, 0, 0);
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

    LightingPass = new RenderPass();
    if (LightingPass != nullptr)
    {
        if (!LightingPass->Init("data/shaders/defered_shading.glsl"))
        {
            return -1;
        }
    }
    else
    {
        return -1;
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

    SetupCubemap();

    Camera.rotation(90.0f, 0.0f);

    glCreateVertexArrays(1, &DummyVAO);

    std::vector< ShaderPermutationParameter> parameters;
    std::vector<Shader_ptr> shaders;

    parameters.push_back(ShaderPermutationParameter::MakePermutationParameterForShader("data/shaders/defered_shading.glsl", "SHADING_MODEL=1"));

    shaders.push_back(std::make_shared<Shader>("data/shaders/defered_shading.glsl", EShaderType::VertexShader, parameters));
    shaders.push_back(std::make_shared<Shader>("data/shaders/defered_shading.glsl", EShaderType::PixelShader, parameters));

    UnlitLightingShader = std::make_shared<ShaderProgram>(shaders, true);

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
    static bool bWasUpdatedOnce = false;
    //SetupTCPJobs();
    if (key_state(SDLK_RIGHT))
    {
        Camera.rotation(-delta * 15.0f / 100.0f, 0.0f);
    }
    if (key_state(SDLK_LEFT))
    {
        Camera.rotation(delta * 15.0f / 100.0f, 0.0f);
    }
    bGBufferDisplay = key_state(SDLK_SPACE);
    bDepthDisplay = key_state(SDLK_d);
    bStencilDisplay = key_state(SDLK_s);

    if (bWasUpdatedOnce) // rotation broken on first call of "update"
    {
    }
    //WaitTCPJobs();
    bWasUpdatedOnce = true;
    return 0;
}

int DefferedShadingApp::render()
{
    Transform ViewTransform = Camera.view();
    Transform ProjectionTransform = Camera.projection((float)WindowWidth, (float)WindowHeight, FOV);

	Transform mv = ViewTransform;
	Transform mvp = Transform::Perspective(FOV, (float)WindowWidth / (float)WindowHeight, 0.00001f, 1000.0f) * ViewTransform;

    GLuint program = 0;
	gBufferPass->BeginPass();
    gBuffer->BindForWriting();

    glDepthMask(GL_TRUE);

    glEnable(GL_STENCIL_TEST); 
    glStencilFunc(GL_ALWAYS, 0xff, 0xff);  
    //glDepthFunc(GL_LEQUAL);
    //glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);
    
    program = CubemapRenderPass_deffered->GetProgram();
    //std::cout << "deffered cubemap programID=" << program << '\n';
    glUseProgram(program);
	glBindVertexArray(CubemapBuffer);

    program_uniform(program, "gView", ViewTransform);
    program_uniform(program, "gProj", ProjectionTransform);
    program_use_texture(program, "cubemap", 0, *Cubemaptexture);

    glStencilMask(0x88); // stencil 2 -> shading model unlit
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)CubemapMesh->positions().size());

    program = gBufferPass->GetProgram();
	glBindVertexArray(Exemple_buffer);

    glUseProgram(program);
	program_uniform(program, "gWVP", mvp);
	program_uniform(program, "gWorld", mv);
    program_use_texture(program, "gColorMap", 0, Exemple_Texture);

    glStencilMask(0xff); // stencil 1 -> shading model lit
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)Exemple_Mesh->positions().size());
    gBufferPass->EndPass();
    
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

    // AFTER GBUFFER RENDERING
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gBuffer->BindForReading();

    CubemapRenderPass->BeginPass();
    program = CubemapRenderPass->GetProgram();
    
    LightingPass->BeginPass();
    program = LightingPass->GetProgram();
    //program_use_texture(program, "gWorldPos0", 0, gBuffer->GetTextures()[0]);
    program_use_texture(program, "gDiffuse0", 1, gBuffer->GetTextures()[1]);
    program_use_texture(program, "gNormal0", 2, gBuffer->GetTextures()[2]);
    //program_use_texture(program, "gDepth", 3, gBuffer->GetDepthTexture());
    //program_use_texture(program, "gStencil", 4, gBuffer->GetStencilTexture());

    glBindVertexArray(DummyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    program = UnlitLightingShader->GetProgram();
    program_use_texture(program, "gDiffuse0", 1, gBuffer->GetTextures()[1]);
    program_use_texture(program, "gNormal0", 2, gBuffer->GetTextures()[2]);
    //program_use_texture(program, "gDepth", 3, gBuffer->GetDepthTexture());
    //program_use_texture(program, "gStencil", 4, gBuffer->GetStencilTexture());

    glBindVertexArray(DummyVAO);
    //glDrawArrays(GL_TRIANGLES, 0, 3);
    gBufferPass->EndPass();

    if (bGBufferDisplay)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetFBO());
        //glReadBuffer(3); //why was this here?? What does it do? xD
        glBlitFramebuffer(0, 0, WindowWidth, WindowHeight,
            0, 0, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        GLsizei HalfWidth = (GLsizei)(WindowWidth / 2.0f);
        GLsizei HalfHeight = (GLsizei)(WindowHeight / 2.0f);

        gBuffer->SetReadBuffer(GBufferTextureType_RGB32F::EType::Position);
        glBlitFramebuffer(0, 0, WindowWidth, WindowHeight,
            0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        gBuffer->SetReadBuffer(GBufferTextureType_RGB32F::EType::Diffuse);
        glBlitFramebuffer(0, 0, WindowWidth, WindowHeight,
            0, HalfHeight, HalfWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        gBuffer->SetReadBuffer(GBufferTextureType_RGB32F::EType::Normal);
        glBlitFramebuffer(0, 0, WindowWidth, WindowHeight,
            HalfWidth, HalfHeight, WindowWidth, WindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        gBuffer->SetReadBuffer(GBufferTextureType_RGB32F::EType::TexCoord);
        glBlitFramebuffer(0, 0, WindowWidth, WindowHeight,
            HalfWidth, 0, WindowWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }
    if (bDepthDisplay || bStencilDisplay)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->GetFBO());
        if (bDepthDisplay)
        {
            gBuffer->SetReadBuffer_Depth();
        }
        else
        {
            gBuffer->SetReadBuffer_Stencil();
        }

        glBlitFramebuffer(0, 0, WindowWidth, WindowHeight,
            0, 0, WindowWidth, WindowHeight, bDepthDisplay ? GL_DEPTH_BUFFER_BIT : GL_STENCIL_BUFFER_BIT, GL_LINEAR);
    }
    return 1;
}

void DefferedShadingApp::LuaRun()
{
    run();
}

void DefferedShadingApp::SetupCubemap()
{
    for (auto ImageName : {"posx", "negx", "posy", "negy", "posz", "negz"})
    {
        static const std::string FilenameBase = "data/cubemap/cubemap_opensea/opensea_";
        std::string CurrentFilename = FilenameBase + ImageName;
        CurrentFilename.append(".png");
        CubemapImages.push_back(ResourceManager::Instance().requestImage(CurrentFilename));
    }
    Cubemaptexture = std::make_shared<Texture2D>();
    Cubemaptexture->InitCubemap(CubemapImages);



    static const float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    CubemapMesh = std::make_shared<Mesh>();
    for (int i = 0; i < sizeof(skyboxVertices) / sizeof(float); i += 3)
    {
        CubemapMesh->vertex(vec3(skyboxVertices[i]*1000.0f, skyboxVertices[i+1]*1000.0f, skyboxVertices[i+2]*1000.0f));
    }
    CubemapBuffer = CubemapMesh->create_buffers(false, false, false);

    CubemapRenderPass = std::make_shared<RenderPass>();
    if (CubemapRenderPass != nullptr)
    {
        //std::cout << "setting up skybox shader pass";
        CubemapRenderPass->Init("data/shaders/skybox.glsl");
    }

    CubemapRenderPass_deffered = std::make_shared<RenderPass>();
    if (CubemapRenderPass_deffered != nullptr)
    {
        //std::cout << "setting up skybox shader pass";
        CubemapRenderPass_deffered->Init("data/shaders/skybox_deffered.glsl");
    }
}
} // end namespace FalconEye