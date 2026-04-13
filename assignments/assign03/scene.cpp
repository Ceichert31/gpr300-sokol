#include "scene.h"

// batteries
#include "batteries/math.h"
#include "batteries/opengl.h"

// ew
#include "ew/procGen.h"

// imgui
#include "imgui/imgui.h"

constexpr float orbit_radius = 2.0f;
constexpr glm::vec4 light_orbit_radius = {2.0f, 4.0f, -4.0f, 1.0f};

#pragma region Structs
struct FullscreenQuad
{
    GLuint vao;
    GLuint vbo;

    void Initialize()
    {
        // clang-format off
        float quad_vertices[] = {
            // pos (x, y) texcoord (u, v)
            -1.0f,  1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f,  1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f,  1.0f, 1.0f, 1.0f,
        };
        // clang-format on

        // initialize fullscreen quad, buffer object
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        // bind vao, and vbo
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // buffer data to vbo
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

        // positions and texcoords
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

        glBindVertexArray(0);
    }
} fullscreen_quad;

struct Framebuffer
{
    //Textures
    GLuint fbo;
    GLuint position;
    GLuint normal;
    GLuint albedo;
    GLuint material;
    GLuint depth;
    const int kFrameBufferWidth = 800;
    const int kFrameBufferHeight = 600;

    void Initialize()
    {
        // initialize framebuffer
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        #pragma region Position Attachment
        {
            glGenTextures(1, &position);
            glBindTexture(GL_TEXTURE_2D, position);

            //Create 800/600 render texture with 8 unsigned bytes
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, kFrameBufferWidth, kFrameBufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position, 0);

        }
        #pragma endregion

        #pragma region Normal Attachment
        {
            glGenTextures(1, &normal);
            glBindTexture(GL_TEXTURE_2D, normal);

            //Create 800/600 render texture with 8 unsigned bytes
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, kFrameBufferWidth, kFrameBufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);

        }
        #pragma endregion

        #pragma region Albedo Attachment
        {
            glGenTextures(1, &albedo);
            glBindTexture(GL_TEXTURE_2D, albedo);

            //Create 800/600 render texture with 8 unsigned bytes
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, kFrameBufferWidth, kFrameBufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedo, 0);

        }
        #pragma endregion
   
        #pragma region Material Attachment
        {
            glGenTextures(1, &material);
            glBindTexture(GL_TEXTURE_2D, material);

            //Create 800/600 render texture with 8 unsigned bytes
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, kFrameBufferWidth, kFrameBufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, material, 0);

        }
        #pragma endregion

        #pragma region Depth Attachment
        {
            glGenTextures(1, &depth);
            glBindTexture(GL_TEXTURE_2D, depth);

            //Create 800/600 render texture with 8 unsigned bytes
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, kFrameBufferWidth, kFrameBufferHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

        }
        #pragma endregion

        //Draw buffers besides depth since its default
        GLenum bufferArray[] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3};
        glDrawBuffers(4, bufferArray);

        // check completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Not so victorious\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} framebuffer;

struct LighVolumebuffer
{
    GLuint fbo;
    GLuint color;
    GLuint depth;
     const int kVolumeBufferWidth = 800;
    const int kVolumeBufferHeight = 600;

