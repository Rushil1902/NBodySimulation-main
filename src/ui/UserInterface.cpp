#include "UserInterface.hpp"

#include "physics/Physics.hpp"
#include "rendering/Renderer.hpp"
#include "scene/Scene.hpp"
#include "simulation/Simulation.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

namespace {
struct TextureOption {
    const char* name;
    const char* path;
};

constexpr TextureOption textureOptions[] = {
    {"Sun", "../src/textures/planets/2k_sun.png"},
    {"Earth", "../src/textures/planets/2k_earth_daymap.png"},
    {"Jupiter", "../src/textures/planets/2k_jupiter.png"},
    {"Saturn", "../src/textures/planets/2k_saturn.png"}
};

bool inputVector3(const char* label, glm::dvec3& vector)
{
    double values[] = {vector.x, vector.y, vector.z};
    if (!ImGui::InputScalarN(
            label,
            ImGuiDataType_Double,
            values,
            3,
            nullptr,
            nullptr,
            "%.8g"))
        return false;

    vector = glm::dvec3(values[0], values[1], values[2]);
    return true;
}

int findTextureOption(const std::string& texturePath)
{
    for (int index = 0; index < static_cast<int>(std::size(textureOptions)); ++index)
    {
        if (texturePath == textureOptions[index].path)
            return index;
    }
    return -1;
}

void showVector(const char* label, const glm::dvec3& vector)
{
    ImGui::Text("%s: (%.5e, %.5e, %.5e)", label, vector.x, vector.y, vector.z);
}
}

UserInterface::UserInterface(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.ItemSpacing = ImVec2(8.0f, 7.0f);

    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
        return;

    if (!ImGui_ImplOpenGL3_Init("#version 330 core"))
    {
        ImGui_ImplGlfw_Shutdown();
        return;
    }

    initialized = true;
}

UserInterface::~UserInterface()
{
    if (initialized)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }
    ImGui::DestroyContext();
}

