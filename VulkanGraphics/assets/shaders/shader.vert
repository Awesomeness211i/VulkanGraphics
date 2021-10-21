#version 450

//Inputs
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec2 uv;

//Outputs
layout(location = 0) out vec4 o_Color;

layout(set = 0, binding = 0) uniform GlobalUBO {
	mat4 projectionViewMatrix;
	vec3 directionToLight;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

const float AMBIENT = 0.02;

void main() {
	gl_Position = ubo.projectionViewMatrix * push.modelMatrix * position;

	vec3 normalWorldSpace = normalize((push.normalMatrix * normal).xyz);
	float lightIntensity = max(dot(normalWorldSpace, ubo.directionToLight), AMBIENT);

	o_Color = lightIntensity * color;
}