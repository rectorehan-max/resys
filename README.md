# resys
A simple and lightweight resource management system.

## Features
- Single-header (stb-style)
- Customizable loader
- User counting system
- Lazy loading

## Usage
- Just copy `resys.hpp` into your project then define only once `RESYS_IMPLEMENTATION`
- Additionaly you can define `RESYS_DEBUG` for loging.
- You need to define, `resys::ResourceType` which is a enum class inside namespace resys (it's only forward declared in the .hpp file)
Example:
```cpp
namespace resys
{

enum class ResourceType {
    IMAGE, SOUND, FONT, // ...whatever you like
}

}
```
- Lastly you need to define the loader, which you can do by using the `resys::defineLoader` function.

Example:
```cpp
resys::defineLoader([](const std::string& name, resys::ResourceType type) -> std::unique_ptr<resys::Resource>
{
    if (name == "test") {
        return std::move(std::make_unique<resys::Texture>(name, "test.png"));
    } else {
        return nullptr;
    }
});
```

# About
This system is intended to be used for raylib loading and unloading functions. This system is designed to prevent multiple loading of resources and lazy loading.

# How the system works
- The system keeps track of how many users are using a resource by using `resys::getRes_` (to increment users, or to make a new resource if it's not made yet) and `resys::releaseRes_` (decrements users, and if the user count is 0, the resource will be unloaded and deleted).
- The loader should return a resource. The resource's name should match the name in the `resys::getRes_` parameter to keep the names consistent. If this rule isn't met, the system will not add that resource.
- The functions `resys::getRes_` and `resys::releaseRes_` trusts the programmer to use it properly. Make sure that if you call `resys::getRes_` you don't lose the reference to it so that you can release it later. And make sure to only call `resys::releaseRes_` on a borrowing resource ptr just once. If those rules are not followed the user count will be out of synch.