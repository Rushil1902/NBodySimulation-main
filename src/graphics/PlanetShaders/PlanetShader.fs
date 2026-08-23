#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 ObjectPosition;

uniform sampler2D planetTexture;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform int surfaceType;
uniform float proceduralSeed;
uniform float terrainScale;
uniform float oceanLevel;
uniform float cloudCoverage;
uniform float animationSpeed;
uniform vec3 surfaceColorA;
uniform vec3 surfaceColorB;
uniform vec3 surfaceColorC;
uniform vec3 surfaceColorD;
uniform vec3 surfaceColorE;
uniform vec3 surfaceColorF;
uniform vec3 surfaceColorG;
uniform float surfaceVibrancy;
uniform float surfaceColorBanding;
uniform float timeSeconds;

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
                vec3 feature = hash33(cell + neighbor);
                float featureDistance = length(neighbor + feature - localPoint);
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
    vec3 local = fract(point);
    local = local * local * (3.0f - 2.0f * local);

    float lower = mix(
        mix(
            mix(hash31(cell + vec3(0, 0, 0)), hash31(cell + vec3(1, 0, 0)), local.x),
            mix(hash31(cell + vec3(0, 1, 0)), hash31(cell + vec3(1, 1, 0)), local.x),
            local.y),
        mix(
            mix(hash31(cell + vec3(0, 0, 1)), hash31(cell + vec3(1, 0, 1)), local.x),
            mix(hash31(cell + vec3(0, 1, 1)), hash31(cell + vec3(1, 1, 1)), local.x),
            local.y),
        local.z);
    return lower;
}

