//
// Created by Koiro on 06/03/2025.
//

#ifndef OISEAU_ECS_ENTITYMANAGER_H
#define OISEAU_ECS_ENTITYMANAGER_H

#include "library.h"
#include "queue"

class EntityManager {
  private:
    std::queue<Entity> availableEntities{};
    std::array<Signature, MAX_ENTITIES> signatures{};
    Entity livingEntityCount{};

  public:
    EntityManager() {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
            availableEntities.push(entity);
        }
    }

    Entity CreateEntity() {
        assert(livingEntityCount < MAX_ENTITIES && "Entity out of range!");

        Entity id = availableEntities.front();
        availableEntities.pop();
        ++livingEntityCount;

        return id;
    }

    void DestroyEntity(Entity entity) {
        assert(entity < MAX_ENTITIES && "Entity out of range!");

        signatures[entity].reset();

        availableEntities.push(entity);
        --livingEntityCount;
    }

    void SetSignature(Entity entity, Signature signature) {
        assert(entity < MAX_ENTITIES && "Entity out of range!");

        signatures[entity] = signature;
    }

    Signature GetSignature(Entity entity) {
        assert(entity < MAX_ENTITIES && "Entity out of range!");

        return signatures[entity];
    }
};

#endif // OISEAU_ECS_ENTITYMANAGER_H
