

#ifndef MODELSHOLDER_H
#define MODELSHOLDER_H
#include "../models/Model.h"
#include "../objects/Entity.h"

class EntitiesHolder {
public:
    EntitiesHolder(std::vector<Entity> entities): entities(std::move(entities)) {}
    ~EntitiesHolder() = default;
    const std::vector<Entity>& GetEntities() const { return entities; }
private:
    std::vector<Entity> entities;
};



#endif //MODELSHOLDER_H
