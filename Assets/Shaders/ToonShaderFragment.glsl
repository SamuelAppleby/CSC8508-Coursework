#version 460

uniform sampler2D 	mainTex;

uniform vec3	lightPos;
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

    vec3 lightDir = normalize(lightPos);

    float intensity = smoothstep(0.1, 0.6, dot(lightDir, IN.normal));

    if (intensity > 0.95)
		color = vec4(color.xyz * 0.8, 1.0);
	else if (intensity > 0.6)
		color = vec4(color.xyz * 0.5,1.0);
	else if (intensity > 0.25)
		color = vec4(color.xyz * 0.2,1.0);
	else
		color = vec4(color.xyz * 0.1,1.0);

    fragColor = color * lightColour;
}