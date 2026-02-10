#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

glm::mat4 lightMatrix = glm::mat4(1.0f);
glm::vec3 lightColor = glm::vec3(1.0f);

const glm::vec4 backgroundColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

struct{
    float alpha = 128.0f;
    bool isNormalMapOn = true;
    float strength = 16.0f;
} debug;

struct FullScreenQuad   
{
    //Vertex attribute object
    GLuint vao;
    //Vertex buffer object
    GLuint vbo;

    int stride = 4;
    int verticesNumber = 6;

    bool Initialize()
    {
        float vertices[] = {
            //Pos (x, y), texcoord (u, v)
            //Triangle 1
            -1, 1, 0, 1,
            -1, -1, 0, 0,
            1, -1, 1, 0,

            //Triangle 2
            -1, 1, 0, 1,
            1, -1, 1, 0,
            1, 1, 1, 1,
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        //Bind VAO 
        glBindVertexArray(vao);

        //VBO is bound to VAO state because VAO is bound first
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //If we unbind the vao, vbo will be unbound, vice versa
        //Dynamic draw allows us to change buffer data later on
        //Stream draw means we will change it very frequently, once per frame
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        //Vector 2 position (x,y)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        //Vector 2 tex coords (u,v)
        //Start at end of position (void*)(sizeof(float)*2)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(sizeof(float)*2));

        //Unbind VAO
        glBindVertexArray(0);
        return true;
    }
} fullscreenQuad;

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    //suzanneBP = std::make_unique<ew::Model>("assets/models/suzanne.obj")

    toon = std::make_unique<ew::Shader>("assets/shaders/WindWaker.vs", "assets/shaders/WindWaker.fs");
    postShader = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/blur.fs");
   
    mainTexture = std::make_unique<ew::Texture>("assets/textures/Bricks.jpg");
    normalTexture = std::make_unique<ew::Texture>("assets/textures/Bricks_Normal.jpg");
    gradientTexture = std::make_unique<ew::Texture>("assets/textures/ZAtoon.png");

    light = {
        .brightness = 1.0f,
        .color = {1.0f, 1.0f, 1.0f},
        .position = {2.0f, 2.0f, 2.0f},
    };

    lightColor = light.color;

    palette = {
        .color1 = {1.0f, 0.0f, 1.0f},
        .color2 = {0.0f, 0.0f, 1.0f}
    };

    fullscreenQuad.Initialize();

    //Allocate frame buffer
    glCreateFramebuffers(1, &framebuffer);

    //Create image while frame buffer is bound
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    {
        glGenTextures(1, &fboTexture);
        glBindTexture(GL_TEXTURE_2D, fboTexture);

        //Create 800/600 render texture with 8 unsigned bytes
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

        //Create depth texture
        glGenTextures(1, &fboDepth);
        glBindTexture(GL_TEXTURE_2D, fboDepth);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);  

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fboDepth, 0);

        //Cleanup textures
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Warning! Frame buffer is not complete!\n");
    }

    //Unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene::~Scene()
{
    glDeleteFramebuffers(1, &framebuffer);
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

auto objectMatrix = glm::mat4(1.0f);

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();


    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //Re-enable depth test
    glEnable(GL_DEPTH_TEST);
  
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    {
        glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Set main texture
        glBindTextureUnit(0, mainTexture->getID());
        //Set normal texture
        glBindTextureUnit(1, normalTexture->getID());
        //Set gradient toon texture
        glBindTextureUnit(2, gradientTexture->getID());


        toon->use();

        // scene matrices

        toon->setInt("main_texture", 0);
        toon->setInt("normal_map", 1);
        toon->setInt("gradient_texture", 2);

        toon->setMat4("model", objectMatrix);
        toon->setMat4("view_proj", view_proj);
        toon->setVec3("camera", camera.position);

        toon->setVec3("light.position", light.position);
        toon->setVec3("light.color", light.color);
        toon->setFloat("material.shininess", debug.alpha);
        toon->setInt("normalMapOn", debug.isNormalMapOn);

        toon->setVec3("material.diffuse", glm::vec3(1));
        toon->setVec3("material.specular", glm::vec3(1));
        toon->setVec3("material.ambient", backgroundColor * 0.1f);

        toon->setVec3("palette.color1", palette.color1);
        toon->setVec3("palette.color2", palette.color2);

        // draw suzanne
        suzanne->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Render fullscreen quad
    postShader->use();
    postShader->setInt("screen", 0);
    postShader->setFloat("strength", debug.strength);

    //Disable depth test
    glDisable(GL_DEPTH_TEST);

    //Clear default framebuffer
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Draw fullscreen quad
    glBindVertexArray(fullscreenQuad.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboTexture);

    //Draw triangles from first array all the way to 6
    glDrawArrays(GL_TRIANGLES, 0, fullscreenQuad.verticesNumber);
}

void Scene::Debug(void)
{
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    glm::mat4 m{1.0f};
    auto *view = glm::value_ptr(camera.View());
    auto *proj = glm::value_ptr(camera.Projection());
    
    //ImGuizmo::DrawGrid(view, proj, glm::value_ptr(m), 100.0f);

    light.color = lightColor;

    if (ImGuizmo::IsUsing()){
        light.position = glm::vec3(lightMatrix[3]);
    }

    ImGuizmo::Manipulate(
        view,
        proj,
        ImGuizmo::TRANSLATE,
        ImGuizmo::WORLD,
        glm::value_ptr(lightMatrix) // &matrix[0][0]
    );

    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::SliderFloat("Alpha", &debug.alpha, 0, 128);
    ImGui::Checkbox("Normal Mapping On", &debug.isNormalMapOn);
    ImGui::SliderFloat("Blur Strength", &debug.strength, 0, 300);
    ImGui::ColorEdit3("Light Color", &lightColor[0]);
    ImGui::SeparatorText("Color Palette");
    ImGui::ColorEdit3("Color 1", &palette.color1[0]);
    ImGui::ColorEdit3("Color 2", &palette.color2[0]);

    ImGui::Image((void*)(intptr_t)fboTexture, ImVec2(400,300), ImVec2(0,1), ImVec2(1,0));
       ImGui::Image((void*)(intptr_t)fboDepth, ImVec2(400,300), ImVec2(0,1), ImVec2(1,0));

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    /* build debug ui here */

    ImGui::End();
}