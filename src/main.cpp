// #include <SDL2/SDL.h>
// #include <GL/glew.h>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <iostream>
//
// const char *vertexShaderSource = R"(
// #version 420 core
// layout (location = 0) in vec2 aPos;
//
// uniform mat4 uProjection;
//
// void main() {
//     gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
// }
// )";
//
// const char *fragmentShaderSource = R"(
// #version 420 core
// out vec4 FragColor;
//
// void main() {
//     FragColor = vec4(1.0, 0.3, 0.2, 1.0); // Red-ish
// }
// )";
//
// // Compile shader and check errors
// GLuint compileShader(GLenum type, const char *source)
// {
//     GLuint shader = glCreateShader(type);
//     glShaderSource(shader, 1, &source, NULL);
//     glCompileShader(shader);
//
//     GLint status;
//     glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
//     if (!status)
//     {
//         char log[512];
//         glGetShaderInfoLog(shader, 512, NULL, log);
//         std::cerr << "Shader compile error: " << log << std::endl;
//     }
//     return shader;
// }
//
// // Link shaders into a program
// GLuint createShaderProgram()
// {
//     GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
//     GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
//
//     GLuint program = glCreateProgram();
//     glAttachShader(program, vs);
//     glAttachShader(program, fs);
//     glLinkProgram(program);
//
//     glDeleteShader(vs);
//     glDeleteShader(fs);
//
//     return program;
// }
//
// int main(int argc, char *argv[])
// {
//     SDL_Init(SDL_INIT_VIDEO);
//
//     SDL_Window *window = SDL_CreateWindow("2D OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
//     SDL_GLContext context = SDL_GL_CreateContext(window);
//
//     glewExperimental = GL_TRUE;
//     glewInit();
//
//     glViewport(0, 0, 800, 600);
//     glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//
//     GLuint shaderProgram = createShaderProgram();
//
//     float vertices[] = {
//         // x, y
//         100.0f, 100.0f,
//         200.0f, 100.0f,
//         200.0f, 200.0f,
//         100.0f, 200.0f};
//
//     unsigned int indices[] = {
//         0, 1, 2,
//         2, 3, 0};
//
//     GLuint VAO, VBO, EBO;
//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);
//     glGenBuffers(1, &EBO);
//
//     glBindVertexArray(VAO);
//
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//
//     glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
//     glEnableVertexAttribArray(0);
//
//     glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f);
//
//     bool running = true;
//     SDL_Event event;
//
//     while (running)
//     {
//         while (SDL_PollEvent(&event))
//         {
//             if (event.type == SDL_QUIT)
//                 running = false;
//         }
//
//         glClear(GL_COLOR_BUFFER_BIT);
//
//         glUseProgram(shaderProgram);
//         glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, &projection[0][0]);
//         glBindVertexArray(VAO);
//         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//         SDL_GL_SwapWindow(window);
//     }
//
//     glDeleteProgram(shaderProgram);
//     glDeleteVertexArrays(1, &VAO);
//     glDeleteBuffers(1, &VBO);
//     glDeleteBuffers(1, &EBO);
//
//     SDL_GL_DeleteContext(context);
//     SDL_DestroyWindow(window);
//     SDL_Quit();
//
//     return 0;
// }

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/objects/Objects.hpp"
#include "engine/HelperFunctions.hpp"
#include "engine/scripting/ScriptingHeaders.hpp"
#include <string>
#include <memory>
#include <iostream>

std::vector<float> vertices = {
    //  x,     y,    u,   v
    -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
    0.5f, -0.5f, 1.0f, 0.0f,  // bottom-right
    0.5f, 0.5f, 1.0f, 1.0f,   // top-right
    -0.5f, 0.5f, 0.0f, 1.0f   // top-left
};

std::vector<unsigned int> indices = {
    0, 1, 2,
    2, 3, 0};

