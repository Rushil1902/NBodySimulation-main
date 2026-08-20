cmake_minimum_required(VERSION 3.14)
project(NBodySimulation VERSION 1.0 LANGUAGES C CXX)

cmake_policy(SET CMP0074 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add third-party libraries (one per call)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)

# Make sure to define where OpenGL is located, on Windows this is generally available
find_package(OpenGL REQUIRED)


# Create the executable
add_executable(NBodySimulation 
    src/main.cpp
    src/application/Application.cpp
    src/glad.c  # Add glad.c as a source file
    src/core/Mesh.cpp
    src/geometry/ShapeGenerator.cpp
    src/geometry/Transform.cpp
    src/physics/RigidBody.cpp 
    src/physics/Physics.cpp 
    src/graphics/camera.cpp
    src/rendering/Renderer.cpp
    src/scene/Scene.cpp
    src/simulation/Simulation.cpp
    src/textures/stb_image.cpp
    src/core/SkyboxMesh.cpp
    src/core/TrailMesh.cpp
    src/physics/SpaceTimeGrid.cpp
    )

# Link libraries
target_link_libraries(NBodySimulation PRIVATE OpenGL::GL glfw)

# Include the directories for GLFW and Glad
target_include_directories(NBodySimulation PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/thirdparty/glfw/include
    ${CMAKE_SOURCE_DIR}/thirdparty/glad/include
    ${CMAKE_SOURCE_DIR}/thirdparty/KHR/include
    ${CMAKE_SOURCE_DIR}/thirdparty/glm
    ${CMAKE_SOURCE_DIR}/thirdparty/stb

    )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
if(BUILD_TESTING)
    add_executable(PhysicsValidation
        tests/PhysicsValidation.cpp
        src/physics/Physics.cpp
    )

    target_include_directories(PhysicsValidation PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/thirdparty/glm
    )

    add_test(NAME TwoBodyOrbitValidation COMMAND PhysicsValidation)
endif()
