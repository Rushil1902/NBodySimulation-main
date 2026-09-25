#version 330 core

out vec4 FragColor;
in float fieldStrength;
uniform bool surfacePass;
uniform float surfaceOpacity;

void main()
{
    vec3 weakField = vec3(0.08f, 0.42f, 0.85f);
    vec3 mediumField = vec3(0.15f, 0.85f, 0.90f);
    vec3 strongField = vec3(1.00f, 0.48f, 0.08f);
    vec3 color = fieldStrength < 0.5f
        ? mix(weakField, mediumField, fieldStrength * 2.0f)
        : mix(mediumField, strongField, (fieldStrength - 0.5f) * 2.0f);
    float alpha = surfacePass ? surfaceOpacity : 0.82f;
    FragColor = vec4(color, alpha);
}
