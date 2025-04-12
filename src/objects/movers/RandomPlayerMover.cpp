

#include "RandomPlayerMover.h"

const float MOVEMENT_SPEED = 5.55f;
int iterations = 0;
int lastRandom = 0;
int maxIterations = 500;

void RandomPlayerMover::move(float deltaTime) {
    // random movement front, left, right. No back
    float velocity = MOVEMENT_SPEED * deltaTime;
    glm::vec3 position;
    int random;

    if (iterations == 0) {
        random = rand() % 4;
        lastRandom = random;
    } else if (iterations != maxIterations) {
        random = lastRandom;
    } else {
        iterations = -1;
        random = rand() % 4;
        lastRandom = random;
    }

    switch (random) {
        case 0:
            position = glm::vec3(0.3f, 0.0f, -0.3f);
            break;
        case 1:
            position = glm::vec3(0.5f, 0.0f, -0.1f);
            break;
        case 2:
            position = glm::vec3(0.4f, 0.0f, -0.2f);
        break;
        case 3:
            position = glm::vec3(0.1f, 0.0f, -0.5f);
        break;
    }
    player.Move(position * velocity);
    iterations++;
}
