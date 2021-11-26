#version 450

//Inputs
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec2 uv;

//Outputs
layout(location = 0) out vec4 o_Color;
layout(location = 1) out vec4 o_WorldPosition;
layout(location = 2) out vec4 o_WorldNormal;

layout(set = 0, binding = 0) uniform GlobalUBO {
	mat4 projectionViewMatrix;
	vec4 ambientLightColor; //4th component is light intensity
	vec4 lightPosition; //ignore 4th component
	vec4 lightColor; //4th component is light intensity
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	vec4 positionWorld = push.modelMatrix * position;
	gl_Position = ubo.projectionViewMatrix * positionWorld;

	o_WorldPosition = positionWorld;
	o_WorldNormal = normalize(push.normalMatrix * normal);
	o_Color = color;
}