int main(int argc, char *argv[])
{
    glm::vec2 res(900, 500);
    HelperFunctions *renderingEngine = new HelperFunctionsOpenGl(res, "Game", SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, 8, false, 0, true);

    Shader *apiShader = new ShaderOpenGl();
    Image *apiImage = new ImageOpenGl();

    // this is the constants
    const float MOUSE_SENSITIVITY = 0.1;
    const Bigint WALK_SPEED = Bigint(10);
    Bigint run_speed = Bigint("100");

    // this is the camera, cameras are neat
    Camera *camera = new Camera(res, BigVec3(0.0f, 0.0f, -2.0f));
    camera->rotation.y = 180.0f;

    // this sets up the shader and texture
    Shader *shader = apiShader->makeNewShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader *pointShader = apiShader->makeNewShader("shaders/pointVert.glsl", "shaders/pointFrag.glsl");
    Image *image = apiImage->makeNewImage("assets/textures/FISH.png");

    Font defaultFont("assets/unifont-16.0.04.otf", 48);
    Mesh *mesh2d = new OpenGlMesh(vertices, indices, {2, 2});
    Shader *shader2d = new ShaderOpenGl("shaders/2dVertex.glsl", "shaders/2dFragment.glsl");

    RenderObject2d object2d(mesh2d, shader2d, camera);

    RenderObject::init(pointShader, new OpenGlMesh(), camera);

    delete apiImage;
    delete apiShader;

    LuaHeaders::LuaScriptLib lua;
    lua.include(LuaHeaders::LuaLibEnum::glm);
    lua.include(LuaHeaders::LuaLibEnum::bigvars);
    lua.include(LuaHeaders::LuaLibEnum::objects);

    lua.includeInitialized("shader1", *shader);
    lua.includeInitialized("image1", *image);
    lua.includeInitialized("camera1", *camera);

    lua.includeScripts("./scripts");

    // lua.getFunction("test")();

    // starts running the game loop
    bool running = true;
    Uint64 lastTicks = SDL_GetTicks64();
    Uint64 currentTicks;
    SDL_Event event;
    float deltaTime;
    const Bigint *speed;

    glm::mat4 cameraMatrixTmp;

    int i = 0;

    int accumulatedMouseX = 0;
    int accumulatedMouseY = 0;
    int frames = 0.0f;
    Uint64 start = SDL_GetTicks64();

    while (running)
    {
        currentTicks = SDL_GetTicks64();
        deltaTime = (currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        // gets events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;

            // rotates camera
            if (event.type == SDL_MOUSEMOTION)
            {
                accumulatedMouseX += event.motion.xrel;
                accumulatedMouseY += event.motion.yrel;
            }

            // when you press escape, leave
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_z)
                {
                    run_speed *= Bigint(10);
                }
                if (event.key.keysym.sym == SDLK_x)
                {
                    run_speed /= Bigint(10);
                }
            }
        }

        // gets all keystates
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);

        // if your running, run, otherwise dont
        speed = keystates[SDL_SCANCODE_LSHIFT] ? &run_speed : &WALK_SPEED;

        // movement
        if (keystates[SDL_SCANCODE_W])
        {
            camera->position -= BigVec3(camera->getForwardVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_S])
        {
            camera->position += BigVec3(camera->getForwardVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_D])
        {
            camera->position -= BigVec3(camera->getRightVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_A])
        {
            camera->position += BigVec3(camera->getRightVector() * deltaTime) * *speed;
        }

        if (keystates[SDL_SCANCODE_SPACE])
        {
            camera->position += BigVec3(camera->getDownVector() * deltaTime) * *speed;
        }
        if (keystates[SDL_SCANCODE_LCTRL])
        {
            camera->position -= BigVec3(camera->getDownVector() * deltaTime) * *speed;
        }

        if (deltaTime > 0)
        {

            camera->rotation.y -= (accumulatedMouseX / deltaTime) * MOUSE_SENSITIVITY * deltaTime;
            camera->rotation.x -= (accumulatedMouseY / deltaTime) * MOUSE_SENSITIVITY * deltaTime;

            accumulatedMouseX = 0;
            accumulatedMouseY = 0;
        }

        RenderObject::updateTime();

        RenderObject::UpdateAllObjects(deltaTime);

        renderingEngine->clearBackground();

        RenderObject::DrawAllObjects();

        frames++;

        if (SDL_GetTicks64() - start >= 1000)
        {
            std::cout << frames << "\n";
            frames = 0;
            start = SDL_GetTicks64();
        }

        // Image *test = new ImageOpenGl(defaultFont.renderText("test", {255, 255, 255, 255}));

        object2d.Draw(image);

        renderingEngine->swapBuffer();
        // delete test;
    }
    // delete everything
    delete shader;
    delete pointShader;
    delete image;
    delete renderingEngine;
    delete camera;
    return 0;
}
