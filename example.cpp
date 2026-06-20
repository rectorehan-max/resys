#define RESYS_IMPLEMENTATION // define only once
#define RESYS_DEBUG // define for debugging
#include "resys.hpp"
#include "raylib.h" // using raylib for example

#include <iostream>

// defining ResType enum class inside of namespace resys
namespace resys
{

enum class ResType {
    TEXTURE_IMAGE, // for example
};

}

// making a child class of resys::Resource
class TextureImage : public resys::Resource {
public:
    TextureImage(const std::string& name, const std::string& path, 
                 int width, int height) :

                 resys::Resource(name, path, resys::ResType::TEXTURE_IMAGE), // calling resys::Resource constructor
                 width(width), height(height) {}

    const int width; // width of the texture image
    const int height; // height of the texture image

    Texture2D getTexture() const {
        return texture;
    }

    // overiding virtual functions inherited from resys::Resource

    virtual int loadAction() override { // load action for the resource. return 0 if succesful, != 0 if not succesful.
        texture = LoadTexture(path.c_str());
        return 0; // for the example, we will always return 0 (succesful)
    }

    virtual int unloadAction() override { // unload action for the resource
        UnloadTexture(texture);
        return 0;
    }

    virtual void logFailLoad() override {
        std::cout << "Failed to load \n";
    }

    virtual void logFailUnload() override {
        std::cout << "Failed to unload \n";
    }

private:
    Texture2D texture;
};

void setLoader() { // to set the loader of the resys system
    resys::defineLoader([](const std::string& name, resys::ResType type) -> std::unique_ptr<resys::Resource> // <- to stop the compiler from whining
    {
        if (name == "test") {
            return std::move(std::make_unique<TextureImage>(name, "test.png", 64, 64)); // example
        }

        return nullptr;
    });
}


int main() {
    InitWindow(1230, 768, "resys"); // raylib stuff
    SetTargetFPS(60);

    setLoader(); // setting the definition of the loader

    TextureImage* image_ = resys::borrowAs<TextureImage>("test", resys::ResType::TEXTURE_IMAGE); // borrowing test
    TextureImage* image2_ = resys::borrowAs<TextureImage>("test", resys::ResType::TEXTURE_IMAGE); // borrowing test
    // users for test will be 2

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyPressed(KEY_ENTER)) {
            resys::release(image2_); // users for test 1 now. and because it's not == 0, test will be be unloaded and deleted
        }

        EndDrawing();

    }

    resys::release(image_); // users for test is 0, so test will be unloaded and deleted
    
    CloseWindow();
    return 0;
}