    void Initialize()
    {
        // initialize framebuffer
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        #pragma region Color Attachment
        {
            glGenTextures(1, &color);
            glBindTexture(GL_TEXTURE_2D, color);

            //Create 800/600 render texture with 8 unsigned bytes
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, kVolumeBufferWidth, kVolumeBufferHeight, 0, GL_RGBA, GL_FLOAT, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

        }
        #pragma endregion
      
        #pragma region Depth Attachment
        {
            glGenTextures(1, &depth);
            glBindTexture(GL_TEXTURE_2D, depth);

            //Create 800/600 render texture with 8 unsigned bytes
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, kVolumeBufferWidth, kVolumeBufferHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

        }
        #pragma endregion

        // check completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Light Volume buffer failed to initialize!\n");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} lightvolumebuffer;

struct Material
{
    float ambient = 1.0f;
    float diffuse = 0.5f;
    float specular = 0.5f;
    float shininess = 0.5f;
} material;

struct
{
    int width = 1;
    float light_radius = 2.5f;
    bool draw_light_volume = false;
} debug;
#pragma endregion

#pragma region Setup/Cleanup
Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    geometry = std::make_unique<ew::Shader>("assets/shaders/deferred/geometry.vs", "assets/shaders/deferred/geometry.fs");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/deferred/blinnphong.vs", "assets/shaders/deferred/blinnphong.fs");
    noprocess = std::make_unique<ew::Shader>("assets/shaders/deferred/fullscreen.vs", "assets/shaders/deferred/fullscreen.fs");
    lightsphere = std::make_unique<ew::Shader>("assets/shaders/deferred/light.vs", "assets/shaders/deferred/light.fs");
    
    brickTexture = std::make_unique<ew::Texture>("assets/textures/Bricks.jpg");

    sphere.load(ew::createSphere(1.0f, 8));
    plane.load(ew::createPlane(100, 100, 1));

    ambient = {
        .intensity = 1.0f,
        .color = {0.5f, 0.5f, 0.5f},
    };

    framebuffer.Initialize();
    lightvolumebuffer.Initialize();
    fullscreen_quad.Initialize();

    InitializeInstanceData();
}

Scene::~Scene()
{
}

void Scene::InitializeInstanceData(void)
{
    auto width = debug.width;
    auto size = (width - (-width) + 1) * (width - (-width) + 1);
    model_instances.resize(size);
    light_instances.resize(size);

    auto i = 0;
    for (auto x = -debug.width; x <= debug.width; x++)
    {
        for (auto y = -debug.width; y <= debug.width; y++, i++)
        {
            const auto position = glm::vec3(x * 3.0f, 0, y * 3.0f);
            const auto orbit = batteries::random_point_on_sphere();

            // light instances
            light_instances[i] = {
                .color = batteries::random_color(),
                .position = glm::vec4(position, 1.0f) + orbit  * orbit_radius,
            };

            // model instances
            model_instances[i] = batteries::random_model_matrix(position);
        }
    }
}
#pragma endregion

#pragma region Update Loop
void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    #pragma region Render G-Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
    {
        //Setup rendering scope
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Setup shader program
        geometry->use();

        geometry->setMat4("view_proj", view_proj);

        geometry->setInt("objectTexture", 0);

        geometry->setFloat("material.ambient", material.ambient);
        geometry->setFloat("material.diffuse", material.diffuse);
        geometry->setFloat("material.specular", material.specular);
        geometry->setFloat("material.shininess", material.shininess);


        auto planeTransform = glm::translate(glm::mat4(1.0), glm::vec3(0, -10, 0));
        geometry->setMat4("model", planeTransform);
        plane.draw();

        //Draw models
        auto i = 0;
        for (auto x = -debug.width; x <= debug.width; x++)
        {
            for (auto y = -debug.width; y <= debug.width; y++, i++)
            {
                geometry->setMat4("model", model_instances[i]);

                // Draw the object
                suzanne->draw();
            }
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    #pragma endregion

    #pragma region Render Light volume
    glBindFramebuffer(GL_FRAMEBUFFER, lightvolumebuffer.fbo);
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        //Clear buffer
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Send in color attachments
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer.position);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, framebuffer.normal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, framebuffer.albedo);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, framebuffer.material);

        blinnphong->use();

        //Camera data
        blinnphong->setMat4("view_proj", view_proj);
        blinnphong->setVec3("camera", camera.position);

        //Deferred rending data
        blinnphong->setInt("g_position", 0);
        blinnphong->setInt("g_normal", 1);
        blinnphong->setInt("g_albedo", 2);
        blinnphong->setInt("g_material", 3);
        blinnphong->setVec2("screenSize", glm::vec2(framebuffer.kFrameBufferWidth, framebuffer.kFrameBufferHeight));

        for (int i = 0; i < light_instances.size(); ++i){
            //Lighting data
            blinnphong->setVec3("light.position",  light_instances[i].position);
            blinnphong->setVec3("light.color", light_instances[i].color);
            blinnphong->setFloat("light.radius", debug.light_radius);

            //Model data
            auto sphere_mat4 = glm::translate(glm::mat4(1.0), light_instances[i].position) * glm::scale(glm::mat4(1.0), glm::vec3(debug.light_radius));
            blinnphong->setMat4("model", sphere_mat4);

            //Render spheres
            sphere.draw();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    #pragma endregion

    #pragma region Render Fullscreen Quad
    { // render fullscreen quad

        noprocess->use();
        noprocess->setInt("screen", 0);
        noprocess->setInt("lighting", 1);

        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBindVertexArray(fullscreen_quad.vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer.albedo);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, lightvolumebuffer.color);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    #pragma endregion

    #pragma region Render Light Sources (Forward Render)
    { 
        if (!debug.draw_light_volume) return;

        //glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);

        // glClearColor(0.0f,0.0f,0.0f,0.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        
        //Blit the old depth buffer onto the current forward rendering
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, 
            framebuffer.kFrameBufferWidth, 
            framebuffer.kFrameBufferHeight,
            0, 0, 
            framebuffer.kFrameBufferWidth, 
            framebuffer.kFrameBufferHeight, 
            GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //Draw lighting volumes 
        lightsphere->use();
       

        for (auto light : light_instances){
            auto sphere_mat4 = glm::translate(glm::mat4(1), light.position);

            sphere_mat4 = glm::scale(sphere_mat4, glm::vec3(debug.light_radius));

            lightsphere->setMat4("view_proj", view_proj);
            lightsphere->setMat4("model", sphere_mat4);
            lightsphere->setVec3("color", light.color);
            sphere.draw(ew::DrawMode::LINES);
        }
    }
    #pragma endregion
}

