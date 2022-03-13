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

struct PointLight {
	vec4 position; //ignore w
	vec4 color; // w is intensity
};

layout(set = 0, binding = 0) uniform GlobalUBO {
	mat4 projectionMatrix;
	mat4 viewMatrix;
	vec4 ambientLightColor; //4th component is light intensity
	PointLight pointlights[10];
	int numLights;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	vec4 positionWorld = push.modelMatrix * position;
	gl_Position = ubo.projectionMatrix * ubo.viewMatrix * positionWorld;

	o_WorldPosition = positionWorld;
	o_WorldNormal = normalize(push.normalMatrix * normal);
	o_Color = color;
}
