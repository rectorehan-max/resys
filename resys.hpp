// resys - Resource management system, designed for raylib
// author - rectorehan-max
// github - https://github.com/rectorehan-max/resys 

/*
How to set this up?

-   resys is packaged as a stb single header file so you need to #define RESYS_IMPLEMENTATION once and #include resys.hpp
    additionally, you can also #define RESYS_DEBUG to check the system's logs.

-   resys uses an enum class ResourceType to distinguish different types of resources from each other. In the source code,
    ResourceType is only forward declared, so to set this up properly, you need to define ResourceType yourself.

*/

#ifndef RESYS_HPP
#define RESYS_HPP

#include <string>
#include <functional>
#include <memory>
#include <cstddef>
#include <type_traits>

#ifdef RESYS_DEBUG
    #include <iostream>
    #define RESYS_LOG(msg) std::cout << "[RESYS] " << msg << std::endl;
#endif

namespace resys
{

// Define this enum to your liking
enum class ResType;

class Resource {
protected:
    Resource(const std::string& name, ResType type);

public:
    const std::string name;
    const ResType type;

    virtual bool loadAction() = 0;
    virtual bool unloadAction() = 0;

    virtual void logFailLoad() = 0;
    virtual void logFailUnload() = 0;

    virtual ~Resource() = default;
};

void defineLoader(const std::function<std::unique_ptr<Resource>(const std::string& name, ResType type)>& loadAction);
Resource* borrow(const std::string& name, ResType type);
void release(Resource* res_);

template<typename T>
T* borrowAs(const std::string& name, ResType type) {
    Resource* res_ = borrow(name, type);

    static_assert(
        std::is_base_of_v<Resource, T>,
        "T must be a child of Resource"
    );

    T* t_ = nullptr;

    #ifdef RESYS_DEBUG
        t_ = dynamic_cast<T*>(res_);
    #else 
        t_ = static_cast<T*>(res_);
    #endif

    return t_;
}

}

#endif // RESYS_HPP



#ifdef RESYS_IMPLEMENTATION
namespace resys
{

struct Entry {
    std::unique_ptr<Resource> res_u;
    size_t users;
};

std::unordered_map<std::string, Entry> resEntry;
std::function<std::unique_ptr<Resource>(const std::string& name, ResType type)> loader = nullptr;


Resource::Resource(const std::string& name, ResType type) :
                   name(name),
                   type(type) {}

void defineLoader(const std::function<std::unique_ptr<Resource>(const std::string& name, ResType type)>& l) {
    loader = l;
}

Resource* borrow(const std::string& name, ResType type) {
    auto it = resEntry.find(name);

    if (it != resEntry.end()) {
        ++it->second.users;

        #ifdef RESYS_DEBUG
            RESYS_LOG("Added from: " + name + " | Users: " + std::to_string(it->second.users));
        #endif

        return it->second.res_u.get();
    }

    if (!loader) return nullptr;

    std::unique_ptr<Resource> r_u = loader(name, type);

    if (!r_u) return nullptr;

    if (r_u->name != name) {

        #ifdef RESYS_DEBUG
            RESYS_LOG("Name mismatch. Expected name: " + name + " Got: " + r_u->name);
        #endif

        return nullptr;
    }

    if (!r_u->loadAction())  {
        r_u->logFailLoad();
        return nullptr;
    }

    auto& entry = resEntry[name];
    entry.res_u = std::move(r_u);
    entry.users = 1;

    #ifdef RESYS_DEBUG
        RESYS_LOG(name + " Cached, returning ptr. | Users: 1");
    #endif

    return entry.res_u.get();

}

void release(Resource* res_) {
    if (!res_) return;

    auto it = resEntry.find(res_->name);

    if (it == resEntry.end()) return;

    --it->second.users;

    #ifdef RESYS_DEBUG
        RESYS_LOG("Removed from: " + res_->name + " | Users: " + std::to_string(it->second.users));
    #endif

    if (it->second.users == 0) {

        if (!it->second.res_u->unloadAction()) {
            it->second.res_u->logFailUnload();
        }

        resEntry.erase(it);

        #ifdef RESYS_DEBUG
            RESYS_LOG(res_->name + " Released");
        #endif
    }
}


}

#endif // RESYS_IMPLEMENTATION