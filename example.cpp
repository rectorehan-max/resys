// example usage using raylib

#include "raylib.h"
#define RESYS_IMPLEMENTATION // define only ones
#define RESYS_DEBUG // for loging
#include "resys.hpp" // include resys

#include <iostream>

namespace resys
{

// defining ResourceType enum class inside of resys namespace
enum class ResType {
    TEXTURE
};


class Texture : public resys::Resource {
public: 
    Texture(const std::string& name, const std::string& path) :
          resys::Resource(name, path, resys::ResType::TEXTURE) {}

    virtual int loadAction() override {
        texture = LoadTexture(path.c_str());
        return 0; // let's assume that the texture will load succesfully
    }

    virtual int unloadAction() override {
        UnloadTexture(texture);
        return 0;
    }

    virtual void logFailLoad() override {
        std::cout << "Load failed \n";
    }

    virtual void logFailUnload() override {
        std::cout << "Unload failed \n";
    }

    Texture2D getTexture() const {
        return texture;
    }

private:
    Texture2D texture = { 0 };
};

}

int main() {
    InitWindow(1280, 768, "resys");
    SetTargetFPS(60);

    // defining the loader
    resys::defineLoader([](const std::string& name, resys::ResType type) -> std::unique_ptr<resys::Resource>
    {
        if (name == "test") {
            return std::move(std::make_unique<resys::Texture>(name, "test.png"));
        } else {
            return nullptr;
        }
    });

    resys::Texture* texture_ = dynamic_cast<resys::Texture*>(resys::borrow("test", resys::ResType::TEXTURE));
    resys::Texture* texture2_ = dynamic_cast<resys::Texture*>(resys::borrow("test", resys::ResType::TEXTURE));

    resys::Texture* texture3_ = resys::borrowAs<resys::Texture>("test", resys::ResType::TEXTURE);

    // getting test texture

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        EndDrawing();
    }

    CloseWindow();

    resys::release(texture_);
    resys::release(texture2_);

    resys::release(texture3_);
    // freeing test texture when the user are equals to 0

    return 0;
}