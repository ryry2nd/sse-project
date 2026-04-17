#include "Rendering.hpp"
#include <algorithm>

template<typename T>
void Objects::addParticle(std::unique_ptr<T> obj)
{
    static_assert(std::is_base_of_v<Object, T>, "T must inherit from Particle");
    
    ObjectID index = allObjects.size();
    allObjects.push_back(std::move(obj));

    if (std::is_base_of_v<Camera, T>) {
        allCams.push_back(static_cast<Camera*>(obj.get()));
    }
}

void Objects::removeParticle(ObjectID id) {
    Camera* cam = dynamic_cast<Camera*>(allObjects[id].get());
    if (cam != nullptr) {
        auto it = std::find_if(allObjects.begin(), allObjects.end(),
        [&](const std::unique_ptr<Object>& obj) {
            return obj.get() == cam;
        });
        
        if (it != allObjects.end()) {
            allObjects.erase(it);
        }
    }

    allObjects.erase(allObjects.begin() + id);
}