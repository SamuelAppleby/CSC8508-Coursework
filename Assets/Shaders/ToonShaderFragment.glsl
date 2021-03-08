#version 460

uniform mat4 modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;

uniform vec3	lightPos;
uniform float	lightRadius;
uniform vec4	lightColour;

in Vertex{ 
    vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main()
{
    vec4 color;
    vec3 lightDir = normalize(lightPos);

    float intensity = dot(lightDir, IN.normal);

    if (intensity > 0.95)      color = vec4(1.0, 1.0, 1.0, 1.0);
    else if (intensity > 0.75) color = vec4(0.8, 0.8, 0.8, 1.0);
    else if (intensity > 0.50) color = vec4(0.6, 0.6, 0.6, 1.0);
    else if (intensity > 0.25) color = vec4(0.4, 0.4, 0.4, 1.0);
    else                       color = vec4(0.2, 0.2, 0.2, 1.0);

    fragColor = color;
}