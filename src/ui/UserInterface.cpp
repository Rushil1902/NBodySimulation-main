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
#include <random>
#include <string>

namespace {
struct TextureOption {
    const char* name;
    const char* path;
};

constexpr TextureOption textureOptions[] = {
    {"Earth", "../src/textures/planets/2k_earth_daymap.png"},
    {"Jupiter", "../src/textures/planets/2k_jupiter.png"},
    {"Saturn", "../src/textures/planets/2k_saturn.png"}
};

constexpr const char* surfaceTypeNames[] = {
    "Texture",
    "Procedural Earth-like",
    "Procedural gas giant",
    "Procedural lava",
    "Procedural icy moon"
};

constexpr const char* starPresetNames[] = {
    "Solar",
    "Blue-white",
    "Red dwarf",
    "Alien"
};

glm::vec3 hsvColor(float hue, float saturation, float value)
{
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
    ImGui::ColorConvertHSVtoRGB(hue, saturation, value, red, green, blue);
    return glm::vec3(red, green, blue);
}

float wrapHue(float hue)
{
    hue = std::fmod(hue, 1.0f);
    return hue < 0.0f ? hue + 1.0f : hue;
}

std::mt19937& surfaceRandomGenerator()
{
    static std::mt19937 generator(std::random_device{}());
    return generator;
}

float randomFloat(float minimum, float maximum)
{
    return std::uniform_real_distribution<float>(minimum, maximum)(surfaceRandomGenerator());
}

void randomizeSeed(RigidBody& body)
{
    body.proceduralSeed = std::uniform_int_distribution<int>(
        1,
        9999999)(surfaceRandomGenerator());
}

void randomizeStarSeed(RigidBody& body)
{
    body.starSeed = std::uniform_int_distribution<int>(
        1,
        9999999)(surfaceRandomGenerator());
}

void setDefaultPalette(RigidBody& body, PlanetSurfaceType type)
{
    if (type == PlanetSurfaceType::EarthLike)
    {
        body.surfaceColorA = glm::vec3(0.015f, 0.07f, 0.18f);
        body.surfaceColorB = glm::vec3(0.0f, 0.55f, 0.75f);
        body.surfaceColorC = glm::vec3(0.03f, 0.24f, 0.10f);
        body.surfaceColorD = glm::vec3(0.48f, 0.82f, 0.08f);
        body.surfaceColorE = glm::vec3(0.64f, 0.38f, 0.82f);
        body.surfaceColorF = glm::vec3(0.88f, 0.94f, 1.0f);
        body.surfaceColorG = glm::vec3(0.98f, 0.99f, 1.0f);
    }
    else if (type == PlanetSurfaceType::GasGiant)
    {
        body.surfaceColorA = glm::vec3(0.08f, 0.015f, 0.18f);
        body.surfaceColorB = glm::vec3(0.28f, 0.05f, 0.55f);
        body.surfaceColorC = glm::vec3(0.65f, 0.08f, 0.72f);
        body.surfaceColorD = glm::vec3(0.95f, 0.20f, 0.52f);
        body.surfaceColorE = glm::vec3(1.0f, 0.55f, 0.18f);
        body.surfaceColorF = glm::vec3(0.02f, 0.75f, 0.92f);
        body.surfaceColorG = glm::vec3(0.80f, 0.98f, 1.0f);
    }
    else if (type == PlanetSurfaceType::IcyMoon)
    {
        body.surfaceColorA = glm::vec3(0.015f, 0.025f, 0.18f);
        body.surfaceColorB = glm::vec3(0.05f, 0.44f, 0.78f);
        body.surfaceColorC = glm::vec3(0.58f, 0.90f, 1.0f);
        body.surfaceColorD = glm::vec3(0.58f, 0.12f, 1.0f);
        body.surfaceColorE = glm::vec3(0.08f, 0.03f, 0.28f);
        body.surfaceColorF = glm::vec3(0.0f, 0.95f, 0.78f);
        body.surfaceColorG = glm::vec3(0.94f, 0.99f, 1.0f);
    }

    body.surfaceVibrancy = 1.35f;
    body.surfaceColorBanding = 0.55f;
}

void randomizePalette(RigidBody& body)
{
    const float hue = randomFloat(0.0f, 1.0f);

    if (body.surfaceType == PlanetSurfaceType::EarthLike)
    {
        const float landHue = wrapHue(hue + randomFloat(0.20f, 0.55f));
        const float accentHue = wrapHue(landHue + randomFloat(0.08f, 0.25f));
        body.surfaceColorA = hsvColor(hue, 0.95f, 0.24f);
        body.surfaceColorB = hsvColor(wrapHue(hue + 0.04f), 0.85f, 0.88f);
        body.surfaceColorC = hsvColor(landHue, 0.92f, 0.30f);
        body.surfaceColorD = hsvColor(wrapHue(landHue + 0.03f), 0.82f, 0.84f);
        body.surfaceColorE = hsvColor(accentHue, 0.60f, 0.90f);
        body.surfaceColorF = hsvColor(wrapHue(hue + 0.10f), 0.18f, 1.0f);
        body.surfaceColorG = hsvColor(wrapHue(accentHue + 0.45f), 0.10f, 1.0f);
    }
    else if (body.surfaceType == PlanetSurfaceType::GasGiant)
    {
        const float secondaryHue = wrapHue(hue + randomFloat(0.08f, 0.25f));
        const float stormHue = wrapHue(hue + randomFloat(0.35f, 0.65f));
        body.surfaceColorA = hsvColor(hue, 0.92f, 0.26f);
        body.surfaceColorB = hsvColor(wrapHue(hue + 0.03f), 0.86f, 0.55f);
        body.surfaceColorC = hsvColor(secondaryHue, 0.78f, 0.82f);
        body.surfaceColorD = hsvColor(wrapHue(secondaryHue + 0.06f), 0.68f, 0.96f);
        body.surfaceColorE = hsvColor(wrapHue(secondaryHue + 0.14f), 0.52f, 1.0f);
        body.surfaceColorF = hsvColor(stormHue, 0.90f, 0.88f);
        body.surfaceColorG = hsvColor(wrapHue(stormHue + 0.04f), 0.22f, 1.0f);
    }
    else if (body.surfaceType == PlanetSurfaceType::IcyMoon)
    {
        const float crackHue = wrapHue(hue + randomFloat(0.18f, 0.48f));
        body.surfaceColorA = hsvColor(hue, 0.92f, 0.22f);
        body.surfaceColorB = hsvColor(wrapHue(hue + 0.04f), 0.78f, 0.68f);
        body.surfaceColorC = hsvColor(wrapHue(hue + 0.08f), 0.34f, 0.98f);
        body.surfaceColorD = hsvColor(crackHue, 0.90f, 0.96f);
        body.surfaceColorE = hsvColor(wrapHue(hue - 0.03f), 0.88f, 0.32f);
        body.surfaceColorF = hsvColor(wrapHue(crackHue + 0.12f), 0.78f, 0.98f);
        body.surfaceColorG = hsvColor(wrapHue(hue + 0.12f), 0.12f, 1.0f);
    }
}

void setStarPreset(RigidBody& body, StarSurfacePreset preset)
{
    body.starSurfacePreset = preset;

    if (preset == StarSurfacePreset::Solar)
    {
        body.starColorA = glm::vec3(0.12f, 0.012f, 0.004f);
        body.starColorB = glm::vec3(0.52f, 0.055f, 0.004f);
        body.starColorC = glm::vec3(1.0f, 0.24f, 0.008f);
        body.starColorD = glm::vec3(1.0f, 0.56f, 0.035f);
        body.starColorE = glm::vec3(1.0f, 0.87f, 0.32f);
        body.starColorF = glm::vec3(1.0f, 0.98f, 0.76f);
        body.starColorG = glm::vec3(1.0f, 0.42f, 0.04f);
        body.starCellScale = 7.0f;
        body.starActivity = 1.0f;
        body.starSpotAmount = 0.24f;
        body.starCoronaSize = 0.11f;
        body.starCoronaIntensity = 1.0f;
    }
    else if (preset == StarSurfacePreset::BlueWhite)
    {
        body.starColorA = glm::vec3(0.005f, 0.05f, 0.14f);
        body.starColorB = glm::vec3(0.0f, 0.30f, 0.48f);
        body.starColorC = glm::vec3(0.0f, 0.68f, 0.72f);
        body.starColorD = glm::vec3(0.38f, 0.90f, 0.82f);
        body.starColorE = glm::vec3(1.0f, 0.98f, 0.70f);
        body.starColorF = glm::vec3(1.0f, 1.0f, 0.95f);
        body.starColorG = glm::vec3(0.12f, 0.85f, 1.0f);
        body.starCellScale = 7.0f;
        body.starActivity = 1.15f;
        body.starSpotAmount = 0.30f;
        body.starCoronaSize = 0.13f;
        body.starCoronaIntensity = 1.15f;
    }
    else if (preset == StarSurfacePreset::RedDwarf)
    {
        body.starColorA = glm::vec3(0.06f, 0.002f, 0.008f);
        body.starColorB = glm::vec3(0.28f, 0.008f, 0.02f);
        body.starColorC = glm::vec3(0.62f, 0.025f, 0.035f);
        body.starColorD = glm::vec3(0.95f, 0.10f, 0.055f);
        body.starColorE = glm::vec3(1.0f, 0.36f, 0.16f);
        body.starColorF = glm::vec3(1.0f, 0.72f, 0.48f);
        body.starColorG = glm::vec3(1.0f, 0.12f, 0.08f);
        body.starCellScale = 5.5f;
        body.starActivity = 0.82f;
        body.starSpotAmount = 0.46f;
        body.starCoronaSize = 0.09f;
        body.starCoronaIntensity = 0.82f;
    }
    else
    {
        body.starColorA = glm::vec3(0.035f, 0.002f, 0.11f);
        body.starColorB = glm::vec3(0.20f, 0.01f, 0.48f);
        body.starColorC = glm::vec3(0.52f, 0.04f, 0.82f);
        body.starColorD = glm::vec3(0.10f, 0.82f, 0.68f);
        body.starColorE = glm::vec3(0.62f, 1.0f, 0.30f);
        body.starColorF = glm::vec3(0.92f, 1.0f, 0.70f);
        body.starColorG = glm::vec3(0.08f, 0.95f, 1.0f);
        body.starCellScale = 8.5f;
        body.starActivity = 1.35f;
        body.starSpotAmount = 0.22f;
        body.starCoronaSize = 0.16f;
        body.starCoronaIntensity = 1.30f;
    }
}

void randomizeStarPalette(RigidBody& body)
{
    const float baseHue = randomFloat(0.0f, 1.0f);
    const float hotHue = wrapHue(baseHue + randomFloat(0.02f, 0.12f));
    const float coronaHue = wrapHue(baseHue + randomFloat(0.12f, 0.42f));

    body.starColorA = hsvColor(wrapHue(baseHue - 0.03f), 0.98f, 0.13f);
    body.starColorB = hsvColor(baseHue, 0.92f, 0.40f);
    body.starColorC = hsvColor(wrapHue(baseHue + 0.02f), 0.82f, 0.72f);
    body.starColorD = hsvColor(wrapHue(baseHue + 0.04f), 0.62f, 0.94f);
    body.starColorE = hsvColor(hotHue, 0.36f, 1.0f);
    body.starColorF = hsvColor(wrapHue(hotHue + 0.03f), 0.14f, 1.0f);
    body.starColorG = hsvColor(coronaHue, 0.72f, 1.0f);
    body.starSurfacePreset = StarSurfacePreset::Alien;
}

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
            newStar.displayScale = 5.0f;
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
                std::sqrt(Physics::gravitationalConstant * centralMass / orbitRadius),
                0.0);
            newBody.surfaceType = PlanetSurfaceType::EarthLike;
            newBody.proceduralSeed = static_cast<int>(bodies.size()) * 97 + 11;
            newBody.displayScale = 300.0f;
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

        ImGui::Checkbox("Render as star", &body.isStar);
        ImGui::SliderFloat(
            "Display scale",
            &body.displayScale,
            0.1f,
            500.0f,
            "%.1f",
            ImGuiSliderFlags_Logarithmic);

        if (body.isStar)
        {
            int starPreset = static_cast<int>(body.starSurfacePreset);
            if (ImGui::Combo(
                    "Stellar preset",
                    &starPreset,
                    starPresetNames,
                    static_cast<int>(std::size(starPresetNames))))
            {
                setStarPreset(body, static_cast<StarSurfacePreset>(starPreset));
            }

            ImGui::InputInt("Star seed", &body.starSeed);
            if (ImGui::Button("Randomize star seed"))
                randomizeStarSeed(body);
            ImGui::SameLine();
            if (ImGui::Button("Randomize star palette"))
                randomizeStarPalette(body);

            ImGui::SliderFloat(
                "Convection scale",
                &body.starCellScale,
                2.0f,
                16.0f,
                "%.2f",
                ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Stellar activity", &body.starActivity, 0.0f, 2.0f, "%.2f");
            ImGui::SliderFloat("Starspots", &body.starSpotAmount, 0.0f, 1.0f, "%.2f");
            ImGui::SliderFloat(
                "Surface motion",
                &body.starAnimationSpeed,
                0.0f,
                3.0f,
                "%.2f");
            ImGui::SliderFloat("Corona size", &body.starCoronaSize, 0.02f, 0.35f, "%.2f");
            ImGui::SliderFloat(
                "Corona intensity",
                &body.starCoronaIntensity,
                0.0f,
                3.0f,
                "%.2f");

            if (ImGui::TreeNodeEx(
                    "Stellar palette",
                    ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit3("Starspot color", &body.starColorA.x);
                ImGui::ColorEdit3("Cell boundaries", &body.starColorB.x);
                ImGui::ColorEdit3("Lower plasma", &body.starColorC.x);
                ImGui::ColorEdit3("Main surface", &body.starColorD.x);
                ImGui::ColorEdit3("Hot cells", &body.starColorE.x);
                ImGui::ColorEdit3("Flares", &body.starColorF.x);
                ImGui::ColorEdit3("Corona", &body.starColorG.x);
                ImGui::TreePop();
            }
        }

        if (!body.isStar)
        {
            int surfaceType = static_cast<int>(body.surfaceType);
            if (ImGui::Combo(
                    "Surface",
                    &surfaceType,
                    surfaceTypeNames,
                    static_cast<int>(std::size(surfaceTypeNames))))
            {
                body.surfaceType = static_cast<PlanetSurfaceType>(surfaceType);
                setDefaultPalette(body, body.surfaceType);
            }

            if (body.surfaceType == PlanetSurfaceType::Texture)
            {
                int textureIndex = findTextureOption(body.texturePath);
                const char* texturePreview = textureIndex >= 0
                    ? textureOptions[textureIndex].name
                    : "Select texture";
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
            }
            else
            {
                ImGui::InputInt("Surface seed", &body.proceduralSeed);
                if (ImGui::Button("Randomize seed"))
                    randomizeSeed(body);
                ImGui::SliderFloat(
                    "Surface scale",
                    &body.terrainScale,
                    0.5f,
                    12.0f,
                    "%.2f",
                    ImGuiSliderFlags_Logarithmic);
                if (body.surfaceType == PlanetSurfaceType::EarthLike)
                {
                    ImGui::SliderFloat("Ocean level", &body.oceanLevel, 0.20f, 0.80f, "%.2f");
                    ImGui::SliderFloat("Cloud coverage", &body.cloudCoverage, 0.0f, 0.90f, "%.2f");
                }

                if (body.surfaceType == PlanetSurfaceType::EarthLike
                    || body.surfaceType == PlanetSurfaceType::GasGiant
                    || body.surfaceType == PlanetSurfaceType::IcyMoon)
                {
                    if (ImGui::Button("Randomize palette"))
                        randomizePalette(body);
                    ImGui::SliderFloat(
                        "Vibrancy",
                        &body.surfaceVibrancy,
                        0.50f,
                        2.0f,
                        "%.2f");
                    ImGui::SliderFloat(
                        "Color banding",
                        &body.surfaceColorBanding,
                        0.0f,
                        1.0f,
                        "%.2f");

                    if (ImGui::TreeNodeEx(
                            "Palette colors",
                            ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        if (body.surfaceType == PlanetSurfaceType::EarthLike)
                        {
                            ImGui::ColorEdit3("Deep ocean", &body.surfaceColorA.x);
                            ImGui::ColorEdit3("Shallow ocean", &body.surfaceColorB.x);
                            ImGui::ColorEdit3("Lowlands", &body.surfaceColorC.x);
                            ImGui::ColorEdit3("Vegetation", &body.surfaceColorD.x);
                            ImGui::ColorEdit3("Highlands", &body.surfaceColorE.x);
                            ImGui::ColorEdit3("Polar ice", &body.surfaceColorF.x);
                            ImGui::ColorEdit3("Clouds", &body.surfaceColorG.x);
                        }
                        else if (body.surfaceType == PlanetSurfaceType::GasGiant)
                        {
                            ImGui::ColorEdit3("Deep band", &body.surfaceColorA.x);
                            ImGui::ColorEdit3("Primary band", &body.surfaceColorB.x);
                            ImGui::ColorEdit3("Secondary band", &body.surfaceColorC.x);
                            ImGui::ColorEdit3("Bright band", &body.surfaceColorD.x);
                            ImGui::ColorEdit3("Fine band", &body.surfaceColorE.x);
                            ImGui::ColorEdit3("Storm", &body.surfaceColorF.x);
                            ImGui::ColorEdit3("Storm highlight", &body.surfaceColorG.x);
                        }
                        else
                        {
                            ImGui::ColorEdit3("Abyssal ice", &body.surfaceColorA.x);
                            ImGui::ColorEdit3("Base ice", &body.surfaceColorB.x);
                            ImGui::ColorEdit3("Frost", &body.surfaceColorC.x);
                            ImGui::ColorEdit3("Cracks", &body.surfaceColorD.x);
                            ImGui::ColorEdit3("Crater shadows", &body.surfaceColorE.x);
                            ImGui::ColorEdit3("Crystal accent", &body.surfaceColorF.x);
                            ImGui::ColorEdit3("Highlights", &body.surfaceColorG.x);
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::SliderFloat(
                    "Surface motion",
                    &body.surfaceAnimationSpeed,
                    0.0f,
                    4.0f,
                    "%.2f");
            }
        }

        body.bodyMass = std::max(body.bodyMass, 1.0e-12);
        body.bodyRadius = std::max(body.bodyRadius, 1.0e-12);
        body.displayScale = std::max(body.displayScale, 0.01f);
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
