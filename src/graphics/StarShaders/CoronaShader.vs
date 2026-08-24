#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 ObjectPosition;
out float CoronaNoise;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float starSeed;
uniform float activity;
uniform float animationSpeed;
uniform float timeSeconds;

float hash31(vec3 point)
{
    point = fract(point * 0.1031f);
    point += dot(point, point.yzx + 33.33f);
    return fract((point.x + point.y) * point.z);
}

float noise3(vec3 point)
{
    vec3 cell = floor(point);
    vec3 localPoint = fract(point);
    localPoint = localPoint * localPoint * (3.0f - 2.0f * localPoint);
    return mix(
        mix(
            mix(hash31(cell + vec3(0, 0, 0)), hash31(cell + vec3(1, 0, 0)), localPoint.x),
            mix(hash31(cell + vec3(0, 1, 0)), hash31(cell + vec3(1, 1, 0)), localPoint.x),
            localPoint.y),
        mix(
            mix(hash31(cell + vec3(0, 0, 1)), hash31(cell + vec3(1, 0, 1)), localPoint.x),
            mix(hash31(cell + vec3(0, 1, 1)), hash31(cell + vec3(1, 1, 1)), localPoint.x),
            localPoint.y),
        localPoint.z);
}

void main()
{
    vec3 point = normalize(aPos);
    float animatedTime = timeSeconds * animationSpeed;
    vec3 seedOffset = vec3(
        mod(abs(starSeed), 977.0f) * 0.071f,
        mod(abs(starSeed), 971.0f) * 0.113f,
        mod(abs(starSeed), 967.0f) * 0.157f);
    float broadNoise = noise3(
        point * 5.0f + seedOffset
            + vec3(animatedTime * 0.035f, -animatedTime * 0.021f, 0.0f));
    float fineNoise = noise3(
        point * 13.0f - seedOffset
            + vec3(0.0f, animatedTime * 0.047f, animatedTime * 0.026f));
    CoronaNoise = broadNoise * 0.72f + fineNoise * 0.28f;

    float flare = pow(max(broadNoise - 0.48f, 0.0f) * 1.92f, 3.0f);
    float displacement = (0.012f + CoronaNoise * 0.025f + flare * 0.075f)
        * max(activity, 0.0f);
    vec3 displacedPosition = aPos * (1.0f + displacement);
    vec4 worldPosition = model * vec4(displacedPosition, 1.0f);
    FragPos = worldPosition.xyz;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    ObjectPosition = point;
    gl_Position = projection * view * worldPosition;
}
