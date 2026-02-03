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
} debug;

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    //suzanneBP = std::make_unique<ew::Model>("assets/models/suzanne.obj")

    toon = std::make_unique<ew::Shader>("assets/shaders/WindWaker.vs", "assets/shaders/WindWaker.fs");
    //blinnphong = std::make_unique<ew::Shader>("assets/shaders/BlinnPhong.vs", "assets/shaders/BlinnPhong.fs");
   
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

    //Allocate frame buffer
    glCreateFramebuffers(1, &framebuffer);

    //Create image while frame buffer is bound
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    {
        glGenTextures(1, &fboTexture);
        glBindTexture(GL_TEXTURE_2D, fboTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);

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
    ImGui::ColorEdit3("Light Color", &lightColor[0]);
    ImGui::SeparatorText("Color Palette");
    ImGui::ColorEdit3("Color 1", &palette.color1[0]);
    ImGui::ColorEdit3("Color 2", &palette.color2[0]);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    /* build debug ui here */

    ImGui::End();
}