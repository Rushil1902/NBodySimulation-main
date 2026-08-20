#pragma once

#include "physics/RigidBody.hpp"

#include <vector>

class Scene {
public:
    static Scene createDefault();

    std::vector<RigidBody>& bodies();
    const std::vector<RigidBody>& bodies() const;
    void reset();

private:
    void captureInitialState();

    std::vector<RigidBody> sceneBodies;
    std::vector<RigidBody> initialBodies;
};