void Scene::Debug(void)
{
    cameracontroller.Debug();

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    if (ImGui::SliderInt("Width", &debug.width, 1, 100))
    {
        InitializeInstanceData();
    }

    if (ImGui::CollapsingHeader("Lights"))
    {
        ImGui::Checkbox("Draw Volumes", &debug.draw_light_volume);
        ImGui::SliderFloat("Light Radius", &debug.light_radius, 1.0f, 100.0f);
    }

    if (ImGui::CollapsingHeader("Material"))
    {
        ImGui::SliderFloat("Ambient", &material.ambient, 0.0f, 1.0f);
        ImGui::SliderFloat("Diffuse", &material.diffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular", &material.specular, 0.0f, 1.0f);
        ImGui::SliderFloat("Shininess", &material.shininess, 0.0f, 1.0f);
    }

    if (ImGui::CollapsingHeader("Geometry Buffer"))
    {
        ImVec2 uv_min(0.0f, 1.0f);
        ImVec2 uv_max(1.0f, 0.0f);

        ImGui::Text("Lighting:");
        ImGui::Image((ImTextureID)(intptr_t)lightvolumebuffer.color, ImVec2(200, 150), uv_min, uv_max);

        ImGui::Text("Albedo:");
        ImGui::Image((ImTextureID)(intptr_t)framebuffer.albedo, ImVec2(200, 150), uv_min, uv_max);

        ImGui::Text("Material:");
        ImGui::Image((ImTextureID)(intptr_t)framebuffer.material, ImVec2(200, 150), uv_min, uv_max);

        ImGui::Text("Position:");
        ImGui::Image((ImTextureID)(intptr_t)framebuffer.position, ImVec2(200, 150), uv_min, uv_max);

        ImGui::Text("Normal:");
        ImGui::Image((ImTextureID)(intptr_t)framebuffer.normal, ImVec2(200, 150), uv_min, uv_max);

        ImGui::Text("Depth:");
        ImGui::Image((ImTextureID)(intptr_t)framebuffer.depth, ImVec2(200, 150), uv_min, uv_max);
    }
    ImGui::End();
}
#pragma endregion