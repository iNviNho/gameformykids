

#include "RandomPlayerMover.h"

const float MOVEMENT_SPEED = 3.15f;
int iterations = 0;
int lastRandom = 0;
int maxIterations = 500;

void RandomPlayerMover::move(float deltaTime) {
    // random movement front, left, right. No back
    float velocity = MOVEMENT_SPEED * deltaTime;
    glm::vec3 position;
    int random;

    if (iterations == 0) {
        // random rotation between 170 and 190 degrees
        float rotation = 170.0f + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(190.0f-170.0f)));
        random = rand() % 2;
        lastRandom = random;
    } else if (iterations != maxIterations) {
        random = lastRandom;
    } else {
        iterations = -1;
        random = rand() % 2;
        lastRandom = random;
    }

    switch (random) {
        case 0:
            position = glm::vec3(0.4f, 0.0f, -1.0f);
            break;
        case 1:
            position = glm::vec3(-0.4f, 0.0f, -1.0f);
            break;
    }
    player.Move(position * velocity);
    iterations++;
}
