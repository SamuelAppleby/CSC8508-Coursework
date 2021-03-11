#version 460

uniform mat4 modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat3 normalMatrix;
uniform mat4 shadowMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

uniform vec4 	objectColour = vec4(1,1,1,1);
uniform vec3 lightPos;

out Vertex{
    vec4 colour;
    vec2 texCoord;
    vec4 shadowProj;
    vec3 normal;
    vec3 worldPos;
} OUT;

void main(void)
{
    mat4 mvp = (projMatrix * viewMatrix * modelMatrix);
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    OUT.shadowProj 	=  shadowMatrix * vec4 ( position,1);
	OUT.worldPos 	= ( modelMatrix * vec4 ( position ,1)). xyz ;
	OUT.normal 		= normalize ( normalMatrix * normalize ( normal ));
	
	OUT.texCoord	= texCoord;
	OUT.colour		= objectColour;

    gl_Position = mvp * vec4(position, 1.0);
}