void UserInterface::beginFrame()
{
    if (!initialized)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UserInterface::draw(Scene& scene, Simulation& simulation, Renderer& renderer)
{
    if (!initialized)
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(12.0f, 12.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(
        ImVec2(390.0f, std::max(520.0f, io.DisplaySize.y - 24.0f)),
        ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("N-Body Laboratory"))
    {
        ImGui::End();
        return;
    }

    ImGui::SeparatorText("Simulation");
    if (ImGui::Button(simulation.isPaused() ? "Resume" : "Pause", ImVec2(92.0f, 0.0f)))
        simulation.togglePaused();
    ImGui::SameLine();
    if (ImGui::Button("Single step", ImVec2(105.0f, 0.0f)))
        simulation.requestSingleStep();
    ImGui::SameLine();
    if (ImGui::Button("Reset", ImVec2(75.0f, 0.0f)))
    {
        simulation.reset();
        selectedBody = std::min(selectedBody, static_cast<int>(scene.bodies().size()) - 1);
        followSelectedBody = false;
        renderer.resetCameraFocus();
    }

    double simulationRate = simulation.yearsPerRealSecond();
    constexpr double minimumRate = 0.001;
    constexpr double maximumRate = 64.0;
    if (ImGui::SliderScalar(
            "Speed (years/sec)",
            ImGuiDataType_Double,
            &simulationRate,
            &minimumRate,
            &maximumRate,
            "%.3f",
            ImGuiSliderFlags_Logarithmic))
        simulation.setYearsPerRealSecond(simulationRate);

    double timeStep = simulation.timeStepYears();
    if (ImGui::InputDouble("Physics step (years)", &timeStep, 0.0001, 0.001, "%.6f"))
        simulation.setTimeStepYears(timeStep);

    bool trailsVisible = renderer.areTrailsVisible();
    if (ImGui::Checkbox("Trails", &trailsVisible))
        renderer.setTrailsVisible(trailsVisible);
    ImGui::SameLine();
    bool gridVisible = renderer.isGridVisible();
    if (ImGui::Checkbox("Gravity field slice", &gridVisible))
        renderer.setGridVisible(gridVisible);

    if (gridVisible && ImGui::CollapsingHeader(
            "Field slice controls",
            ImGuiTreeNodeFlags_DefaultOpen))
    {
        GravityFieldSettings& field = renderer.gravityFieldSettings();
        const std::vector<RigidBody>& currentBodies = scene.bodies();
        const bool hasSelectedBody = selectedBody >= 0
            && selectedBody < static_cast<int>(currentBodies.size());

        if (followSelectedSlice && hasSelectedBody)
            field.sliceZAU = currentBodies[selectedBody].position.z;

        ImGui::TextWrapped(
            "Samples the full 3D gravity field across an XY plane at the selected Z level.");
        ImGui::BeginDisabled(followSelectedSlice);
        constexpr double minimumSliceZ = -40.0;
        constexpr double maximumSliceZ = 40.0;
        ImGui::SliderScalar(
            "Slice Z (AU)",
            ImGuiDataType_Double,
            &field.sliceZAU,
            &minimumSliceZ,
            &maximumSliceZ,
            "%.2f");
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!hasSelectedBody);
        ImGui::Checkbox("Follow selected body's Z", &followSelectedSlice);
        ImGui::EndDisabled();
        if (!hasSelectedBody)
            followSelectedSlice = false;

        ImGui::Checkbox("Flat heatmap", &field.flatHeatmap);
        ImGui::SameLine();
        ImGui::Checkbox("Surface fill", &field.surfaceVisible);
        ImGui::SliderFloat("Slice width (AU)", &field.widthAU, 4.0f, 80.0f, "%.1f");

        ImGui::BeginDisabled(field.flatHeatmap);
        ImGui::SliderFloat("Well depth", &field.maximumDepth, 0.0f, 24.0f, "%.1f");
        ImGui::EndDisabled();
        ImGui::SliderFloat(
            "Well width (AU)",
            &field.softeningAU,
            0.01f,
            5.0f,
            "%.3f",
            ImGuiSliderFlags_Logarithmic);
        ImGui::SliderFloat(
            "Field intensity",
            &field.fieldStrength,
            0.01f,
            4.0f,
            "%.3f",
            ImGuiSliderFlags_Logarithmic);
        ImGui::BeginDisabled(!field.surfaceVisible);
        ImGui::SliderFloat("Surface opacity", &field.surfaceOpacity, 0.02f, 0.60f, "%.2f");
        ImGui::EndDisabled();
        ImGui::TextDisabled("Blue: weaker field | Orange: stronger field");
    }

    ImGui::SeparatorText("Diagnostics");
    ImGui::Text("State: %s", simulation.isPaused() ? "Paused" : "Running");
    ImGui::Text("Elapsed time: %.6f years", simulation.elapsedYears());
    ImGui::Text("Total energy: %.8e", simulation.totalEnergy());
    ImGui::Text("Relative energy drift: %.5e", simulation.relativeEnergyDrift());
    showVector("Momentum", simulation.totalMomentum());
    showVector("Angular momentum", simulation.totalAngularMomentum());

    ImGui::SeparatorText("Bodies");
    std::vector<RigidBody>& bodies = scene.bodies();
    if (bodies.empty())
        selectedBody = -1;
    else
        selectedBody = std::clamp(selectedBody, 0, static_cast<int>(bodies.size()) - 1);

    if (ImGui::BeginListBox("##BodyList", ImVec2(-1.0f, 105.0f)))
    {
        for (int index = 0; index < static_cast<int>(bodies.size()); ++index)
        {
            ImGui::PushID(index);
            const bool selected = selectedBody == index;
            const std::string label = bodies[index].name.empty()
                ? "Body " + std::to_string(index + 1)
                : bodies[index].name;
            if (ImGui::Selectable(label.c_str(), selected))
                selectedBody = index;
            if (selected)
                ImGui::SetItemDefaultFocus();
            ImGui::PopID();
        }
        ImGui::EndListBox();
    }

    if (ImGui::Button("Add body"))
    {
        simulation.setPaused(true);
        if (bodies.empty())
        {
            RigidBody newStar(1.0, 0.00465047);
            newStar.name = "Primary Star";
            newStar.isStar = true;
            newStar.loadTexture(textureOptions[0].path);
            scene.addBody(std::move(newStar));
        }
        else
        {
            const double orbitRadius = std::max(1.0, static_cast<double>(bodies.size()));
            const double centralMass = std::max(1.0e-12, bodies.front().bodyMass);

            RigidBody newBody(3.0034896e-6, 4.26352e-5);
            newBody.name = "Body " + std::to_string(bodies.size() + 1);
            newBody.position = glm::dvec3(orbitRadius, 0.0, 0.0);
            newBody.velocity = glm::dvec3(
                0.0,
                0.0,
                std::sqrt(Physics::gravitationalConstant * centralMass / orbitRadius));
            newBody.loadTexture(textureOptions[1].path);
            scene.addBody(std::move(newBody));
        }
        selectedBody = static_cast<int>(scene.bodies().size()) - 1;
        simulation.notifySceneChanged();
    }
    ImGui::SameLine();
    const bool canRemove = selectedBody >= 0 && selectedBody < static_cast<int>(bodies.size());
    ImGui::BeginDisabled(!canRemove);
    if (ImGui::Button("Remove selected"))
    {
        scene.removeBody(static_cast<std::size_t>(selectedBody));
        selectedBody = std::min(selectedBody, static_cast<int>(scene.bodies().size()) - 1);
        followSelectedBody = false;
        renderer.resetCameraFocus();
        simulation.notifySceneChanged();
    }
    ImGui::EndDisabled();

    if (selectedBody >= 0 && selectedBody < static_cast<int>(bodies.size()))
    {
        RigidBody& body = bodies[selectedBody];
        ImGui::PushID(selectedBody);
        ImGui::SeparatorText("Selected body");

        char nameBuffer[128] = {};
        std::snprintf(nameBuffer, sizeof(nameBuffer), "%s", body.name.c_str());
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
            body.name = nameBuffer;

        bool physicsChanged = false;
        physicsChanged |= ImGui::InputDouble("Mass (solar masses)", &body.bodyMass, 0.0, 0.0, "%.10g");
        physicsChanged |= ImGui::InputDouble("Radius (AU)", &body.bodyRadius, 0.0, 0.0, "%.10g");
        physicsChanged |= inputVector3("Position (AU)", body.position);
        physicsChanged |= inputVector3("Velocity (AU/year)", body.velocity);
        physicsChanged |= ImGui::Checkbox("Fixed in place", &body.fixed);

        if (ImGui::Checkbox("Render as star", &body.isStar))
        {
            if (body.isStar && body.texturePath.empty())
                body.loadTexture(textureOptions[0].path);
        }

        int textureIndex = findTextureOption(body.texturePath);
        const char* texturePreview = textureIndex >= 0 ? textureOptions[textureIndex].name : "Custom";
        if (ImGui::BeginCombo("Texture", texturePreview))
        {
            for (int index = 0; index < static_cast<int>(std::size(textureOptions)); ++index)
            {
                const bool selected = textureIndex == index;
                if (ImGui::Selectable(textureOptions[index].name, selected))
                {
                    body.loadTexture(textureOptions[index].path);
                    textureIndex = index;
                }
                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        body.bodyMass = std::max(body.bodyMass, 1.0e-12);
        body.bodyRadius = std::max(body.bodyRadius, 1.0e-12);
        if (physicsChanged)
        {
            body.clearPath();
            simulation.notifySceneChanged();
        }

        if (ImGui::Button("Focus camera"))
        {
            followSelectedBody = true;
            renderer.focusOnBody(body);
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear focus"))
        {
            followSelectedBody = false;
            renderer.resetCameraFocus();
        }

        if (followSelectedBody)
        {
            renderer.focusOnBody(body);
            ImGui::TextDisabled("Camera following selected body");
        }
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Advanced physics"))
    {
        double softening = simulation.softeningLengthAU();
        if (ImGui::InputDouble("Softening length (AU)", &softening, 0.0, 0.0, "%.8g"))
            simulation.setSofteningLengthAU(softening);
        ImGui::TextDisabled("Edits pause the simulation and reset the energy baseline.");
    }

    ImGui::Separator();
    ImGui::TextDisabled("Mouse: orbit | Shift+drag: pan | Wheel: zoom");
    ImGui::TextDisabled("Space: pause | N: step | R: reset | Esc: close");
    ImGui::End();
}

void UserInterface::render()
{
    if (!initialized)
        return;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool UserInterface::wantsKeyboardCapture() const
{
    return initialized && ImGui::GetIO().WantCaptureKeyboard;
}

bool UserInterface::wantsMouseCapture() const
{
    return initialized && ImGui::GetIO().WantCaptureMouse;
}
