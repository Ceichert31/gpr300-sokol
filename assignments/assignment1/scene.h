#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"

// ew
#include "ew/model.h"
#include "ew/shader.h"
#include "ew/texture.h"

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void PostProcess(ew::Shader* shader);
    void Render(void);
    void Debug(void);

  private:
    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Model> suzanneBP;
    std::unique_ptr<ew::Shader> toon;
     std::unique_ptr<ew::Shader> blinnPhong;
    std::unique_ptr<ew::Texture> mainTexture;
    std::unique_ptr<ew::Texture> normalTexture;
    std::unique_ptr<ew::Texture> noiseTexture;

    batteries::light_t light;

    struct {
      glm::vec3 color1;
      glm::vec3 color2;
    } palette;

    enum Effects {
      DisableEffect,
      BoxBlur,
      Outline,
      UVNoise,
      PixelFilter,
      Vignette
    };

    unsigned int framebuffer;
    unsigned int fboTexture;
    unsigned int fboDepth;
};
