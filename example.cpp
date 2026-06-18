// example usage using raylib

#include "raylib.h"
#define RESYS_IMPLEMENTATION // define only ones
#define RESYS_DEBUG // for loging
#include "resys.hpp" // include resys

#include <iostream>

namespace resys
{

// defining ResourceType enum class inside of resys namespace
enum class ResourceType {
    TEXTURE
};


class Texture : public resys::Resource {
public: 
    Texture(const std::string& name, const std::string& path) :
          resys::Resource(name, path, resys::ResourceType::TEXTURE) {}

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
    resys::defineLoader([](const std::string& name, resys::ResourceType type) -> std::unique_ptr<resys::Resource>
    {
        if (name == "test") {
            return std::move(std::make_unique<resys::Texture>(name, "test.png"));
        } else {
            return nullptr;
        }
    });

    resys::Texture* texture_ = dynamic_cast<resys::Texture*>(resys::getRes_("test", resys::ResourceType::TEXTURE));
    resys::Texture* texture2_ = dynamic_cast<resys::Texture*>(resys::getRes_("test", resys::ResourceType::TEXTURE));
    // getting test texture

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        EndDrawing();
    }

    CloseWindow();

    resys::releaseRes_(texture_);
    resys::releaseRes_(texture2_);
    // freeing test texture when the user are equals to 0

    return 0;
}