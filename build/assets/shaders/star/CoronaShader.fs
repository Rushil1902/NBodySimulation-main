#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 ObjectPosition;
in float CoronaNoise;

uniform vec3 viewPos;
uniform vec3 coronaColorInner;
uniform vec3 coronaColorOuter;
uniform float starSeed;
uniform float activity;
uniform float animationSpeed;
uniform float coronaIntensity;
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
    vec3 viewDirection = normalize(viewPos - FragPos);
    float facing = abs(dot(normalize(Normal), viewDirection));
    float fresnel = pow(1.0f - clamp(facing, 0.0f, 1.0f), 2.2f);

    float animatedTime = timeSeconds * animationSpeed;
    vec3 seedOffset = vec3(
        mod(abs(starSeed), 953.0f) * 0.037f,
        mod(abs(starSeed), 947.0f) * 0.067f,
        mod(abs(starSeed), 941.0f) * 0.097f);
    float plasmaNoise = noise3(
        ObjectPosition * 11.0f + seedOffset
            + vec3(-animatedTime * 0.035f, animatedTime * 0.041f, 0.0f));
    float eruption = smoothstep(0.68f, 0.92f, CoronaNoise + plasmaNoise * 0.22f);
    float alpha = (0.035f + fresnel * 0.58f + eruption * 0.16f)
        * (0.48f + CoronaNoise * 0.72f)
        * max(coronaIntensity, 0.0f);
    alpha = clamp(alpha, 0.0f, 0.88f);

    vec3 color = mix(
        coronaColorInner,
        coronaColorOuter,
        clamp(fresnel * 0.72f + eruption * 0.35f, 0.0f, 1.0f));
    FragColor = vec4(color, alpha);
}
