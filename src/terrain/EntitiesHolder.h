#ifndef MODELSHOLDER_H
#define MODELSHOLDER_H
#include "../objects/Entity.h"
#include <glm/glm.hpp>
#include <limits>
#include <optional>

// Represents a simple container for Entity instances and basic manipulation APIs
class EntitiesHolder {
public:
    EntitiesHolder() = default;
    explicit EntitiesHolder(std::vector<Entity>&& entities): entities(std::move(entities)) {}
    ~EntitiesHolder() = default;
    const std::vector<Entity>& GetEntities() const { return entities; }

    void AddEntity(const Entity& entity) {
        entities.push_back(entity);
    }

    // Find the nearest entity to `pos` within `maxDistance` (inclusive).
    // Returns the position of the found entity if one was found; otherwise std::nullopt.
    std::optional<glm::vec3> FindNearest(const glm::vec3& pos, float maxDistance = 1.5f) const {
        if (entities.empty()) return std::nullopt;

        const float maxDistSq = maxDistance * maxDistance;
        std::size_t bestIdx = std::numeric_limits<std::size_t>::max();
        float bestDistSq = std::numeric_limits<float>::infinity();

        for (std::size_t i = 0; i < entities.size(); ++i) {
            const glm::vec3& epos = entities[i].GetPosition();
            const float dx = epos.x - pos.x;
            const float dy = epos.y - pos.y;
            const float dz = epos.z - pos.z;
            const float distSq = dx*dx + dy*dy + dz*dz;
            if (distSq <= maxDistSq && distSq < bestDistSq) {
                bestDistSq = distSq;
                bestIdx = i;
            }
        }

        if (bestIdx != std::numeric_limits<std::size_t>::max()) {
            const glm::vec3 foundPos = entities[bestIdx].GetPosition();
            return foundPos;
        }

        return std::nullopt;
    }

    // Remove the entity that exactly matches the given position (within epsilon). Returns true if removed.
    bool RemoveByPosition(const glm::vec3& pos, float epsilon = 1e-4f) {
        for (auto it = entities.begin(); it != entities.end(); ++it) {
            const glm::vec3& epos = it->GetPosition();
            const float dx = epos.x - pos.x;
            const float dy = epos.y - pos.y;
            const float dz = epos.z - pos.z;
            if ((dx*dx + dy*dy + dz*dz) <= epsilon*epsilon) {
                entities.erase(it);
                return true;
            }
        }
        return false;
    }

private:
    std::vector<Entity> entities{};
};


#endif //MODELSHOLDER_H
