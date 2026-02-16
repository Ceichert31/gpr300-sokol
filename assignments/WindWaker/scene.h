#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"

// ew
#include "ew/model.h"
#include "ew/shader.h"
#include "ew/texture.h"
#include "ew/mesh.h"

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Debug(void);

  private:
    std::unique_ptr<ew::Shader> waterShader;

        //Mipmaps
    std::unique_ptr<ew::Texture> water128; //0
    std::unique_ptr<ew::Texture> water64; //1
    std::unique_ptr<ew::Texture> water32; //2
    std::unique_ptr<ew::Texture> water16; //3
    std::unique_ptr<ew::Texture> water8; //4

    ew::Mesh plane;
};


