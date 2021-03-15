#version 460

uniform sampler2D 	mainTex;

uniform vec3 lightPos;
uniform vec3	lightDir;
uniform float	lightRadius;
uniform vec4	lightColour;

uniform bool hasTexture;

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
    vec4 color = IN.colour;

    if(hasTexture){
        color *= texture(mainTex, IN.texCoord);
    }

    float intensity = smoothstep(0.1, 0.6, dot(lightDir, -IN.normal));

	intensity = intensity > 0.95 ?  0.8 : intensity > 0.6 ? 0.5 : intensity > 0.25 ? 0.2 : 0.1;

    fragColor = color * intensity * lightColour;
}