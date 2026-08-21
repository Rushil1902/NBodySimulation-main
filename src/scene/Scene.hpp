#pragma once

#include "physics/RigidBody.hpp"

#include <cstddef>
#include <vector>

class Scene {
public:
    static Scene createDefault();

    std::vector<RigidBody>& bodies();
    const std::vector<RigidBody>& bodies() const;
    void addBody(RigidBody body);
    bool removeBody(std::size_t bodyIndex);
    void reset();

private:
    void captureInitialState();

    std::vector<RigidBody> sceneBodies;
    std::vector<RigidBody> initialBodies;
};
