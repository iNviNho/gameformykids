

#ifndef MODELSHOLDER_H
#define MODELSHOLDER_H
#include "../models/Model.h"
#include "../objects/Entity.h"

class EntitiesHolder {
public:
    EntitiesHolder() = default;
    EntitiesHolder(std::vector<Entity>&& entities): entities(std::move(entities)) {}
    ~EntitiesHolder() = default;
    const std::vector<Entity>& GetEntities() const { return entities; }

    void AddEntity(const Entity& entity) {
        entities.push_back(entity);
    }
private:
    std::vector<Entity> entities{};
};



#endif //MODELSHOLDER_H
