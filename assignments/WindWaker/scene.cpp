#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

#include "ew/procGen.h"

glm::mat4 lightMatrix = glm::mat4(1.0f);
glm::vec3 lightColor = glm::vec3(1.0f);

const glm::vec4 backgroundColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

struct{
    float alpha = 128.0f;
    bool isNormalMapOn = true;
    glm::vec2 scrollSpeed;
    glm::vec3 waterColor;
} debug;

Scene::Scene()
{
    waterShader = std::make_unique<ew::Shader>("assets/shaders/doubleDash.vs", "assets/shaders/doubleDash.fs");

    waveTex = std::make_unique<ew::Texture>("assets/textures/doubledash/wave_tex.png");
    waveSpec = std::make_unique<ew::Texture>("assets/textures/doubledash/wave_spec.png");
    waveWarp = std::make_unique<ew::Texture>("assets/textures/doubledash/wave_warp.png");
    water16 = std::make_unique<ew::Texture>("assets/textures/windwaker/water16.png");
    water8 = std::make_unique<ew::Texture>("assets/textures/windwaker/water8.png");

    plane.load(ew::createPlane(100, 100, 10));
}

Scene::~Scene()
{
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

    //Set main texture
    glBindTextureUnit(0, waveTex->getID());
    //Set normal texture
    glBindTextureUnit(1, waveSpec->getID());
    glBindTextureUnit(2, waveWarp->getID());

    //Set shader uniforms
    waterShader->use();

    waterShader->setInt("tex", 0);
    waterShader->setInt("spec", 1);
    waterShader->setInt("warp", 2);
    waterShader->setVec3("camera_position", camera.position);
    waterShader->setVec3("_waterColor", debug.waterColor);
    waterShader->setFloat("_deltaTime", (float)time.absolute);
    waterShader->setVec2("_floatSpeed", debug.scrollSpeed);

    waterShader->setMat4("model", objectMatrix);
    waterShader->setMat4("view_proj", view_proj);
    waterShader->setVec3("camera", camera.position);

    waterShader->setFloat("material.shininess", debug.alpha);
    waterShader->setInt("normalMapOn", debug.isNormalMapOn);

    waterShader->setVec3("material.diffuse", glm::vec3(1));
    waterShader->setVec3("material.specular", glm::vec3(1));
    waterShader->setVec3("material.ambient", backgroundColor * 0.1f);

    //Draw plane
    plane.draw();
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

    ImGuizmo::Manipulate(
        view,
        proj,
        ImGuizmo::TRANSLATE,
        ImGuizmo::WORLD,
        glm::value_ptr(lightMatrix) // &matrix[0][0]
    );

    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::DragFloat2("Scroll Speed", &debug.scrollSpeed[0]);
    ImGui::ColorEdit3("Water Color", &debug.waterColor[0]);
    ImGui::Checkbox("Normal Mapping On", &debug.isNormalMapOn);
    ImGui::ColorEdit3("Light Color", &lightColor.x);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    /* build debug ui here */

    ImGui::End();
}