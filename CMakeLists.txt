cmake_minimum_required(VERSION 3.14)
project(NBodySimulation VERSION 1.0 LANGUAGES C CXX)

cmake_policy(SET CMP0074 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add third-party libraries (one per call)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/thirdparty/imgui)

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
    src/ui/UserInterface.cpp
    src/textures/stb_image.cpp
    src/core/SkyboxMesh.cpp
    src/core/TrailMesh.cpp
    src/physics/SpaceTimeGrid.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )

set_target_properties(NBodySimulation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
foreach(NBODY_BUILD_CONFIG Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${NBODY_BUILD_CONFIG}" NBODY_BUILD_CONFIG_UPPER)
    set_target_properties(NBodySimulation PROPERTIES
        "RUNTIME_OUTPUT_DIRECTORY_${NBODY_BUILD_CONFIG_UPPER}" "${CMAKE_BINARY_DIR}"
    )
endforeach()

set(NBODY_RUNTIME_ASSET_DIR "${CMAKE_BINARY_DIR}/assets")
add_custom_target(NBodyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/graphics/meshShaders"
        "${NBODY_RUNTIME_ASSET_DIR}/shaders/mesh"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/graphics/SkyboxShaders"
        "${NBODY_RUNTIME_ASSET_DIR}/shaders/skybox"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/graphics/TrailShaders"
        "${NBODY_RUNTIME_ASSET_DIR}/shaders/trail"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/graphics/PlanetShaders"
        "${NBODY_RUNTIME_ASSET_DIR}/shaders/planet"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/graphics/StarShaders"
        "${NBODY_RUNTIME_ASSET_DIR}/shaders/star"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/graphics/GridShaders"
        "${NBODY_RUNTIME_ASSET_DIR}/shaders/grid"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/textures/planets"
        "${NBODY_RUNTIME_ASSET_DIR}/textures/planets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/textures/skybox"
        "${NBODY_RUNTIME_ASSET_DIR}/textures/skybox"
    COMMENT "Staging runtime shaders and textures"
    VERBATIM
)
add_dependencies(NBodySimulation NBodyAssets)

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
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends

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
