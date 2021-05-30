#include <stdio.h>

#include <GL/includes.h>
#include <GLFW/glfw3native.h>
#include <glm/ext.hpp>

// #include <RGBNetworking/JSONSerializer.h>
#include <RGBLogger/DebugLogger.h>
#include <RGBLogger/StdoutLogger.h>
#include <Engine/Core/Window.h>
#include <Engine/Core/Context.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/Geometry/Frustum.h>
#include <Engine/Graphics/Program.h>

using namespace CubeWorld;
using namespace CubeWorld::Engine;

void GetDotProducts(float outDots[], const glm::vec4 planes[], const glm::vec4 pos)
{
    LOG_ALWAYS("Dot products for ({}, {}, {}):", pos.x, pos.y, pos.z);
    for (size_t i = 0; i < 6; i++)
    {
        outDots[i] = glm::dot(planes[i], pos);
        LOG_ALWAYS("  - {}", outDots[i]);
    }
}

int main(int, char**)
{
    Logger::StdoutLogger::Instance();
    Logger::DebugLogger::Instance();

    glm::mat4 perspective = glm::perspective(0.3f, 1.0f, 0.1f, 10.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(1, 0, 0), glm::vec3(0), glm::vec3(0, 1, 0));

    Engine::Graphics::Frustum f = Engine::Graphics::Frustum::Create(perspective * view);

    float dots[6];
    GetDotProducts(dots, f.planes, glm::vec4{ 0, 0, 0, 1 });      // Should be dead center
    GetDotProducts(dots, f.planes, glm::vec4{ 1, 0, 0, 1 });      // At camera (too close)
    GetDotProducts(dots, f.planes, glm::vec4{ -1, 0, 0, 1 });     // A little further
    GetDotProducts(dots, f.planes, glm::vec4{ -10, 0, 0, 1 });    // Too far
    GetDotProducts(dots, f.planes, glm::vec4{ 0, 10, 0, 1 });     // Too high?
    GetDotProducts(dots, f.planes, glm::vec4{ 0, 100, 0, 1 });    // Way too high?
    GetDotProducts(dots, f.planes, glm::vec4{ 0, 0, 10, 1 });     // Too side?
    GetDotProducts(dots, f.planes, glm::vec4{ 0, 0, 100, 1 });    // Way too side?

    if (dots[0] != 50294) {
        return 0;
    }

    /*

    // Setup main window
    Window::Options windowOptions;
    windowOptions.title = "NCW Editor";
    windowOptions.fullscreen = false;
    windowOptions.width = 1280;
    windowOptions.height = 760;
    windowOptions.lockCursor = false;
    Window& window = Window::Instance();
    if (auto result = window.Initialize(windowOptions); !result)
    {
        return 1;
    }

    auto _ = window.AddCallback(GLFW_KEY_ESCAPE, [&](int, int, int) {
        window.SetShouldClose(true);
    });
    */

    if (!glfwInit())
    {
        LOG_ERROR("Bye baby");
        return 1;
    }

    int width = 1280;
    int height = 720;

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(width, height, "Example", nullptr, nullptr);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    GLFWwindow* partner = glfwCreateWindow(width, height, "Example", nullptr, window);

    if (!window || !partner) {
        glfwTerminate();
        LOG_ERROR("Failed to create glfw windows");
        return 1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
    {
        LOG_ERROR("Failed to initialize glad!");
        glfwDestroyWindow(window);
        glfwDestroyWindow(partner);
        glfwTerminate();
        return 1;
    }

    GLuint vbo1, vbo2;
    glGenBuffers(1, &vbo1);
    glGenBuffers(1, &vbo2);

    std::mutex mutex;
    bool threadShouldRun = true;

    bool isFBOdirty = true; //true when last frame was displayed, false
                            //when just updated and not yet displayed
    bool isFBOready = false; //set by worker thread when initialized
    //bool isFBOsetupOnce = false; //set by main thread when initialized

    GLuint fb[2] = {std::numeric_limits<GLuint>::max(), std::numeric_limits<GLuint>::max()}; //framebuffers
    GLuint rb[2] = {std::numeric_limits<GLuint>::max(), std::numeric_limits<GLuint>::max()}; //renderbuffers, color and depth

    GLuint buf[2] = {std::numeric_limits<GLuint>::max(), std::numeric_limits<GLuint>::max()};

    std::thread t([&]
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{1000});
        /*

        //Engine::Context::Instance().Activate();
        std::unique_lock<std::mutex> lock{mutex};
        vbo.BufferData(points);
        glFlush();
        ready = true;
        */

        glfwMakeContextCurrent(partner);
        //create new shared framebuffer object
        {
            std::unique_lock<std::mutex> lock{mutex};
            glGenFramebuffers(1, &fb[1]);
            glBindFramebuffer(GL_FRAMEBUFFER, fb[1]);
            glGenRenderbuffers(2, rb);
            glBindRenderbuffer(GL_RENDERBUFFER, rb[0]);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 2, GL_RGBA8, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, rb[1]);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 2, GL_DEPTH24_STENCIL8, width, height);

            glBindRenderbuffer(GL_RENDERBUFFER, rb[0]);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rb[0]);
            glBindRenderbuffer(GL_RENDERBUFFER, rb[1]);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb[1]);


            std::vector<glm::vec3> points{
                {-1, 0, 0},
                {0, 0, 0},
                {0, 1, 0},
            };

            glGenBuffers(1, &buf[1]);
            glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
            glBufferData(GL_ARRAY_BUFFER, GLsizei(sizeof(glm::vec3) * points.size()), &points[0], GL_STATIC_DRAW);

            glFlush();
            isFBOready = true;
        }

        for (;;) {
            std::unique_lock<std::mutex> lock{mutex};
            if (!threadShouldRun) {
                break;
            }
            if (isFBOdirty) {
                glBindFramebuffer(GL_FRAMEBUFFER, fb[1]);
                float r = (float)rand() / (float)RAND_MAX;
                glClearColor(r, r, r, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glFlush();
                isFBOdirty = false;
            }
        }
        printf("Exiting thread..\n");
        return;
    }
    );

    std::vector<glm::vec3> points{
        {0, 0, 0},
        {1, 0, 0},
        {1, 1, 0},
    };

    glGenFramebuffers(1, &fb[0]);

    Engine::Graphics::VBO vbo;

    glGenBuffers(1, &buf[0]);
    glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
    glBufferData(GL_ARRAY_BUFFER, GLsizei(sizeof(glm::vec3) * points.size()), &points[0], GL_STATIC_DRAW);

    vbo.BufferData(points);

    auto maybeProgram = Engine::Graphics::Program::Load("Shaders/Stupid.vert", "Shaders/Stupid.frag");
    assert(maybeProgram.Succeeded());

    auto program = std::move(*maybeProgram);

    //glfwMakeContextCurrent(window.get());

    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds{50});

        std::unique_lock<std::mutex> lock{mutex};

        /*
        if (!isFBOsetupOnce)
        {
            if (isFBOready) { //is other thread finished setting up FBO?
                if (glIsRenderbuffer(rb[0]) && glIsRenderbuffer(rb[1])) {
                    glBindFramebuffer(GL_FRAMEBUFFER, fb[0]);
                    glBindRenderbuffer(GL_RENDERBUFFER, rb[0]);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rb[0]);
                    glBindRenderbuffer(GL_RENDERBUFFER, rb[1]);
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb[1]);
                    isFBOsetupOnce = true;
                }
            }
        }
        else
        {
            bool isFB = (bool)glIsFramebuffer(fb[0]);
            bool isCA = (bool)glIsRenderbuffer(rb[0]);
            bool isDSA = (bool)glIsRenderbuffer(rb[1]);
            bool isComplete = false;
            if (isFB) {
                glBindFramebuffer(GL_FRAMEBUFFER, fb[0]);
                isComplete = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
            }

            if (isFB && isCA && isDSA && isComplete) {
                if (!isFBOdirty) {
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, fb[0]);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                    glBlitFramebuffer(0, 0, width, height,
                        0, 0, width, height,
                        GL_COLOR_BUFFER_BIT,
                        GL_NEAREST);
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                    glfwSwapBuffers(window);
                    isFBOdirty = true;
                    //printf("Framebuffer OK\n");			
                }
                else {
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                }
            }
            else {
                LOG_ERROR("Framebuffer not ready!");
                GLenum e = glGetError();
                LOG_ERROR("OpenGL error: %1", e);
            }
        }
        */

        {
            BIND_PROGRAM_IN_SCOPE(program);

            glm::mat4 model(1);
            program->UniformMatrix4f("uProjMatrix", model);
            program->UniformMatrix4f("uViewMatrix", model);
            program->UniformMatrix4f("uModelMatrix", model);

            glEnableVertexAttribArray(program->Attrib("aPosition"));
            glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
            glVertexAttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

            glEnableVertexAttribArray(program->Attrib("aColor"));
            glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
            glVertexAttribPointer(program->Attrib("aColor"), 3, GL_FLOAT, GL_FALSE, 0, 0);

            glDrawArrays(GL_TRIANGLES, 0, 3);

            if (isFBOready)
            {
                glEnableVertexAttribArray(program->Attrib("aPosition"));
                glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
                glVertexAttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);

                glEnableVertexAttribArray(program->Attrib("aColor"));
                glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
                glVertexAttribPointer(program->Attrib("aColor"), 3, GL_FLOAT, GL_FALSE, 0, 0);

                glDrawArrays(GL_TRIANGLES, 0, 3);
            }

            glFlush();
        }

        /*
        if (ready)
        {
            BIND_PROGRAM_IN_SCOPE(program);

            vbo.AttribPointer(program->Attrib("aPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0);
            vbo.AttribPointer(program->Attrib("aColor"), 3, GL_FLOAT, GL_FALSE, 0, 0);

            glm::mat4 model(1);
            program->UniformMatrix4f("uProjMatrix", model);
            program->UniformMatrix4f("uViewMatrix", model);
            program->UniformMatrix4f("uModelMatrix", model);

            glDrawArrays(GL_TRIANGLES, 0, 3);
            glFlush();
        }
        */

        // Swap buffers
        {
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    } // Check if the ESC key was pressed or the window was closed
    while (!glfwWindowShouldClose(window));
    threadShouldRun = false;

    t.join();

    glfwDestroyWindow(window);
    glfwDestroyWindow(partner);
    glfwTerminate();
    return 0;
}
