#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 ObjectPosition;

uniform vec3 viewPos;
uniform float starSeed;
uniform float cellScale;
uniform float activity;
uniform float animationSpeed;
uniform float spotAmount;
uniform float timeSeconds;
uniform vec3 starColorA;
uniform vec3 starColorB;
uniform vec3 starColorC;
uniform vec3 starColorD;
uniform vec3 starColorE;
uniform vec3 starColorF;
uniform vec3 starColorG;

float hash31(vec3 point)
{
    point = fract(point * 0.1031f);
    point += dot(point, point.yzx + 33.33f);
    return fract((point.x + point.y) * point.z);
}

vec3 hash33(vec3 point)
{
    return vec3(
        hash31(point + vec3(17.0f, 59.4f, 15.0f)),
        hash31(point + vec3(43.0f, 27.1f, 61.7f)),
        hash31(point + vec3(11.3f, 83.2f, 37.9f)));
}

vec2 cellularMetrics(vec3 point)
{
    vec3 cell = floor(point);
    vec3 localPoint = fract(point);
    float nearest = 10.0f;
    float secondNearest = 10.0f;

    for (int z = -1; z <= 1; ++z)
    {
        for (int y = -1; y <= 1; ++y)
        {
            for (int x = -1; x <= 1; ++x)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));
                float featureDistance = length(
                    neighbor + hash33(cell + neighbor) - localPoint);
                if (featureDistance < nearest)
                {
                    secondNearest = nearest;
                    nearest = featureDistance;
                }
                else if (featureDistance < secondNearest)
                {
                    secondNearest = featureDistance;
                }
            }
        }
    }
    return vec2(nearest, secondNearest - nearest);
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

float fbm(vec3 point)
{
    float value = 0.0f;
    float amplitude = 0.5f;
    for (int octave = 0; octave < 4; ++octave)
    {
        value += amplitude * noise3(point);
        point = point * 2.03f + vec3(13.1f, 27.7f, 9.5f);
        amplitude *= 0.5f;
    }
    return value;
}

float warpedNoise(vec3 point)
{
    vec3 warp = vec3(
        fbm(point + vec3(5.2f, 1.3f, 7.1f)),
        fbm(point + vec3(9.2f, 2.8f, 3.4f)),
        fbm(point + vec3(2.1f, 8.3f, 4.7f)));
    return fbm(point + 1.65f * (warp - 0.5f));
}

vec3 plasmaRamp(float heat)
{
    float scaled = clamp(heat, 0.0f, 1.0f) * 4.0f;
    float blendAmount = smoothstep(0.24f, 0.76f, fract(scaled));
    if (scaled < 1.0f)
        return mix(starColorB, starColorC, blendAmount);
    if (scaled < 2.0f)
        return mix(starColorC, starColorD, blendAmount);
    if (scaled < 3.0f)
        return mix(starColorD, starColorE, blendAmount);
    return mix(starColorE, starColorF, blendAmount);
}

void main()
{
    vec3 point = normalize(ObjectPosition);
    vec3 seedOffset = vec3(
        mod(abs(starSeed), 997.0f) * 0.103f,
        mod(abs(starSeed), 991.0f) * 0.173f,
        mod(abs(starSeed), 983.0f) * 0.197f);
    float animatedTime = timeSeconds * animationSpeed;
    float activeAmount = max(activity, 0.0f);

    vec3 flow = vec3(
        fbm(point * 2.7f + seedOffset + vec3(animatedTime * 0.018f, 0.0f, 0.0f)),
        fbm(point * 2.7f - seedOffset + vec3(0.0f, animatedTime * 0.014f, 0.0f)),
        fbm(point * 2.7f + seedOffset.yzx + vec3(0.0f, 0.0f, animatedTime * 0.016f)));
    vec3 boilingPoint = point + (flow - 0.5f) * 0.18f * activeAmount;
    vec2 cells = cellularMetrics(
        boilingPoint * max(cellScale, 0.5f)
            + seedOffset
            + vec3(animatedTime * 0.025f, -animatedTime * 0.018f, animatedTime * 0.012f));

    float turbulence = warpedNoise(
        point * max(cellScale, 0.5f) * 0.72f
            - seedOffset
            + vec3(-animatedTime * 0.012f, animatedTime * 0.016f, 0.0f));
    float cellHeat = 1.0f - smoothstep(0.12f, 0.82f, cells.x);
    float heat = clamp(cellHeat * 0.72f + turbulence * 0.48f, 0.0f, 1.0f);
    vec3 color = plasmaRamp(heat);

    float boundary = 1.0f - smoothstep(0.025f, 0.115f, cells.y);
    color = mix(color, starColorB, boundary * 0.72f);

    float spotNoise = warpedNoise(
        point * 1.45f + seedOffset * 0.27f
            + vec3(0.0f, animatedTime * 0.003f, 0.0f));
    float spotThreshold = mix(0.96f, 0.58f, clamp(spotAmount, 0.0f, 1.0f));
    float starspot = smoothstep(spotThreshold, spotThreshold + 0.055f, spotNoise);
    color = mix(color, starColorA, starspot * 0.88f);

    float flareFleck = smoothstep(0.88f, 0.98f, turbulence + boundary * 0.16f);
    color = mix(color, starColorF, flareFleck * 0.62f * activeAmount);

    vec3 viewDirection = normalize(viewPos - FragPos);
    float rim = pow(1.0f - max(dot(normalize(Normal), viewDirection), 0.0f), 2.4f);
    color = mix(color, starColorG, rim * 0.30f);

    float pulse = 0.96f + 0.04f * sin(animatedTime * 1.7f + turbulence * 8.0f);
    FragColor = vec4(color * pulse, 1.0f);
}
