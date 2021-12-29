#include "RaycastRenderingApp.h"
#include <algorithm>

namespace FalconEye
{
    int RaycastRenderingApp::init()
    {
        

        if (scene == nullptr)
        {
            std::cout << "nullptr scene creating default one" << std::endl; 
            scene = std::make_shared<Scene>();
        }
        
        sceneRenderer = std::make_shared<SceneRenderer>();
        renderingContext = std::make_shared<RenderingContext>(scene.get(), sceneRenderer.get());
        sceneRenderer->SetContext(renderingContext);

        image = Image(renderOption->getWidth(), renderOption->getHeight());
        startTime = std::chrono::high_resolution_clock::now();

        
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(), 0,
             GL_RGBA, GL_UNSIGNED_BYTE, image.buffer());

        glGenFramebuffers(1, &readFboId);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D, texture, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        return sceneRenderer->renderScene(&image, renderingJobs, renderOption);
    }

    int RaycastRenderingApp::quit()
    {
        return 0;
    }

    int RaycastRenderingApp::update(const float time, const float delta)
    {
        renderingJobs.erase(
            std::remove_if(
                renderingJobs.begin(), 
                renderingJobs.end(),
                [](auto job) { return job->GetStatus() & Threading::EJobStatus::Finished; }
            ), 
            renderingJobs.end()
        ); 

        if (renderingJobs.empty() && !bRenderFinished)
        {
            bRenderFinished = true;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
            std::cout << "render time: " << ms << "ms" << std::endl;
            
        }
        return 0;
    }

    int RaycastRenderingApp::render()
    {
        glBindTexture(GL_TEXTURE_2D, texture);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(),
                GL_RGBA, GL_FLOAT, image.buffer());
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
        glBlitFramebuffer(0, 0, image.width(), image.height(),
                        0, 0, image.width(), image.height(),
                        GL_COLOR_BUFFER_BIT, GL_LINEAR);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        return 1;
    }

    void RaycastRenderingApp::LuaRun()
    {
        run();
    }

    void RaycastRenderingApp::setRenderOption(const SceneRenderOption_ptr& InRenderOption)
    { 
        renderOption = InRenderOption;
        if (renderOption != nullptr)
        {
            resize_window(renderOption->getWidth(), renderOption->getHeight());
        }
    }
} // en namespace FalconEye