float fbm(vec3 point)
{
    float value = 0.0f;
    float amplitude = 0.5f;
    for (int octave = 0; octave < 5; ++octave)
    {
        value += amplitude * noise3(point);
        point = point * 2.03f + vec3(17.1f, 31.7f, 11.5f);
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
    return fbm(point + 1.8f * (warp - 0.5f));
}

float paletteBlend(float value)
{
    float smoothValue = smoothstep(0.0f, 1.0f, clamp(value, 0.0f, 1.0f));
    float steppedValue = step(0.5f, smoothValue);
    return mix(smoothValue, steppedValue, clamp(surfaceColorBanding, 0.0f, 1.0f));
}

vec3 paletteRamp4(float value, vec3 colorA, vec3 colorB, vec3 colorC, vec3 colorD)
{
    float scaled = clamp(value, 0.0f, 1.0f) * 3.0f;
    if (scaled < 1.0f)
        return mix(colorA, colorB, paletteBlend(scaled));
    if (scaled < 2.0f)
        return mix(colorB, colorC, paletteBlend(scaled - 1.0f));
    return mix(colorC, colorD, paletteBlend(scaled - 2.0f));
}

vec3 paletteRamp5(
    float value,
    vec3 colorA,
    vec3 colorB,
    vec3 colorC,
    vec3 colorD,
    vec3 colorE)
{
    float scaled = clamp(value, 0.0f, 1.0f) * 4.0f;
    if (scaled < 1.0f)
        return mix(colorA, colorB, paletteBlend(scaled));
    if (scaled < 2.0f)
        return mix(colorB, colorC, paletteBlend(scaled - 1.0f));
    if (scaled < 3.0f)
        return mix(colorC, colorD, paletteBlend(scaled - 2.0f));
    return mix(colorD, colorE, paletteBlend(scaled - 3.0f));
}

vec3 applyVibrancy(vec3 color)
{
    float luminance = dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
    return clamp(
        mix(vec3(luminance), color, max(surfaceVibrancy, 0.0f)),
        0.0f,
        1.0f);
}

vec3 surfaceColor(vec3 point, out float emission, out float specularAmount)
{
    emission = 0.0f;
    specularAmount = 0.08f;
    vec3 seedOffset = vec3(
        proceduralSeed * 0.103f,
        proceduralSeed * 0.173f,
        proceduralSeed * 0.197f);
    float animatedTime = timeSeconds * animationSpeed;

    if (surfaceType == 1)
    {
        float height = mix(
            warpedNoise(point * terrainScale + seedOffset),
            fbm(point * terrainScale * 4.0f + seedOffset),
            0.18f);
        float land = smoothstep(oceanLevel - 0.025f, oceanLevel + 0.025f, height);
        vec3 ocean = mix(
            surfaceColorA,
            surfaceColorB,
            paletteBlend(smoothstep(0.20f, oceanLevel, height)));
        float landHeight = smoothstep(oceanLevel, 0.92f, height);
        vec3 terrain = paletteRamp4(
            landHeight,
            surfaceColorC,
            surfaceColorD,
            surfaceColorE,
            surfaceColorE);
        vec3 color = mix(ocean, terrain, land);

        float polarIce = smoothstep(0.72f, 0.96f, abs(point.z));
        color = mix(color, surfaceColorF, polarIce * 0.90f);

        float cloudNoise = fbm(
            point * 5.5f + seedOffset * 1.7f
                + vec3(animatedTime * 0.025f, 0.0f, 0.0f));
        float cloudThreshold = clamp(1.0f - cloudCoverage, 0.08f, 0.98f);
        float clouds = smoothstep(cloudThreshold, cloudThreshold + 0.08f, cloudNoise);
        color = mix(color, surfaceColorG, clouds * 0.78f);
        specularAmount = (1.0f - land) * 0.55f;
        return color;
    }

    if (surfaceType == 2)
    {
        float turbulence = fbm(
            point * terrainScale + seedOffset
                + vec3(animatedTime * 0.012f, 0.0f, 0.0f));
        float latitude = point.z + (turbulence - 0.5f) * 0.16f;
        float broadBands = 0.5f + 0.5f * sin(latitude * 32.0f + turbulence * 7.0f);
        float fineBands = 0.5f + 0.5f * sin(latitude * 91.0f + turbulence * 13.0f);
        float bandValue = clamp(broadBands * 0.76f + fineBands * 0.24f, 0.0f, 1.0f);
        vec3 color = paletteRamp5(
            bandValue,
            surfaceColorA,
            surfaceColorB,
            surfaceColorC,
            surfaceColorD,
            surfaceColorE);
        float storm = smoothstep(
            0.80f,
            0.91f,
            warpedNoise(point * 3.2f + seedOffset * 2.0f));
        vec3 stormColor = mix(surfaceColorF, surfaceColorG, paletteBlend(storm));
        return mix(color, stormColor, storm * 0.74f);
    }

    if (surfaceType == 3)
    {
        float rock = warpedNoise(point * terrainScale + seedOffset);
        float ridge = abs(
            fbm(point * terrainScale * 2.3f + seedOffset
                + vec3(0.0f, animatedTime * 0.018f, 0.0f)) * 2.0f - 1.0f);
        float fissure = 1.0f - smoothstep(0.025f, 0.14f, ridge);
        vec3 cooledRock = mix(
            vec3(0.035f, 0.02f, 0.025f),
            vec3(0.20f, 0.07f, 0.025f),
            rock);
        emission = fissure * (1.2f + 0.8f * rock);
        return mix(cooledRock, vec3(1.0f, 0.18f, 0.015f), fissure);
    }

    if (surfaceType == 4)
    {
        float scale = max(terrainScale, 0.2f);
        float iceLayers = warpedNoise(point * scale + seedOffset);
        float fineIce = fbm(point * scale * 4.0f - seedOffset);
        vec2 cells = cellularMetrics(point * scale * 1.35f + seedOffset);

        float crack = 1.0f - smoothstep(0.025f, 0.13f, cells.y);
        float craterRim = 1.0f - smoothstep(
            0.035f,
            0.095f,
            abs(cells.x - 0.33f));
        float craterFloor = 1.0f - smoothstep(0.18f, 0.33f, cells.x);

        vec3 ice = mix(
            surfaceColorA,
            surfaceColorB,
            paletteBlend(smoothstep(0.22f, 0.82f, iceLayers)));
        ice = mix(
            ice,
            surfaceColorC,
            smoothstep(0.62f, 0.92f, fineIce) * 0.38f);
        ice = mix(ice, surfaceColorD, crack * 0.88f);
        ice = mix(ice, surfaceColorE, craterFloor * 0.26f);
        ice = mix(
            ice,
            surfaceColorF,
            craterRim * 0.30f);
        float crystal = smoothstep(0.82f, 0.96f, fineIce);
        ice = mix(ice, surfaceColorG, crystal * 0.34f);
        specularAmount = 0.42f;
        return ice;
    }

    specularAmount = 0.25f;
    return texture(planetTexture, TexCoords).rgb;
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);

    float emission = 0.0f;
    float specularAmount = 0.0f;
    vec3 baseColor = surfaceColor(normalize(ObjectPosition), emission, specularAmount);
    bool vividSurface = surfaceType == 1 || surfaceType == 2 || surfaceType == 4;
    if (vividSurface)
        baseColor = applyVibrancy(baseColor);

    float ambientLight = vividSurface ? 0.25f : 0.16f;
    vec3 result = baseColor * lightColor * (ambientLight + 0.88f * diff);
    result += specularAmount * spec * lightColor;
    result += emission * vec3(1.0f, 0.24f, 0.02f);

    if (vividSurface)
    {
        float rim = pow(1.0f - max(dot(norm, viewDir), 0.0f), 2.6f);
        vec3 rimColor = mix(baseColor, surfaceColorG, 0.35f);
        result += rimColor * rim * 0.22f;
    }

    FragColor = vec4(result, 1.0f);
}
